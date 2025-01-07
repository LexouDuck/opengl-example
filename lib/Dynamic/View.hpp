#ifndef View_Hpp
#define View_Hpp

#include <Structs/Pos.hpp>

namespace Dynamic {

    struct View {

        Pos pos = Pos(0, 0);
        Pos lastPos = Pos(0, 0);
        float scale = 25.0;
        float minScale = scale / 50.f;
        float maxScale = scale * 10.f;

        // Default constructor
        View() {}

        // Adjust view position based on zoom around a specific point
        
        void zoomPos(float amount, Pos focusPos) {

            // Calculate the scale multiplier
            float newScale = scale * amount;

            if (newScale > maxScale) { newScale = maxScale; }
            if (newScale < minScale) { newScale = minScale; }

            // Determine the offset of the focus point in the current scale
            float offsetX = (focusPos.x - pos.x) * (newScale - scale) / newScale;
            float offsetY = (focusPos.y - pos.y) * (newScale - scale) / newScale;

            // Update position and scale
            pos.x += offsetX;
            pos.y += offsetY;

            scale = newScale;
        }
    };
}

#endif