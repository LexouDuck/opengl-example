#ifndef Style_Hpp
#define Style_Hpp

#include "./Size.hpp"
#include "./Position.hpp"
#include "./Margin.hpp"
#include "./Padding.hpp"
#include "./Border.hpp"
#include "./Background.hpp"

namespace Style {

    enum class Arrangement {
        Horizontal = 0,
        Vertical = 1
    };

    enum class Alignment {
        Start = 0,
        End = 1,
        Center = 2
    };

    enum class Overflow {
        Hidden = 0,
        Visible = 1,
        Scroll = 2,
        Wrap = 3
    };

    struct Style {

        Position position;
        Size size;
        Margin margin;
        Padding padding;
        Border border;
        Background background;

        Overflow overflow = Overflow::Visible;
        Alignment verticalAlign = Alignment::Start;
        Alignment horizontalAlign = Alignment::Start;
        Arrangement arrange = Arrangement::Vertical;

        Style() {}
    };
}

#endif