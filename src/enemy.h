#pragma once

#include "utils.h"
#include "texture.h"
#include "player.h"
#include "particle.h"
#include "particleManager.h"
#include "audio.h"
#include "const.h"
#include "pattern.h"

extern Player player;
extern ParticleManager particleManager;
extern int elapsed_frame;

class Enemy {
public:
    Vec2d position, direction;
    SE deadSE;
    SDL_Rect spriteClip;

    int elapsedTime = 0;
    int hp = 1;
    double velocity = 1.0;

    Enemy(Vec2d pos, Vec2d dir, double vel): position(pos), direction(dir), velocity(vel) {}
    void gotHit() {
        particleManager.playEnemyHitAnimation(position);
        hp--;
        if (hp == 0) {
            particleManager.playEnemyDeadAnimation(position);
            deadSE.play();
            player.kills++;
            if (player.kills % 3 == 0) {
                if (player.kills % 2 == 0) particleManager.dropPowerItem(position);
                else particleManager.dropScoreItem(position);
            }
        }
    }

    void virtual update() {}
};

class BlueFairy : public Enemy {
public:
    BlueFairy(Vec2d pos, Vec2d dir, double vel): Enemy(pos, dir, vel) {
        spriteClip.x = 306, spriteClip.y = 306, spriteClip.w = 32, spriteClip.h = 32;
        deadSE.load("tan00");
    }
    void update() override {
        if (hp == 0) return;

        position = position + direction * velocity;
        elapsedTime++;

        for (auto& bullet : player.bullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit();
            }
        }

        if (hp == 0) return;

        for (auto& bullet : player.chaseBullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit();
            }
        }
    }
};

class PinkFairy : public Enemy {
public:
    PinkFairy(Vec2d pos, Vec2d dir, double vel): Enemy(pos, dir, vel) {
        spriteClip.x = 306, spriteClip.y = 338, spriteClip.w = 32, spriteClip.h = 32;
        deadSE.load("tan00");
        hp = 3;
    }
    void update() override {
        if (hp == 0) return;
        elapsedTime++;
        if (elapsedTime > 90 && elapsedTime <= 120) return;

        if (elapsedTime == 90) {
            int half = (FIELD_X + FIELD_WIDTH - 1) / 2;
            if (position.x >= half) direction = Vec2d(1, 0);
            else direction = Vec2d(-1, 0);
            velocity /= 2;

            Vec2d dir = player.position - position;
            dir = dir.normalized();

            double angle = atan2(dir.y, dir.x); 
            for (int i = -2; i <= +2; ++i) {
                double a = angle + i * 10 * M_PI / 180.0;
                double x = dir.length() * cos(a);
                double y = dir.length() * sin(a);
                particleManager.addBullet(position, Vec2d(x, y), 5, 2);
            }
        }
        position = position + direction * velocity * elapsedTime / 90.0;

        for (auto& bullet : player.bullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit();
            }
        }

        if (hp == 0) return;

        for (auto& bullet : player.chaseBullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit();
            }
        }
    }
};

struct Rumia {
    Vec2d position, direction, initialPosition;
    SE deadSE, hitSE;
    SDL_Rect spriteClip;
    int spawned = 0, dead = 0;

    int movingFrame = 1e9;

    vector<unique_ptr<Pattern>> patterns;

    int elapsedTime = 0;
    int hp = 1000, skillDelay = 120;
    double velocity = 1.0;

    #define SKILL_CD 60 * 3

    void initialize() {
        spriteClip = {996, 25, 32, 64};
        hitSE.load("damage00");

        // patterns.emplace_back(make_unique<RingPattern>());
        // patterns.emplace_back(make_unique<FanPattern>());
        // patterns.emplace_back(make_unique<ShootPattern>());

        patterns.emplace_back(make_unique<RumiaPatternA>());
        patterns.emplace_back(make_unique<RumiaPatternB>());
        patterns.emplace_back(make_unique<RumiaPatternC>());


        for (auto& p : patterns) {
            p->initialize();
        }
    }

    void spawn() {
        spawned = 1;
        position = Vec2d(FIELD_X + FIELD_WIDTH / 2, 64);
        cout << "Rumia spawned!\n";
    }

    void gotHit() {
        particleManager.playBossHitAnimation(position);
        hp--;
        if (hp <= 0) {
            particleManager.playEnemyDeadAnimation(position);
            deadSE.play();
        }
    }

    void update() {
        if (hp <= 0) {
            spawned = 0;
            dead = 1;
            return;
        }

        for (auto& bullet : player.bullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit();
            }
        }
        for (auto& bullet : player.chaseBullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit();
            }
        }

        // cerr << hp << '\n';

        skillDelay--;
        if (skillDelay <= 0) {
            skillDelay = SKILL_CD;

            vector<int> nextPatternCandidate;
            for (int i = 0; i < patterns.size(); ++i) if (!patterns[i]->onGoing()) {
                nextPatternCandidate.emplace_back(i);
            }
            if (!nextPatternCandidate.empty()) {
                int i = random(0, nextPatternCandidate.size() - 1);
                patterns[nextPatternCandidate[i]]->reset(position);
            }

            Vec2d nextPosition = Vec2d(random(FIELD_X + 16, FIELD_X2 - 16), random(FIELD_Y + 16, FIELD_Y + 200));
            initialPosition = position;
            direction = nextPosition - position;
            movingFrame = 0;
        }

        for (auto& pattern : patterns) {
            pattern->update(position);
        }

        if (movingFrame < 45) {
            position = initialPosition + direction * easeIn(movingFrame / 60.0);
            movingFrame++;
        }


    }
};

struct EnemyManager {
    string texturePath;
    Texture texture;

    vector<unique_ptr<Enemy>> enemies;
    Rumia rumia;


    void initialize() {
        texture.load("enemy");
        rumia.initialize();
    }

    void updatePlayerBullets() {
        vector<unique_ptr<Particle>> newBullets;
        for (auto& bullet : player.bullets) {
            bullet->update();

            if (bullet->inBound() && !bullet->hit) {
                newBullets.emplace_back(move(bullet));
            }
        }
        player.bullets = move(newBullets);

        vector<unique_ptr<Particle>> newChaseBullets;
        for (auto& bullet : player.chaseBullets) {
            // bullet find a enemy to shoot
            if (enemies.size() || rumia.spawned) {
                auto position = rumia.spawned ? rumia.position : (*enemies[0]).position;
                Vec2d dir = position - bullet->position;
                dir = dir * (1.0 / dir.length());
                bullet->position = bullet->position + (dir * (bullet->elapsedTime / 3.0));
                bullet->elapsedTime++;
            } else {
                bullet->update();
            }

            if (bullet->inBound() && !bullet->hit) {
                newChaseBullets.emplace_back(move(bullet));
            }
        }
        player.chaseBullets = move(newChaseBullets);
    }

    void updateEnemies() {
        vector<unique_ptr<Enemy>> newEnemies;
        for (auto& enemy : enemies) {
            enemy->update();
            if (enemy->hp > 0 && enemy->position.inPlayField()) {
                newEnemies.emplace_back(move(enemy));
            }
        }
        enemies = move(newEnemies);

        for (auto& enemy : enemies) {
            if (enemy->position.distance(player.position) <= 64) {
                player.gotHit();
            }
        }

        // player bullet
        updatePlayerBullets();
    }

    int deltaPink = 60;
    int deltaBlue = 10;
    void generateEnemies() {
        deltaPink--;
        if (deltaPink == 0) {
            deltaPink = 30;
            Vec2d position = Vec2d(random(FIELD_X, FIELD_X + FIELD_WIDTH - 1), FIELD_Y);
            Vec2d direction = Vec2d(0, 1);

            unique_ptr<PinkFairy> pinkFairy = make_unique<PinkFairy>(position, direction, 1);
            enemies.emplace_back(move(pinkFairy));

        }

        deltaBlue--;
        if (deltaBlue == 0) {
            deltaBlue = 10;
            Vec2d position = Vec2d(random(FIELD_X, FIELD_X + FIELD_WIDTH - 1), FIELD_Y);
            Vec2d direction = Vec2d(0, 1);

            unique_ptr<BlueFairy> blueFairy = make_unique<BlueFairy>(position, direction, 1);
            enemies.emplace_back(move(blueFairy));

        }
    }

    void update() {
        updateEnemies();
        if (0 && elapsed_frame <= 60 * 30) {
           generateEnemies();
        } else if (!rumia.dead) {
            if (!rumia.spawned) {
                rumia.spawn();
            } else {
                rumia.update();
            }
        }
    }

    void render() {
        for (auto& enemy : enemies) {
            texture.render(enemy->position.x, enemy->position.y, &enemy->spriteClip);
        }
        if (rumia.spawned) {
            SDL_Rect hpRect = {FIELD_X + 64, FIELD_Y + 16, (int)((rumia.hp / 1000.0) * (FIELD_WIDTH - (FIELD_X + 64 + 4))), 4};
            SDL_RenderFillRect(renderer, &hpRect);
            texture.render(rumia.position.x, rumia.position.y, &rumia.spriteClip);
        }
    }
};
