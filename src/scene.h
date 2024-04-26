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

    unique_ptr<Sprite> circle;

    double scrollingOffset = 0;
    int paused = 0;

    int currentPointer = 0;
    vector<unique_ptr<Sprite>> buttons;

    vector<unique_ptr<Sprite>> titles;

    void initialize() {
        background.load("background");
        foreground.load("foreground");
        font.load("font");

        bgm.load("th06_02");
        player.initialize();
        enemyManager.initialize();
        particleManager.initialize();

        titleInit();
        buttonInit();
        circle = make_unique<Sprite>("foreground", SDL_Rect{153, 153, 128, 128});
        circle->position = Vec2d(463 + 64, 312 + 64);
        circle->setAngle(360, 300, 1);
    }

    void titleInit() {
        for (int i = 0, x = 25, y = 25; i < 5; ++i) {
            titles.emplace_back(make_unique<Sprite>("foreground", SDL_Rect{x, y, 64, 64}));
            if (i == 3) {
                x = 25;
                y += 64;
            } else {
                x += 64;
            }

            if (i != 2) titles[i]->texture.setAlpha(155);
        }

        int x = 463 + 64, y = 312 + 64;
        
        titles[0]->position = Vec2d(x - 56, y - 56);
        titles[1]->position = Vec2d(x, y - 56);

        titles[2]->position = Vec2d(x, y);

        titles[3]->position = Vec2d(x, y + 56);
        titles[4]->position = Vec2d(x + 56, y + 56);
    }

    void buttonInit() {
        buttons.emplace_back(make_unique<Sprite>("title", SDL_Rect{1430, 1878, 64 + 8, 32}));
        buttons.emplace_back(make_unique<Sprite>("title", SDL_Rect{1622, 1911, 64, 32}));

        Vec2d pos(FIELD_X + FIELD_WIDTH / 2, FIELD_Y + FIELD_HEIGHT / 2 + 32);
        for (auto& button : buttons) {
            button->position = pos;
            pos.y += 32; 
        }
    }

    void start() {
        cout << "Playing BGM\n";
        bgm.play();
    }

    void handleInput(const SDL_Event& e) {

        const auto key = e.key.keysym.sym; 
        bool is_down = 0;
        switch (e.type) {
            case SDL_KEYDOWN: is_down = 1; break;
            case SDL_KEYUP: is_down = 0; break;
            default: break;
        }
        

        if (!paused) {
            player.handleInput(e);
            
            if (key == SDLK_x) {
                currentPointer = 0;
                paused = 1;
                player.clearInput();
            }

            return;
        }

        if (!is_down) return;

        if (key == SDLK_x) {
            if (!PLAYER_LOST) paused = 0;
        }

        if (key == SDLK_RETURN) {
            if (currentPointer == 0) {
                paused = 0;
                PLAYER_LOST = 0;
                player.lives = 3;
                player.score = 0;
            } else if (currentPointer == 1) {
                SDL_Quit();
            }
        }


        if (currentPointer != -1) {
            if (key == SDLK_DOWN) currentPointer = (currentPointer + 1) % buttons.size();
            if (key == SDLK_UP) currentPointer = (currentPointer - 1 + buttons.size()) % buttons.size();
        }
    }

    void update() {

        if (!paused && PLAYER_LOST) {
            paused = 1;
            player.clearInput();
        }

        if (paused) {
            if (currentPointer == 0) {
                buttons[0]->clip.x = 1430 + 281;
                buttons[1]->clip.x = 1622;
            } else if (currentPointer == 1) {
                buttons[0]->clip.x = 1430;
                buttons[1]->clip.x = 1622 + 281;
            }
            return;
        }

        scrollingOffset += scrollingSpeed;
        if (scrollingOffset >= WINDOW_HEIGHT) {
            scrollingOffset = 0;
        }
        elapsedFrame++;
        circle->update();

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


        circle->render();

        for (auto& title : titles) {
            title->render();
        }
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
        int score = max(player._score, 100000);
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
        dstRect = {8 * 53 + 64, 8 * 10, 16, 16};

        vector<int> nums;
        int score = player._score;
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

    void renderPlayerGraze() {
        SDL_Rect clip, dstRect; 

        clip.x = 57, clip.y = 232 + 16, clip.w = 48, clip.h = 16;
        dstRect.x = 8 * 53, dstRect.y = 8 * 23 + 24, dstRect.w = 48, dstRect.h = 16;
        foreground.render(&clip, &dstRect);

        vector<int> nums;
        int graze = player.graze;
        while (graze > 0) nums.emplace_back(graze % 10), graze /= 10;
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

    void renderPlayerChi() {
        SDL_Rect clip, dstRect; 

        clip.x = 73, clip.y = 185, clip.w = 32, clip.h = 32;
        dstRect.x = 8 * 53 + 24, dstRect.y = 8 * 23 + 24 + 24, dstRect.w = 32, dstRect.h = 32;
        foreground.render(&clip, &dstRect);

        vector<int> nums;
        int chi = player.chi;
        while (chi > 0) nums.emplace_back(chi % 10), chi /= 10;
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
        // renderPlayerBomb();

        renderPlayerPower();
        renderPlayerGraze();
        renderPlayerChi();


    }

    void render() {
        renderBG();

        player.render();
        enemyManager.render();
        particleManager.render();

        renderFG();

        if (paused) {
            SDL_Rect rect = {FIELD_X, FIELD_Y, FIELD_WIDTH, FIELD_HEIGHT};
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            if (PLAYER_LOST) SDL_SetRenderDrawColor(renderer, 155, 0, 0, 100);
            else SDL_SetRenderDrawColor(renderer, 0, 0, 155, 100);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            for (auto& button : buttons) {
                button->render();
            }
        }
    }
};
struct Scene_MainMenu {
    BGM bgm;

    Texture background;
    SDL_Rect clip = {690, 530, 640, 480}, dstRect = {0, 0, 650, 480};

    int gameStarted = 0;

    int currentPointer = -1;

    vector<unique_ptr<Sprite>> titles;
    vector<unique_ptr<Sprite>> buttons;

    void initialize() {
        background.load("title");

        bgm.load("th06_01");
        bgm.play();

        titleInit();
        buttonInit();
    }

    void titleInit() { 
        for (int i = 0, x = 1992, y = 1878; i < 5; ++i) {
            if (i == 2) {
                titles.emplace_back(make_unique<Sprite>("title", SDL_Rect{2273, 1878, 96, 96}));
                x = 1992;
                y += 96;
                continue;
            }
            titles.emplace_back(make_unique<Sprite>("title", SDL_Rect{x, y, 96, 96}));
            x += 96;
        }

        int halfX = WINDOW_WIDTH / 2, halfY = WINDOW_HEIGHT / 2 - 20;
        const int tick = 30;

        titles[2]->setAngle(30, tick);
        
        titles[0]->position = Vec2d(halfX, -96); titles[0]->setPosition(Vec2d(halfX, halfY - 80 * 2), tick, 2);
        titles[1]->position = Vec2d(-96, halfY - 80); titles[1]->setPosition(Vec2d(halfX, halfY - 80), tick, 2);

        titles[2]->position = Vec2d(halfX, halfY);

        titles[3]->position = Vec2d(WINDOW_WIDTH + 96, halfY + 80); titles[3]->setPosition(Vec2d(halfX, halfY + 80), tick, 2);
        titles[4]->position = Vec2d(halfX, WINDOW_HEIGHT + 96 - 20); titles[4]->setPosition(Vec2d(halfX, halfY + 80 * 2), tick, 2);
    }

    void buttonInit() {
        buttons.emplace_back(make_unique<Sprite>("title", SDL_Rect{1430, 1878, 64 + 8, 32}));
        buttons.emplace_back(make_unique<Sprite>("title", SDL_Rect{1622, 1911, 64, 32}));

        Vec2d pos(WINDOW_WIDTH + 72, 224);
        for (auto& button : buttons) {
            button->position = pos;
            pos.y += 32; 
        }
    }

    void startGame() {
        gameStarted = 1;
        bgm.stop();
        cout << "Game Start!\n";
    }

    void loadMenu() {
        currentPointer = 0;

        for (auto& title : titles) {
            title->finishAnimation();
            Vec2d pos = title->position;
            pos.x -= 64 * 3;
            title->setPosition(pos, 60, 2);
        }

        for (auto& button : buttons) {
            Vec2d pos = button->position;
            pos.x -= 64 * 3;
            button->setPosition(pos, 60, 2);
        }
    }

    void handleInput(const SDL_Event& e) {
        const auto key = e.key.keysym.sym; 
        bool is_down = 0;
        switch (e.type) {
            case SDL_KEYDOWN: is_down = 1; break;
            case SDL_KEYUP: is_down = 0; break;
            default: break;
        }
        if (!is_down) return;

        if (key == SDLK_RETURN) {
            if (currentPointer == -1) {
                loadMenu();
            } else if (currentPointer == 0) {
                startGame();
            } else if (currentPointer == 1) {
                SDL_Quit();
            }
        }

        if (currentPointer != -1) {
            if (key == SDLK_DOWN) currentPointer = (currentPointer + 1) % buttons.size();
            if (key == SDLK_UP) currentPointer = (currentPointer - 1 + buttons.size()) % buttons.size();
        }
    }

    void updateButton() {

        if (currentPointer == 0) {
            buttons[0]->clip.x = 1430 + 281;
            buttons[1]->clip.x = 1622;
        } else if (currentPointer == 1) {
            buttons[0]->clip.x = 1430;
            buttons[1]->clip.x = 1622 + 281;
        }

        for (auto& button : buttons) {
            button->update();
        }
    }

    void update() {
        for (auto& title : titles) {
            title->update();
        }
        
        updateButton();
    }

    void render() {
        background.render(&clip, &dstRect);

        for (auto& title : titles) {
            title->render();
        }

        for (auto& button : buttons) {
            button->render();
        }


    }
};