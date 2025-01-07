#ifndef Event_Hpp
#define Event_Hpp

#include <ctime>
#include <Structs/Pos.hpp>

namespace Dynamic {

    struct Event {

        struct Button {

            int id = 0;
            std::chrono::steady_clock::time_point lastPressTime;
            int pressTimeDiff = 0;

            // The button has a pos so we know when we last clicked it
            Pos lastPressPos;
            Pos pressPosDiff;

            void set(bool down, Pos pos = Pos(0, 0)) {

                if (down) { this->press(pos); }
                else { this->release(); }
            }

            void press(Pos& pos) {

                // Increment id
                id = abs(id) + 1;

                // Set last press time
                auto now = std::chrono::steady_clock::now();
                pressTimeDiff = std::chrono::duration<double, std::milli>(now - lastPressTime).count();
                lastPressTime = now;

                // Set last press pos
                pressPosDiff = (pos - lastPressPos);
                lastPressPos = pos;
            }

            void release() {

                // Change sign of id
                id *= -1;
            }

            // Return whether this is a double click (must be within length / time window)
            bool isDoubleClick(float timeThresh = 200, float lenThresh = 10) {
                return (
                    pressTimeDiff < timeThresh &&
                    pressPosDiff.pythag() < lenThresh
                );
            }

            // Return true if pressed
            operator bool() {
                return (this->id > 0);
            }
        };

        struct Mouse {

            Pos down;
            Pos up;
            Pos pos;
            Pos drag;
            Pos diff;
            Pos dragStart;
            Pos dragEnd;
            Pos wheel;

            Button lb;
            Button mb;
            Button rb;

            Mouse() {}
        };

        struct Keyboard {

            Button ctrl;
            Button shift;
            Button alt;
            Button del;
        };

        Mouse mouse;
        Keyboard keyboard;

        int id = 0; // To keep track of continuous events (id is only reset when button state is changed)
        bool propagate = true;
        void* subject = nullptr;

        Event() {}

        template <typename T>
        T* getSubject() {
            return static_cast<T*>(subject);
        }
    };
}

#endif