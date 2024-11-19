#ifndef Size_Hpp
#define Size_Hpp

#include "./Distance.hpp"

namespace Style {

    // The size of the element
    struct Size {

        // Abstract distances
        Dist width; Dist height;
        Dist minWidth; Dist minHeight;
        Dist maxWidth; Dist maxHeight;

        // Return if any of the positions are set
        operator bool() {
            return width || height || minWidth || minHeight || maxWidth || maxHeight;
        }
    };
}

#endif