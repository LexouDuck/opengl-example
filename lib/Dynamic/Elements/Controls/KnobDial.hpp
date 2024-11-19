#ifndef KnobDial_H
#define KnobDial_H

#include <cmath>
#include <Dynamic/Element.h>
#include <dbg.hpp>

namespace Elements {

    class KnobDial : public Element {

    public:

        Style::Color fillColor = Style::Color(0.1, 0.1, 0.1, 1.0);
        Style::Color edgeColor = Style::Color(1, 1, 1, 1);
        Style::Dist edgeWidth = Style::Pct(20);

        Style::Color lineColor = Style::Color(1, 1, 1, 0.333);
        Style::Dist lineStart = Style::Pct(25);
        Style::Dist lineEnd = Style::Pct(100);
        Style::Dist lineWidth = Style::Pct(10);

        bool hasMouse = false;

        float val = 50;
        float lastVal = val;
        float min = 0;
        float max = 100;
        float startAngle = (-4.f/3.f) * M_PI;
        float endAngle = (1.f/3.f) * M_PI;

        // Constructor with default position, dimensions, and fill color
        KnobDial(Element* parent = nullptr): Element(parent) {

            this->name = "KnobDial";
            this->style.size.width = Style::Px(20);
            this->style.size.height = Style::Px(20);
        }

        void draw(Event& e) override {

            // The rect we're drawing within
            Rect drawRect = this->rect;

            // Circle
            //--------------------------------------------------

            float cx, cy, r;
            
            cx = drawRect.x + drawRect.width / 2;
            cy = drawRect.y + drawRect.height / 2;
            
            if (drawRect.width > drawRect.height) { r = drawRect.height / 2; }
            else { r = drawRect.width / 2; }

            float ew = 0;
            ew = edgeWidth.resolve(r);

            this->drawFillCircle(fillColor, cx, cy, r);

            glEnable(GL_LINE_SMOOTH);
            this->drawCircle(edgeColor, cx, cy, r, ew);
            //this->drawCircle(edgeColor, cx, cy, r, ew);
            glDisable(GL_LINE_SMOOTH);

            // Line
            //--------------------------------------------------

            float lw = 1.0;
            float r1 = 0, r2 = 1;
            float x1, y1, x2, y2;

            if (lineWidth) { lw = lineWidth.resolve(r); }
            if (lineStart) { r1 = lineStart.resolve(r); }
            if (lineEnd) { r2 = lineEnd.resolve(r) - ew; }

            // Calculate angle
            float range = endAngle - startAngle;
            float pctValue = (min + val) / (max - min);
            float theta = startAngle + pctValue * range;

            // A line from the center to the edge, with start/end radii
            x1 = cx + r1 * cos(theta); y1 = cy + r1 * sin(theta);
            x2 = cx + r2 * cos(theta); y2 = cy + r2 * sin(theta);

            // Draw line again
            glEnable(GL_LINE_SMOOTH);
            this->drawLine(lineColor, x1, y1, x2, y2, lw);
            glDisable(GL_LINE_SMOOTH);

            // Call Element::draw(e) if there are child elements
            Element::draw(e);
        }
    };
}

#endif // KnobDial_H