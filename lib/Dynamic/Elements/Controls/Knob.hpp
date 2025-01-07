#ifndef Knob_H
#define Knob_H

#include <cmath>
#include "../Text/Text.hpp"
#include "./KnobDial.hpp"
#include <Dynamic/Element.hpp>
#include "../Box.hpp"
#include <dbg.hpp>

namespace Elements {

    class Knob : public Element {

    public:

        struct HintMap {

            // Hint and hints vector
            struct Hint {

                float val;
                std::string text;

                Hint() {}

                Hint(float val, std::string text) {
                    this->val = val;
                    this->text = text;
                }
            };

            std::vector<Hint> hints;

            // Add hint
            void addHint(std::string text, float val) {
                hints.emplace_back(val, text);
            }

            // A terrible and inefficient hint-getting algorithm
            std::string getHint(float val) {

                if (hints.empty()) { return ""; }

                float closestDistance = std::numeric_limits<float>::max();
                const Hint* closestHint = nullptr;

                for (const auto& hint : hints) {

                    float distance = std::abs(hint.val - val);
                    
                    if (distance < closestDistance) {
                        closestDistance = distance;
                        closestHint = &hint;
                    }
                }

                if (!closestHint) { return ""; }
                return closestHint->text;
            }
        };

        HintMap hintMap;

        std::vector<float*> attached;
        float def = 50;
        float val = 50;
        float min = 0;
        float max = 100;

        std::string unit = "";
        float power = 1.f;

        // Children
        Box* textContainer;
        Text* labelText;
        Text* displayText;
        KnobDial* dial;

        // Constructor with default position, dimensions, and fill color
        Knob(Element* parent = nullptr): Element(parent) {

            this->name = "Knob";
            style.arrange = Arrangement::Horizontal;
            style.verticalAlign = Alignment::Center;
            style.horizontalAlign = Alignment::Start;

            this->dial = new KnobDial(this);
            dial->style.size = Style::Size({ .width = Px(26), .height = Px(26) });

            dial->onMouseDown([this](Event& e) {

                // Reset on double click
                if (e.mouse.lb.isDoubleClick()) { 

                    this->val = this->def;
                    this->dial->pctVal = this->toDial(this->val);

                    this->refresh(e);
                }
            });

            // Text container to hold the boxes
            this->textContainer = new Box(this);
            textContainer->style.margin.left = Style::Px(8);
            textContainer->style.verticalAlign = Alignment::Center;

                // Label
                this->labelText = new Text(textContainer);
                labelText->setText("Knob");
                labelText->fontColor = Style::Color(1, 1, 1, 0.8);
                labelText->fontSize = 6;
                labelText->style.margin.bottom = Style::Px(4);

                // Display text
                this->displayText = new Text(textContainer);
                displayText->fontColor = Style::Color(1, 1, 1, 0.8);
                displayText->fontSize = 6;
                displayText->style.margin.bottom = Style::Px(4);
        }

        // Attach value pointer
        void attach(float* valPtr) {
            if (valPtr && std::find(attached.begin(), attached.end(), valPtr) == attached.end()) {
                attached.push_back(valPtr);
            }
        }

        // Detach value pointer
        void detach(float* valPtr) {
            attached.erase(std::remove(attached.begin(), attached.end(), valPtr), attached.end());
        }

        // Convert from dial to value
        inline float fromDial(float pctVal) {
            return min + std::powf(pctVal, power) * (max - min);
        }

        // Convert from value to dial
        inline float toDial(float absVal) {
            return std::powf((absVal - min) / (max - min), 1.f / power);
        }

        void refresh(Event& e) override {

            // Transform dial value into actual value
            val = fromDial(dial->pctVal);

            // Set attached values
            for (float* ptr : attached) {
                if (ptr) { *ptr = val; }
            }

            Element::refresh(e);
        }

        void beforeDraw(Event& e) override {

            // Set based on attached values
            for (float* ptr : attached) {
                if (ptr) { val = *ptr; break; } // No knob can serve two masters
            }
            
            
            dial->pctVal = toDial(val);
            
            // Get hint
            std::string hint = hintMap.getHint(val);

            this->displayText->setText("%.2f%s %s", val, unit.c_str(), hint.c_str());

            Element::beforeDraw(e);
        }
    };
}

#endif // Knob_H