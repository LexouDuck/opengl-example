#ifndef Color_Hpp
#define Color_Hpp

namespace Style {

    // General rgba Style::Color(hex and hsa suck)
    struct Color{

        float r = 0.0f; float g = 0.0f;
        float b = 0.0f; float a = 0.0f;
        bool set = false;

        Color() {}

        Color(float r, float g, float b, float a) {
            this->r = r; this->g = g;
            this->b = b; this->a = a;
            this->set = true;
        }

        // "if (color)" returns true
        operator bool() { return this->set; }
    };
}

#endif