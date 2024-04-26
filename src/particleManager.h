#pragma once

#include "player.h"
#include "utils.h"
#include "animation.h"

extern Player player;
extern double scrollingSpeed;

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

    void hitPlayer() {
        if (player.gotHit()) {
            playPlayerDeadAnimation(player.position);
            clear();
            
        }
    }

    void updateBullets() {
        vector<shared_ptr<Particle>> newParticles;
        for (auto& particle : particles) {
            particle->update();
            if (!particle->shouldDelete()) {
                newParticles.emplace_back(move(particle));
            }
        }
        particles = move(newParticles);

        for (auto& particle : particles) {
            if (particle->collide(player.position)) {
                particle->hit = 1;
                if (particle->type == 0) { // bullet
                    hitPlayer();
                } else if (particle->type == 1) { // score
                    player.increaseChi(1);
                } else if (particle->type == 2) { // power
                    player.increasePower(1);
                }
            } else if (particle->position.distance(player.position) <= 25 * 25) {
                player.gotGraze();
                playPlayerGrazeAnimation(player.position);
            }
        }
    }

    void updateAnimation() {
        vector<shared_ptr<Animation>> newAnimations;
        for (auto& animation : animations) {
            if (animation->update()) {
                newAnimations.emplace_back(animation);
            }
        }
        animations = move(newAnimations);
    }

    int currentPowerLv = 1;

    void update() {
        updateBullets();
        updateAnimation();

        if (player.getPowerLv() != currentPowerLv) {
            playPowerUpAnimation(player.position);
            currentPowerLv = player.getPowerLv();
        }

        if (player.requestClear) {
            player.requestClear = 0;
            playPlayerDeadAnimation(player.position);
            clear();

            SDL_Rect clip = {306, 25, 16, 16};
            Vec2d pos = player.position; pos.y -= 160;
            for (int i = 0; i < 10; ++i) {
                shared_ptr<PowerItem> item = make_shared<PowerItem>(pos, clip);
                particles.emplace_back(item);
            }
        }

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

    shared_ptr<EnemyBullet> addBullet(Vec2d pos, Vec2d dir, double radius, double velocity, int row = 0, int col = 2, int angle = 0, int mode = 0) {
        shared_ptr<EnemyBullet> particle = make_shared<EnemyBullet>(pos, dir, radius, velocity, bulletClip[row][col], angle, mode);
        particles.emplace_back(particle);
        return particle;
    }



    void clear() {
        for (auto& particle : particles) {
            animations.emplace_back(make_shared<EnemyDeadAnimation>(particle->position));
        }
        particles.clear();


    }

    void playEnemyDeadAnimation(Vec2d pos, int type = 0) {
        animations.emplace_back(make_shared<EnemyDeadAnimation>(pos, type));
    }

    void playPlayerDeadAnimation(Vec2d pos) {
        animations.emplace_back(make_shared<PlayerDeadAnimation>(pos));
    }

    void playPlayerGrazeAnimation(Vec2d pos) {
        Vec2d dir = Vec2d(0, 1);
        double a = random(1, 360);
        double x = dir.length() * cos(a);
        double y = dir.length() * sin(a);
        animations.emplace_back(make_shared<GrazeAnimation>(pos, Vec2d(x, y)));
    }

    void playEnemyHitAnimation(Vec2d pos) {
        for (int i = 0; i < 4; ++i) {
            Vec2d dir = Vec2d(0, 1);
            double a = random(1, 360);
            double x = dir.length() * cos(a);
            double y = dir.length() * sin(a);
            animations.emplace_back(make_shared<HitAnimation>(pos, Vec2d(x, y)));
            animations.emplace_back(make_shared<PopAnimation>(pos, Vec2d(0, -1)));
        }
    }

    void playPowerUpAnimation(Vec2d pos) {
        // animations.emplace_back(make_shared<PowerUpAnimation>(pos, Vec2d(0, -1)));
    }

    void dropPowerItem(Vec2d pos) {
        SDL_Rect clip = {306, 25, 16, 16};
        shared_ptr<PowerItem> item = make_shared<PowerItem>(pos, clip);
        particles.emplace_back(item);
    }

    void dropScoreItem(Vec2d pos) {
        SDL_Rect clip = {322, 25, 16, 16};
        shared_ptr<ScoreItem> item = make_shared<ScoreItem>(pos, clip);
        particles.emplace_back(item);
    }

};