#include <chrono>
#include <GLFW/glfw3.h>
#include <Dynamic/Element.hpp>
#include <dbg.hpp>

#include "./Interface/Interface.hpp"
#include "./EventBridge.hpp"

int main(void) {

    // Initialize self and top level element
    //--------------------------------------------------

    // Initialize and return with error if fail
    int initialized = glfwInit();
    if (!initialized) { return -1; }


    // Create window and register callbacks
    //--------------------------------------------------

    GLFWwindow* window;
    float initialWidth = 640;
    float initialHeight = 480;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(
        initialWidth,
        initialHeight,
        "Hello World",
        NULL, NULL
    );

    // If window creation failed, terminate with error
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Create element and set up draw loop
    //--------------------------------------------------

    // Create and call resize function
    Interface* interface = new Interface();

    // When the interface refreshes, do a redraw
    interface->onRefresh([interface, window](Dynamic::Event& e) {

        glClear(GL_COLOR_BUFFER_BIT);    	// Clear buffer

        // Go through all the draw steps
        interface->beforeDraw(e);
        interface->resolveStaticSize();
        interface->resolveRelativeSize();
        interface->arrangeChildren();
        interface->centerChildren();
        interface->calcScroll(0);
        interface->calcInnerRect();
        interface->drawSelf(e);

        glfwSwapBuffers(window);        	// Swap front and back buffers
    });

    // Poll events after setting things up
    //--------------------------------------------------

    // Set context and callback functions
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, EventBridge::mouseButton);
	glfwSetCursorPosCallback(window, EventBridge::mouseMove);
    glfwSetScrollCallback(window, EventBridge::mouseWheel);
    glfwSetKeyCallback(window, EventBridge::key);
    glfwSetWindowSizeCallback(window, EventBridge::resize);

    EventBridge::element = interface;
    EventBridge::resize(window, initialWidth, initialHeight);

    // Thread pauses when no events
    while (!glfwWindowShouldClose(window)) {

        glfwWaitEvents();
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
