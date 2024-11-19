#ifndef PolarGrid_H
#define PolarGrid_H

#include <cmath>
#include <Dynamic/Element.h>
#include <dbg.hpp>
#include <vector>
#include <algorithm> // For std::max_elementd

namespace Elements {

    class PolarGrid : public Element {

    public:

        Style::Color lineColor = Style::Color(1, 1, 1, 1);
        Style::Dist lineWidth = Style::Px(1);
        Style::Dist scale = Style::Pct(50);
        Style::Dist offsetX = Style::Px(0);
        Style::Dist offsetY = Style::Px(0);

        // Axis and radii
        std::vector<float> axis = { 0, M_PI / 3, M_PI / 4, 2 * M_PI / 3, M_PI / 2 };
        std::vector<float> radii = { .25f, .5f, .75f, 1.f };

        // Constructor with default position, dimensions, and fill color
        PolarGrid(Element* parent = nullptr) : Element(parent) {
            this->name = "PolarGrid";
        }

        void draw(Event& e) override {
            
            // Resolve scales and get center, etc...
            float absScale = this->scale.resolve(std::min(rect.height, rect.width));
            float lw = this->lineWidth.resolve(rect.width);
            float maxRadius = *std::max_element(radii.begin(), radii.end());
            Pos center = rect.center();

            this->setScissor();
            glEnable(GL_LINE_SMOOTH);
            glLineWidth(lw);
            glBegin(GL_LINES);

            glColor4f(this->lineColor.r, this->lineColor.g, this->lineColor.b, this->lineColor.a);
            
            // Draw the specified axes in all quadrants, limited to maxRadius
            for (float angle : axis) {
                for (int quadrant = 0; quadrant < 4; quadrant++) {

                    float quadrantAngle = angle + quadrant * M_PI / 2;

                    // Start from center
                    glVertex2f(
                        center.x,
                        center.y
                    );

                    // Extend to max radius
                    glVertex2f(
                        center.x + absScale * maxRadius * cos(quadrantAngle),
                        center.y + absScale * maxRadius * sin(quadrantAngle)
                    );
                }
            }

            glDisable(GL_LINE_SMOOTH);
            glEnd();

            // Draw concentric circles based on specified radii
            for (float radius : radii) {

                if (radius <= maxRadius) {

                    glEnable(GL_LINE_SMOOTH);
                    glBegin(GL_LINE_LOOP);

                    int segments = 100; // Adjust for smoothness
                    float angleIncrement = 2.0f * M_PI / segments;

                    for (int i = 0; i <= segments; i++) { // Include the endpoint to close the loop

                        float angle = i * angleIncrement;
 
                        glVertex2f(
                            center.x + absScale * radius * cos(angle),
                            center.y + absScale * radius * sin(angle)
                        );
                    }

                    glDisable(GL_LINE_SMOOTH);
                    glEnd();
                }
            }

            // Continue draw
            Element::draw(e);
        }
    };
}

#endif // PolarGrid_H
