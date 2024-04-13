#include <iostream>

#ifndef AUDIO_H_
#define AUDIO_H_
#include "audio.h"
#endif /* AUDIO_H_ */

#ifndef PLAYER_H_
#define PLAYER_H_
#include "player.h"
#endif /* PLAYER_H_ */

#ifndef ENEMY_H_
#define ENEMY_H_
#include "enemy.h"
#endif /* ENEMY_H_ */

struct Scene_Gameplay {

    Texture background, foreground;
    BGM bgm;

    double scrollingOffset = 0;

    void initialize() {
        background.load("background");
        foreground.load("foreground");

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
    
    void renderFG() {
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

        // render text 25, 169, 48, 16
        srcRect.x = 25, srcRect.y = 201, srcRect.w = 48, srcRect.h = 16;
        dstRect.x = 16 * 27, dstRect.y = 16 * 8, dstRect.w = 48, dstRect.h = 16;
        foreground.render(&srcRect, &dstRect);
        srcRect.x = 57, srcRect.y = 265, srcRect.w = 16, srcRect.h = 16;
        dstRect.x += dstRect.w + 8; dstRect.w = 16, dstRect.h = 16;
        for (int i = 0; i < player.lives; ++i, dstRect.x += 16 + 8) {
            foreground.render(&srcRect, &dstRect);
        }

        srcRect.x = 25, srcRect.y = 201 - 16, srcRect.w = 48, srcRect.h = 16;
        dstRect.x = 16 * 27, dstRect.y = 16 * 8 - 32, dstRect.w = 48, dstRect.h = 16;
        foreground.render(&srcRect, &dstRect);


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