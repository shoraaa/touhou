#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <iostream>
#include <stdio.h>
#include <filesystem>
#include <vector>

using namespace std;

// Screen dimension constants
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

// Play Field dimentsion
const int FIELD_X = 32;
const int FIELD_Y = 16;
const int FIELD_WIDTH = 480;
const int FIELD_HEIGHT = WINDOW_HEIGHT - 16 * 2;

// global variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

class Texture {
    private:
		SDL_Texture* texture = NULL;
	public:
		Texture() {}

		~Texture() {
            free();
        }

		bool load(string name) {
            free();
            const string path = filesystem::current_path().string() + "\\assets\\picture\\" + name + ".png";
            texture = IMG_LoadTexture(renderer, path.c_str());
            return texture != NULL;
        }

		void free() {
            if (texture != NULL) {
                SDL_DestroyTexture(texture);
                texture = NULL;
            }
        }

        void render(SDL_Rect* clip, SDL_Rect* dest) {
            SDL_RenderCopy(renderer, texture, clip, dest);
        }

		void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE) {
            SDL_Rect renderQuad = { x, y, 32, 32 };
            if( clip != NULL ) {
                renderQuad.w = clip->w;
                renderQuad.h = clip->h;
            }
            SDL_RenderCopyEx(renderer, texture, clip, &renderQuad, angle, center, flip );
        }
};


class BGM {
private:
    Mix_Music* music = NULL;
public:
    BGM() {}
    ~BGM() {
        Mix_FreeMusic(music);
        music = NULL;
    }
    void load(string name) {
        string path = filesystem::current_path().string() + "\\assets\\audio\\bgm\\" + name + ".wav";
        music = Mix_LoadMUS(path.c_str());
        if (music == NULL) {
            printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
            exit(1);
        }
    }
    void play() {
        Mix_PlayMusic(music, -1);
    }
    void stop() {
        Mix_HaltMusic();
    }
};
class SE {
private:
    Mix_Chunk* se = NULL;
public:
    SE() {}
    ~SE() {
        Mix_FreeChunk(se);
        se = NULL;
    }
    void load(string name) {
        string path = filesystem::current_path().string() + "\\assets\\audio\\se\\" + name + ".wav";
        se = Mix_LoadWAV(path.c_str());
        if (se == NULL) {
            printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
            exit(1);
        }
    }
    void play() {
        Mix_PlayChannel(-1, se, 0);
    }
};


struct Vec2d {
    double x, y;
    Vec2d(double x = 0, double y = 0): x(x), y(y) {}
    Vec2d operator+ (const Vec2d& a) const {
        return Vec2d(x + a.x, y + a.y);
    }
    Vec2d operator* (double k) const {
        return Vec2d(x * k, y * k);
    }
};

bool inPlayField(const Vec2d& position) {
    return position.x >= FIELD_X && position.x < FIELD_WIDTH && position.y >= FIELD_Y && position.y < FIELD_HEIGHT;
}

class Particle {
    public:
    Vec2d initialPosition, position;
    int elapsedTime = 0;
    int type;
    Particle() = default;
    Particle(Vec2d pos): initialPosition(pos), position(pos) {}

    bool inBound() {
        return inPlayField(position);
    }
    
    virtual void update() {}
};

class LinearParticle : public Particle {
    // vector linear
    public:
    Vec2d direction;
    double velocity = 1.0;
    LinearParticle(Vec2d pos, Vec2d dir, double vel): Particle(pos), direction(dir), velocity(vel) {}
    void update() override {
        position = initialPosition + (direction * velocity * elapsedTime);
        elapsedTime++;
    }
};

struct ParticleManager {
    string texturePath;
    Texture texture;

    vector<Particle> particles;

    void initialize() {
        texture.load("particle");
    }

    void push(Particle& p) {
        particles.push_back(p);
    }

    void update() {
        vector<Particle> newParticles;
        for (auto& particle : particles) {
            particle.update();
            if (particle.inBound()) {
                newParticles.push_back(particle);
            }
        }
        particles = newParticles;

    }

    void render() {

    }

} particleManager;

struct Player {
    Texture spriteTexture;

    SDL_Rect srcRect[2][8], bulletSrcRect;
    SDL_Rect dstRect;
    int lastBullet = 0, currentPowerLv = 1;

    vector<unique_ptr<Particle>> bullets;

    #define DELTA_DELAY 60
    int sprite_row = 0;
    int sprite_col = 0;
    int sprite_delta = DELTA_DELAY;
    int idle = 0, movingRight = 0, movingLeft = 0;

	const double DELTA_X = 5;
	const double DELTA_Y = 5;
	Vec2d position, velocity;

    int pressed[8] = { 0 };
    #define KEY_UP 0
    #define KEY_DOWN 1
    #define KEY_RIGHT 2
    #define KEY_LEFT 3
    #define KEY_SHOOT 4
    #define KEY_BOMB 5

    #define SPRITE_WIDTH 32
    #define SPRITE_HEIGHT 48

    #define BULLET_WIDTH 16
    #define BULLET_HEIGHT 32

    void initialize() {
        spriteTexture.load("marisa");
        position = Vec2d(FIELD_X + FIELD_WIDTH / 2, FIELD_HEIGHT - 32);
        velocity = Vec2d(5, 5);

        // source rectangle
        for (int i = 0, y = 25; i < 2; ++i) {
            int x = 868;
            for (int j = 0; j < 8; ++j) {
                srcRect[i][j].w = SPRITE_WIDTH, srcRect[i][j].h = SPRITE_HEIGHT;
                srcRect[i][j].x = x, srcRect[i][j].y = y;
                x += SPRITE_WIDTH;
            }

            if (i == 0) {
                bulletSrcRect.w = BULLET_WIDTH, bulletSrcRect.h = BULLET_HEIGHT;
                bulletSrcRect.x = 996, bulletSrcRect.y = y;
            }

            y += SPRITE_HEIGHT;
        }

    }

    void render() {
        spriteTexture.render(position.x - SPRITE_WIDTH / 2, dstRect.y = position.y - SPRITE_HEIGHT / 2, 
                            &srcRect[!idle][sprite_col], 0.0, NULL, movingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

        for (auto& particle : bullets) {
            spriteTexture.render(particle->position.x - BULLET_WIDTH / 2, particle->position.y - BULLET_HEIGHT / 2, &bulletSrcRect);
        }
    }

    void handleInput(const SDL_Event& e) {

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
            case SDLK_z: button = KEY_SHOOT; break;
            case SDLK_x: button = KEY_BOMB; break;
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
        else if (!idle) startIdle();

        // shoot
        int currentTick = SDL_GetTicks();
        if (pressed[KEY_SHOOT] && currentTick - lastBullet > 80) {
            shoot();
            lastBullet = currentTick;
        }

        updateSprite();
        updateBullets();
	}

	void moveUp() {
		position.y -= velocity.y;
        if (!inPlayField(position)) position.y += velocity.y;
	}

	void moveDown() {
		position.y += velocity.y;
        if (!inPlayField(position)) position.y -= velocity.y;
	}

	void moveRight() {
        if (!movingRight) {
            sprite_row = 2;
            sprite_col = 0;
            sprite_delta = DELTA_DELAY;
            idle = 0;
            movingLeft = 0;
            movingRight = 1;
        }
		position.x += velocity.x;
        if (!inPlayField(position)) position.x -= velocity.x;
	}

	void moveLeft() {
        if (!movingLeft) {
            sprite_row = 1;
            sprite_col = 0;
            sprite_delta = DELTA_DELAY;
            idle = 0;
            movingLeft = 1;
            movingRight = 0;
        }
		position.x -= velocity.x;
        if (!inPlayField(position)) position.x += velocity.x;
	}

    void startIdle() {
        sprite_row = 0;
        sprite_col = 0;
        sprite_delta = DELTA_DELAY;
        idle = 1;
        movingLeft = 0;
        movingRight = 0;
    }

    void shoot() {
        switch (currentPowerLv) {
            case 1: 
                unique_ptr<LinearParticle> bullet = make_unique<LinearParticle>(position, Vec2d(0, -1), 8);
                bullets.emplace_back(move(bullet));
                break;
                
        }
    }

    void updateSprite() {
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
    }

    void updateBullets() {
        vector<unique_ptr<Particle>> newBullets;
        for (auto& bullet : bullets) {
            bullet->update();
            if (bullet->inBound()) {
                newBullets.emplace_back(move(bullet));
            }
        }
        bullets = move(newBullets);
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

struct Scene_Gameplay {

    Texture background;
    BGM bgm;

    Player player;
    double scrollingOffset = 0;

    void initialize() {
        // game init
        background.load("background");
        bgm.load("th06_03");
        player.initialize();
    }

    void start() {
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

        particleManager.update();
    }

    void render() {
        // render bg
        SDL_Rect srcRect, dstRect; 
        srcRect.x = 306, srcRect.y = 25, srcRect.w = srcRect.h = 256;
        dstRect.x=0, dstRect.y=scrollingOffset, dstRect.w=WINDOW_WIDTH, dstRect.h=WINDOW_HEIGHT;
        background.render(&srcRect, &dstRect);
        dstRect.x=0, dstRect.y=-WINDOW_HEIGHT + scrollingOffset, dstRect.w=WINDOW_WIDTH, dstRect.h=WINDOW_HEIGHT;
        background.render(&srcRect, &dstRect);

        // render player
        player.render();
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
        if (menu.gameStarted) {
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
