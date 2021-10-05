#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <functional>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "scenes.h"
#include "metadata.h"
#include "assets.h"
#include "sprites.h"
#include "utils.h"

/* -------------------------------------------------
 * Scene Context def
 * -------------------------------------------------
 */
SceneContext::SceneContext(SceneID scene)
{	
	switch(scene) {
		case MENU_SCENE:
			this->pScene = new MenuScene(this);
			break;
		case PLAY_SCENE:
			this->pScene = new PlayScene(this);
			break;
		case GAMEOVER_SCENE:
			this->pScene = new GameOverScene(this);
			break;
		default:
			this->pScene = NULL;
			break;
	}
}

SceneContext::~SceneContext() {
	delete this->pScene;
	this->pScene = NULL;
}

void SceneContext::changeScene(SceneID scene)
{
    delete this->pScene;
	this->pScene = NULL;
	
	switch(scene) {
		case MENU_SCENE:
			this->pScene = new MenuScene(this);
			break;
		case PLAY_SCENE:
			this->pScene = new PlayScene(this);
			break;
		case GAMEOVER_SCENE:
			this->pScene = new GameOverScene(this);
			break;
		default:
			this->pScene = NULL;
			break;
	}

}

void SceneContext::quit() {
	delete this->pScene;
	this->pScene = NULL;
}

void SceneContext::HandleEvents(SDL_Event *e)
{
	if(this->pScene != NULL) {
		pScene->handleEvents(e);
	}
    
}

void SceneContext::Update()
{
	if(this->pScene != NULL) {
		pScene->update();
	}
}

void SceneContext::Draw(SDL_Renderer *renderer)
{
	if(this->pScene != NULL) {
		pScene->draw(renderer);
	}
}


/* -------------------------------------------------
 * Menu scene def
 * -------------------------------------------------
 */
 
 
 MenuScene::MenuScene(SceneContext* context) {
	mContext = context;
	z_mouse.isClicked = false;
	
	if(z_mouse.isClicked) {
		printf("Mouse is clicked.\n");
	}
	else {
		printf("Mouse is not clicked.\n");
	}
	
	// Buttons
	Button* btn1 = new Button(btnTexture, "TESTING", 96, 32, 100, 100);
	btn1->bindCallback(std::bind(&MenuScene::chs_playGame, this));
	buttons.push_back(btn1);
	
 }
 
 MenuScene::~MenuScene() {
	for(Button* btn : buttons) {
		delete btn;
		btn = NULL;
	}
	printf("Deleted menu scene.\n");
 }
 
 void MenuScene::handleEvents(SDL_Event* e) {
	// Handle events --------------------------------------
    while (SDL_PollEvent(e))
    {
        if (e->type == SDL_QUIT)
        {
            mContext->quit();
        }
        else if (e->type == SDL_MOUSEBUTTONDOWN)
        {
            z_mouse.isClicked = true;
        }
        else if (e->type == SDL_MOUSEBUTTONUP)
        {
            z_mouse.isClicked = false;
        }
    }
	
	// Update mouse position
    SDL_GetMouseState(&z_mouse.pos.x, &z_mouse.pos.y);

 }
 
void MenuScene::update() {
	//  Update buttons
	for(Button* btn : buttons) {
		btn->update(&z_mouse);
	}
}
 
void MenuScene::draw(SDL_Renderer* renderer) {
	// Set render draw color, and clear renderer ----------
	SDL_SetRenderDrawColor(renderer, 5, 10, 10, 255);
	SDL_RenderClear(renderer);

	drawText(renderer, "MENU SCENE", gFont, 0, 0, {255, 255, 255});
	drawText(renderer, "PRESS ENTER TO PLAY", gFont, 0, 64, {255, 255, 255});

	// Draw buttons --------------------------
	for (Button* button : buttons) {
		button->draw(renderer);
	}

	// Render
	SDL_RenderPresent(renderer);
}
 
void MenuScene::chs_playGame() {
	mContext->changeScene(PLAY_SCENE);
}










/* -------------------------------------------------
 * Play Scene def
 * -------------------------------------------------
 */
PlayScene::PlayScene(SceneContext *context)
{
	
    mContext = context;
    isGameOver = false;
    gameOverMessage = "GAME OVER MESSAGE";
    mMouseClicked = false;

	// Anchor points, offsets
    const int centerW = (WINDOW_WIDTH / 2) - (HOLE_WIDTH / 2);
    const int centerH = (WINDOW_HEIGHT / 2) - (HOLE_HEIGHT / 2);
    const int offsetW = 48;
    const int offsetH = 32;
	
    // Column 1
    holeSprites.push_back(new HoleSprite(spritesTexture, centerW - offsetW, centerH - 72 + offsetH));
    holeSprites.push_back(new HoleSprite(spritesTexture, centerW - offsetW - 32, centerH + offsetH));
    holeSprites.push_back(new HoleSprite(spritesTexture, centerW - offsetW, centerH + 72 + offsetH));

    // Column 2
    holeSprites.push_back(new HoleSprite(spritesTexture, centerW + offsetW, centerH - 72 + offsetH));
    holeSprites.push_back(new HoleSprite(spritesTexture, centerW + offsetW + 32, centerH + offsetH));
    holeSprites.push_back(new HoleSprite(spritesTexture, centerW + offsetW, centerH + 72 + offsetH));
	
}

PlayScene::~PlayScene()
{
	for (HoleSprite* hole : holeSprites)
    {
        delete hole;
    }
	holeSprites.clear();
	printf("Deleted play scene.\n");
}

void PlayScene::handleEvents(SDL_Event *e)
{
    // Handle events --------------------------------------
    while (SDL_PollEvent(e))
    {
        if (e->type == SDL_QUIT)
        {
            mContext->quit();
        }
        else if (e->type == SDL_MOUSEBUTTONDOWN)
        {
            mMouseClicked = true;
        }
        else if (e->type == SDL_MOUSEBUTTONUP)
        {
            mMouseClicked = false;
        }
		else if (e->type == SDL_KEYDOWN) {
			switch(e->key.keysym.sym) {
			case SDLK_ESCAPE:
				mContext->changeScene(MENU_SCENE);
				break;
			default:
				break;
			}
		}
    }

    // Game over check
    if (isGameOver)
    {
        printf("Game over: %s!\n", gameOverMessage.c_str());
		mContext->changeScene(GAMEOVER_SCENE);
    }

    // Update mouse position
    SDL_GetMouseState(&mpos.x, &mpos.y);
}

void PlayScene::update()
{
	u_timer();
	u_timecheck();
	u_collision();
    u_holes();
	u_activateDur();
	u_activateHoles();
}

void PlayScene::u_timer() {
	tmr_game = SDL_GetTicks();
}

void PlayScene::u_timecheck() {
	if( (GAME_DUR - tmr_game) / 1000 <= 0) {
		isGameOver = true;
		gameOverMessage = "Time's up!";
	}
}

void PlayScene::u_collision() {
	
    for (HoleSprite* hole : holeSprites)
    {
        bool collided = isPointCollide(mpos, hole->getRect());
        if (collided && mMouseClicked)
        {
            bool isWhacked = hole->whack();
            mMouseClicked = false;

            if (isWhacked)
            {
				switch(hole->getType()) {
					case HT_Goon:
						score++;
						break;
					case HT_Townie:
						score -= SCOR_PENALTY;
						break;
					case HT_Mayor:
						isGameOver = true;
						gameOverMessage = "You hit the Mayor!\n";
						break;
					default:
						printf("Warning: default case has been reached for collision check.\n");
						break;
				}
            }

            // neg score check
            if (score < 0)
                score = 0;
        }
    }
	
}

void PlayScene::u_holes() {
    for (HoleSprite* hole : holeSprites)
    {
        hole->update();
    }
}

void PlayScene::u_activateDur() {
	// Update dur_activateHole as time goes on.
	int now = SDL_GetTicks();
	bool timesUp = now - tmr_upd_durActv > DUR_UPD_DURACTV;
	bool aboveMin = dur_activateHole > MIN_DURACTV_VAL;
	
	if(timesUp && aboveMin) {
		tmr_upd_durActv = now;
		
		dur_activateHole -= 800;
		printf("Debug: dur_activateHole is %d.\n", dur_activateHole);
	}
	
	
}

void PlayScene::u_activateHoles() {
	int now = SDL_GetTicks();
	bool timesUp = now - tmr_activateHole > dur_activateHole;
	
	if(timesUp) {
		tmr_activateHole = now;
		for(HoleSprite* hole : holeSprites) {
			
			bool selectThis = rand() & 1; // choose either 0 or 1
			bool holeIsResting = hole->getAnimState() == AS_Resting;
			
			if(selectThis && holeIsResting) {
				// Awake hole
				HoleType ht = (HoleType) pick_holeType();
				hole->awake(ht);
				break;
			}
			
		}
	}
}

void PlayScene::draw(SDL_Renderer *renderer) {
	// Colors
	SDL_Color BLACK = {0, 0, 0, 255};
	SDL_Color WHITE = {255, 255, 255, 255};
	SDL_Color BG_COLOR = BLACK;
	
    SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
    SDL_RenderClear(renderer);
	
	draw_bg(renderer);
	draw_holes(renderer);
	draw_texts(renderer);

    SDL_RenderPresent(renderer);
}

void PlayScene::draw_texts(SDL_Renderer* renderer) {
	// Colors
	SDL_Color WHITE = {255, 255, 255, 255};
	SDL_Color RED = {255, 0, 0, 255};
	
	// Anchor points, offsets
	const int txtCenterW = WINDOW_WIDTH / 2;
    const int offsetX = 16;
	
	// Messages
	int timeLeft = (GAME_DUR - tmr_game) / 1000;
	std::string timeMessage = std::to_string(timeLeft) + "s";
	std::string scoreMessage = std::to_string(score);
	
	// Time message color
	SDL_Color timeColor = WHITE;
	int critTimeThreshold = 10;
	bool critTime = ((GAME_DUR - tmr_game) / 1000) <= critTimeThreshold;
	if(critTime) { timeColor = RED; }
	
	// Draw texts
    drawText(renderer, "SCORE", gFont, txtCenterW - 64, 42, WHITE, true);
    drawText(renderer, scoreMessage.c_str(), gFont, txtCenterW - 64, 64, WHITE, true);
    drawText(renderer, "TIME", gFont, txtCenterW + 64, 42, timeColor, true);
    drawText(renderer, timeMessage.c_str(), gFont, txtCenterW + 64, 64, timeColor, true);
}

void PlayScene::draw_holes(SDL_Renderer* renderer) {
    for (HoleSprite* hole : holeSprites)
    {
        hole->draw(renderer);
    }
}

void PlayScene::draw_bg(SDL_Renderer* renderer) {
	SDL_RenderCopy(renderer, bgTexture, NULL, NULL);
}

int PlayScene::pick_holeType() {
	// Pick hole type
	std::vector<HoleType> bag;
	int wgt_goon = 85;
	int wgt_town = 10;
	int wgt_mayr = 5;
	
	for(int i=0; i < wgt_goon; i++) {
		bag.push_back(HT_Goon);
	}
	for(int i=0; i < wgt_town; i++) {
		bag.push_back(HT_Townie);
	}
	for(int i=0; i < wgt_mayr; i++) {
		bag.push_back(HT_Mayor);
	}
	
	std::random_shuffle(bag.begin(), bag.end());
	int pick = rand() % bag.size() + 0;
	
	return bag[pick];
}

/* -------------------------------------------------
 * Game over scene def
 * -------------------------------------------------
 */
 
 GameOverScene::GameOverScene(SceneContext* context) {
	mContext = context;
	mMouseClicked = false;
 }
 
 GameOverScene::~GameOverScene() {
	printf("Deleted game over scene.\n");
 }
 
 void GameOverScene::handleEvents(SDL_Event* e) {
	// Handle events --------------------------------------
    while (SDL_PollEvent(e))
    {
        if (e->type == SDL_QUIT)
        {
            mContext->quit();
        }
        else if (e->type == SDL_MOUSEBUTTONDOWN)
        {
            mMouseClicked = true;
        }
        else if (e->type == SDL_MOUSEBUTTONUP)
        {
            mMouseClicked = false;
        }
		
		else if(e->type == SDL_KEYDOWN) {
			switch(e->key.keysym.sym) {
			case SDLK_RETURN:
				mContext->changeScene(MENU_SCENE);
				break;
			default:
				break;
			}
		}
    }

 }
 
 void GameOverScene::update() {

 }
 
 void GameOverScene::draw(SDL_Renderer* renderer) {
	// Set render draw color, and clear renderer ----------
    SDL_SetRenderDrawColor(renderer, 5, 10, 10, 255);
    SDL_RenderClear(renderer);
	
	drawText(renderer, "GAME OVER", gFont, 0, 0, {255, 255, 255});
	drawText(renderer, "PRESS ENTER TO GO TO MENU", gFont, 0, 64, {255, 255, 255});
	
	// Render 
    SDL_RenderPresent(renderer);
 }
 
 
 
