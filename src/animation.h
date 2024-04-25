#pragma once

#include "player.h"

class Animation {
public:
    Vec2d position, direction;
    int frame = 0;
    Animation() = default;
    Animation(Vec2d position): position(position) {}
    Animation(Vec2d position, Vec2d direction): position(position), direction(direction) {}

    virtual bool update() {}
    virtual void render(Texture& texture) {}
};

class PlayerDeadAnimation : public Animation {
public:
    SDL_Rect clip = {25, 25, 32, 32};
    PlayerDeadAnimation(Vec2d position): Animation(position) {}
    bool update() override {
        if (frame >= 60) return 0;
        frame++;
        return 1;
    }
    void render(Texture& texture) override {
        if (frame >= 60) return;
        texture.setAlpha((1.0 - (frame / 8.0)) * 255);
        texture.render(position.x, position.y, &clip, clip.w + frame * 32, clip.h + frame * 32);
        texture.setAlpha(255);
        
    }
};

class EnemyDeadAnimation : public Animation {
public:
    SDL_Rect clip = {25, 25, 32, 32};
    EnemyDeadAnimation(Vec2d position): Animation(position) {}
    bool update() override {
        if (frame >= 8) return 0;
        frame++;
        return 1;
    }
    void render(Texture& texture) override {
        texture.setAlpha((1.0 - (frame / 8.0)) * 255);
        texture.render(position.x, position.y, &clip, clip.w + frame * 4, clip.h + frame * 4, frame * 32);
        texture.setAlpha(255);
    }
};

class HitAnimation : public Animation {
public:
    SDL_Rect clip[2] = {{89, 58, 16, 16}, {217, 58, 16, 16}};
    HitAnimation(Vec2d position, Vec2d direction): Animation(position, direction) {}
    bool update() override {
        if (frame >= 12) return 0;
        frame++;
        position = position + direction * 8 * easeOut(frame / 12.0);
        return 1;
    }
    void render(Texture& texture) override {
        texture.setAlpha((1.0 - (frame / 16.0)) * 200);
        clip.x = 217 + (frame / 4) * 16;
        texture.render(position.x, position.y, &clip[random(0, 1)]);
        texture.setAlpha(255);
    }
};