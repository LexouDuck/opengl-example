#ifndef Interface_Hpp
#define Interface_Hpp

#include <Dynamic/Element.hpp>
#include <Dynamic/Elements/Circle.hpp>

class Interface : public Element {

    public:

    // Children
    Elements::Circle* circle;

    // Constructor takes NO arguments (no parent as it is top level)
    Interface() {

        // Name and parent (none)
        this->name = "Interface";
        this->primeAncestor = this;

        this->style.size = Style::Size({ .width = Pct(100), .height = Pct(100) });
        this->style.background.color = Color(0.1, 0.1, 0.1, 1.0);


        this->circle = new Elements::Circle(this);
        circle->fillColor = Color(1, 0, 0, 1);
        circle->radius = 30;

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

        // Save initial position on click
        circle->onMouseDown([this](Event& e) {
            circle->refPos = circle->pos;
        });

        // Drag circle
        circle->onDrag([this](Event& e) {
            circle->pos = circle->refPos + e.mouse.drag;
            circle->refresh(e);
        });
    }

    // Returns own rect as there is no parent
    Rect getParentRect() override {
        return this->rect;
    }

    void refresh(Event& e) {
        this->tell(&Element::refresh, e);
    }
};

#endif