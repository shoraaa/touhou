
#include "utils.h"
#include "particle.h"
#include "particleManager.h"

extern ParticleManager particleManager;

class Pattern {
public: 
    void initialize() {

    }

    void update() {

    }
};

class RingPattern : public Pattern {
public:
    void initialize(Vec2d position) {
        Vec2d dir = player.position - position;
        dir = dir.normalized();

        double angle = atan2(dir.y, dir.x); 
        for (int i = 0; i <= 10; ++i) {
            double a = angle + i * 36 * M_PI / 180.0;
            double x = dir.length() * cos(a);
            double y = dir.length() * sin(a);
            particleManager.addBullet(position, Vec2d(x, y), 5, 2);
        }

    }
    void update(Vec2d position) {

    }
};