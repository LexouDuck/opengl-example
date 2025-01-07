#ifndef Rect_Hpp
#define Rect_Hpp

#include <Structs/Pos.hpp>

namespace Dynamic {

    struct Rect {

        float x = 0; float y = 0;
        float width = 0; float height = 0;

        Rect() {}

        Rect(float x, float y, float width, float height) {
            this->x = x; this->y = y;
            this->width = width; this->height = height;
        }

        static Rect fromPair(Pos& start, Pos& end) {
            
            // Calculate the top-left corner (x, y)
            float x = std::min(start.x, end.x);
            float y = std::min(start.y, end.y);

            // Calculate the width and height
            float width = std::abs(end.x - start.x);
            float height = std::abs(end.y - start.y);

            // Return the normalized rect
            return Rect(x, y, width, height);
        }

        // Get center of rect as a position
        Pos center() {
            return Pos(x + width / 2, y + height / 2);
        }

        // Return whether this contains pos
        bool contains(Pos& pos) {

            // Disqualify cases
            if (pos.x < x) { return false; }
            if (pos.y < y) { return false; }
            if (pos.x > x + width) { return false; }
            if (pos.y > y + height) { return false; }

            // Return true by default
            return true;
        }

        // Return whether two rects intersect
        bool intersects(Rect& other) {

            // Disqualify cases
            if (x + width < other.x || other.x + other.width < x) { return false; }
            if (y + height < other.y || other.y + other.height < y) { return false; }

            // Return true by default
            return true;
        }
    };
}

#endif