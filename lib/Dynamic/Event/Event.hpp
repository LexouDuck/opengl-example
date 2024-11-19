#ifndef Event_Hpp
#define Event_Hpp

#include <Structs/Pos.hpp>

namespace Dynamic {

    struct Event {

        struct Mouse {

            Pos down;
            Pos up;
            Pos pos;
            Pos drag;
            Pos wheel;

            bool lb = false;
            bool mb = false;
            bool rb = false;

            Mouse() {}
        };

        struct Keyboard {

            bool ctrl = false;
            bool shift = false;
            bool alt = false;
        };

        int id = 0; // To keep track of continuous events (id is only reset when button state is changed)
        bool propagate = true;
        void* subject = nullptr;

        Mouse mouse;
        Keyboard keyboard;

        Event() {}

        template <typename T>
        T* getSubject() {
            return static_cast<T*>(subject);
        }
    };
}

#endif