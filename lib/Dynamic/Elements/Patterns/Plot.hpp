#ifndef Plot_Hpp
#define Plot_Hpp

#include <Dynamic/Elements/Box.hpp>

using namespace Elements;

namespace Elements {

    class Plot : public Box {

        public:

        // Coordinates for the plot
        struct Coord {
            float x;
            float y;
        };

        // Range for the plot (NOT a rect)
        struct Range {
            
            float left = 0;
            float right = 10;
            float bottom = 0;
            float top = 10;

            Range() {}
            Range(float l, float r, float b, float t) : left(l), right(r), bottom(b), top(t) {}
        };

        // Power for scales
        struct Power {
            float x = 1;
            float y = 1;
        };

        Range range;
        Power power;
        std::vector<Coord> data;
        std::vector<float> horizontalRules;
        std::vector<float> verticalRules;

        Style::Color lineColor = Style::Color(1, 1, 1, 0.667);
        Style::Color fillColor = Style::Color(1, 1, 1, 0.1);
        Style::Color ruleColor = Style::Color(1, 1, 1, 0.1);

        // Constructor with default position, dimensions, and fill color
        Plot(Element* parent = nullptr): Box(parent) {

            this->name = "Plot";

            // Size and position
            this->style.size = Style::Size({ .width = Pct(100), .height = Px(100) });
            this->style.border.color = Style::Color(1, 1, 1, 0.667);    // White for now
        }

        // Draw function to render the plot
        void draw(Event& e) override {

            // Call parent draw function
            Box::draw(e);

            this->startScissor();

            // Ensure there are points to draw
            if (data.empty() || rect.width <= 0 || rect.height <= 0) return;

            // Calculate scale factors with power-law scaling
            float xScale = rect.width / (pow(range.right, power.x) - pow(range.left, power.x));
            float yScale = rect.height / (pow(range.top, power.y) - pow(range.bottom, power.y));

            // Draw filled area under the line using GL_TRIANGLE_STRIP
            glBegin(GL_TRIANGLE_STRIP);

            // Set the fill color
            glColor4f(fillColor.r, fillColor.g, fillColor.b, fillColor.a);

            for (const auto& coord : data) {
                // Apply power-law scaling
                float poweredX = pow(coord.x, power.x);
                float poweredY = pow(coord.y, power.y);

                // Map coordinates to rect space
                float mappedX = rect.x + (poweredX - pow(range.left, power.x)) * xScale;
                float mappedY = rect.y + rect.height - (poweredY - pow(range.bottom, power.y)) * yScale; // Flip Y for top-down rendering

                // Add vertices
                glVertex2f(mappedX, mappedY);            // Line vertex
                glVertex2f(mappedX, rect.y + rect.height); // Bottom vertex
            }

            glEnd();

            // Draw the line over the polygon
            glEnable(GL_LINE_SMOOTH);
            glBegin(GL_LINE_STRIP);

            // Set the line color
            glColor4f(lineColor.r, lineColor.g, lineColor.b, lineColor.a);

            for (const auto& coord : data) {
                // Apply power-law scaling
                float poweredX = pow(coord.x, power.x);
                float poweredY = pow(coord.y, power.y);

                // Map coordinates to rect space
                float mappedX = rect.x + (poweredX - pow(range.left, power.x)) * xScale;
                float mappedY = rect.y + rect.height - (poweredY - pow(range.bottom, power.y)) * yScale; // Flip Y for top-down rendering

                // Add the vertex
                glVertex2f(mappedX, mappedY);
            }

            glEnd();
            glDisable(GL_LINE_SMOOTH);

            // Draw horizontal rules
            if (!horizontalRules.empty()) {

                glBegin(GL_LINES);
                glColor4f(ruleColor.r, ruleColor.g, ruleColor.b, ruleColor.a);

                for (float ruleY : horizontalRules) {

                    float poweredY = pow(ruleY, power.y);
                    float mappedY = rect.y + rect.height - (poweredY - pow(range.bottom, power.y)) * yScale;

                    glVertex2f(rect.x, mappedY);
                    glVertex2f(rect.x + rect.width, mappedY);
                }

                glEnd();
            }

            // Draw vertical rules
            if (!verticalRules.empty()) {

                glBegin(GL_LINES);
                glColor4f(ruleColor.r, ruleColor.g, ruleColor.b, ruleColor.a);

                for (float ruleX : verticalRules) {

                    float poweredX = pow(ruleX, power.x);
                    float mappedX = rect.x + (poweredX - pow(range.left, power.x)) * xScale;

                    glVertex2f(mappedX, rect.y);
                    glVertex2f(mappedX, rect.y + rect.height);
                }

                glEnd();
            }

            this->endScissor();
        }
    };
}

#endif // Plot_Hpp