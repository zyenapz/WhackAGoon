/*
 * Whack-A-Goon!
 * Author: zyenapz
 * The source code for this game is available on my GitHub page
 * and is licensed under the GPLv3 license.
 * My GitHub Page: github.com/zyenapz
 * My E-mail: zyenapz@gmail.com
 */

#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "scenes.h"
#include "metadata.h"
#include "assets.h"
#include "utils.h"
#include "frames.h"

// Window and renderer
SDL_Window* gWindow;
SDL_Renderer* gRenderer;

// Font
TTF_Font* gFont;
TTF_Font* gFontM;
TTF_Font* gFontL;

// Textures
SDL_Texture* bgTexture;
SDL_Texture* spritesTexture;
SDL_Texture* btnTexture;

// Game Context
SceneContext* gContext;

bool initialize();
bool loadAssets();
void cleanUp();

int main(int argv, char** args) {
    // Initialize and load assets
    bool isInitialized = initialize();
    bool hasLoadedAssets = loadAssets();

    // Seed rng
    srand(time(0));

    // Disable cursor
    SDL_ShowCursor(SDL_ENABLE);

    // Game loop
    if (isInitialized && hasLoadedAssets)
    {
        SDL_Event e;
		
		// Create game context
		gContext = new SceneContext(MENU_SCENE);
		if (gContext == NULL) {
			printf("Failed to initialize the game context.\n");
		}

        while (gContext->get_scene() != NULL && gContext != NULL)
        {
            SDL_Delay(MAX_FPS);
            gContext->HandleEvents(&e);
            gContext->Update();
            gContext->Draw(gRenderer);
        }

    }

    // Clean up
    cleanUp();
	
	getchar();
    return 0;
}

bool initialize() {
    bool isSuccessful = true;

    // Initialize SDL subsystems
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL failed to initialize. Error: %s.\n", SDL_GetError());
        isSuccessful = false;
    }
    else
    {
        // Create window
        gWindow = SDL_CreateWindow(GAME_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL)
        {
            printf("SDL failed to create window. Error: %s.\n", SDL_GetError());
            isSuccessful = false;
        }
        else
        {
            // Create renderer
            int rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
            gRenderer = SDL_CreateRenderer(gWindow, -1, rendererFlags);
            if (gRenderer == NULL)
            {
                printf("SDL failed to create renderer. Error: %s.\n", SDL_GetError());
                isSuccessful = false;
            }
            else
            {
                // Initialize SDL_image
                int flags = IMG_INIT_JPG | IMG_INIT_PNG;
                if ((IMG_Init(flags) & flags) != flags)
                {
                    printf("SDL_image failed to initialize. Error: %s.\n", IMG_GetError());
                    isSuccessful = false;
                }

                // Initialize SDL_ttf
                if (TTF_Init() < 0)
                {
                    printf("SDL_ttf failed to initialize. Error: %s.\n", TTF_GetError());
                    isSuccessful = false;
                }
            }
        }
    }

    return isSuccessful;
}

bool loadAssets() {
    bool isSuccessful = true;

    // Load frame data
    loadFrameData();

    // Set window icon
    SDL_Surface *iconSurface = IMG_Load("assets/images/icon.png");
    SDL_SetWindowIcon(gWindow, iconSurface);
    SDL_FreeSurface(iconSurface);
    iconSurface = NULL;

    // Load font
    gFont = TTF_OpenFont("assets/fonts/zyn8.ttf", 25);
    if (gFont == NULL)
    {
        printf("Failed to load font.\n");
        isSuccessful = false;
    }
	
	gFontM = TTF_OpenFont("assets/fonts/zyn8.ttf", 31);
	if(gFontM == NULL) {
		printf("Failed to load gFontM.\n");
		isSuccessful = false;
	}
	
	gFontL = TTF_OpenFont("assets/fonts/zyn8.ttf", 41);
	if(gFontL == NULL) {
		printf("Failed to load gFontL.\n");
		isSuccessful = false;
	}

    // Load textures
    bgTexture = loadTextureFromFile(gRenderer, "assets/images/bg.png");
    if (bgTexture == NULL)
    {
        printf("Failed to load bgTexture.\n");
        isSuccessful = false;
    }

    spritesTexture = loadTextureFromFile(gRenderer, "assets/images/sprites.png");
    if (spritesTexture == NULL)
    {
        printf("Failed to load spritesTexture.\n");
        isSuccessful = false;
    }
	
	btnTexture = loadTextureFromFile(gRenderer, "assets/images/button.png");
	if(btnTexture == NULL) {
		printf("Failed to load btnTexture.\n");
		isSuccessful = false;
	}

    return isSuccessful;
}

void cleanUp() {	
	// Destroy font
	TTF_CloseFont(gFont);	
	gFont = NULL;
	
    // Clean up textures
    cleanUpTexture(bgTexture);
    cleanUpTexture(spritesTexture);
	cleanUpTexture(btnTexture);

    // Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    // Destroy renderer
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
	
	// Clean up game context
	delete gContext;
	gContext = NULL;

    // Quit SDL subsystems
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
