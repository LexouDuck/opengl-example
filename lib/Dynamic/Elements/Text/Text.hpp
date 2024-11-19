#ifndef Text_H
#define Text_H

#define STB_TRUETYPE_IMPLEMENTATION

#include <cmath>
#include "./Fonts/SwanSea.h"
#include <Dynamic/Element.h>
#include <stb/stb_truetype.h>
#include <dbg.hpp>
#include <fstream>
#include <vector>

namespace Elements {

    class Text : public Element {

    public:

        float borderRadius;
        Style::Color fontColor = Style::Color(.0, 1.0, 1.0, 1.0);
        float fontSize = 10.0f;
        float lineHeight = 2.0f;

        GLuint fontTexture;     // Texture for the font atlas
        stbtt_bakedchar charData[96];  // ASCII 32..126
        bool fontLoaded = false;
        float loadedFontSize = 0.0f;
        std::string text = "Hello\nworld!";

        float minX, maxX, minY, maxY;

        // Constructor with default position, dimensions, and fill color
        Text(Element* parent = nullptr) : Element(parent) {
            this->name = "Text";
        }

        void loadFont(float size) {

            // Do nothing if the size isn't different
            if (size == loadedFontSize) { return; }
            loadedFontSize = size;

            //dbg("Loading font");

            unsigned char tempBitmap[512 * 512];  // Temporary bitmap for font texture

            // Bake the font bitmap from the embedded font data
            stbtt_BakeFontBitmap(SwanSea_Ttf, 0, size, tempBitmap, 512, 512, 32, 96, charData);

            // Generate a texture for the font
            glGenTextures(1, &fontTexture);
            glBindTexture(GL_TEXTURE_2D, fontTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tempBitmap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            fontLoaded = true;
        }

        // Render the text inside the box with a specific height, or calculate its dimensions if render is false
        Rect renderText(const char* text, float x, float y, float desiredTextHeight, bool render = true) {

            loadFont(desiredTextHeight);

            // Save the original x position to reset after a line break
            float startX = x;

            // Initialize min and max values for bounding box
            float minX = std::numeric_limits<float>::max();
            float minY = std::numeric_limits<float>::max();
            float maxX = std::numeric_limits<float>::lowest();
            float maxY = std::numeric_limits<float>::lowest();

            if (render) {
                glBindTexture(GL_TEXTURE_2D, fontTexture);
                glEnable(GL_TEXTURE_2D);
                glBegin(GL_QUADS);
            }

            while (*text) {

                // On a newline, reset x to the starting position and move y down by the line height
                if (*text == '\n') {

                    x = startX;
                    y += desiredTextHeight * lineHeight;  // Adjust the y position to handle the new line
                    ++text;  // Move to the next character

                    continue;
                }

                if (*text >= 32 && *text < 128) {  // ASCII range
                    stbtt_aligned_quad q;
                    stbtt_GetBakedQuad(charData, 512, 512, *text - 32, &x, &y, &q, 1);

                    // Update bounding box min/max values
                    minX = std::min({minX, q.x0, q.x1});
                    minY = std::min({minY, q.y0, q.y1});
                    maxX = std::max({maxX, q.x0, q.x1});
                    maxY = std::max({maxY, q.y0, q.y1});

                    if (render) {
                        // Render character as a textured quad with the adjusted size
                        glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y0);
                        glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y0);
                        glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y1);
                        glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y1);
                    }
                }

                ++text;
            }

            if (render) {
                glEnd();
                glDisable(GL_TEXTURE_2D);
            }

            // Return the bounding box dimensions (Rect) without rendering if render is false
            return Rect(minX, minY, maxX - minX, maxY - minY);
        }

        Rect resolveStaticSize() override {

            //dbg("%s: Resolving size", this->name.c_str());

            Rect size = renderText(text.c_str(), 0, 0, fontSize, false);

            if (this->style.size.width && !this->style.size.width.setting) { size.width = this->style.size.width.val; }
            if (this->style.size.height && !this->style.size.height.setting) { size.height = this->style.size.height.val; }

            this->rect.width = size.width;
            this->rect.height = size.height;

            return size;
        }

        // Set the text content using a format string and variable arguments
        void setText(const char* format, ...) {

            // Create a buffer to hold the formatted string
            char buffer[512];  // Assuming a max buffer size of 512 characters

            // Start handling the variable arguments
            va_list args;
            va_start(args, format);
            vsnprintf(buffer, sizeof(buffer), format, args);
            va_end(args);

            // Set the text to the formatted string
            this->text = std::string(buffer);
        }

        void draw(Event& e) override {

            Rect drawRect = this->rect;

            // --- Draw the text "hello world" inside the box ---
            float textX = drawRect.x;  // Slight padding from the left edge
            float textY = drawRect.y + drawRect.height;  // Slight padding from the top

            glColor4f(fontColor.r, fontColor.g, fontColor.b, fontColor.a);  // Set text Style::Color(black)
            renderText(text.c_str(), textX, textY, fontSize);

            // Call Element::draw(e) if there are child elements
            Element::draw(e);
        }
    };
}

#endif // Text_H