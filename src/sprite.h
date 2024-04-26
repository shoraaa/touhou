#pragma once

#include "utils.h"
#include "texture.h"

class Sprite {
public:
    Texture texture;
    SDL_Rect clip;

    int positionAnimationType = 0;
    Vec2d position, direction, initialPosition;
    int currentPositionTick = 0, totalPositionTick = -1;

    int width = 0, height = 0;
    
    int angleAnimationType = 0;
    int angle = 0, angleDelta = 0, intitalAngle = 0;
    int currentAngleTick = 0, totalAngleTick = -1;

    Sprite() = default;
    Sprite(string filename, SDL_Rect clip): clip(clip) {
        texture.load(filename);
    }

    void update() {
        if (currentPositionTick <= totalPositionTick) updatePosition();
        if (currentAngleTick <= totalAngleTick) updateAngle();

    }

    void updatePosition() {
        double t = min(1.0, (double)currentPositionTick / totalPositionTick);
        t = animate(positionAnimationType, t);
        position = initialPosition + direction * t;
        currentPositionTick++;
    }

    void updateAngle() {
        double t = min(1.0, (double)currentAngleTick / totalAngleTick);
        t = animate(angleAnimationType, t);
        angle = intitalAngle + angleDelta * t;
        currentAngleTick++;
    }

    void finishAnimation() {
        if (currentPositionTick <= totalPositionTick) position = initialPosition + direction;
        if (currentAngleTick <= totalAngleTick) angle = intitalAngle + angleDelta;
    }

    void setPosition(Vec2d dest, int tick = 1, int type = 0) {
        direction = dest - position; initialPosition = position;
        currentPositionTick = 1, totalPositionTick = tick;
        positionAnimationType = type;
    }

    void setAngle(int dest, int tick = 1, int type = 0) {
        angleDelta = dest - angle; intitalAngle = angle;
        currentAngleTick = 1, totalAngleTick = tick;
        angleAnimationType = type;
    }

    void render() {
        texture.render(position.x, position.y, &clip, width, height, angle);
    }
};