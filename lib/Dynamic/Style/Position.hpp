#ifndef Position_Hpp
#define Position_Hpp

#include "./Distance.hpp"

namespace Style {

    // Margins control how close an element can get to something else
    struct Position {

        Dist left; Dist right;
        Dist top; Dist bottom;

        // Return if any of the positions are set
        operator bool() {
            return left || right || top || bottom;
        }
    };
}

#endif