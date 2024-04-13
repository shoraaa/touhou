struct Player {
    Texture spriteTexture;
    SE shootSE;
    int lives = 3;

    SDL_Rect srcRect[2][8], bulletSrcRect;
    SDL_Rect dstRect;
    int lastBullet = 0, currentPowerLv = 1;
    int bulletRadius = 16, bulletVelocity = 12;

    vector<unique_ptr<Particle>> bullets;

    #define DELTA_DELAY 30
    int sprite_row = 0;
    int sprite_col = 0;
    int sprite_delta = DELTA_DELAY;
    int idle = 0, movingRight = 0, movingLeft = 0;

	const double DELTA_X = 5;
	const double DELTA_Y = 5;
	Vec2d position, velocity;
    Vec2d normalVelocity, slowVelocity;

    int pressed[8] = { 0 };
    #define KEY_UP 0
    #define KEY_DOWN 1
    #define KEY_RIGHT 2
    #define KEY_LEFT 3
    #define KEY_SHOOT 4
    #define KEY_BOMB 5
    #define KEY_SHIFT 6

    #define SPRITE_WIDTH 32
    #define SPRITE_HEIGHT 48

    #define BULLET_WIDTH 16
    #define BULLET_HEIGHT 32

    void initialize() {
        spriteTexture.load("marisa");
        position = Vec2d(FIELD_X + FIELD_WIDTH / 2, FIELD_HEIGHT - 32);
        velocity = Vec2d(5, 5);

        normalVelocity = Vec2d(5, 5);
        slowVelocity = Vec2d(2.5, 2.5);

        shootSE.load("plst00");

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
        spriteTexture.render(position.x - SPRITE_WIDTH / 2, position.y - SPRITE_HEIGHT / 2, 
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
            case SDLK_LSHIFT: button = KEY_SHIFT; break;
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

        if (pressed[KEY_SHIFT]) velocity = slowVelocity;
        else velocity = normalVelocity; 

        // shoot
        int currentTick = SDL_GetTicks();
        if (pressed[KEY_SHOOT] && currentTick - lastBullet > 120) {
            shoot();
            lastBullet = currentTick;
        }

        updateSprite();
        updateBullets();
	}

	void moveUp() {
		position.y -= velocity.y;
        if (!position.inPlayField()) position.y += velocity.y;
	}

	void moveDown() {
		position.y += velocity.y;
        if (!position.inPlayField()) position.y -= velocity.y;
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
        if (!position.inPlayField()) position.x -= velocity.x;
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
        if (!position.inPlayField()) position.x += velocity.x;
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
        shootSE.play();
        switch (currentPowerLv) {
            case 1: 
                unique_ptr<LinearParticle> bullet = make_unique<LinearParticle>(position, Vec2d(0, -1), bulletRadius, bulletVelocity);
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
            if (bullet->inBound() && !bullet->hit) {
                newBullets.emplace_back(move(bullet));
            }
        }
        bullets = move(newBullets);
    }
	
};