#ifndef Knob_H
#define Knob_H

#include <cmath>
#include "../Text/Text.hpp"
#include "./KnobDial.hpp"
#include <Dynamic/Element.h>
#include "../Box.hpp"
#include <dbg.hpp>

namespace Elements {

    class Knob : public Element {

    public:

        Box* textContainer;
        Text* labelText;
        Text* displayText;
        KnobDial* dial;

        Style::Color fillColor = Style::Color(0.1, 0.1, 0.1, 1.0);
        Style::Color edgeColor = Style::Color(1, 1, 1, 1);
        Style::Dist edgeWidth = Style::Pct(20);

        Style::Color lineColor = Style::Color(1, 1, 1, 1);
        Style::Dist lineStart = Style::Pct(25);
        Style::Dist lineEnd = Style::Pct(100);
        Style::Dist lineWidth = Style::Pct(20);

        bool hasMouse = false;
        float val = 50;
        float lastVal = val;
        float min = 0;
        float max = 100;
        float startAngle = (-4.f/3.f) * M_PI;
        float endAngle = (1.f/3.f) * M_PI;

        // Constructor with default position, dimensions, and fill color
        Knob(Element* parent = nullptr): Element(parent) {

            this->name = "Knob";
            this->style.arrange = Arrangement::Horizontal;
            this->style.verticalAlign = Alignment::Center;
            this->style.horizontalAlign = Alignment::Start;

            this->dial = new KnobDial(this);
            this->dial->style.size.width = Style::Px(30);
            this->dial->style.size.height = Style::Px(30);
            this->dial->style.margin.top = Style::Px(4);
            this->dial->style.margin.bottom = Style::Px(4);
            this->dial->style.margin.left = Style::Px(4);
            this->dial->style.margin.right = Style::Px(4);

            this->dial->onMouseEnter([this](Event& e) {
                this->hasMouse = true;
                this->refresh(e);
            });

            this->dial->onMouseLeave([this](Event& e) {
                this->hasMouse = false;
                this->refresh(e);
            });

            this->dial->onMouseDown([this](Event& event) {
                this->lastVal = val;
            });

            this->dial->onDrag([this](Event& event) {

                // Update and clip value
                val = lastVal + (-1 * event.mouse.drag.y / 100) * (max - min);
                if (val < min) { val = min; }
                else if (val > max) { val = max; }

                this->refresh(event);
            });

            // Text container to hold the boxes
            this->textContainer = new Box(this);
            this->textContainer->style.margin.right = Style::Px(4);
            this->textContainer->style.verticalAlign = Alignment::Center;
            this->textContainer->style.horizontalAlign = Alignment::Start;

                // Label
                this->labelText = new Text(this->textContainer);
                this->labelText->setText("Knob");
                this->labelText->fontColor = Style::Color(1, 1, 1, 0.8);
                this->labelText->fontSize = 7;
                this->labelText->style.margin.bottom = Style::Px(4);

                // Display text
                this->displayText = new Text(this->textContainer);
                this->displayText->fontColor = Style::Color(1, 1, 1, 0.8);
                this->displayText->fontSize = 7;
                this->displayText->style.margin.bottom = Style::Px(4);
        }

        void beforeDraw(Event& e) override {

            if (this->hasMouse) { this->dial->edgeColor.a = 1.0; }
            else if (this->isDragTarget(e)) { this->dial->edgeColor.a = 1.0; }
            else { this->dial->edgeColor.a = 0.5; }

            this->dial->val = val;
            this->dial->min = min;
            this->dial->max = max;

            this->displayText->setText("%.2f", this->val);

            Element::beforeDraw(e);
        }
    };
}

#endif // Knob_H