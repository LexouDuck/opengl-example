#ifndef Margin_Hpp
#define Margin_Hpp

#include "./Distance.hpp"

namespace Style {

    // Margins control how close an element can get to something else
    struct Margin {

        Dist left; Dist right;
        Dist top; Dist bottom;

        // Return if any of the positions are set
        operator bool() {
            return left || right || top || bottom;
        }
    };
}

#endif