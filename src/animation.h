#pragma once

#include "player.h"

class Animation {
public:
    Vec2d position, direction;
    int frame = 0;
    Animation() = default;
    Animation(Vec2d position): position(position) {}
    Animation(Vec2d position, Vec2d direction): position(position), direction(direction) {}

    virtual bool update() { return 1; }
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
    int type;
    EnemyDeadAnimation(Vec2d position, int type = 0): Animation(position), type(type) {}
    bool update() override {
        if (frame >= 8) return 0;
        frame++;
        return 1;
    }
    void render(Texture& texture) override {
        texture.setAlpha((1.0 - (frame / 8.0)) * 255);
        clip.x += type * 32;
        texture.render(position.x, position.y, &clip, clip.w + frame * 4, clip.h + frame * 4, frame * 32);
        clip.x -= type * 32;
        texture.setAlpha(255);
    }
};

class HitAnimation : public Animation {
public:
    SDL_Rect clip[2] = {{89, 58, 16, 16}, {217, 58, 16, 16}};
    int id, totalFrame = 16;
    HitAnimation(Vec2d position, Vec2d direction): Animation(position, direction * random(4, 16)) {
        id = random(0, 1);
    }
    bool update() override {
        if (frame >= totalFrame) return 0;
        frame++;
        position = position + direction * easeOut((double)frame / totalFrame);
        return 1;
    }
    void render(Texture& texture) override {
        int i = random(0, 1);
        double t = double(frame) / totalFrame;
        int w = easeOut(t) * 48;
        texture.setAlpha((1.0 - t) * 255);
        clip[id].x += (frame / 4) * 16;
        texture.setBlendMode(SDL_BLENDMODE_ADD);
        texture.render(position.x, position.y, &clip[id], w, w);
        texture.setBlendMode(SDL_BLENDMODE_BLEND);
        texture.setAlpha(255);
        clip[id].x -= (frame / 4) * 16;
    }
};

class GrazeAnimation : public Animation {
public:
    SDL_Rect clip =  {25, 58, 16, 16};
    int totalFrame = 16;
    GrazeAnimation(Vec2d position, Vec2d direction): Animation(position, direction * random(4, 16)) {}
    bool update() override {
        if (frame >= totalFrame) return 0;
        frame++;
        position = position + direction * easeOut((double)frame / totalFrame);
        return 1;
    }
    void render(Texture& texture) override {
        int i = random(0, 1);
        double t = double(frame) / totalFrame;
        int w = easeIn(t) * 24;
        texture.setAlpha((1.0 - t) * 255);
        clip.x += (frame / 4) * 16;
        texture.setBlendMode(SDL_BLENDMODE_ADD);
        texture.render(position.x, position.y, &clip, w, w);
        texture.setBlendMode(SDL_BLENDMODE_BLEND);
        texture.setAlpha(255);
        clip.x -= (frame / 4) * 16;
    }
};

class PopAnimation : public Animation {
public:
    SDL_Rect clip = {450 + 16, 137, 16, 16};
    PopAnimation(Vec2d position, Vec2d direction): Animation(position, direction) {}
    bool update() override {
        if (frame >= 12) return 0;
        frame++;
        position = position + direction * easeOut(frame / 12.0);
        return 1;
    }
    void render(Texture& texture) override {
        texture.setAlpha((1.0 - (frame / 16.0)) * 32);
        texture.render(position.x, position.y, &clip, 24, 24);
        texture.setAlpha(255);
    }
};
