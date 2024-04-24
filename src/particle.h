#pragma once

#include "const.h"
#include "utils.h"
#include "texture.h"
#include "player.h"
class Particle {
    public:
    Vec2d initialPosition, position;
    int elapsedTime = 0, totalTime = 0;
    int hit = 0, radius = 5, type = -1;
    SDL_Rect srcRect;

    Particle() = default;
    Particle(Vec2d pos, int radius, int type): initialPosition(pos), position(pos), radius(radius), type(type) {}

    bool inBound() {
        return position.inPlayField();
    }

    double easeOut() {
        double p = 3.0;
        return 1 - pow(1 - ((double)elapsedTime / totalTime), p);
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
    LinearParticle(Vec2d pos, Vec2d dir, double radius, double vel): Particle(pos, radius, 0), direction(dir * vel) {
        srcRect.x = 322, srcRect.y = 57, srcRect.w = 16, srcRect.h = 16;

        totalTime = std::numeric_limits<int>::max();

        if (direction.x != 0.0) {
            int timeToReachXEdge = (direction.x > 0.0) ? (FIELD_X2 - pos.x) / direction.x : (FIELD_X - pos.x) / direction.x;
            totalTime = min(totalTime, timeToReachXEdge);
        }

        if (direction.y != 0.0) {
            int timeToReachYEdge = (direction.y > 0.0) ? (FIELD_Y2 - pos.y) / direction.y : (FIELD_Y - pos.y) / direction.y;
            totalTime = min(totalTime, timeToReachYEdge);
        }

    }
    void update() override {
        position = position + direction * easeOut();
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


