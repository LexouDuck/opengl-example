#ifndef Border_Hpp
#define Border_Hpp

#include "./Distance.hpp"
#include "./Color.hpp"

namespace Style {

    struct Side {

        Color color;
        Dist radius;

        Side() {}
    };

    // A border
    struct Border {

        Color color;    // Global color
        Dist radius;    // Global radius

        Side left;
        Side right;
        Side top;
        Side bottom;

        Border() {}
    };
}

#endif