#pragma once

#include <iostream>

#include "audio.h"
#include "enemy.h"
#include "player.h"

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
        SDL_Rect srcRect, dstRect; 
        srcRect.x = 306, srcRect.y = 25, srcRect.w = srcRect.h = 256;
        dstRect.x=0, dstRect.y=scrollingOffset, dstRect.w=WINDOW_WIDTH, dstRect.h=WINDOW_HEIGHT;
        background.render(&srcRect, &dstRect);
        dstRect.x=0, dstRect.y=-WINDOW_HEIGHT + scrollingOffset, dstRect.w=WINDOW_WIDTH, dstRect.h=WINDOW_HEIGHT;
        background.render(&srcRect, &dstRect);
    }

    void renderSideBar() {
        // render fg
        SDL_Rect srcRect, dstRect; 
        srcRect.x = 121, srcRect.y = 105, srcRect.w = 160, srcRect.h = 16;
        dstRect.x = 0, dstRect.w = 160, dstRect.h = 16;
        for (int i = 0; i < 4; ++i, dstRect.x += dstRect.w) {
            dstRect.y = 0; foreground.render(&srcRect, &dstRect);
            dstRect.y = WINDOW_HEIGHT - 16; foreground.render(&srcRect, &dstRect);
        }
        srcRect.x = 25, srcRect.y = 249, srcRect.w = 32, srcRect.h = 32;
        dstRect.y = 16, dstRect.w = 32, dstRect.h = 32;
        for (int i = 0, j; i < 14; ++i, dstRect.y += dstRect.h) {
            dstRect.x = 0; foreground.render(&srcRect, &dstRect);
            for (dstRect.x = dstRect.w * 13, j = 0; j < 7; ++j, dstRect.x += dstRect.w) {
                foreground.render(&srcRect, &dstRect);
            }
        }


        srcRect.x = 153, srcRect.y = 153, srcRect.w = 128, srcRect.h = 128;
        dstRect.x = dstRect.x = 463, dstRect.y = 312, dstRect.w = 128, dstRect.h = 128;
        foreground.render(&srcRect, &dstRect);

    }

    void renderPlayerHighScore() {
        SDL_Rect srcRect, dstRect; 

        srcRect.x = 25, srcRect.y = 216, srcRect.w = 64, srcRect.h = 16;
        dstRect.x = 8 * 53, dstRect.y = 8 * 7, dstRect.w = 64, dstRect.h = 16;
        foreground.render(&srcRect, &dstRect);

        // 25, 73 + 16 * num, 16, 16
        srcRect.x = 25, srcRect.y = 73, srcRect.w = 16, srcRect.h = 16;
        dstRect.x = 8 * 53 + 64, dstRect.y = 8 * 7, dstRect.w = 16, dstRect.h = 16;

        vector<int> nums;
        int score = 0;
        for (int i = 0; i < 9; ++i) nums.emplace_back(score % 10), score /= 10;
        reverse(nums.begin(), nums.end());
        for (int num : nums) {
            srcRect.x = 25 + 16 * num;
            font.render(&srcRect, &dstRect);
            dstRect.x += 16;
        }
        
    }

    void renderPlayerScore() {
        SDL_Rect srcRect, dstRect; 

        srcRect.x = 25, srcRect.y = 232, srcRect.w = 32, srcRect.h = 16;
        dstRect.x = 8 * 53, dstRect.y = 8 * 10, dstRect.w = 32, dstRect.h = 16;
        foreground.render(&srcRect, &dstRect);

        // 25, 73 + 16 * num, 16, 16
        srcRect.x = 25, srcRect.y = 73, srcRect.w = 16, srcRect.h = 16;
        dstRect.x = 8 * 53 + 64, dstRect.w = 16, dstRect.h = 16;

        vector<int> nums;
        int score = player.score;
        for (int i = 0; i < 9; ++i) nums.emplace_back(score % 10), score /= 10;
        reverse(nums.begin(), nums.end());
        for (int num : nums) {
            srcRect.x = 25 + 16 * num;
            font.render(&srcRect, &dstRect);
            dstRect.x += 16;
        }
        
    }

    void renderPlayerLives() {
        // render text 25, 169, 48, 16
        SDL_Rect srcRect, dstRect; 
        srcRect.x = 25, srcRect.y = 201, srcRect.w = 48, srcRect.h = 16;
        dstRect.x = 8 * 53, dstRect.y = 8 * 15, dstRect.w = 48, dstRect.h = 16;
        foreground.render(&srcRect, &dstRect);
        srcRect.x = 57, srcRect.y = 265, srcRect.w = 16, srcRect.h = 16;
        dstRect.x = 8 * 53 + 64; dstRect.w = 16, dstRect.h = 16;
        for (int i = 0; i < player.lives; ++i, dstRect.x += 16 + 4) {
            foreground.render(&srcRect, &dstRect);
        }
    }

    void renderPlayerBomb() {
        SDL_Rect srcRect, dstRect; 
        srcRect.x = 25, srcRect.y = 201 - 16, srcRect.w = 48, srcRect.h = 16;
        dstRect.x = 8 * 53, dstRect.y = 8 * 18, dstRect.w = 48, dstRect.h = 16;
        foreground.render(&srcRect, &dstRect);
    }


    void renderPlayerPower() {
        // 57, 232, 48, 16
        SDL_Rect srcRect, dstRect; 

        srcRect.x = 57, srcRect.y = 232, srcRect.w = 48, srcRect.h = 16;
        dstRect.x = 8 * 53, dstRect.y = 8 * 23, dstRect.w = 48, dstRect.h = 16;
        foreground.render(&srcRect, &dstRect);

        vector<int> nums;
        int power = player.power;
        while (power > 0) nums.emplace_back(power % 10), power /= 10;
        reverse(nums.begin(), nums.end());
        if (nums.empty()) nums.emplace_back(0);

        srcRect.x = 25, srcRect.y = 73, srcRect.w = 16, srcRect.h = 16;
        dstRect.x = 8 * 53 + 64, dstRect.w = 16, dstRect.h = 16;
        for (int num : nums) {
            srcRect.x = 25 + 16 * num;
            font.render(&srcRect, &dstRect);
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
    SDL_Rect srcRect = {690, 530, 640, 480}, dstRect = {0, 0, 650, 480};

    int gameStarted = 0;

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
        background.render(&srcRect, &dstRect);
    }
};