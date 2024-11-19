#ifndef PolarPattern_H
#define PolarPattern_H

#include <Dynamic/Element.h>
#include <dbg.hpp>
#include <Structs/PolarEquation.hpp> // Include the PolarEquation header

namespace Elements {

    class PolarPattern : public Element {

    public:

        // Parameters
        float angle = 0;
        float numPoints = 100;

        Style::Dist lineWidth = Style::Px(1); // Default line width
        Style::Dist scale = Style::Px(10);

        Style::Color lineColor = Style::Color(1, 1, 1, 1);
        Style::Color fillColor;

        PolarEquation equation; // Use PolarEquation for radius values
        std::vector<std::pair<float, float>> vertices;

        // Track angle changes
        float lastAngle = angle;

        // Constructor with default position, dimensions, and fill color
        PolarPattern(Element* parent = nullptr) : Element(parent) {

            this->name = "PolarPattern";
            this->style.size.width = Style::Px(0);
            this->style.size.height = Style::Px(0);

            equation.addPoint(0, 1);
            equation.addPoint(M_PI, 0);
        }

        bool contains(Pos& pos) override {

            // Center of the PolarPattern
            float cx = rect.x + rect.width / 2;
            float cy = rect.y + rect.height / 2;

            // Translate point to PolarPattern's local coordinates
            float localX = pos.x - cx;
            float localY = pos.y - cy;

            // Convert to polar coordinates
            float pointRadius = sqrt(localX * localX + localY * localY);
            float pointAngle = atan2(localY, localX);

            // Adjust for the PolarPattern's rotation
            float adjustedAngle = pointAngle + this->angle;

            // Use PolarEquation to get the radius at this angle
            float equationRadius = equation.atAngle(adjustedAngle) * scale.resolve(1.0);

            // Check if the point is within the PolarPattern's boundary
            return pointRadius <= equationRadius;
        }

        void evaluate() {

            vertices.clear();

            float absScale = scale.resolve(1.0);
            float angleIncrement = (2 * M_PI) / numPoints;
            float rotationAngle = angle;

            for (int i = 0; i < numPoints; ++i) {

                float theta = i * angleIncrement;
                float radius = equation.atAngle(theta) * absScale;

                // Calculate the x and y, then apply rotation by `angle`
                float x = radius * cos(theta - angle);
                float y = radius * sin(theta - angle);

                vertices.emplace_back(x, y);
            }
        }

        void beforeDraw(Event& e) override {
            this->evaluate();
            Element::beforeDraw(e);
        }

        void draw(Event& e) override {

            float centerX = rect.x + rect.width / 2;
            float centerY = rect.y + rect.height / 2;
            float lw = lineWidth.resolve(1.0);

            // Draw filled pattern using GL_TRIANGLE_FAN if fillColor is set
            if (fillColor) {
                
                glColor4f(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
                glBegin(GL_TRIANGLE_FAN);

                // Start at the center of the polar coordinates
                glVertex2f(centerX, centerY);

                // Loop through each vertex to form the fan
                for (auto& point : vertices) {
                    glVertex2f(centerX + point.first, centerY + point.second);
                }

                // Close the fan by connecting back to the first vertex
                glVertex2f(centerX + vertices[0].first, centerY + vertices[0].second);

                glEnd();
            }

            // Draw line if lineColor and lineWidth are set
            if (lineColor && lw > 0) {

                glEnable(GL_LINE_SMOOTH);

                glLineWidth(lineWidth.val);
                glColor4f(lineColor.r, lineColor.g, lineColor.b, lineColor.a);

                glBegin(GL_LINE_LOOP);

                for (auto& point : vertices) {
                    glVertex2f(centerX + point.first, centerY + point.second);
                }

                glDisable(GL_LINE_SMOOTH);
                glEnd();
            }

            // Call Element::draw(e) if there are child elements
            Element::draw(e);
        }
    };
}

#endif // PolarPattern_H
