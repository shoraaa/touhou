#pragma once

#include "player.h"

extern Player player;
struct ParticleManager {
    string texturePath;
    Texture texture;
    int delta = 30;

    SDL_Rect bulletRect, deadRect;

    vector<unique_ptr<Particle>> particles;

    vector<pair<Vec2d, int>> enemyDeadState;

    void initialize() {
        texture.load("particle"); 
        bulletRect.x = 322, bulletRect.y = 57, bulletRect.w = 16, bulletRect.h = 16;
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
                player.gotHit();
            }
        }
    }

    void updateDeadAnimation() {
        vector<pair<Vec2d, int>> newEnemyDeadState;
        for (auto& p : enemyDeadState) {
            auto pos = p.first;
            int frame = p.second;

            deadRect.w += frame;
            deadRect.h += frame;

            texture.render(pos.x - deadRect.w / 2, pos.y - deadRect.h / 2, &deadRect, frame);

            deadRect.w -= frame;
            deadRect.h -= frame;

            frame++;
            if (frame < 30) {
                newEnemyDeadState.push_back(p);
            }

        }
        enemyDeadState = newEnemyDeadState;
    }

    void update() {
        updateBullets();
        updateDeadAnimation();

    }

    void render() {
        for (auto& particle : particles) {
            texture.render(particle->position.x - bulletRect.w / 2, particle->position.y - bulletRect.h / 2, &bulletRect);
        }
    }


    void playEnemyDeadAnimation(Vec2d pos) {
        enemyDeadState.emplace_back(pos, 0);
    }

};