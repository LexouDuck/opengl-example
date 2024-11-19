#ifndef EventBridge_Hpp
#define EventBridge_Hpp

/*
    Obviously, something must actually tell the parent element what's occuring, and depending
    on the system, the events will come in different styles and flavors. Therefore, a bridge
    is necessary to recieve callbacks from the window's source and proceed to interperet them
    and trigger the necessary events in the parent element.
*/

#include <dbg.hpp>
#include <Dynamic/Element.hpp>
#include <Dynamic/Event/Event.hpp>

namespace EventBridge {

    // Keep track of current event
    Element* element = nullptr;
    Dynamic::Event e;

    // When a mouse button is pressed or released
    void mouseButton(GLFWwindow* window, int button, int action, int mods) {

    }

    // When the mouse moves
    void mouseMove(GLFWwindow* window, double x, double y) {

        e.mouse.pos.x = x;
        e.mouse.pos.y = y;

        if (!element) { return; }
        element->mouseMove(e);
    }

    // When the mouse scrolls
    void mouseWheel(GLFWwindow* window, double dx, double dy) {

        e.mouse.wheel.x = dx;
        e.mouse.wheel.y = dy;

        if (!element) { return; }
        element->mouseWheel(e);
    }

    // When a key is pressed or released
    void key(GLFWwindow* window, int key, int scancode, int action, int mods) {
        
        if (action == GLFW_PRESS) {
            dbg("Key pressed: %d", key);
        } else if (action == GLFW_RELEASE) {
            dbg("Key released: %d", key);
        }
    }

    // When window is resized
    void resize(GLFWwindow* window, int width, int height) {

        // Update the OpenGL viewport
        glViewport(0, 0, width, height);

        // Update projection matrix
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, 0, height, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Notify the Interface about the resize
        if (element) {
            element->style.size.width = Style::Px(width);
            element->style.size.height = Style::Px(height);
            element->refresh(e);
        }
    }
}


#endif