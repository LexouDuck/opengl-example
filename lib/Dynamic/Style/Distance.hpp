#ifndef Distance_Hpp
#define Distance_Hpp

// Proportions
#define STATIC 0
#define RELATIVE 1
#define GROW 2
#define SHRINK 3

namespace Style {

    // A general "distance" struct for storing absolute/proportional distances
    struct Dist {

        float val = 0;              // The nominal distance
        int setting = STATIC;
        bool set = false;

        Dist() {};

        // Constructor
        Dist(float val, int setting) {
            this->val = val;
            this->setting = setting;
            this->set = true;
        }

        // Inline function to quickly resolve actual value
        inline float resolve(float compare) {
            
            switch (setting) {
                case (STATIC): { return val; }
                case (RELATIVE): { return val * compare; }
            }

            return val;
        }

        // "if (dist)" returns true
        operator bool() { return this->set; }
    };

    // Free-standing function to create an absolute distance
    static Dist Px(float value) { return Dist(value, STATIC); }
    static Dist Pct(float percentage) { return Dist(percentage / 100.0f, RELATIVE); }
    static Dist Grow() { return Dist(0, GROW); }
    static Dist Shrink() { return Dist(0, SHRINK); }
}

#endif