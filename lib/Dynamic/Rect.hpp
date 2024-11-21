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

        // Get center of rect as a position
        Pos center() {
            return Pos(x + width / 2, y + height / 2);
        }
    };
}

#endif