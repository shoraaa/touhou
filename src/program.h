#pragma once

#include "scene.h"
#include "const.h"


// global variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int PLAYER_LOST = 0;

int elapsedFrame = 0;
double scrollingSpeed = 0.5;

Player player;
ParticleManager particleManager;
EnemyManager enemyManager;
int current_channel = 0;
#define TOTAL_CHANNEL 64

struct Program {
    Scene_MainMenu menu;
    Scene_Gameplay game;
    int currentScene = 0;
    double fps;
    Texture font;

	bool initialize() {
        cout << "Initializing..\n";
		if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
			cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << '\n';
			return 0;
		}

        window = SDL_CreateWindow("Touhou - By Shora", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            cout << "Window could not be created! SDL_Error: " << SDL_GetError() << '\n';
            return 0;
        }

        // Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;
        if(!(IMG_Init(imgFlags) & imgFlags)){
            printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
            return 0;
        }
        // Init AUdio
        if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0) {
            printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
            return 0;
        }
        Mix_VolumeMusic(32);
        Mix_MasterVolume(8);
        Mix_AllocateChannels(TOTAL_CHANNEL);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if(renderer == NULL) {
            cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << '\n';
            return 0;
        }

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        menu.initialize();
        game.initialize();
        font.load("font");

		return 1;
	}

    void handleInput(const SDL_Event& e) {
        if (currentScene == 0) {
            menu.handleInput(e);
        } else {
            game.handleInput(e);
        }
    }

    void update() {
        if (currentScene == 0) {
            menu.update();
        } else {
            game.update();
        }

        if (menu.gameStarted == 1) {
            menu.gameStarted = 2;
            currentScene = 1;
            game.start();
        }
    }

    void render() {
        SDL_RenderClear(renderer);
        if (currentScene == 0) {
            menu.render();
        } else {
            game.render();
        }
        renderFPS();
        SDL_RenderPresent(renderer);
    }

    void renderFPS() {
        SDL_Rect clip = {25, 73, 16, 16};
        SDL_Rect dstRect = {8 * 58, WINDOW_HEIGHT - 16, 16, 16};

        clip.x = 25 + 16 * (int(fps / 10) % 10);
        font.render(&clip, &dstRect);
        dstRect.x += 16;

        clip.x = 25 + 16 * (int(fps) % 10);
        font.render(&clip, &dstRect);
        dstRect.x += 16;

        // dot :)
        clip.x = 25 + 16 * 14, clip.y -= 16;
        font.render(&clip, &dstRect);
        dstRect.x += 16, clip.y += 16;

        clip = {25, 73, 16, 16};
        clip.x = 25 + 16 * (int(fps * 10) % 10);
        font.render(&clip, &dstRect);
        dstRect.x += 16;

        clip.x = 25 + 16 * (int(fps * 100) % 10);
        font.render(&clip, &dstRect);
        dstRect.x += 16;
        
        dstRect.x += 16;

        // FPS
        clip.y += 16 * 3;

        clip.x = 25 + 16 * 6;
        font.render(&clip, &dstRect);
        dstRect.x += 16;

        clip.x = 25; clip.y += 16;
        font.render(&clip, &dstRect);
        dstRect.x += 16;

        clip.x = 25 + 16 * 3; 
        font.render(&clip, &dstRect);
        dstRect.x += 16; 

    }

	void loop() {
		SDL_Event e; 
		bool quit = false; 

        unsigned int currentTick = 0;

        double desiredDelta = 1000.0 / 60, delta;
        double fpsDelay = 1000.0 / 30;
        
		while (!quit) { 

            delta = SDL_GetTicks() - currentTick;
            if (delta <= desiredDelta) {
                continue;
            }
            currentTick = SDL_GetTicks();
            fps = 1000.0 / delta;
            
            while (SDL_PollEvent(&e)) { 
                if (e.type == SDL_QUIT) quit = true;
                
                handleInput(e);
                
            }	

            update();
            render();
		}

	}


	void close() {
        IMG_Quit();
 
        SDL_DestroyRenderer(renderer);
        renderer = NULL;

		SDL_DestroyWindow(window);
        window = NULL;

		SDL_Quit();
	}

};
