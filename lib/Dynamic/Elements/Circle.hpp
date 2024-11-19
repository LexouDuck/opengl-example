#ifndef Circle_H
#define Circle_H

#include <cmath>
#include <Dynamic/Element.hpp>
#include <dbg.hpp>

namespace Elements {

    class Circle : public Element {

    public:

        Style::Color fillColor = Style::Color(0, 0, 0, 1);
        Style::Color edgeColor;
        Style::Dist edgeWidth = Style::Px(1);

        Pos pos;
        Pos refPos = pos;
        float radius = 10.0;

        // Constructor with default position, dimensions, and fill color
        Circle(Element* parent = nullptr): Element(parent) {
            this->name = "Circle";
        }

        // We must set our own size if we have a radius
        void beforeDraw(Event& e) override {

            this->style.size.width = Style::Px(2 * radius);
            this->style.size.height = Style::Px(2 * radius);

            if (pos) {
                this->style.position.left = Style::Px(pos.x - radius);
                this->style.position.top = Style::Px(pos.y - radius);
            }

            Element::beforeDraw(e);
        }

        void draw(Event& e) override {

            float cx, cy, r;
            cx = rect.x + rect.width / 2;
            cy = rect.y + rect.height / 2;
            
            if (rect.width > rect.height) { r = rect.height / 2; }
            else { r = rect.width / 2; }

            float ew = 0;
            if (edgeWidth) { ew = edgeWidth.resolve(r); }

            glEnable(GL_LINE_SMOOTH);
            if (fillColor) { drawFillCircle(fillColor, cx, cy, r); }
            if (edgeColor) { drawCircle(edgeColor, cx, cy, r, ew); }
            glDisable(GL_LINE_SMOOTH);

            Element::draw(e);
        }
    };
}

#endif // Circle_H