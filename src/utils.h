#pragma once

#include "const.h"
#include <random>

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
int random(int l, int r) {
    return rng() % (r - l + 1) + l;
}

struct Vec2d {
    double x, y;
    Vec2d(double x = 0, double y = 0): x(x), y(y) {}

    bool inPlayField() {
        return x >= FIELD_X && x <= FIELD_X + FIELD_WIDTH - 4 && y >= FIELD_Y && y <= FIELD_Y + FIELD_HEIGHT;
    }

    Vec2d normalized() {
        return (*this) * (1.0 / length());
    }

    double distance(const Vec2d& other) {
        return (x - other.x) * (x - other.x) + (y - other.y) * (y - other.y);
    }

    double length() {
        return sqrt(x * x + y * y);
    }

    Vec2d operator+ (const Vec2d& a) const {
        return Vec2d(x + a.x, y + a.y);
    }
    Vec2d operator- (const Vec2d& a) const {
        return Vec2d(x - a.x, y - a.y);
    }
    Vec2d operator* (double k) const {
        return Vec2d(x * k, y * k);
    }
    Vec2d operator/ (double k) const {
        return Vec2d(x / k, y / k);
    }
};

Vec2d getRandomPoint() {
    return Vec2d(random(FIELD_X, FIELD_X2), random(FIELD_Y, FIELD_Y2));
}

double easeIn(double t, double p = 2.0) {
    return pow(t, p);
}

double easeOut(double t, double p = 2.0) {
    return 1 - pow(1 - t, p);
}
