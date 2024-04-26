#pragma once

#include "const.h"
#include "utils.h"
#include "texture.h"
#include "player.h"
class Particle {
    public:
    Vec2d initialPosition, position;
    int elapsedTime = 0, totalTime = 0;
    int hit = 0, radius = 5, type = -1, angle = 0;
    SDL_Rect bulletClip;

    Particle() = default;
    Particle(Vec2d pos, int radius, int type, SDL_Rect bulletClip): initialPosition(pos), position(pos), radius(radius), type(type), bulletClip(bulletClip) {}

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

    bool shouldDelete() {
        return elapsedTime >= 60 * 15 || hit;
    }

    virtual void update() {}
};

class EnemyBullet : public Particle {
    // vector linear
    public:
    Vec2d direction;
    int mode;
    EnemyBullet(Vec2d pos, Vec2d dir, double radius, double vel, SDL_Rect bulletClip, int angle, int mode = 2): Particle(pos, radius, 0, bulletClip), direction(dir * vel) {
        this->angle = angle;
        this->mode = mode;
        totalTime = 120;

    }
    void update() override {
        double t = min(1.0, (double)elapsedTime / totalTime);
        if (mode == 1) t = easeIn(t, 2);
        else if (mode == 2) t = easeOut(t, 2);
        position = position + direction * t;
        elapsedTime++;
    }
};

class PlayerBullet : public Particle {
    // vector linear
    public:
    Vec2d direction;
    PlayerBullet(Vec2d pos, Vec2d dir, double radius, double vel, SDL_Rect bulletClip = SDL_Rect()): Particle(pos, radius, 0, bulletClip), direction(dir * vel) {
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
        position = position + direction;
        elapsedTime++;
    }
};

#define ITEM_VELOCITY 5
#define GRAVITY 1
#define ITEM_RADIUS 16
class ScoreItem : public Particle {
public:
    ScoreItem(Vec2d pos, SDL_Rect bulletClip): Particle(pos, ITEM_RADIUS, 1, bulletClip) {
        bulletClip.x = 322, bulletClip.y = 25, bulletClip.w = 16, bulletClip.h = 16;
    }
    void update() override {
        position.y = initialPosition.y - (ITEM_VELOCITY * elapsedTime / 60.0) + (GRAVITY * elapsedTime * elapsedTime) / (2.0 * 60.0);
        elapsedTime++;
    }
};

class PowerItem : public Particle {
public:
    int velocity;
    PowerItem(Vec2d pos, SDL_Rect bulletClip): Particle(pos, ITEM_RADIUS, 2, bulletClip), velocity(ITEM_VELOCITY) {
        bulletClip.x = 306, bulletClip.y = 25, bulletClip.w = 16, bulletClip.h = 16;
    }
    PowerItem(Vec2d pos, SDL_Rect bulletClip, int velocity): Particle(pos, ITEM_RADIUS, 2, bulletClip), velocity(velocity) {
        bulletClip.x = 306, bulletClip.y = 25, bulletClip.w = 16, bulletClip.h = 16;
    }
    void update() override {
        position.y = initialPosition.y - (velocity * elapsedTime / 60.0) + (GRAVITY * elapsedTime * elapsedTime) / (2.0 * 60.0);
        elapsedTime++;
    }
};


