#pragma once

#include "player.h"

extern Player player;
struct ParticleManager {
    string texturePath;
    Texture texture;
    int delta = 30;

    SDL_Rect deadRect;

    vector<unique_ptr<Particle>> particles;

    vector<pair<Vec2d, int>> enemyDeadState;

    void initialize() {
        texture.load("particle"); 
        deadRect.x = 57, deadRect.y = 25, deadRect.w = 32, deadRect.h = 32;

    }

    void push(Vec2d pos, int radius, int velocity) {
        Vec2d position = pos;
        Vec2d direction = player.position - position;
        direction = direction * (1.0 / direction.length());

        unique_ptr<LinearParticle> particle = make_unique<LinearParticle>(position, direction, radius, velocity);
        particles.emplace_back(move(particle));
    }

    void updateBullets() {
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
                particle->hit = 1;
                if (particle->type == 0) { // bullet
                    player.gotHit();
                } else if (particle->type == 1) { // score
                    player.increaseScore(10);
                } else if (particle->type == 2) { // power
                    player.increasePower(1);
                }
            }
        }
    }

    void updatePlayerBullets() {
        vector<unique_ptr<Particle>> newBullets;
        for (auto& bullet : player.bullets) {
            // bullet find a enemy to shoot
            if (!enemyManager.enemies.size()) {
                auto enemy = *enemyManager.enemies[0];
                Vec2d dir = player.position - enemy.position;
                dir = dir * (1.0 / dir.length());
                bullet->position = bullet->initialPosition + (dir * bullet->elapsedTime);
                bullet->elapsedTime++;

            } else {
                bullet->update();
            }

            if (bullet->inBound() && !bullet->hit) {
                newBullets.emplace_back(move(bullet));
            }
        }
        player.bullets = move(newBullets);
    }

    void updateDeadAnimation() {
        vector<pair<Vec2d, int>> newEnemyDeadState;
        for (auto& p : enemyDeadState) {
            auto pos = p.first;
            int frame = p.second;

            frame++;
            if (frame < 6) {
                newEnemyDeadState.emplace_back(pos, frame);
            }

        }
        enemyDeadState = newEnemyDeadState;
    }

    void update() {
        updateBullets();
        updatePlayerBullets();
        updateDeadAnimation();

    }

    void renderBullets() {
        for (auto& particle : particles) {
            texture.render(particle->position.x, particle->position.y, &particle->srcRect);
        }
    }

    void renderDeadAnimation() {
        for (auto& p : enemyDeadState) {
            auto pos = p.first;
            int frame = p.second;
            texture.setAlpha(155);
            texture.render(pos.x, pos.y, &deadRect, deadRect.w + frame * 6, deadRect.h + frame * 6, frame * 32);
            texture.setAlpha(255);

        }
    }

    void render() {
        renderBullets();
        renderDeadAnimation();
    }

    void addBullet(Vec2d pos, Vec2d dir, int radius, int velocity) {
        unique_ptr<LinearParticle> particle = make_unique<LinearParticle>(pos, dir, radius, velocity);
        particles.emplace_back(move(particle));
    }


    void playEnemyDeadAnimation(Vec2d pos) {
        enemyDeadState.emplace_back(pos, 0);
    }

    void dropPowerItem(Vec2d pos) {
        unique_ptr<PowerItem> item = make_unique<PowerItem>(pos);
        particles.emplace_back(move(item));
    }

    void dropScoreItem(Vec2d pos) {
        unique_ptr<ScoreItem> item = make_unique<ScoreItem>(pos);
        particles.emplace_back(move(item));
    }

};