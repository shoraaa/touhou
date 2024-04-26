#pragma once

#include "utils.h"
#include "texture.h"
#include "particle.h"
#include "audio.h"
#include "sprite.h"

extern int PLAYER_LOST;
struct Player {
    Texture spriteTexture;
    SE shootSE, deadSE, itemSE, grazeSE;
    int shootChannel, deadChannel, itemChannel;
    int lives = 3;
    int score = 0, _score = 0;
    int kills = 0;
    int power = 1;
    int graze = 0, grazeDelay = 0;
    int chi = 0;

    SDL_Rect spriteClip[2][7], bulletClip, chaseBulletClip, taoClip;

    unique_ptr<Sprite> tao[2], deltaTao;

    int lastBullet = 0;
    int bulletRadius = 16, bulletVelocity = 12;
    #define BULLET_DELAY 90

    int lastChaseBullet = 0;
    int chaseBulletRadius = 16, chaseBulletVelocity = 4;
    #define CHASE_BULLET_DELAY 360

    vector<unique_ptr<Particle>> bullets, chaseBullets;

    #define DELTA_DELAY 8
    int sprite_row = 0;
    int sprite_col = 0;
    int sprite_delta = DELTA_DELAY;
    int idle = 0, movingRight = 0, movingLeft = 0;
    int tick = 0;
    int bomb = 3;

	const double DELTA_X = 5;
	const double DELTA_Y = 5;
	Vec2d position, velocity;
    Vec2d normalVelocity, slowVelocity;

    int invicibleFrame = 0;
    int bombDelay = 0;

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
    #define BULLET_HEIGHT 16

    void initialize() {
        spriteTexture.load("reimu");
        position = Vec2d(FIELD_X + FIELD_WIDTH / 2, FIELD_HEIGHT - 32);
        velocity = Vec2d(3, 3);

        normalVelocity = Vec2d(4, 4);
        slowVelocity = Vec2d(2, 2);

        shootSE.load("plst00");
        deadSE.load("pldead00");
        itemSE.load("item00");
        grazeSE.load("graze");

        lives = 3;
        bomb = 3;

        // source rectangle
        for (int i = 0, y = 25; i < 2; ++i) {
            int x = 868;
            for (int j = 0; j < 7; ++j) {
                spriteClip[i][j] = {x, y, SPRITE_WIDTH, SPRITE_HEIGHT};
                x += SPRITE_WIDTH;
            }
            y += SPRITE_HEIGHT;
        }

        bulletClip = {996, 25, BULLET_WIDTH, BULLET_HEIGHT};
        chaseBulletClip = {996 + BULLET_WIDTH, 25, BULLET_WIDTH, BULLET_HEIGHT};

        for (int i = 0; i < 2; ++i)
            tao[i] = make_unique<Sprite>("reimu", SDL_Rect{996, 25 + BULLET_HEIGHT, BULLET_WIDTH, BULLET_HEIGHT});
        tao[0]->setPosition(Vec2d(position.x - 24, position.y), 15, 1);
        tao[1]->setPosition(Vec2d(position.x + 24, position.y), 15, 1);

        deltaTao = make_unique<Sprite>("reimu", SDL_Rect{996, 25 + BULLET_HEIGHT, BULLET_WIDTH, BULLET_HEIGHT});
        deltaTao->position = Vec2d(16, 0);

    }

    void render() {
        if (PLAYER_LOST) return;

        // player
        if (invicibleFrame % 2 == 0) {
            spriteTexture.render(position.x, position.y, &spriteClip[!idle][sprite_col], SPRITE_WIDTH, SPRITE_HEIGHT, 0.0, NULL, movingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        } 

        // tao

        tao[0]->position = Vec2d(position.x - deltaTao->position.x, position.y + deltaTao->position.y);
        tao[1]->position = Vec2d(position.x + deltaTao->position.x, position.y + deltaTao->position.y);
        tao[0]->angle = tao[1]->angle = (tick * 4) % 360;

        tao[0]->render(); tao[1]->render();

        // normal bullet
        spriteTexture.setAlpha(128);
        for (auto& particle : bullets) {
            spriteTexture.render(particle->position.x, particle->position.y, &bulletClip, 24, 24, particle->elapsedTime * 8);
        }
        // chase bullet
        for (auto& particle : chaseBullets) {
            spriteTexture.render(particle->position.x, particle->position.y, &chaseBulletClip, 20, 20, particle->elapsedTime * 8);
        }
        spriteTexture.setAlpha(255);
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

    void clearInput() {
        for (int i = 0; i < 7; ++i) {
            pressed[i] = 0;
        }
    }

	void update() {
        if (PLAYER_LOST) return;
        tick++;

        if (invicibleFrame) invicibleFrame--;

        if (pressed[KEY_UP]) moveUp();
        else if (pressed[KEY_DOWN]) moveDown();

        if (pressed[KEY_LEFT]) moveLeft();
        else if (pressed[KEY_RIGHT]) moveRight();
        else if (!idle) startIdle();

        if (pressed[KEY_SHIFT] && velocity.x != slowVelocity.x) {
            deltaTao->setPosition(Vec2d(12, -12), 30, 1);
            velocity = slowVelocity;
        } else if (!pressed[KEY_SHIFT] && velocity.x != normalVelocity.x) { 
            deltaTao->setPosition(Vec2d(24, 0), 30, 1);
            velocity = normalVelocity; 
        }
        deltaTao->update();


        // shoot
        if (pressed[KEY_SHOOT]) {
            shoot();
        }

        // bomb
        if (bombDelay > 0) bombDelay--;
        if (bombDelay == 0 && pressed[KEY_BOMB]) {
            throwBomb();
            bombDelay = 240;
            invicibleFrame = 240;
        }

        updateSprite();
        

        if (_score < score) {
            _score += 10;
        }

        if (chi == 100) {
            chi = 0;
            lives++;
        }
	}

    void increaseChi(int value) {
        chi += value;
        itemSE.play();

        if (chi >= 100) {
            chi = 0;
            lives++;
        }
    }

    void increasePower(int value) {
        power += value;
        itemSE.play();
    }

    bool gotHit() {
        if (PLAYER_LOST) return 0;

        if (invicibleFrame) return 0;

        deadSE.play();
        if (lives == 0) {
            PLAYER_LOST = 1;
            return 1;
        } 

        lives--;
        power = max(power - 10, 1);
        invicibleFrame = 120;
        return 1;
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

    int getPowerLv() {
        int powerLV = 1;
        if (power >= 10) powerLV = 2;
        if (power >= 20) powerLV = 3;
        if (power >= 35) powerLV = 4;
        if (power >= 60) powerLV = 5;
        return powerLV;
    }

    void shoot() {
        shootSE.play();

        int powerLV = getPowerLv();

        int currentTick = SDL_GetTicks();
        if (powerLV == 1) {
            int currentTick = SDL_GetTicks();
            if (currentTick - lastBullet > BULLET_DELAY) {
                unique_ptr<PlayerBullet> bullet = make_unique<PlayerBullet>(position, Vec2d(0, -1), bulletRadius, bulletVelocity);
                bullets.emplace_back(move(bullet));

                lastBullet = currentTick;
            }
        }  else if (powerLV == 2) {
            if (currentTick - lastBullet > BULLET_DELAY) {
                unique_ptr<PlayerBullet> bullet = make_unique<PlayerBullet>(position, Vec2d(0, -1), bulletRadius, bulletVelocity);
                bullets.emplace_back(move(bullet));

                lastBullet = currentTick;
            }

            if (currentTick - lastChaseBullet > CHASE_BULLET_DELAY * 2) {
                unique_ptr<PlayerBullet> chaseBulletLeft = make_unique<PlayerBullet>(Vec2d(position.x - 16, position.y), Vec2d(0, -1), chaseBulletRadius, chaseBulletVelocity / 2);
                unique_ptr<PlayerBullet> chaseBulletRight = make_unique<PlayerBullet>(Vec2d(position.x + 16, position.y), Vec2d(0, -1), chaseBulletRadius, chaseBulletVelocity / 2);
                chaseBullets.emplace_back(move(chaseBulletLeft));
                chaseBullets.emplace_back(move(chaseBulletRight));

                lastChaseBullet = currentTick;
            }

        }  else if (powerLV == 3) {

            if (currentTick - lastBullet > BULLET_DELAY) {
                unique_ptr<PlayerBullet> bulletLeft = make_unique<PlayerBullet>(Vec2d(position.x - 8, position.y - 4), Vec2d(0, -1), bulletRadius, bulletVelocity);
                unique_ptr<PlayerBullet> bulletRight = make_unique<PlayerBullet>(Vec2d(position.x + 8, position.y - 4), Vec2d(0, -1), bulletRadius, bulletVelocity);
                bullets.emplace_back(move(bulletLeft));
                bullets.emplace_back(move(bulletRight));

                lastBullet = currentTick;
            }

            if (currentTick - lastChaseBullet > CHASE_BULLET_DELAY) {
                unique_ptr<PlayerBullet> chaseBulletLeft = make_unique<PlayerBullet>(Vec2d(position.x - 16, position.y), Vec2d(0, -1), chaseBulletRadius, chaseBulletVelocity + 2);
                unique_ptr<PlayerBullet> chaseBulletRight = make_unique<PlayerBullet>(Vec2d(position.x + 16, position.y), Vec2d(0, -1), chaseBulletRadius, chaseBulletVelocity + 2);
                chaseBullets.emplace_back(move(chaseBulletLeft));
                chaseBullets.emplace_back(move(chaseBulletRight));

                lastChaseBullet = currentTick;
            }
        } else if (powerLV == 4) {

            if (currentTick - lastBullet > BULLET_DELAY) {
                Vec2d dir = Vec2d(0, -1);

                double angle = atan2(dir.y, dir.x); 
                for (int i = -1; i <= +1; ++i) {
                    double a = angle + i * 5 * M_PI / 180.0;
                    double x = dir.length() * cos(a);
                    double y = dir.length() * sin(a);
                    unique_ptr<PlayerBullet> bullet = make_unique<PlayerBullet>(position, Vec2d(x, y), bulletRadius, bulletVelocity);
                    bullets.emplace_back(move(bullet));
                }

                lastBullet = currentTick;
            }


            if (currentTick - lastChaseBullet > CHASE_BULLET_DELAY / 2) {
                unique_ptr<PlayerBullet> chaseBulletLeft = make_unique<PlayerBullet>(Vec2d(position.x - 16, position.y), Vec2d(0, -1), chaseBulletRadius, chaseBulletVelocity * 2);
                unique_ptr<PlayerBullet> chaseBulletRight = make_unique<PlayerBullet>(Vec2d(position.x + 16, position.y), Vec2d(0, -1), chaseBulletRadius, chaseBulletVelocity * 2);
                chaseBullets.emplace_back(move(chaseBulletLeft));
                chaseBullets.emplace_back(move(chaseBulletRight));

                lastChaseBullet = currentTick;
            }
        } else {

             if (currentTick - lastBullet > BULLET_DELAY) {
                Vec2d dir = Vec2d(0, -1);

                double angle = atan2(dir.y, dir.x); 
                for (int i = -2; i <= +2; ++i) {
                    double a = angle + i * 5 * M_PI / 180.0;
                    double x = dir.length() * cos(a);
                    double y = dir.length() * sin(a);
                    unique_ptr<PlayerBullet> bullet = make_unique<PlayerBullet>(position, Vec2d(x, y), bulletRadius, bulletVelocity);
                    bullets.emplace_back(move(bullet));
                }

                lastBullet = currentTick;
            }


            if (currentTick - lastChaseBullet > CHASE_BULLET_DELAY / 3) {
                unique_ptr<PlayerBullet> chaseBulletLeft = make_unique<PlayerBullet>(Vec2d(position.x - 16, position.y), Vec2d(0, -1), chaseBulletRadius, chaseBulletVelocity * 3);
                unique_ptr<PlayerBullet> chaseBulletRight = make_unique<PlayerBullet>(Vec2d(position.x + 16, position.y), Vec2d(0, -1), chaseBulletRadius, chaseBulletVelocity * 3);
                chaseBullets.emplace_back(move(chaseBulletLeft));
                chaseBullets.emplace_back(move(chaseBulletRight));

                lastChaseBullet = currentTick;
            }

        }

    }
    
    int requestClear = 0;
    void throwBomb() {
        if (!bomb) return; 
        bomb--;
        requestClear = 1;

    }

    void updateSprite() {
        sprite_delta--;
        if (sprite_delta == 0) {
            sprite_delta = DELTA_DELAY;
            sprite_col++;
            if (idle) {
                if (sprite_col == 4) sprite_col = 0;
            } else {
                if (sprite_col == 7) sprite_col = 2;
            }
        }
    }

    void gotHitOther() {
        score += 10;
    }

    void gotKill() {
        kills++;
        score += 100;
    }

    void gotGraze() {
        grazeDelay--;
        if (grazeDelay <= 0) {
            graze++;
            grazeDelay = 30;
            score += 1000;
            grazeSE.play();
        }
    }
	
};