#ifndef UTILS_H_
#define UTILS_H_
#include "utils.h"
#endif /* UTILS_H_ */

#ifndef TEXTURE_H_
#define TEXTURE_H_
#include "texture.h"
#endif /* TEXTURE_H_ */


class Particle {
    public:
    Vec2d initialPosition, position;
    int elapsedTime = 0;
    int type, hit = 0;
    int radius = 5;
    Particle() = default;
    Particle(Vec2d pos, int radius): initialPosition(pos), position(pos), radius(radius) {}

    bool inBound() {
        return position.inPlayField();
    }
    
    bool collide(Vec2d other) {
        auto x = position.x, y = position.y;
        auto px = other.x, py = other.y;
        if ((x - px) * (x - px) + (y - py) * (y - py) <= radius * radius) {
            return 1;
        }
        return 0;
    }

    virtual void update() {}
};

class LinearParticle : public Particle {
    // vector linear
    public:
    Vec2d direction;
    double velocity = 1.0;
    LinearParticle(Vec2d pos, Vec2d dir, double radius, double vel): Particle(pos, radius), direction(dir), velocity(vel) {}
    void update() override {
        position = initialPosition + (direction * velocity * elapsedTime);
        elapsedTime++;
    }
};

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