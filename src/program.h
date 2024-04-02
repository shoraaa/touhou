#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include <stdio.h>
#include <filesystem>

using namespace std;

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// global variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

struct Button {

};

struct Player {
    SDL_Texture* texture = NULL;

	const float DELTA_X = 0.2;
	const float DELTA_Y = 0.2;
	float x = 0, y = 0;
    int pressed[8] = { 0 };
    #define KEY_UP 0
    #define KEY_DOWN 1
    #define KEY_RIGHT 2
    #define KEY_LEFT 3
    #define KEY_SHOOT 4
    #define KEY_BOMB 5

    void initialize() {
        auto path = filesystem::current_path().string() + "\\assets\\player.png";
        texture = IMG_LoadTexture(renderer, path.c_str());
    }

    void destroy() {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }

    void render() {
        
        SDL_Rect srcrect;
        srcrect.x = 3;
        srcrect.y = 2;
        srcrect.w = 28;
        srcrect.h = 43;

        SDL_Rect dstrect;
        dstrect.x = this->x;
        dstrect.y = this->y;
        dstrect.w = 28;
        dstrect.h = 43;

        SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
    }

    void handle_input(const SDL_Event& e) {

        const auto key = e.key.keysym.sym; 
        int is_down = -1;  
        switch (e.type) {
            case SDL_KEYDOWN: is_down = 1; break;
            case SDL_KEYUP: is_down = 0; break;
            default: break;
        }
        if (is_down == -1) return;

        int button = -1;
        switch (key) {
            case SDLK_UP: button = KEY_UP; break;
            case SDLK_DOWN: button = KEY_DOWN; break;
            case SDLK_RIGHT: button = KEY_RIGHT; break;
            case SDLK_LEFT: button = KEY_LEFT; break;
            case SDLK_x: button = KEY_SHOOT; break;
            case SDLK_z: button = KEY_BOMB; break;
            case SDLK_q: { printf("Quit\n"); exit(0); }
            default: break;
        }

        if (button == -1) return;
        pressed[button] = is_down;

    }

	void update() {

        if (pressed[KEY_UP]) moveUp();
        else if (pressed[KEY_DOWN]) moveDown();

        if (pressed[KEY_LEFT]) moveLeft();
        else if (pressed[KEY_RIGHT]) moveRight();

        // shoot
	}

	void moveUp() {
		y -= DELTA_Y;
	}

	void moveDown() {
		y += DELTA_Y;
	}

	void moveRight() {
		x += DELTA_X;
	}

	void moveLeft() {
		x -= DELTA_X;
	}
	
};

struct Program {
    SDL_Texture* backgound = NULL;

    SDL_Surface* screenSurface = NULL;

    Player player;

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

        //Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;
        if(!(IMG_Init(imgFlags) & imgFlags)){
            printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
            return 0;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED );
        if(renderer == NULL) {
            cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << '\n';
            return 0;
        }

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        screenSurface = SDL_GetWindowSurface(window);
        
        // game init
        auto backgoundPath = filesystem::current_path().string() + "\\assets\\background.jpg";
        backgound = IMG_LoadTexture(renderer, backgoundPath.c_str());
        player.initialize();


		return 1;
	}

    void update() {
        player.update();

        // TODO: update scrolling effect on bg
    }

    void render() {
        SDL_RenderClear(renderer);

        // render bg
        SDL_RenderCopy(renderer, backgound, NULL, NULL);

        // render player
        player.render();

        // render opp
        SDL_RenderPresent(renderer);
    }

	void loop() {
		SDL_Event e; 
		bool quit = false; 
        
		while (!quit) { 
			while (SDL_PollEvent(&e)) { 
				if (e.type == SDL_QUIT) quit = true;
				
                player.handle_input(e);
                
			}	

            update();
            render();
		}

	}


	void close() {
        player.destroy();

        SDL_DestroyTexture(backgound);
        backgound = NULL;
        IMG_Quit();
 
        SDL_DestroyRenderer(renderer);
        renderer = NULL;

		SDL_DestroyWindow(window);
        window = NULL;

		SDL_Quit();
	}

};
