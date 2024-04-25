#pragma once

#include "player.h"
#include "utils.h"
#include "animation.h"

extern Player player;

struct ParticleManager {
    string texturePath;
    Texture texture;
    int delta = 30;
    SE damage;

    SDL_Rect deadRect, hitParticles, deadParticles, bulletClip[6][16];

    vector<shared_ptr<Particle>> particles, tmp;

    vector<shared_ptr<Animation>> animations;

    void initialize() {
        texture.load("particle"); 
        damage.load("damage00");

        for (int i = 0, cx = 306, y = 57; i < 5; ++i) {
            int x = cx;
            for (int j = 0; j < 16; ++j) {
                bulletClip[i][j] = {x, y, 16, 16};
                x += 16;
            }
            y += 16;
        }

        for (int i = 0, x = 434, y = 233; i < 8; ++i) {
            bulletClip[5][i] = {x, y, 8, 8};
            if (i == 3) {
                x = 434, y = 233 + 8;
            } else {
                x += 8;
            }
        }
    }

    void updateBullets() {
        vector<shared_ptr<Particle>> newParticles;
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
                    if (player.gotHit()) {
                        playPlayerDeadAnimation(player.position);
                        clear();
                    }
                } else if (particle->type == 1) { // score
                    player.increaseScore(10);
                } else if (particle->type == 2) { // power
                    player.increasePower(1);
                }
            }
        }
    }

    void updateAnimation() {
        vector<shared_ptr<Animation>> newAnimations;
        for (auto& animation : animations) {
            if (animation->update()) {
                newAnimations.emplace_back(move(animation));
            }
        }
        animations = move(newAnimations);
    }

    void update() {
        updateBullets();
        updateAnimation();

    }

    void renderBullets() {
        for (auto& particle : particles) {
            render(particle);
        }
        for (auto& particle : tmp) {
            render(particle);
        }
    }

    void renderAnimations() {
        for (auto& animation : animations) {
            animation->render(texture);
        }
    }

    void render(shared_ptr<Particle> &particle) {
        texture.render(particle->position.x, particle->position.y, &particle->bulletClip, 0, 0, particle->angle);
    }

    void render() {
        renderBullets();
        renderAnimations();
    }

    void addBullet(Vec2d pos, Vec2d dir, double radius, double velocity, int row = 0, int col = 2, int angle = 0, int mode = 0) {
        shared_ptr<EnemyBullet> particle = make_shared<EnemyBullet>(pos, dir, radius, velocity, bulletClip[row][col], angle, mode);
        particles.emplace_back(move(particle));
    }

    shared_ptr<EnemyBullet> getBullet(Vec2d pos, Vec2d dir, double radius, double velocity, int row = 0, int col = 2, int angle = 0) {
        return make_shared<EnemyBullet>(pos, dir, radius, velocity, bulletClip[row][col], angle);
    }

    void clear() {
        vector<shared_ptr<Particle>> newParticles;
        for (auto& particle : particles) {
            SDL_Rect clip = {322, 25, 16, 16};
            shared_ptr<ScoreItem> item = make_shared<ScoreItem>(particle->position, clip);
            newParticles.emplace_back(move(item));
        }
        particles = move(newParticles);
    }

    void playEnemyDeadAnimation(Vec2d pos) {
        animations.emplace_back(make_shared<EnemyDeadAnimation>(pos));
    }

    void playPlayerDeadAnimation(Vec2d pos) {
        animations.emplace_back(make_shared<PlayerDeadAnimation>(pos));
    }

    void playEnemyHitAnimation(Vec2d pos) {
        damage.play();
        for (int i = 0; i < 8; ++i) {
            Vec2d dir = getRandomPoint() - pos;
            dir = dir.normalized();
            animations.emplace_back(make_shared<HitAnimation>(pos, dir));
        }
    }

    void playBossHitAnimation(Vec2d pos) {
        for (int i = 0; i < 8; ++i) {
            Vec2d dir = getRandomPoint() - pos;
            dir = dir.normalized();
            dir.x = -dir.x; dir.y = -dir.y;
            animations.emplace_back(make_shared<HitAnimation>(pos, dir));
        }
    }


    void dropPowerItem(Vec2d pos) {
        SDL_Rect clip = {306, 25, 16, 16};
        shared_ptr<PowerItem> item = make_shared<PowerItem>(pos, clip);
        particles.emplace_back(move(item));
    }

    void dropScoreItem(Vec2d pos) {
        SDL_Rect clip = {322, 25, 16, 16};
        shared_ptr<ScoreItem> item = make_shared<ScoreItem>(pos, clip);
        particles.emplace_back(move(item));
    }

};