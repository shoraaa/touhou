#include <SDL.h>
#include <SDL_image.h>
#include "game.h"

#include <iostream>
#include <stdio.h>
using namespace std;

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct Program {
	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;

	Game game;

	Program() = default;
	bool initialize() {
        cout << "Initializing..\n";
		if(SDL_Init(SDL_INIT_VIDEO) < 0) {
			cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << '\n';
			return 0;
		}

        window = SDL_CreateWindow("Touhou - By Shora", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            cout << "Window could not be created! SDL_Error: " << SDL_GetError() << '\n';
            return 0;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED );
        if(renderer == NULL) {
            cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << '\n';
            return 0;
        }

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        //Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;
        if(!(IMG_Init(imgFlags) & imgFlags)){
            cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << '\n';
            return 0;
        }

        screenSurface = SDL_GetWindowSurface(window);
        game.initialize();
		return 1;
	}

	void loop() {
		SDL_Event e; 
		bool quit = false; 
		while (!quit) { 
			while (SDL_PollEvent(&e)) { 
				if (e.type == SDL_QUIT) quit = true;
				game.loop(e);

                if (texture == NULL) {
                    IMG_LoadTexture(renderer, "1.jpg");
                }

                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);
			}	
		}

	}


	void close() {
        SDL_DestroyTexture(texture);
        texture = NULL;
 
        SDL_DestroyRenderer(renderer);
        renderer = NULL;

		SDL_DestroyWindow(window);
        window = NULL;

		SDL_Quit();
	}

};
