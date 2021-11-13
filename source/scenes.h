#ifndef SCENES_H
#define SCENES_H
#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <map>
#include "sprites.h"
#include "widgets.h"
#include "metadata.h"
#include "assets.h"

enum SceneID
{
	MENU_SCENE,
	PLAY_SCENE,
	GAMEOVER_SCENE,
	DEBUG_SCENE
};

enum PlaySceneState
{
	PS_PAUSED,
	PS_WARMUP,
	PS_RUNNING,
	PS_GAMEOVER
};

class SceneContext;

class AbstractScene
{
protected:
	SceneContext *m_context;

public:
	virtual ~AbstractScene() = default;

	virtual void initialize() = 0;
	virtual void destroy() = 0;

	virtual void handleEvents(SDL_Event *e) = 0;
	virtual void update() = 0;
	virtual void draw(SDL_Renderer *renderer) = 0;
};

class SceneContext
{
public:
	GameInformation gInfo;

	SceneContext(SceneID scene = MENU_SCENE);
	~SceneContext();
	void changeScene(SceneID scene);
	void quit();
	AbstractScene *get_scene() { return pScene; };

	void HandleEvents(SDL_Event *e);
	void Update();
	void Draw(SDL_Renderer *renderer);
	void sceneSwitch(SceneID scene);

private:
	AbstractScene *pScene;
};

class MenuScene : public AbstractScene
{
public:
	MenuScene(SceneContext *context);
	~MenuScene();

	void initialize();
	void destroy();

	void handleEvents(SDL_Event *e);
	void update();
	void draw(SDL_Renderer *renderer);

	void cb_playGame();
	void cb_help();
	void cb_info();

private:
	void createButtons();

	// Buttons
	std::vector<Button *> m_buttons;

	// Context
	//SceneContext *mContext;

	// Mouse
	MouseState z_mouse;
};

// class PlayScene : public AbstractScene
// {
// public:
// 	PlayScene(SceneContext *mContext);
// 	~PlayScene();

// 	void initialize();
// 	void destroy();

// 	void handleEvents(SDL_Event *e);
// 	void update();
// 	void draw(SDL_Renderer *renderer);

// private:
// 	// update() sub-methods
// 	void u_initTransitionTimer();
// 	void u_checkDeathTimer();
// 	void u_checkopinion();
// 	void u_wuTimer();
// 	void u_transgameover();
// 	void u_collision();
// 	void u_holes();
// 	void u_activateDur();
// 	void u_activateHoles(bool isForced = false, HoleType forcedType = HT_None);
// 	void u_spawnPrt();
// 	void u_prt();
// 	void u_uistuff();
// 	void u_fadetexts();
// 	void u_buttons();

// 	// draw() sub-methods
// 	void draw_texts(SDL_Renderer *renderer);
// 	void draw_holes(SDL_Renderer *renderer);
// 	void draw_bg(SDL_Renderer *renderer);
// 	void draw_deathTimer(SDL_Renderer *renderer);
// 	void draw_prt(SDL_Renderer *renderer, SDL_Texture *parentTargTexture);
// 	void draw_city(SDL_Renderer *renderer);
// 	void draw_uistuff(SDL_Renderer *renderer);
// 	void draw_fadetexts(SDL_Renderer *renderer);
// 	void draw_buttons(SDL_Renderer *renderer);

// 	// helper methods
// 	void mk_holes();
// 	int pick_holeType();
// 	void ch_gstate(PlaySceneState n_state);
// 	void delayDeathCdown();
// 	void decrDthCdownDur();
// 	void initDthCdown();
// 	void init_uistuff();
// 	void spawnFadeText();
// 	void mk_buttons();
// 	void goback(); // callback for the button

// 	// For screen shake
// 	int tmr_shake;
// 	const int DELAY_SHAKE = 50;
// 	const int MAX_SHAKE = 5;
// 	std::vector<int> vShake; // 'generate' -n and n alternately and store into this vector
// 	void genShake();
// 	void shake();

// 	// Target texture stuff
// 	SDL_Rect targRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

// 	// Context
// 	//SceneContext *mContext;

// 	// Messages
// 	std::string gOverMsg = "DEBUG";

// 	// Mouse
// 	//bool mMouseClicked = false;
// 	//SDL_Point mpos;
// 	MouseState z_mouse;

// 	// Sprite groups, and sprites
// 	std::vector<HoleSprite *> holeSprites;
// 	std::vector<Particle *> m_particles;
// 	std::vector<FadeText> m_fadetexts;
// 	std::vector<Button *> m_buttons;
// 	UI_Icon scoreIcon;
// 	UI_Icon opinionIcon;

// 	// Game state
// 	PlaySceneState m_gstate = PS_WARMUP;

// 	// Game rules
// 	const int SCOR_PENALTY = 10;
// 	const int MAX_GAME_DURATION = 450000; // 1000 ticks is approx 1 sec
// 	int score = 0;
// 	int opinion = 3;
// 	const int DECREMENT_DUR_DEATHCDOWN = 500;
// 	const int MAX_DUR_DEATHCDOWN = 3250;
// 	int dur_deathCdown = MAX_DUR_DEATHCDOWN;
// 	int tmr_deathCdown = 0; // initialized after player hits the first goon

// 	// Durations and timers
// 	int tmr_fspawning;
// 	const int DUR_FSPAWNING = 2000;

// 	int tmr_activateHole;
// 	int dur_activateHole = 3000;

// 	int tmr_progression;
// 	const int DUR_PROGRESSION = 1950;
// 	const int MIN_DURACTV_VAL = 500;
// 	const int DECREMENT_MIN = 100;
// 	const int DECREMENT_MAX = 400;

// 	const int DUR_TRANSTOGAMEOVER = 3000;
// 	int tmr_transtogameover = 0;

// 	const int DUR_WARMUPTIMER = 3500;
// 	int tmr_warmuptimer = SDL_GetTicks();

// 	const int DUR_PRTSPAWN = 500;
// 	int tmr_prtspawn = 0; // init at ch_gstate()

// 	int old_frame = 0; // used at draw_deathTimer()
// };

// class GameOverScene : public AbstractScene
// {
// public:
// 	GameOverScene(SceneContext *context);
// 	~GameOverScene();

// 	void initialize();
// 	void destroy();

// 	void handleEvents(SDL_Event *e);
// 	void update();
// 	void draw(SDL_Renderer *renderer);

// 	// Callbacks
// 	void chs_menu();
// 	void chs_retry();

// private:
// 	//SceneContext *mContext;
// 	std::vector<Button *> m_buttons;

// 	// Mouse
// 	MouseState z_mouse;

// 	int finalScore;
// };
#endif // SCENES_H
