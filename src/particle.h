#pragma once

#include "utils.h"
#include "texture.h"
#include "player.h"
class Particle {
    public:
    Vec2d initialPosition, position;
    int elapsedTime = 0, hit = 0, radius = 5, type = -1;
    SDL_Rect srcRect;

    Particle() = default;
    Particle(Vec2d pos, int radius, int type): initialPosition(pos), position(pos), radius(radius), type(type) {}

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
    LinearParticle(Vec2d pos, Vec2d dir, double radius, double vel): Particle(pos, radius, 0), direction(dir), velocity(vel) {
        srcRect.x = 322, srcRect.y = 57, srcRect.w = 16, srcRect.h = 16;
    }
    void update() override {
        position = initialPosition + (direction * velocity * elapsedTime);
        elapsedTime++;
    }
};

#define ITEM_VELOCITY 5
#define GRAVITY 1
#define ITEM_RADIUS 16
class ScoreItem : public Particle {
public:
    ScoreItem(Vec2d pos): Particle(pos, ITEM_RADIUS, 1) {
        srcRect.x = 322, srcRect.y = 25, srcRect.w = 16, srcRect.h = 16;
    }
    void update() override {
        position.y = initialPosition.y - (ITEM_VELOCITY * elapsedTime / 60.0) + (GRAVITY * elapsedTime * elapsedTime) / (2.0 * 60.0);
        elapsedTime++;
    }
};

class PowerItem : public Particle {
public:
    PowerItem(Vec2d pos): Particle(pos, ITEM_RADIUS, 2) {
        srcRect.x = 306, srcRect.y = 25, srcRect.w = 16, srcRect.h = 16;
    }
    void update() override {
        position.y = initialPosition.y - (ITEM_VELOCITY * elapsedTime / 60.0) + (GRAVITY * elapsedTime * elapsedTime) / (2.0 * 60.0);
        elapsedTime++;
    }
};


