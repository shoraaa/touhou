#pragma once

#include <iostream>

#include "audio.h"
#include "enemy.h"
#include "player.h"
#include "sprite.h"

#include <algorithm>
#include <vector>
using namespace std;

extern EnemyManager enemyManager;
extern Player player;
extern ParticleManager particleManager;

struct Scene_Gameplay {

    Texture background, foreground, font;
    BGM bgm;

    double scrollingOffset = 0;

    void initialize() {
        background.load("background");
        foreground.load("foreground");
        font.load("font");

        bgm.load("th06_02");
        player.initialize();
        enemyManager.initialize();
        particleManager.initialize();
    }

    void start() {
        cout << "Playing BGM\n";
        bgm.play();
    }

    void handleInput(const SDL_Event& e) {
        player.handleInput(e);
    }

    void update() {
        scrollingOffset += 0.5;
        if (scrollingOffset >= WINDOW_HEIGHT) {
            scrollingOffset = 0;
        }
        elapsed_frame++;

        player.update();
        enemyManager.update();

        particleManager.update();
    }

    void renderBG() {
        SDL_Rect clip, dstRect; 
        clip.x = 306, clip.y = 25, clip.w = clip.h = 256;
        dstRect.x=0, dstRect.y=scrollingOffset, dstRect.w=WINDOW_WIDTH, dstRect.h=WINDOW_HEIGHT;
        background.render(&clip, &dstRect);
        dstRect.x=0, dstRect.y=-WINDOW_HEIGHT + scrollingOffset, dstRect.w=WINDOW_WIDTH, dstRect.h=WINDOW_HEIGHT;
        background.render(&clip, &dstRect);
    }

    void renderSideBar() {
        // render fg
        SDL_Rect clip, dstRect; 
        clip.x = 121, clip.y = 105, clip.w = 160, clip.h = 16;
        dstRect.x = 0, dstRect.w = 160, dstRect.h = 16;
        for (int i = 0; i < 4; ++i, dstRect.x += dstRect.w) {
            dstRect.y = 0; foreground.render(&clip, &dstRect);
            dstRect.y = WINDOW_HEIGHT - 16; foreground.render(&clip, &dstRect);
        }
        clip.x = 25, clip.y = 249, clip.w = 32, clip.h = 32;
        dstRect.y = 16, dstRect.w = 32, dstRect.h = 32;
        for (int i = 0, j; i < 14; ++i, dstRect.y += dstRect.h) {
            dstRect.x = 0; foreground.render(&clip, &dstRect);
            for (dstRect.x = dstRect.w * 13, j = 0; j < 7; ++j, dstRect.x += dstRect.w) {
                foreground.render(&clip, &dstRect);
            }
        }


        clip.x = 153, clip.y = 153, clip.w = 128, clip.h = 128;
        dstRect.x = dstRect.x = 463, dstRect.y = 312, dstRect.w = 128, dstRect.h = 128;
        foreground.render(&clip, &dstRect);

    }

    void renderPlayerHighScore() {
        SDL_Rect clip, dstRect; 

        clip = {25, 216, 64, 16};
        dstRect = {8 * 53, 8 * 7, 64, 16};
        foreground.render(&clip, &dstRect);

        // 25, 73 + 16 * num, 16, 16
        clip = {25, 73, 16, 16};
        dstRect = {8 * 53 + 64, 8 * 7, 16, 16};

        vector<int> nums;
        int score = 0;
        for (int i = 0; i < 9; ++i) nums.emplace_back(score % 10), score /= 10;
        reverse(nums.begin(), nums.end());
        for (int num : nums) {
            clip.x = 25 + 16 * num;
            font.render(&clip, &dstRect);
            dstRect.x += 16;
        }
        
    }

    void renderPlayerScore() {
        SDL_Rect clip, dstRect; 

        clip = {25, 232, 32, 16};
        dstRect = {8 * 53, 8 * 10, 32, 16};
        foreground.render(&clip, &dstRect);

        // 25, 73 + 16 * num, 16, 16
        clip = {25, 73, 16, 16};
        dstRect = {8 * 53 + 64, 8 * 7, 16, 16};

        vector<int> nums;
        int score = player.score;
        for (int i = 0; i < 9; ++i) nums.emplace_back(score % 10), score /= 10;
        reverse(nums.begin(), nums.end());
        for (int num : nums) {
            clip.x = 25 + 16 * num;
            font.render(&clip, &dstRect);
            dstRect.x += 16;
        }
        
    }

    void renderPlayerLives() {
        // render text 25, 169, 48, 16
        SDL_Rect clip, dstRect; 
        clip.x = 25, clip.y = 201, clip.w = 48, clip.h = 16;
        dstRect.x = 8 * 53, dstRect.y = 8 * 15, dstRect.w = 48, dstRect.h = 16;
        foreground.render(&clip, &dstRect);
        clip.x = 57, clip.y = 265, clip.w = 16, clip.h = 16;
        dstRect.x = 8 * 53 + 64; dstRect.w = 16, dstRect.h = 16;
        for (int i = 0; i < player.lives; ++i, dstRect.x += 16 + 4) {
            foreground.render(&clip, &dstRect);
        }
    }

    void renderPlayerBomb() {
        SDL_Rect clip, dstRect; 
        clip.x = 25, clip.y = 201 - 16, clip.w = 48, clip.h = 16;
        dstRect.x = 8 * 53, dstRect.y = 8 * 18, dstRect.w = 48, dstRect.h = 16;
        foreground.render(&clip, &dstRect);
    }


    void renderPlayerPower() {
        // 57, 232, 48, 16
        SDL_Rect clip, dstRect; 

        clip.x = 57, clip.y = 232, clip.w = 48, clip.h = 16;
        dstRect.x = 8 * 53, dstRect.y = 8 * 23, dstRect.w = 48, dstRect.h = 16;
        foreground.render(&clip, &dstRect);

        vector<int> nums;
        int power = player.power;
        while (power > 0) nums.emplace_back(power % 10), power /= 10;
        reverse(nums.begin(), nums.end());
        if (nums.empty()) nums.emplace_back(0);

        clip.x = 25, clip.y = 73, clip.w = 16, clip.h = 16;
        dstRect.x = 8 * 53 + 64, dstRect.w = 16, dstRect.h = 16;
        for (int num : nums) {
            clip.x = 25 + 16 * num;
            font.render(&clip, &dstRect);
            dstRect.x += 16;
        }
    }
    
    void renderFG() {
        renderSideBar();

        renderPlayerScore();
        renderPlayerHighScore();

        renderPlayerLives();
        renderPlayerBomb();

        renderPlayerPower();


    }

    void render() {
        renderBG();

        player.render();
        enemyManager.render();
        particleManager.render();

        renderFG();
    }
};
struct Scene_MainMenu {
    BGM bgm;

    Texture background;
    SDL_Rect clip = {690, 530, 640, 480}, dstRect = {0, 0, 650, 480};

    int gameStarted = 0;

    vector<Sprite> title;

    void initialize() {
        background.load("title");
        bgm.load("th06_01");

        bgm.play();
    }

    void handleInput(const SDL_Event& e) {
        const auto key = e.key.keysym.sym; 
        if (key == SDLK_RETURN) {
            gameStarted = 1;
            bgm.stop();
            cout << "Game Start!\n";
        }
    }

    void update() {

    }

    void render() {
        background.render(&clip, &dstRect);
    }
};