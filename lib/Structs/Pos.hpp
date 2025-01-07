#ifndef Pos_H
#define Pos_H

#include <cmath>

// Include and alias
#include <nlohmann/json.hpp>
using Json = nlohmann::json;

// A position
struct Pos {

    double x = 0;
    double y = 0;
    bool set = false;

    Pos() {}

    Pos(double x, double y) {
        this->x = x; this->y = y;
        this->set = true;
    }

    static Pos Invalid() {
        Pos pos = Pos(std::nan(""), std::nan(""));
        pos.set = false;
        return pos;
    }

    static Pos fromAngle(double angle) {
        return Pos(
            cos(angle),
            sin(angle)
        );
    }

    Json getState() const {
        return Json{{"x", x}, {"y", y}};
    }

    void setState(const Json& obj) {

        x = obj.value("x", x);
        y = obj.value("y", y);

        set = true;
    }

    // Simply return whether Pos has been set
    inline operator bool() const { return !(this->nan()) || this->set; }

    // Comparator operators based on magnitude
    inline bool operator==(const Pos& other) const { return this->distanceTo(other) < 1e-3; }
    inline bool operator!=(const Pos& other) const { return !(*this == other); }
    inline bool operator<(const Pos& other) const { return this->pythag() < other.pythag(); }
    inline bool operator<=(const Pos& other) const { return this->pythag() <= other.pythag(); }
    inline bool operator>(const Pos& other) const { return this->pythag() > other.pythag(); }
    inline bool operator>=(const Pos& other) const { return this->pythag() >= other.pythag(); }

    // Arithmetic with Pos
    inline Pos operator+(const Pos& other) const { return Pos(x + other.x, y + other.y); }
    inline Pos operator-(const Pos& other) const { return Pos(x - other.x, y - other.y); }
    inline Pos operator*(const Pos& other) const { return Pos(x * other.x, y * other.y); }
    inline Pos operator/(const Pos& other) const { return Pos(x / other.x, y / other.y); }

    // Compound assignment with Pos
    inline Pos& operator+=(const Pos& other) { x += other.x; y += other.y; return *this; }
    inline Pos& operator-=(const Pos& other) { x -= other.x; y -= other.y; return *this; }
    inline Pos& operator*=(const Pos& other) { x *= other.x; y *= other.y; return *this; }
    inline Pos& operator/=(const Pos& other) { x /= other.x; y /= other.y; return *this; }

    // Arithmetic with scalar
    inline Pos operator+(double scalar) const { return Pos(x + scalar, y + scalar); }
    inline Pos operator-(double scalar) const { return Pos(x - scalar, y - scalar); }
    inline Pos operator*(double scalar) const { return Pos(x * scalar, y * scalar); }
    inline Pos operator/(double scalar) const { return Pos(x / scalar, y / scalar); }

    // Compound assignment with scalar
    inline Pos& operator+=(double scalar) { x += scalar; y += scalar; return *this; }
    inline Pos& operator-=(double scalar) { x -= scalar; y -= scalar; return *this; }
    inline Pos& operator*=(double scalar) { x *= scalar; y *= scalar; return *this; }
    inline Pos& operator/=(double scalar) { x /= scalar; y /= scalar; return *this; }

    // Simple operations
    inline Pos setNan() { x = std::nan(""); y = std::nan(""); this->set = false; return *this; }
    inline bool nan() const { return (std::isnan(x) || std::isnan(y)); }
    inline bool isClose(Pos& other, double thresh = 1e-3) { return this->distanceTo(other) < thresh; }
    inline double pythag() const { return sqrt(x*x + y*y); }
    inline double distanceTo(const Pos& pos) const { return (pos - *this).pythag();  }
    inline double angleTo(const Pos& pos) const { return atan2(pos.y - this->y, pos.x - this->x); }
    inline double angle() const { return atan2(y, x); }
    inline double dot(const Pos& other) const { return x * other.x + y * other.y; }
    inline double cross(const Pos& other) const { return x * other.y - y * other.x; }
    inline Pos centerTo(const Pos& pos) const { return (*this + pos) / 2.f; }
    inline void normalize() { *this /= pythag(); }
    inline Pos normalized() { return (*this) / this->pythag(); }

    inline void round() {
        x = std::round(x);
        y = std::round(y);
    }

    // Rotate by 90 deg
    inline Pos swapAxis() { return Pos(y, x); }

    inline void reflect(Pos& a, Pos& b) {

        // Direction vector of the line
        double dx = b.x - a.x;
        double dy = b.y - a.y;

        // Vector from a to this point
        double px = this->x - a.x;
        double py = this->y - a.y;

        // Dot product of (px, py) and (dx, dy)
        double dot = px * dx + py * dy;

        // Length squared of the direction vector
        double lenSq = dx * dx + dy * dy;

        // Scale factor for projection
        double scale = dot / lenSq;

        // Projection point on the line
        double projX = a.x + scale * dx;
        double projY = a.y + scale * dy;

        // Reflect the point about the line
        this->x = 2 * projX - this->x;
        this->y = 2 * projY - this->y;
    }

    // Return reflected copy
    inline Pos reflected(Pos& a, Pos& b) {

        Pos newPos = *this;
        newPos.reflect(a, b);

        return newPos;
    }
};

inline Pos operator+(double scalar, const Pos& pos) { return Pos(pos.x + scalar, pos.y + scalar); }
inline Pos operator-(double scalar, const Pos& pos) { return Pos(pos.x - scalar, pos.y - scalar); }
inline Pos operator*(double scalar, const Pos& pos) { return Pos(pos.x * scalar, pos.y * scalar); }
inline Pos operator/(double scalar, const Pos& pos) { return Pos(pos.x / scalar, pos.y / scalar); }

#endif // Pos_H
