#pragma once

#include "utils.h"
#include "texture.h"
#include "player.h"
#include "particle.h"
#include "particleManager.h"
#include "audio.h"

extern Player player;
extern ParticleManager particleManager;

class Enemy {
public:
    Vec2d position, direction;
    SE deadSE;
    SDL_Rect srcRect;

    int elapsedTime = 0;
    int hp = 1;
    double velocity = 1.0;

    Enemy(Vec2d pos, Vec2d dir, double vel): position(pos), direction(dir), velocity(vel) {}
    double easeIn() {
        return elapsedTime / 80.0;
    }
    void gotHit() {
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
        srcRect.x = 306, srcRect.y = 306, srcRect.w = 32, srcRect.h = 32;
        deadSE.load("tan00");
    }
    void update() override {
        if (hp == 0) return;

        position = position + direction * (velocity);
        elapsedTime++;

        for (auto& bullet : player.bullets) {
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
        srcRect.x = 306, srcRect.y = 338, srcRect.w = 32, srcRect.h = 32;
        deadSE.load("tan00");
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
            dir = dir * (1.0 / dir.length());

            double angle = atan2(dir.y, dir.x); 
            for (int i = -2; i <= +2; ++i) {
                double a = angle + i * 10 * M_PI / 180.0;
                double x = dir.length() * cos(a);
                double y = dir.length() * sin(a);
                particleManager.addBullet(position, Vec2d(x, y), 5, 1);
            }
        }
        position = position + direction * velocity * easeIn();

        for (auto& bullet : player.bullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit();
            }
        }
    }
};

#pragma once

#include "utils.h"
#include "texture.h"
#include "particle.h"
#include "particleManager.h"

struct EnemyManager {
    string texturePath;
    Texture texture;
    int delta = 30;

    vector<unique_ptr<Enemy>> enemies;

    void initialize() {
        texture.load("enemy");
    }

    void updateEnemies() {
        vector<unique_ptr<Enemy>> newEnemies;
        for (auto& enemy : enemies) {
            enemy->update();
            if (enemy->hp > 0) {
                newEnemies.emplace_back(move(enemy));
            }
        }
        enemies = move(newEnemies);

        for (auto& enemy : enemies) {
            if (enemy->position.distance(player.position) <= 64) {
                player.gotHit();
            }
        }
    }

    void generateEnemies() {
        delta--;
        if (delta == 0) {
            delta = 60;
            Vec2d position = Vec2d(random(FIELD_X, FIELD_X + FIELD_WIDTH - 1), FIELD_Y);
            Vec2d direction = Vec2d(0, 1);

            unique_ptr<BlueFairy> blueFairy = make_unique<BlueFairy>(position, direction, 1);
            enemies.emplace_back(move(blueFairy));

            unique_ptr<PinkFairy> pinkFairy = make_unique<PinkFairy>(position, direction, 1);
            enemies.emplace_back(move(pinkFairy));

        }
    }

    void update() {
        updateEnemies();
        generateEnemies();
    }

    void render() {
        for (auto& enemy : enemies) {
            texture.render(enemy->position.x, enemy->position.y, &enemy->srcRect);
        }
    }
};
