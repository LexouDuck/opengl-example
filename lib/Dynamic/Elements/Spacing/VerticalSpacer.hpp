#ifndef VerticalSpacer_H
#define VerticalSpacer_H

#include <Dynamic/Element.hpp>

namespace Elements {

    class VerticalSpacer : public Element {

    public:

        // Constructor with default position, dimensions, and fill color
        VerticalSpacer(Element* parent = nullptr, Dist dist = Px(10)): Element(parent) {

            this->name = "VerticalSpacer";

            // Use half the distance for both top/bottom
            Dist halfDist = dist;
            halfDist.val /= 2.f;

            // Zero high, 100% wide
            this->style.size = Style::Size({
                .width = Pct(100),
                .height = Px(0)
            });

            // Use margins for spacing
            this->style.margin = Margin({
                .top = halfDist,
                .bottom = halfDist
            });
        }
    };
}

#endif // VerticalSpacer_H