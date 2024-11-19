#ifndef Pos_H
#define Pos_H

#include <cmath>

// Include and alias
#include <nlohmann/json.hpp>
using Json = nlohmann::json;

// A position
struct Pos {

    float x = 0;
    float y = 0;
    bool set = false;

    Pos() {}

    Pos(float x, float y) {
        this->x = x; this->y = y;
        this->set = true;
    }

    static Pos fromAngle(float angle) {
        return Pos(
            cos(angle),
            sin(angle)
        );
    }

    Json getState() const {
        return Json{{"x", x}, {"y", y}};
    }

    void setState(const Json& obj) {
        x = obj["x"]; y = obj["y"]; set = true;
    }

    // Simply return whether Pos has been set
    inline operator bool() const { return this->set; }

    // With pos
    inline Pos operator+(const Pos& other) const { return Pos(x + other.x, y + other.y); }
    inline Pos operator-(const Pos& other) const { return Pos(x - other.x, y - other.y); }

    // With scalar
    inline Pos operator*(float scalar) const { return Pos(x * scalar, y * scalar); }
    inline Pos operator/(float scalar) const { return Pos(x / scalar, y / scalar); }
    inline Pos operator+(float scalar) const { return Pos(x + scalar, y + scalar); }
    inline Pos operator-(float scalar) const { return Pos(x - scalar, y - scalar); }

    // Compound assignment operators
    inline Pos& operator+=(const Pos& other) { x += other.x; y += other.y; return *this; }
    inline Pos& operator-=(const Pos& other) { x -= other.x; y -= other.y; return *this; }
    inline Pos& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    inline Pos& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

    // Simple operations
    inline float pythag() { return sqrt(x*x + y*y); }
    inline float distanceTo(const Pos& pos) const { return (pos - *this).pythag();  }
    inline float angleTo(const Pos& pos) const { return atan2(pos.y - this->y, pos.x - this->x); }
    inline float angle() { return atan2(y, x); }
    inline Pos centerTo(const Pos& pos) const { return (*this + pos) / 2; }

    inline Pos reflect(const Pos& a, const Pos& b) const {
        
        // Step 1: Calculate the direction vector of AB
        Pos direction = b - a;

        // Step 2: Calculate the projection of *this onto the line defined by a and b
        float t = ((*this - a).x * direction.x + (*this - a).y * direction.y) / (direction.x * direction.x + direction.y * direction.y);

        // Projection point on the line AB
        Pos projection = a + direction * t;

        // Step 3: Calculate the reflection by mirroring over the projection point
        return projection * 2 - *this;
    }
};

#endif // Pos_H
