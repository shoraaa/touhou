#pragma once

#include "utils.h"
#include "texture.h"
#include "player.h"
#include "particle.h"
#include "particleManager.h"
#include "audio.h"
#include "const.h"
#include "pattern.h"

extern Player player;
extern ParticleManager particleManager;
extern int elapsedFrame;
extern double scrollingSpeed;

class Enemy {
public:
    Vec2d position, direction;
    SE deadSE;
    SDL_Rect spriteClip;

    int elapsedTime = 0;
    int hp = 1;
    double velocity = 1.0;
    int deadType = 0;

    Enemy(Vec2d pos, Vec2d dir, double vel, int deadType = 0): position(pos), direction(dir), velocity(vel), deadType(deadType) {}
    void gotHit(Vec2d& pos) {
        particleManager.playEnemyHitAnimation(pos);
        player.gotHitOther();
        hp--;
        if (hp == 0) {
            particleManager.playEnemyDeadAnimation(position, deadType);
            deadSE.play();
            player.gotKill();
            if (player.kills % 3 == 0) {
                if (player.kills % 2 == 0) particleManager.dropPowerItem(position);
                else particleManager.dropScoreItem(position);
            }
        }
    }

    void virtual update() {}
};

class BlueFairy : public Enemy {
public:
    BlueFairy(Vec2d pos, Vec2d dir, double vel): Enemy(pos, dir, vel, 1) {
        spriteClip.x = 306, spriteClip.y = 306, spriteClip.w = 32, spriteClip.h = 32;
        deadSE.load("tan00");
    }
    
    void update() override {
        if (hp == 0) return;

        position = position + direction * velocity;
        elapsedTime++;

        for (auto& bullet : player.bullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit(bullet->position);
            }
        }

        if (hp == 0) return;

        for (auto& bullet : player.chaseBullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit(bullet->position);
            }
        }
    }
};

class PinkFairy : public Enemy {
public:
    PinkFairy(Vec2d pos, Vec2d dir, double vel): Enemy(pos, dir, vel, 0) {
        spriteClip.x = 306, spriteClip.y = 338, spriteClip.w = 32, spriteClip.h = 32;
        deadSE.load("tan00");
        hp = 4;
    }
    void update() override {
        if (hp == 0) return;
        elapsedTime++;
        if (elapsedTime > 90 && elapsedTime <= 120) return;

        if (elapsedTime == 90) {
            int half = (FIELD_X + FIELD_WIDTH - 1) / 2;
            if (position.x >= half) direction = Vec2d(1, 0);
            else direction = Vec2d(-1, 0);
            velocity /= 2;

            Vec2d dir = player.position - position;
            dir = dir.normalized();

            double angle = atan2(dir.y, dir.x); 
            for (int i = -2; i <= +2; ++i) {
                double a = angle + i * 10 * M_PI / 180.0;
                double x = dir.length() * cos(a);
                double y = dir.length() * sin(a);
                particleManager.addBullet(position, Vec2d(x, y), 5, 2);
            }
        }
        position = position + direction * velocity * elapsedTime / 75.0;

        for (auto& bullet : player.bullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit(bullet->position);
            }
        }

        if (hp == 0) return;

        for (auto& bullet : player.chaseBullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit(bullet->position);
            }
        }
    }
};

class PinkFairyGood : public Enemy {
public:
    PinkFairyGood(Vec2d pos, Vec2d dir, double vel): Enemy(pos, dir, vel) {
        spriteClip.x = 306, spriteClip.y = 338, spriteClip.w = 32, spriteClip.h = 32;
        deadSE.load("tan00");
        hp = 8;
    }
    void update() override {
        if (hp == 0) return;
        elapsedTime++;

        if (elapsedTime == 90 || elapsedTime == 120) {
            int half = (FIELD_X + FIELD_WIDTH - 1) / 2;
            if (position.x >= half) direction = Vec2d(1, 0);
            else direction = Vec2d(-1, 0);
            velocity /= 2;

            Vec2d dir = player.position - position;
            dir = dir.normalized();

            double angle = atan2(dir.y, dir.x); 
            for (int i = -3; i <= +3; ++i) {
                double a = angle + i * 10 * M_PI / 180.0;
                double x = dir.length() * cos(a);
                double y = dir.length() * sin(a);
                particleManager.addBullet(position, Vec2d(x, y), 5, 2);
            }
        }
        if (elapsedTime > 90 && elapsedTime <= 150) return;
        position = position + direction * velocity * elapsedTime / 75.0;

        for (auto& bullet : player.bullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit(bullet->position);
            }
        }

        if (hp == 0) return;

        for (auto& bullet : player.chaseBullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit(bullet->position);
            }
        }
    }
};

struct Rumia {
    Vec2d position, direction, initialPosition;
    SE deadSE, hitSE;
    SDL_Rect spriteClip;
    int spawned = 0, dead = 0, phase = 1;

    unique_ptr<Sprite> enemy;

    int movingFrame = 1e9;

vector<unique_ptr<Pattern>> patterns;

    int elapsedTime = 0;
    int maxHp = 3000, hp = maxHp, skillDelay = 120;
    double velocity = 1.0, moveDelay = 240;
    int MOVE_CD = 180;

    int SKILL_CD = 240;

    void initialize() {
        spriteClip = {996, 25, 32, 64};
        hitSE.load("damage00");
        phase = 1;

        patterns.emplace_back(make_unique<FanPattern>());

        patterns.emplace_back(make_unique<RumiaPatternA>());
        patterns.emplace_back(make_unique<RumiaPatternB>());
        patterns.emplace_back(make_unique<RumiaPatternC>());
        patterns.emplace_back(make_unique<RumiaPatternD>());


        enemy = make_unique<Sprite>("foreground", SDL_Rect{25, 169, 48, 16});
        enemy->position = Vec2d(FIELD_X + 24, FIELD_Y + 8); 

        for (auto& p : patterns) {
            p->initialize();
        }
    }

    void spawn() {
        spawned = 1;
        position = Vec2d(FIELD_X + FIELD_WIDTH / 2, 64);
        cout << "Rumia spawned!\n";
        scrollingSpeed = 2;
        particleManager.clear();
    }

    void gotHit(Vec2d& pos) {
        particleManager.playEnemyHitAnimation(pos);
        player.gotHitOther();
        hp--;
        if (hp <= 0) {
            particleManager.playEnemyDeadAnimation(position);
            deadSE.play();
            particleManager.clear();
            scrollingSpeed = 0.5;
        }
    }

    void update() {
        if (hp <= 0) {
            spawned = 0;
            dead = 1;
            return;
        }

        for (auto& bullet : player.bullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit(bullet->position);
            }
        }
        for (auto& bullet : player.chaseBullets) {
            if (bullet->collide(position)) {
                bullet->hit = 1;
                gotHit(bullet->position);
            }
        }

        // cerr << hp << '\n';

        if (hp <= maxHp / 2 && phase == 1) {
            phase = 2;
            SKILL_CD *= 1.5;
            MOVE_CD *= 1.5;
        }

        skillDelay--;
        if (skillDelay <= 0) {
            skillDelay = SKILL_CD;

            vector<int> nextPatternCandidate;
            for (int i = 0; i < patterns.size(); ++i) if (!patterns[i]->onGoing()) {
                nextPatternCandidate.emplace_back(i);
            }
            if (!nextPatternCandidate.empty()) {
                int i = random(0, nextPatternCandidate.size() - 1);
                patterns[nextPatternCandidate[i]]->reset(position);
            }

        }

        moveDelay--;
        if (moveDelay <= 0) {
            moveDelay = MOVE_CD;
            Vec2d nextPosition = Vec2d(random(FIELD_X + 16, FIELD_X2 - 16), random(FIELD_Y + 16, FIELD_Y + 200));
            initialPosition = position;
            direction = nextPosition - position;
            movingFrame = 0;
        }

        for (auto& pattern : patterns) {
            pattern->update(position);
        }

        if (movingFrame < 45) {
            position = initialPosition + direction * easeIn(movingFrame / 60.0);
            movingFrame++;
            if (direction.x < 0) {
                spriteClip = {1028 + 32, 25, 32, 64};
            } else {
                spriteClip = {1028, 25, 32, 64};
            }
        } else {
            spriteClip = {996, 25, 32, 64};
        }


    }

    void render(Texture& texture) {
        texture.render(position.x, position.y, &spriteClip);
        enemy->render();
        
    }
};

struct EnemyManager {
    string texturePath;
    Texture texture;

    vector<unique_ptr<Enemy>> enemies;
    Rumia rumia;
    int delayGenerateEnemy = 60 * 30;
    int phase = 0;


    void initialize() {
        texture.load("enemy");
        rumia.initialize();
        phase = 0;
    }

    void updatePlayerBullets() {
        vector<unique_ptr<Particle>> newBullets;
        for (auto& bullet : player.bullets) {
            bullet->update();

            if (bullet->inBound() && !bullet->hit) {
                newBullets.emplace_back(move(bullet));
            }
        }
        player.bullets = move(newBullets);

        vector<unique_ptr<Particle>> newChaseBullets;
        for (auto& bullet : player.chaseBullets) {
            // bullet find a enemy to shoot
            if (enemies.size() || rumia.spawned) {
                auto position = rumia.spawned ? rumia.position : (*enemies[0]).position;
                Vec2d dir = position - bullet->position;
                dir = dir * (1.0 / dir.length());
                bullet->position = bullet->position + (dir * (bullet->elapsedTime / 3.0));
                bullet->elapsedTime++;
            } else {
                bullet->update();
            }

            if (bullet->inBound() && !bullet->hit) {
                newChaseBullets.emplace_back(move(bullet));
            }
        }
        player.chaseBullets = move(newChaseBullets);
    }

    void updateEnemies() {
        vector<unique_ptr<Enemy>> newEnemies;
        for (auto& enemy : enemies) {
            enemy->update();
            if (enemy->hp > 0 && enemy->position.inPlayField()) {
                newEnemies.emplace_back(move(enemy));
            }
        }
        enemies = move(newEnemies);

        for (auto& enemy : enemies) {
            if (enemy->position.distance(player.position) <= 64) {
               particleManager.hitPlayer();
            }
        }

        // player bullet
        updatePlayerBullets();
    }

    int defaultDeltaPink = 30;
    int defaultDeltaBlue = 10;

    int deltaPink = defaultDeltaPink;
    int deltaBlue = defaultDeltaBlue;

    int blueX = FIELD_X, directionBlueX = 1;

    void generateBlueFairy() {
        deltaBlue--;

        blueX += directionBlueX;
        if (blueX == FIELD_X2) directionBlueX = -1;
        if (blueX == FIELD_X) directionBlueX = 1;
        if (deltaBlue == 0) {
            deltaBlue = defaultDeltaBlue;
            Vec2d position = Vec2d(blueX, FIELD_Y);
            Vec2d direction = Vec2d(0, 1);

            unique_ptr<BlueFairy> blueFairy = make_unique<BlueFairy>(position, direction, phase + 1);
            enemies.emplace_back(move(blueFairy));

        }
    }

    void generateHardBlueFairy() {
        deltaBlue--;

        blueX += directionBlueX;
        if (blueX == FIELD_X2) directionBlueX = -1;
        if (blueX == FIELD_X) directionBlueX = 1;
        if (deltaBlue == 0) {
            deltaBlue = defaultDeltaBlue;
            Vec2d position = Vec2d(blueX, FIELD_Y);
            Vec2d direction = Vec2d(0, 1);

            unique_ptr<BlueFairy> blueFairy = make_unique<BlueFairy>(position, direction, phase + 1);
            enemies.emplace_back(move(blueFairy));

            position.x += 20;
            enemies.emplace_back(make_unique<BlueFairy>(position, direction, phase + 1));


        }
    }

    void generateExtraBlueFairy() {
        deltaBlue--;

        blueX += directionBlueX;
        if (blueX == FIELD_X2) directionBlueX = -1;
        if (blueX == FIELD_X) directionBlueX = 1;
        if (deltaBlue == 0) {
            deltaBlue = defaultDeltaBlue;
            Vec2d position = Vec2d(blueX, FIELD_Y);
            Vec2d direction = Vec2d(0, 1);

            unique_ptr<BlueFairy> blueFairy = make_unique<BlueFairy>(position, direction, phase + 1);
            enemies.emplace_back(move(blueFairy));

            position.x += 20;
            enemies.emplace_back(make_unique<BlueFairy>(position, direction, phase + 1));

            position = Vec2d(FIELD_X2 - blueX + 1, FIELD_Y);

            enemies.emplace_back(make_unique<BlueFairy>(position, direction, phase + 1));

            position.x += 20;
            enemies.emplace_back(make_unique<BlueFairy>(position, direction, phase + 1));


        }
    }

    void generatePinkFairy() {
        deltaPink--;
        if (deltaPink == 0) {
            deltaPink = defaultDeltaPink;
            Vec2d position = Vec2d(random(FIELD_X, FIELD_X + FIELD_WIDTH - 1), FIELD_Y);
            Vec2d direction = Vec2d(0, 1);

            unique_ptr<PinkFairy> pinkFairy = make_unique<PinkFairy>(position, direction, 1);
            enemies.emplace_back(move(pinkFairy));

        }
    }
    

    void generateGoodPinkFairy() {
        deltaPink--;
        if (deltaPink == 0) {
            deltaPink = defaultDeltaPink;
            Vec2d position = Vec2d(random(FIELD_X, FIELD_X + FIELD_WIDTH - 1), FIELD_Y);
            Vec2d direction = Vec2d(0, 1);

            unique_ptr<PinkFairyGood> pinkFairy = make_unique<PinkFairyGood>(position, direction, 1);
            enemies.emplace_back(move(pinkFairy));

        }
    }

    void generateEnemy() {
        delayGenerateEnemy--;
        if (delayGenerateEnemy <= 0) {
            delayGenerateEnemy = 60 * 30;
            phase++;
        }
  
        if (phase == 0) {
            defaultDeltaBlue = 30;
            generateBlueFairy();

            defaultDeltaPink = 120;
            generatePinkFairy();
        } else if (phase == 1) {
            defaultDeltaBlue = 15;
            generateBlueFairy();

            defaultDeltaPink = 90;
            generatePinkFairy();
        } else if (phase == 2) {
            defaultDeltaBlue = 10;
            generateHardBlueFairy();

            defaultDeltaPink = 60;
            generatePinkFairy();

            scrollingSpeed = 1.0;
        } else if (phase == 3) {
            defaultDeltaBlue = 10;
            generateExtraBlueFairy();

            defaultDeltaPink = 75;
            generateGoodPinkFairy();
        } else {
            if (!rumia.dead) {
                if (!rumia.spawned) {
                    rumia.spawn();
                } else {
                    rumia.update();
                }
            } else {
                // cout << "Win!";
            }
        }
    }

    void update() {
        updateEnemies();
        generateEnemy();
    }

    void render() {
        for (auto& enemy : enemies) {
            texture.render(enemy->position.x, enemy->position.y, &enemy->spriteClip);
        }
        if (rumia.spawned) {
            SDL_Rect hpRect = {FIELD_X + 64, FIELD_Y + 8, (int)((rumia.hp / double(rumia.maxHp)) * (FIELD_WIDTH - (FIELD_X + 64 + 4))), 4};
            SDL_RenderFillRect(renderer, &hpRect);
            rumia.render(texture);
            
        }
    }
};
