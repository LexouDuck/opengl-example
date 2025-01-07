#ifndef KnobDial_H
#define KnobDial_H

#include <cmath>
#include <Dynamic/Element.hpp>
#include <dbg.hpp>

namespace Elements {

    class KnobDial : public Element {

    public:

        enum Mode {

            Normal,
            Hover,
            Drag
        };

        struct Sizes {

            Style::Dist normal;
            Style::Dist hover;
            Style::Dist drag;

            float resolve(Mode mode, float val) {

                Style::Dist* result;

                switch (mode) {
                    case (Mode::Normal): { result = &normal; break; }
                    case (Mode::Hover): { result = &hover; break; }
                    case (Mode::Drag): { result = &drag; break; }
                }

                if (!*result) { result = &normal; }
                return result->resolve(val);
            }
        };

        struct Colors {

            Style::Color normal;
            Style::Color hover;
            Style::Color drag;

            Style::Color& resolve(Mode mode) {

                Style::Color* result;

                switch (mode) {
                    case (Mode::Normal): { result = &normal; break; }
                    case (Mode::Hover): { result = &hover; break; }
                    case (Mode::Drag): { result = &drag; break; }
                }

                if (!*result) { result = &normal; }
                return *result;
            }
        };

        Colors edgeColor;
        Colors fillColor;
        Colors lineColor;

        Sizes edgeWidth;
        Sizes lineWidth;
        Sizes lineStart;
        Sizes lineEnd;

        Mode mode = Mode::Normal;

        // Value, start, and end
        float sensitivity = 1.f / 100.f;
        float pctVal = 0.5;
        float startAngle = (-4.f/3.f) * M_PI;
        float endAngle = (1.f/3.f) * M_PI;

        // Constructor with default position, dimensions, and fill color
        KnobDial(Element* parent = nullptr): Element(parent) {

            this->name = "KnobDial";
            style.size.width = Style::Px(20);
            style.size.height = Style::Px(20);

            // Dial circle
            edgeWidth.normal = Style::Pct(20);
            edgeColor.normal = Style::Color(1, 1, 1, .3f);
            edgeColor.hover = Style::Color(1, 1, 1, .6f);
            edgeColor.drag = Style::Color(1, 1, 1, 1.f);

            // Dial line
            lineColor.normal = Style::Color(1, 1, 1, .3f);
            lineWidth.normal = Style::Pct(10);
            lineStart.normal = Style::Pct(25);
            lineEnd.normal = Style::Pct(100);
        }

        void mouseEnter(Event& e) override {
            this->refresh(e);
            Element::mouseEnter(e);
        }

        void mouseLeave(Event& e) override {
            this->refresh(e);
            Element::mouseLeave(e);
        }

        void mouseDrag(Event& e) override {

            if (!e.mouse.lb) { return Element::mouseDrag(e); }
            
            float sensitivityMod = 1.f;
            if (e.keyboard.shift) { sensitivityMod = .5f; }

            pctVal += sensitivity * sensitivityMod * (e.mouse.diff.x - e.mouse.diff.y);
            
            if (pctVal > 1.f) { pctVal = 1.f; }
            else if (pctVal < 0.f) { pctVal = 0.f; }

            this->refresh(e);

            Element::mouseDrag(e);
        }

        void beforeDraw(Event& e) override {

            mode = Mode::Normal;

            if (this->contains(e.mouse.pos)) { mode = Mode::Hover; }
            if (this->isDragTarget(e)) { mode = Mode::Drag; }

            Element::beforeDraw(e);
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
            ew = edgeWidth.resolve(mode, r);

            Style::Color& drawEdgeColor = edgeColor.resolve(mode);
            Style::Color& drawFillColor = fillColor.resolve(mode);

            // If we should fill our circle
            if (drawFillColor) {
                this->drawFillCircle(drawFillColor, cx, cy, r);
            }

            if (drawEdgeColor) {
                glEnable(GL_LINE_SMOOTH);
                this->drawCircle(drawEdgeColor, cx, cy, r, ew);
                glDisable(GL_LINE_SMOOTH);
            }

            // Line
            //--------------------------------------------------

            Style::Color& drawLineColor = lineColor.resolve(mode);
            float drawLineWidth = lineWidth.resolve(mode, r);
            float drawLineStart = lineStart.resolve(mode, r);
            float drawLineEnd = lineEnd.resolve(mode, r) - ew;

            float lw = 1.0;
            float r1 = 0, r2 = 1;
            float x1, y1, x2, y2;

            if (drawLineWidth) { lw = drawLineWidth; }
            if (drawLineStart) { r1 = drawLineStart; }
            if (drawLineEnd) { r2 = drawLineEnd; }

            // Calculate angle
            float range = endAngle - startAngle;
            float theta = startAngle + pctVal * range;

            // A line from the center to the edge, with start/end radii
            x1 = cx + r1 * cos(theta); y1 = cy + r1 * sin(theta);
            x2 = cx + r2 * cos(theta); y2 = cy + r2 * sin(theta);

            // Draw line again
            glEnable(GL_LINE_SMOOTH);
            this->drawLine(drawLineColor, x1, y1, x2, y2, lw);
            glDisable(GL_LINE_SMOOTH);

            // Call Element::draw(e) if there are child elements
            Element::draw(e);
        }
    };
}

#endif // KnobDial_H