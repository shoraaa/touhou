#pragma once

#include "player.h"

#include "utils.h"

extern Player player;

struct HitAnimation {
    Vec2d pos, dir;
    int frame = 0;
    HitAnimation() = default;
    HitAnimation(Vec2d pos): pos(pos) {}
    HitAnimation(Vec2d pos, Vec2d dir): pos(pos), dir(dir) {}
};
struct ParticleManager {
    string texturePath;
    Texture texture;
    int delta = 30;

    SDL_Rect deadRect, hitParticles, deadParticles;

    vector<unique_ptr<Particle>> particles;

    vector<HitAnimation> enemyDeadAnimation, enemyHitParticles, enemyDeadParticles;
    HitAnimation playerDeadAnimation;

    void initialize() {
        texture.load("particle"); 
        deadRect = {25, 25, 32, 32}; 
        hitParticles = {217, 58, 16, 16};
        deadParticles = {89, 58, 16, 16};
        playerDeadAnimation.frame = 120;
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
                    if (player.gotHit()) {
                        playPlayerDeadAnimation(player.position);
                    }
                } else if (particle->type == 1) { // score
                    player.increaseScore(10);
                } else if (particle->type == 2) { // power
                    player.increasePower(1);
                }
            }
        }
    }

    void updateHitParticles() {
        vector<HitAnimation> newEnemyHitParticles;
        for (auto& a : enemyHitParticles) {
            a.frame++;
            a.pos = a.pos + (a.dir * 8);
            if (a.frame < 16) {
                newEnemyHitParticles.emplace_back(a);
            }

        }
        enemyHitParticles = newEnemyHitParticles;
    }

    void updateDeadAnimation() {
        vector<HitAnimation> newEnemyDeadParticles;
        for (auto& a : enemyDeadParticles) {
            a.frame++;
            a.pos = a.pos + (a.dir * 8);
            if (a.frame < 16) {
                newEnemyDeadParticles.emplace_back(a);
            }

        }
        enemyDeadParticles = newEnemyDeadParticles;

        vector<HitAnimation> newEnemyDeadAnimation;
        for (auto& a : enemyDeadAnimation) {
            a.frame++;
            if (a.frame < 8) {
                newEnemyDeadAnimation.emplace_back(a);
            }

        }
        enemyDeadAnimation = newEnemyDeadAnimation;

        if (playerDeadAnimation.frame < 60) {
            playerDeadAnimation.frame++;
        }
    }

    void update() {
        updateBullets();
        updateHitParticles();
        updateDeadAnimation();

    }

    void renderBullets() {
        for (auto& particle : particles) {
            texture.render(particle->position.x, particle->position.y, &particle->srcRect);
        }
    }

    void renderHitParticles() {
        for (auto& a : enemyHitParticles) {
            texture.setAlpha((1.0 - (a.frame / 16.0)) * 155);
            hitParticles.x = 217 + (a.frame / 4) * 16;
            texture.render(a.pos.x, a.pos.y, &hitParticles);
            texture.setAlpha(255);
        }
    }

    void renderDeadAnimation() {
        for (auto& a : enemyDeadAnimation) {
            texture.setAlpha((1.0 - (a.frame / 8.0)) * 255);
            texture.render(a.pos.x, a.pos.y, &deadRect, deadRect.w + a.frame * 4, deadRect.h + a.frame * 4, a.frame * 32);
            texture.setAlpha(255);

        }

        for (auto& a : enemyDeadParticles) {
            texture.setAlpha((1.0 - (a.frame / 16.0)) * 155);
            deadParticles.x = 89 + (a.frame / 4) * 16;
            texture.render(a.pos.x, a.pos.y, &deadParticles);
            texture.setAlpha(255);
        }

        if (playerDeadAnimation.frame < 60) {
            auto a = playerDeadAnimation;
            texture.setAlpha((1.0 - (a.frame / 8.0)) * 255);
            texture.render(a.pos.x, a.pos.y, &deadRect, deadRect.w + a.frame * 32, deadRect.h + a.frame * 32);
            texture.setAlpha(255);
        }
    }

    void render() {
        renderBullets();
        renderHitParticles();
        renderDeadAnimation();
    }

    void addBullet(Vec2d pos, Vec2d dir, double radius, double velocity) {
        unique_ptr<EnemyBullet> particle = make_unique<EnemyBullet>(pos, dir, radius, velocity);
        particles.emplace_back(move(particle));
    }


    void playEnemyDeadAnimation(Vec2d pos) {
        enemyDeadAnimation.emplace_back(pos);

        Vec2d dir = getRandomPoint() - pos;
        dir = dir.normalized();
        enemyDeadParticles.emplace_back(pos, dir);
    }

    void playPlayerDeadAnimation(Vec2d pos) {
        playerDeadAnimation = HitAnimation(pos);
    }

    void playEnemyHitAnimation(Vec2d pos) {
        for (int i = 0; i < 8; ++i) {
            Vec2d dir = getRandomPoint() - pos;
            dir = dir.normalized();
            enemyHitParticles.emplace_back(pos, dir);
        }
    }

    void playBossHitAnimation(Vec2d pos) {
        for (int i = 0; i < 8; ++i) {
            Vec2d dir = getRandomPoint() - pos;
            dir = dir.normalized();
            dir.x = -dir.x; dir.y = -dir.y;
            enemyDeadParticles.emplace_back(pos, dir);
        }
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