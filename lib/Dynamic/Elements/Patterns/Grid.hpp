#ifndef Grid_H
#define Grid_H

#include <cmath>
#include <Dynamic/Element.hpp>
#include <dbg.hpp>

namespace Elements {

    class Grid : public Element {

    public:

        Style::Color lineColor = Style::Color(1, 1, 1, 1);
        Style::Dist lineWidth = Style::Px(1);
        Style::Dist lineSpacing = Style::Px(10);
        
        Pos spacing;
        Pos offset = Pos(0, 0);

        // Constructor with default position, dimensions, and fill color
        Grid(Element* parent = nullptr) : Element(parent) {
            this->name = "Grid";
        }

        void draw(Event& e) override {

            // The rect we're drawing within
            Rect drawRect = this->rect;

            // Set the Style::Colorfor the grid lines
            glColor4f(this->lineColor.r, this->lineColor.g, this->lineColor.b, this->lineColor.a);

            float colSpacing, rowSpacing;

            if (spacing) {
                colSpacing = fabs(spacing.x);
                rowSpacing = fabs(spacing.y);
            }

            else {
                colSpacing = lineSpacing.resolve(drawRect.width);
                rowSpacing = lineSpacing.resolve(drawRect.height);
            }

            float lw = lineWidth.resolve(drawRect.width);

            // Ensure offset values wrap within the bounds of the drawing area
            float startX = fmod(offset.x, colSpacing);  // Wrap X offset
            float startY = fmod(offset.y, rowSpacing);  // Wrap Y offset

            // Calculate the number of rows and columns based on spacing
            int rows = static_cast<int>(drawRect.height / rowSpacing) + 1;
            int cols = static_cast<int>(drawRect.width / colSpacing) + 1;

            // Set line width for the grid lines
            glLineWidth(lw);
            glBegin(GL_LINES);

            // Draw horizontal lines
            for (int i = 0; i <= rows; i++) {
                float y = drawRect.y + startY + i * rowSpacing;
                if (y >= drawRect.y && y <= drawRect.y + drawRect.height) {  // Ensure within bounds
                    glVertex2f(drawRect.x, y);
                    glVertex2f(drawRect.x + drawRect.width, y);
                }
            }

            // Draw vertical lines
            for (int j = 0; j <= cols; j++) {
                float x = drawRect.x + startX + j * colSpacing;
                if (x >= drawRect.x && x <= drawRect.x + drawRect.width) {  // Ensure within bounds
                    glVertex2f(x, drawRect.y);
                    glVertex2f(x, drawRect.y + drawRect.height);
                }
            }

            glEnd();

            // Call Element::draw(e) if there are child elements
            Element::draw(e);
        }
    };
}

#endif // Grid_H
