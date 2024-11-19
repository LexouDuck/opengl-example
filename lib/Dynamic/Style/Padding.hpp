#ifndef Padding_Hpp
#define Padding_Hpp

#include "./Distance.hpp"

namespace Style {

    // Padding is like an inner margin
    struct Padding {

        // Abstract distances
        Dist left; Dist right;
        Dist top; Dist bottom;

        // Constructor with default values
        Padding() {}

        operator bool() {
            return left || right || top || bottom;
        }
    };
}

#endif