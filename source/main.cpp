/*
* Whack-A-Goon!
* An SDL video game developed by Francis Tayag (zyenapz)
* The source code for this game is available on my GitHub page
* and is licensed under the GPLv3 license.
* My GitHub Page: github.com/zyenapz
* My E-mail: zyenapz@gmail.com
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "ZRect.h"
#include "Scenes.h"
#include "metadata.h"

// Function prototypes
bool initialize(SDL_Window* window, SDL_Renderer* renderer);
void close(SDL_Window* window, SDL_Renderer* renderer);

int main(int argv, char** args){
    // Declare window and renderer pointers
    SDL_Window* gWindow = NULL;
    SDL_Renderer* gRenderer = NULL;

    // Initialize SDL, window, renderer, and SDL_image
    if(!initialize(gWindow, gRenderer)) {
        printf("Game failed to initialize. :(\n");
    }
    else {
        // Construct Scene context
        SceneContext gContext;

        // Game loop
        SDL_Event e;

        while(gContext.isExited()) {

            // Run context methods
            gContext.HandleEvents(&e);
            gContext.Update();
            gContext.Draw(gRenderer);

        }
    }

    // Close SDL
    close(gWindow, gRenderer);

    return 0;
}

bool initialize(SDL_Window* window, SDL_Renderer* renderer) {
    // - Initializes SDL, window, renderer, and SDL_image
    bool isSuccessful = true;

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL failed to initialize. SDL Error: %s\n", SDL_GetError());
        isSuccessful = false;
    }
    else {
        // Initialize Window
        window = SDL_CreateWindow(GAME_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == NULL) {
            printf("SDL could not initialize window. SDL Error: %s\n", SDL_GetError());
            isSuccessful = false;
        }
        else {
            // Initialize Renderer
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if(renderer == NULL) {
                printf("SDL could not initialize renderer. SDL Error: %s\n", SDL_GetError());
                isSuccessful = false;
            }
            else {
                // Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init(imgFlags) & imgFlags)) {
                    printf("SDL_image could not initialize. SDL_image Error: %s\n", IMG_GetError());
                    isSuccessful = false;
                }
            }
        }
    }

    return isSuccessful;
}

void close(SDL_Window* window, SDL_Renderer* renderer) {
    // - Closes SDL

    // Destroy window
    SDL_DestroyWindow(window);
    window = NULL;

    // Destroy renderer
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    // Quit SDL_image, and SDL
    IMG_Quit();
    SDL_Quit();
}
