#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include <stdio.h>
#include <filesystem>
#include <vector>

using namespace std;

// Screen dimension constants
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

// global variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// Texture wrapper class (Credit to LazyFoo)
// class Texture {
//     private:
// 		SDL_Texture* texture;
// 	public:
// 		Texture() {
//             texture = NULL;
//         }


// 		~Texture() {
//             free();
//         }

// 		bool loadFromFile(string path) {
//             free();
//             texture = IMG_LoadTexture(renderer, path.c_str());
//             return texture != NULL;
//         }

// 		void free() {
//             if (texture != NULL) {
//                 SDL_DestroyTexture(texture);
//                 texture = NULL;
//             }
//         }

// 		void setColor(Uint8 red, Uint8 green, Uint8 blue) {
//             SDL_SetTextureColorMod(texture, red, green, blue );
//         }

// 		void setBlendMode(SDL_BlendMode blending) {
//             SDL_SetTextureBlendMode( texture, blending );
//         }

// 		void setAlpha( Uint8 alpha ) {
//             SDL_SetTextureAlphaMod( mTexture, alpha );
//         }
		
// 		//Renders texture at given point
// 		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE ) {

//         }

// 		//Gets image dimensions
// 		int getWidth();
// 		int getHeight();
// };


class Particle {
    public:
    float ox, oy, x, y;
    int type;
    int t = 0;
    Particle(float x, float y) {
        this->x = this->ox = x;
        this->y = this->oy = y;
    }
    
    void update() {}
};

class LinearParticle : Particle {
    public:
    void update() {
        t++;

        x = ox * t;
        y = oy * t;
    }
};

struct ParticleManager {
    string texturePath;
    SDL_Texture* texture = NULL;

    vector<Particle> particles;

    void initialize() {
        texturePath = filesystem::current_path().string() + "\\assets\\particle.png";
        texture = IMG_LoadTexture(renderer, texturePath.c_str());
    }

    void push(Particle& p) {
        particles.push_back(p);
    }

    bool inScreen(float x, float y) {
        // TODO: more complex as it need to compare each particle type source rect
        return x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT;
    }

    void update() {
        vector<Particle> newParticles;
        for (auto& p : particles) {
            p.update();
            if (inScreen(p.x, p.y)) {
                newParticles.push_back(p);
            }
        }
        particles = newParticles;

    }

} particleManager;
struct Player {
    string texturePath;
    SDL_Texture* texture = NULL;

    SDL_Rect srcRect[2][8];
    SDL_Rect dstRect;

    #define DELTA_DELAY 60
    int sprite_row = 0;
    int sprite_col = 0;
    int sprite_delta = DELTA_DELAY;
    int idle = 0, movingRight = 0, movingLeft = 0;

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

    #define TEXTURE_WIDTH 32
    #define TEXTURE_HEIGHT 48

    void initialize() {
        texturePath = filesystem::current_path().string() + "\\assets\\marisa.png";
        texture = IMG_LoadTexture(renderer, texturePath.c_str());

        // destination rectangle
        dstRect.w = TEXTURE_WIDTH;
        dstRect.h = TEXTURE_HEIGHT;

        // source rectangle
        for (int i = 0, y = 25; i < 2; ++i) {
            int x = 868;
            for (int j = 0; j < 8; ++j) {
                srcRect[i][j].w = TEXTURE_WIDTH;
                srcRect[i][j].h = TEXTURE_HEIGHT;
                srcRect[i][j].x = x;
                srcRect[i][j].y = y;
                x += TEXTURE_WIDTH;
            }
            y += TEXTURE_HEIGHT;
        }

    }

    void destroy() {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }

    void render() {
        SDL_RenderCopyEx(renderer, texture, &srcRect[!idle][sprite_col], &dstRect, 0.0, NULL, movingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
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

        if (pressed[KEY_LEFT]) {
            if (!movingLeft) {
                sprite_row = 1;
                sprite_col = 0;
                sprite_delta = DELTA_DELAY;
                idle = 0;
                movingLeft = 1;
                movingRight = 0;
            }
            moveLeft();
        } else if (pressed[KEY_RIGHT]) {
            if (!movingRight) {
                sprite_row = 2;
                sprite_col = 0;
                sprite_delta = DELTA_DELAY;
                idle = 0;
                movingLeft = 0;
                movingRight = 1;
            }
            moveRight();
        } else if (!idle) {
            sprite_row = 0;
            sprite_col = 0;
            sprite_delta = DELTA_DELAY;
            idle = 1;
            movingLeft = 0;
            movingRight = 0;
        }

        dstRect.x = this->x;
        dstRect.y = this->y;

        sprite_delta--;
        if (sprite_delta == 0) {
            sprite_delta = DELTA_DELAY;
            sprite_col++;
            if (idle) {
                if (sprite_col == 4) sprite_col = 0;
            } else {
                if (sprite_col == 8) sprite_col = 2;
            }
        }

        // shoot
        if (pressed[KEY_SHOOT]) {
            Particle bullet(x, y);
            particleManager.push(bullet);
        }
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
    float scrollingOffset = 0;

	Program() = default;
	bool initialize() {
        cout << "Initializing..\n";
		if(SDL_Init(SDL_INIT_VIDEO) < 0) {
			cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << '\n';
			return 0;
		}

        window = SDL_CreateWindow("Touhou - By Shora", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
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
        auto backgoundPath = filesystem::current_path().string() + "\\assets\\background.png";
        backgound = IMG_LoadTexture(renderer, backgoundPath.c_str());
        player.initialize();


		return 1;
	}

    void update() {
        scrollingOffset += 0.01;
        if (scrollingOffset >= WINDOW_HEIGHT) {
            scrollingOffset = 0;
        }

        player.update();

        particleManager.update();
    }

    void render() {
        SDL_RenderClear(renderer);

        // render bg
        SDL_Rect srcRect, dstRect; 
        srcRect.x = 306, srcRect.y = 25, srcRect.w = srcRect.h = 256;

        dstRect.x=0, dstRect.y=scrollingOffset, dstRect.w=WINDOW_WIDTH, dstRect.h=WINDOW_HEIGHT;
        SDL_RenderCopy(renderer, backgound, &srcRect, &dstRect);
        dstRect.x=0, dstRect.y=-WINDOW_HEIGHT + scrollingOffset, dstRect.w=WINDOW_WIDTH, dstRect.h=WINDOW_HEIGHT;
        SDL_RenderCopy(renderer, backgound, &srcRect, &dstRect);

        // render player
        player.render();

        // render opp
        SDL_RenderPresent(renderer);
    }

	void loop() {
		SDL_Event e; 
		bool quit = false; 
        
        uint32_t a, b;
        a = b = SDL_GetTicks();
        double delta = 0;
		while (!quit) { 

            a = SDL_GetTicks();
            delta = b - a;

            if (delta > 1000 / 60.0) {
                cout << "FPS: " << 1000 / delta << '\n';
                b = a;

                while (SDL_PollEvent(&e)) { 
                    if (e.type == SDL_QUIT) quit = true;
                    
                    player.handle_input(e);
                    
                }	

                update();
                render();

            }
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
