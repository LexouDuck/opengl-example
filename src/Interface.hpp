#ifndef Interface_Hpp
#define Interface_Hpp

#include <Dynamic/Element.hpp>
#include <Dynamic/Elements/Circle.hpp>
#include <Dynamic/Elements/Text/Text.hpp>

class Interface : public Element {

    public:

    // Children
    Elements::Circle* circle;
    Elements::Text* label;

    Interface(Element* parent = nullptr): Element(parent) {

        // Name and parent (none)
        this->name = "Interface";

        this->style.size = Style::Size({ .width = Pct(100), .height = Pct(100) });
        this->style.background.color = Color(0.1, 0.1, 0.1, 1.0);

        // Make circle
        this->circle = new Elements::Circle(this);
        circle->pos = Pos(40, 40);
        circle->fillColor = Color(1, 0, 0, 1);
        circle->radius = 30;
        circle->style.verticalAlign = Alignment::Center;
        circle->style.horizontalAlign = Alignment::Center;

        // Make label to go inside circle
        this->label = new Elements::Text(circle);
        this->label->setText("Drag\nMe");


        // Callbacks
        //--------------------------------------------------

        // Blue when mouse enters
        circle->onMouseEnter([this](Event& e) {
            circle->fillColor = Color(0, 0, 1, 1);
            circle->refresh(e);
        });

        // Red when mouse leaves
        circle->onMouseLeave([this](Event& e) {
            circle->fillColor = Color(1, 0, 0, 1);
            circle->refresh(e);
        });

        // Drag circle
        circle->onDrag([this](Event& e) {
            circle->pos = circle->pos + e.mouse.diff;
            circle->refresh(e);
        });
    }
};

#endif