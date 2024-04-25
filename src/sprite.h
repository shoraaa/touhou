#pragma once

#include "utils.h"
#include "texture.h"

class Sprite {
public:
    Texture texture;
    SDL_Rect clip;

    int elapsedTime, totalTime, mode;

    Vec2d position, direction;
    void initialize(string filename, SDL_Rect clip) {
        texture.load(filename);
        this->clip = clip;
        elapsedTime = 0;
        totalTime = 0;
        mode = 0;
    }

    void update() {
        double t = (double)elapsedTime / totalTime;
        if (mode == 1) t = easeIn(t, 2);
        else if (mode == 2) t = easeOut(t, 2);
        position = position + direction * t;
        position = position + direction;
        elapsedTime++;
    }

    void render() {
        texture.render(position.x, position.y, &clip);
    }
};