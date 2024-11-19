#ifndef Box_H
#define Box_H

#include <cmath>
#include <Dynamic/Element.h>
#include <dbg.hpp>

namespace Elements {

    class Box : public Element {

    public:

        Style::Color fillColor;

        // Constructor with default position, dimensions, and fill color
        Box(Element* parent = nullptr): Element(parent) {
            this->name = "Box";
        }

        void draw(Event& e) override {
            
            // Ensure the rectangle has defined dimensions
            if (rect.width <= 0 || rect.height <= 0) return;

            if (fillColor) {
                this->drawFillRect(fillColor, rect.x, rect.y, rect.width, rect.height);
            }

            // Calculate rectangle edges
            float leftEdge = rect.x;
            float rightEdge = rect.x + rect.width;
            float topEdge = rect.y;
            float bottomEdge = rect.y + rect.height;

            // Determine colors for each edge, using a fallback if an edge Style::Coloris not set
            Style::Color leftColor = style.border.color ? style.border.color : style.border.left.color;
            Style::Color rightColor = style.border.color ? style.border.color : style.border.right.color;
            Style::Color topColor = style.border.color ? style.border.color : style.border.top.color;
            Style::Color bottomColor = style.border.color ? style.border.color : style.border.bottom.color;

            // Draw the rectangle border using GL_LINE_LOOP with Style::Color interpolation
            glBegin(GL_LINE_LOOP);

            // Left edge
            glColor4f(leftColor.r, leftColor.g, leftColor.b, leftColor.a);
            glVertex2f(leftEdge, topEdge);
            glVertex2f(leftEdge, bottomEdge);

            // Bottom edge
            glColor4f(bottomColor.r, bottomColor.g, bottomColor.b, bottomColor.a);
            glVertex2f(leftEdge, bottomEdge);
            glVertex2f(rightEdge, bottomEdge);

            // Right edge
            glColor4f(rightColor.r, rightColor.g, rightColor.b, rightColor.a);
            glVertex2f(rightEdge, bottomEdge);
            glVertex2f(rightEdge, topEdge);

            // Top edge
            glColor4f(topColor.r, topColor.g, topColor.b, topColor.a);
            glVertex2f(rightEdge, topEdge);
            glVertex2f(leftEdge, topEdge);

            glEnd();

            // Call Element::draw(e) to render child elements if any
            Element::draw(e);
        }
    };
}

#endif // Box_H