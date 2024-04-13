#ifndef PLAYER_H_
#define PLAYER_H_
#include "player.h"
#endif /* PLAYER_H_ */

extern Player player;
extern int PLAYER_LOST;
struct ParticleManager {
    string texturePath;
    Texture texture;
    int delta = 30;

    SDL_Rect srcRect;

    vector<unique_ptr<Particle>> particles;
    #define PARTICLE_WIDTH 16
    #define PARTICLE_HEIGHT 16

    void initialize() {
        texture.load("particle");

        srcRect.x = 322, srcRect.y = 57, srcRect.w = PARTICLE_WIDTH, srcRect.h = PARTICLE_HEIGHT;
    }

    void push(Vec2d pos, int radius, int velocity) {
        Vec2d position = pos;
        Vec2d direction = player.position - position;
        direction = direction * (1.0 / direction.length());

        unique_ptr<LinearParticle> particle = make_unique<LinearParticle>(position, direction, radius, velocity);
        particles.emplace_back(move(particle));
    }

    void update() {
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
                PLAYER_LOST = 1;
            }
        }

    }

    void render() {
        for (auto& particle : particles) {
            texture.render(particle->position.x - PARTICLE_WIDTH / 2, particle->position.y - PARTICLE_HEIGHT / 2, &srcRect);
        }
    }

};