#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <iostream>
#include <stdio.h>
#include <filesystem>
#include <vector>
#include <random>
#include <string>

#include "audio.h"
#include "player.h"
#include "enemy.h"
#include "particle.h"

using namespace std;

int PLAYER_LOST = 0;

// global variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;



Player player;

struct ParticleManager {
    string texturePath;
    Texture texture;
    int delta = 30;

    SDL_Rect srcRect;

    vector<unique_ptr<Particle>> particles;
    #define PARTICLE_WIDTH 16
    #define PARTICLE_HEIGHT 16

    void initialize() {
        texture.load("particle");

        srcRect.x = 322, srcRect.y = 57, srcRect.w = PARTICLE_WIDTH, srcRect.h = PARTICLE_HEIGHT;
    }

    void push(Vec2d pos, int radius, int velocity) {
        Vec2d position = pos;
        Vec2d direction = player.position - position;
        direction = direction * (1.0 / direction.length());

        unique_ptr<LinearParticle> particle = make_unique<LinearParticle>(position, direction, radius, velocity);
        particles.emplace_back(move(particle));
    }

    void update() {
        vector<unique_ptr<Particle>> newParticles;
        for (auto& particle : particles) {
            particle->update();
            if (particle->inBound() && !particle->hit) {
                newParticles.emplace_back(move(particle));
            }
        }
        particles = move(newParticles);

        for (auto& particle : particles) {
            if (particle->collide(player.position)) {
                PLAYER_LOST = 1;
            }
        }

    }

    void render() {
        for (auto& particle : particles) {
            texture.render(particle->position.x - PARTICLE_WIDTH / 2, particle->position.y - PARTICLE_HEIGHT / 2, &srcRect);
        }
    }

} particleManager;

EnemyManager enemyManager;
struct Scene_Gameplay {

    Texture background, foreground;
    BGM bgm;

    double scrollingOffset = 0;

    void initialize() {
        background.load("background");
        foreground.load("foreground");

        bgm.load("th06_02");
        player.initialize();
        enemyManager.initialize();
        particleManager.initialize();
    }

    void start() {
        cout << "Playing BGM\n";
        bgm.play();
    }

    void handleInput(const SDL_Event& e) {
        player.handleInput(e);
    }

    void update() {
        scrollingOffset += 0.5;
        if (scrollingOffset >= WINDOW_HEIGHT) {
            scrollingOffset = 0;
        }

        player.update();
        enemyManager.update();

        particleManager.update();
    }

    void renderBG() {
        SDL_Rect srcRect, dstRect; 
        srcRect.x = 306, srcRect.y = 25, srcRect.w = srcRect.h = 256;
        dstRect.x=0, dstRect.y=scrollingOffset, dstRect.w=WINDOW_WIDTH, dstRect.h=WINDOW_HEIGHT;
        background.render(&srcRect, &dstRect);
        dstRect.x=0, dstRect.y=-WINDOW_HEIGHT + scrollingOffset, dstRect.w=WINDOW_WIDTH, dstRect.h=WINDOW_HEIGHT;
        background.render(&srcRect, &dstRect);
    }
    
    void renderFG() {
        // render fg
        SDL_Rect srcRect, dstRect; 
        srcRect.x = 121, srcRect.y = 105, srcRect.w = 160, srcRect.h = 16;
        dstRect.x = 0, dstRect.w = 160, dstRect.h = 16;
        for (int i = 0; i < 4; ++i, dstRect.x += dstRect.w) {
            dstRect.y = 0; foreground.render(&srcRect, &dstRect);
            dstRect.y = WINDOW_HEIGHT - 16; foreground.render(&srcRect, &dstRect);
        }
        srcRect.x = 25, srcRect.y = 249, srcRect.w = 32, srcRect.h = 32;
        dstRect.y = 16, dstRect.w = 32, dstRect.h = 32;
        for (int i = 0, j; i < 14; ++i, dstRect.y += dstRect.h) {
            dstRect.x = 0; foreground.render(&srcRect, &dstRect);
            for (dstRect.x = dstRect.w * 13, j = 0; j < 7; ++j, dstRect.x += dstRect.w) {
                foreground.render(&srcRect, &dstRect);
            }
        }

        // render text 25, 169, 48, 16
        srcRect.x = 25, srcRect.y = 201, srcRect.w = 48, srcRect.h = 16;
        dstRect.x = 16 * 27, dstRect.y = 16 * 8, dstRect.w = 48, dstRect.h = 16;
        foreground.render(&srcRect, &dstRect);
        srcRect.x = 57, srcRect.y = 265, srcRect.w = 16, srcRect.h = 16;
        dstRect.x += dstRect.w + 8; dstRect.w = 16, dstRect.h = 16;
        for (int i = 0; i < player.lives; ++i, dstRect.x += 16 + 8) {
            foreground.render(&srcRect, &dstRect);
        }

        srcRect.x = 25, srcRect.y = 201 - 16, srcRect.w = 48, srcRect.h = 16;
        dstRect.x = 16 * 27, dstRect.y = 16 * 8 - 32, dstRect.w = 48, dstRect.h = 16;
        foreground.render(&srcRect, &dstRect);


    }

    void render() {
        renderBG();

        player.render();
        enemyManager.render();
        particleManager.render();

        renderFG();
    }
};


struct Scene_MainMenu {
    BGM bgm;

    Texture background;
    SDL_Rect srcRect = {690, 530, 640, 480}, dstRect = {0, 0, 650, 480};

    int gameStarted = 0;

    void initialize() {
        background.load("title");
        bgm.load("th06_01");

        bgm.play();
    }

    void handleInput(const SDL_Event& e) {
        const auto key = e.key.keysym.sym; 
        if (key == SDLK_RETURN) {
            gameStarted = 1;
            bgm.stop();
            cout << "Game Start!\n";
        }
    }

    void update() {

    }

    void render() {
        background.render(&srcRect, &dstRect);
    }
};
struct Program {
    Scene_MainMenu menu;
    Scene_Gameplay game;
    int currentScene = 0;

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

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if(renderer == NULL) {
            cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << '\n';
            return 0;
        }

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        menu.initialize();
        game.initialize();

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
        SDL_RenderPresent(renderer);
    }

	void loop() {
		SDL_Event e; 
		bool quit = false; 
        
		while (!quit) { 
            
            while (SDL_PollEvent(&e)) { 
                if (e.type == SDL_QUIT) quit = true;
                
                handleInput(e);
                
            }	

            if (PLAYER_LOST) continue;

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
