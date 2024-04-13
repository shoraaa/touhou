#ifndef UTILS_H_
#define UTILS_H_
#include "utils.h"
#endif /* UTILS_H_ */

#ifndef TEXTURE_H_
#define TEXTURE_H_
#include "texture.h"
#endif /* TEXTURE_H_ */

#ifndef PLAYER_H_
#define PLAYER_H_
#include "player.h"
#endif /* PLAYER_H_ */

#ifndef PARTICLE_H_
#define PARTICLE_H_
#include "particle.h"
#endif /* PARTICLE_H_ */


extern Player player;
extern ParticleManager particleManager;
struct Enemy {
    Vec2d position, direction;
    int elapsedTime = 0;
    int type, dead = 0, delay = 60;
    int hp = 1;
    double velocity = 1.0;
    int bulletRadius = 5, bulletVelocity = 1;
    Enemy(Vec2d pos, Vec2d dir, double vel): position(pos), direction(dir), velocity(vel) {}
    void update() {
        if (dead) return;

        position = position + direction * velocity;
        elapsedTime++;

        for (auto& bullet : player.bullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                hp--;
                if (hp == 0) {
                    dead = 1;
                    return;
                }
            }
        }

        delay--;
        if (delay == 0) {
            delay = 30;
            particleManager.push(position, bulletRadius, bulletVelocity);
        }
    }
};


struct EnemyManager {
    string texturePath;
    Texture texture;
    int delta = 30;

    SDL_Rect srcRect;
    #define ENEMY_WIDTH 32
    #define ENEMY_HEIGHT 32

    vector<Enemy> enemies;

    void initialize() {
        texture.load("enemy");

        srcRect.x = 306, srcRect.y = 306, srcRect.w = ENEMY_WIDTH, srcRect.h = ENEMY_HEIGHT;
    }

    void update() {
        vector<Enemy> newEnemies;
        for (auto& enemy : enemies) {
            enemy.update();
            if (!enemy.dead) {
                newEnemies.emplace_back(enemy);
            }
        }
        enemies = newEnemies;

        delta--;
        if (delta == 0) {
            delta = 30;
            Vec2d position = Vec2d(random(FIELD_X, FIELD_X + FIELD_WIDTH - 1), FIELD_Y);
            Vec2d direction = Vec2d(0, 1);

            Enemy enemy = Enemy(position, direction, 1);
            enemies.emplace_back(enemy);
        }

        for (auto& enemy : enemies) {
            if (enemy.position.distance(player.position) <= 64) {
                PLAYER_LOST = 1;
            }
            
        }

    }

    void render() {
        for (auto& enemy : enemies) {
            texture.render(enemy.position.x - ENEMY_WIDTH / 2, enemy.position.y - ENEMY_HEIGHT / 2, &srcRect);
        }
    }
};
