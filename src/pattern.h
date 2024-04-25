
#include "utils.h"
#include "particle.h"
#include "particleManager.h"
#include "audio.h"

extern ParticleManager particleManager;

class Pattern {
public: 
    SE se;

    virtual void initialize() {
        se.load("enep00");
    }
    virtual void reset(Vec2d position) {

    }

    virtual void update(Vec2d position) {

    }
    virtual bool onGoing() {
        return 0;
    }
};

class RingPattern : public Pattern {
public:
    void reset(Vec2d position) override {
        Vec2d dir = player.position - position;
        dir = dir.normalized();

        double angle = atan2(dir.y, dir.x); 
        for (int i = 0; i <= 36; ++i) {
            double a = angle + i * 10 * M_PI / 180.0;
            double x = dir.length() * cos(a);
            double y = dir.length() * sin(a);
            particleManager.addBullet(position, Vec2d(x, y), 5, 2, 2, 1, (a * 180.0) / M_PI - 90);
        }

    }
    void update(Vec2d position) override {

    }
};

class FanPattern : public Pattern {
public:

    void reset(Vec2d position) override {

        for (int i = 0; i < 9; ++i) {
            int vel = 1 + i / 2;
            Vec2d dir = player.position - position;
            dir = dir.normalized();

            double angle = atan2(dir.y, dir.x); 
            for (int i = -3; i <= 3; ++i) {
                double a = angle + i * 10 * M_PI / 180.0;
                double x = dir.length() * cos(a);
                double y = dir.length() * sin(a);
                particleManager.addBullet(position, Vec2d(x, y), 5, vel, 1);
            }
        }

    }
    void update(Vec2d position) override {

    }
};

class RumiaPatternA : public Pattern {
public:
    int currentWave = 1e9;
    int totalWave = 2;
    int delay;
    Vec2d dir;
    
    void reset(Vec2d position) override {
        dir = player.position - position;
        dir = dir.normalized();
        currentWave = 0;
        delay = 30;
    }
    void update(Vec2d position) override {
        if (currentWave >= totalWave) return;
        if (delay > 0) {
            delay--;
            return;
        }
        delay = 30;
        currentWave++;
        se.play();

        int c = random(0, 15);
        for (int i = 0; i < 3; ++i) {
            double vel = 2 + i / 2.0;
            for (int i = 0; i < 36; ++i) {
                double angle = atan2(dir.y, dir.x); 
                double a = angle + i * 10 * M_PI / 180.0;
                double x = dir.length() * cos(a);
                double y = dir.length() * sin(a);
                particleManager.addBullet(position, Vec2d(x, y), 5, vel, 1, c, (a * 180.0) / M_PI - 90);
            }
        }
    }
    bool onGoing() override {
        return currentWave < totalWave;
    }
};

class RumiaPatternB : public Pattern {
public:
    int currentWave = 1e9;
    int totalWave = 16;
    int delay;
    Vec2d dir;
    
    void reset(Vec2d position) override {
        dir = player.position - position;
        dir = dir.normalized();
        currentWave = 0;
        delay = 10;
        se.play();
    }
    void update(Vec2d position) override {
        if (currentWave >= totalWave) return;
        if (delay > 0) {
            delay--;
            return;
        }
        delay = 10;
        currentWave++;

        int c = random(0, 7);
        double vel = 3;
        for (int i = 0; i < 18; ++i) {
            double angle = atan2(dir.y, dir.x); 
            double a = angle + (i * 20 + currentWave * 5) * M_PI / 180.0;
            double x = dir.length() * cos(a);
            double y = dir.length() * sin(a);
            particleManager.addBullet(position, Vec2d(x, y), 3, vel, 3, c, (a * 180.0) / M_PI - 90);
        }
    }
    bool onGoing() override {
        return currentWave < totalWave;
    }
};

class RumiaPatternC : public Pattern {
public:
    int currentBullet = 1e9;
    int totalBullet = 256;
    Vec2d dir;

    void initialize() override {

    }
    
    void reset(Vec2d position) override {
        dir = Vec2d(0, 1);
        dir = dir.normalized();
        currentBullet = 0;
        se.play();
    }
    void update(Vec2d position) override {
        if (currentBullet >= totalBullet) return;
        currentBullet++;
        double a =  atan2(dir.y, dir.x) + random(1, 360) * M_PI / 180.0;
        double x = dir.length() * cos(a);
        double y = dir.length() * sin(a);
        int r = random(1, 2) % 2 == 0 ? 2 : 5;
        int c = random(1, 7);
        double v = random_d(1, 3);
        particleManager.addBullet(position, Vec2d(x, y), 3, v, r, c, (a * 180.0) / M_PI - 90, 1);
    }
    bool onGoing() {
        return currentBullet < totalBullet;
    }

};
