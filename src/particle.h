class Particle {
    public:
    Vec2d initialPosition, position;
    int elapsedTime = 0;
    int type, hit = 0;
    int radius = 5;
    Particle() = default;
    Particle(Vec2d pos, int radius): initialPosition(pos), position(pos), radius(radius) {}

    bool inBound() {
        return position.inPlayField();
    }
    
    bool collide(Vec2d other) {
        auto x = position.x, y = position.y;
        auto px = other.x, py = other.y;
        if ((x - px) * (x - px) + (y - py) * (y - py) <= radius * radius) {
            return 1;
        }
        return 0;
    }

    virtual void update() {}
};

class LinearParticle : public Particle {
    // vector linear
    public:
    Vec2d direction;
    double velocity = 1.0;
    LinearParticle(Vec2d pos, Vec2d dir, double radius, double vel): Particle(pos, radius), direction(dir), velocity(vel) {}
    void update() override {
        position = initialPosition + (direction * velocity * elapsedTime);
        elapsedTime++;
    }
};
