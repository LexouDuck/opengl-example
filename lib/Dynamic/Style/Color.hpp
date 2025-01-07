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

        inline Color operator*(const Color& other) const { return Color(r * other.r, g * other.g, b * other.b, a * other.a); }

        // "if (color)" returns true
        operator bool() { return this->set; }
    };
}

#endif