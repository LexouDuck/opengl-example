#ifndef Window_Hpp
#define Window_Hpp

#define GL_MULTISAMPLE 0x809D
#define GL_MULTISAMPLE_FILTER_HINT_NV     0x8534

#include <GLFW/glfw3.h>
#include <Dynamic/Element.hpp>

class Window : public Element {

    public:

    static Window* self;
    GLFWwindow* window;
    int initialized = -1;
    float initialWidth = 640;
    float initialHeight = 480;

    Window(float width = 640, float height = 480) {

        this->name = "Hello World";
        this->primeAncestor = this;

        this->initialWidth = width;
        this->initialHeight = height;

        Window::self = this;
    }

    void refresh(Event& e) override {

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_MULTISAMPLE);

        // Clear all pixel information
        glClear(GL_COLOR_BUFFER_BIT);

        // Go through all the draw steps
        this->beforeDraw(p);
        this->resolveStaticSize();
        this->resolveRelativeSize();
        this->arrangeChildren();
        this->centerChildren();
        this->calcScroll(0);
        this->calcInnerRect();
        this->drawSelf(p);

        // Swap old buffer for new buffer
        glfwSwapBuffers(window);

        glDisable(GL_BLEND);
        glDisable(GL_MULTISAMPLE);
    }

    // Returns own rect as there is no parent
    Rect getParentRect() override {
        return this->rect;
    }
    
    /*
        IMPORTANT:

        Below are functions pertinent to the creation / destruction of
        windows, bridging all events stemming therefrom to this element,
        among other miscellaneous concerns
    
    */

    /*  We store a persistent event 'p', the reference to which is
        then passed to down-tree event handlers */
    Event p;

    // Mouse button press/release logic
    //--------------------------------------------------------------------------------

    // Define mouse buttons
    enum MouseButton {

        Left = GLFW_MOUSE_BUTTON_1,
        Middle = GLFW_MOUSE_BUTTON_3,
        Right = GLFW_MOUSE_BUTTON_2
    };

    // When a mouse button is pressed or released
    static void glfwMouseButton(GLFWwindow* window, int button, int action, int mods) {

        Window* self = Window::self;
        Event& p = self->p;

        // We increment the id to distinguish events (will eventually move to a different system)
        p.id += 1;

        bool press = (action == GLFW_PRESS);

        // Set correct button
        switch (button) {

            case (MouseButton::Left): { p.mouse.lb = press; break; }
            case (MouseButton::Middle): { p.mouse.mb = press; break; }
            case (MouseButton::Right): { p.mouse.rb = press; break; }
        }

        // Set dragStart / dragEnd
        if (press) { p.mouse.dragStart = p.mouse.pos; }
        else { p.mouse.dragEnd = p.mouse.pos; }

        // Trigger mouseDown / mouseUp
        if (press) { self->mouseDown(p); }
        else { self->mouseUp(p); }
    }

    // Move move logic
    //--------------------------------------------------------------------------------

    static void glfwMouseMove(GLFWwindow* window, double x, double y) {

        Window* self = Window::self;
        Event& p = self->p;

        Pos newPos = Pos(x, y);

        p.mouse.diff = newPos - p.mouse.pos;
        p.mouse.pos = newPos;

        // If lb is pressed (means we're dragging)
        if (p.mouse.lb) {
            p.mouse.drag = p.mouse.pos - p.mouse.dragStart;
        }

        else { p.mouse.drag = Pos(0, 0); }

        self->mouseMove(p);
    }

    // Scroll logic
    //--------------------------------------------------------------------------------

    static void glfwMouseWheel(GLFWwindow* window, double dx, double dy) {

        Window* self = Window::self;
        Event& p = self->p;

        p.mouse.wheel.x = dx;
        p.mouse.wheel.y = dy;

        self->mouseWheel(p);
    }

    // Keyboard logic
    //--------------------------------------------------------------------------------

    // Define keys
    enum Key {

        Ctrl = 0
    };

    static void glfwKey(GLFWwindow* window, int key, int scancode, int action, int mods) {

        Window* self = Window::self;
        Event& p = self->p;
        
        if (action == GLFW_PRESS) {
            dbg("Key pressed: %d", key);
        } else if (action == GLFW_RELEASE) {
            dbg("Key released: %d", key);
        }
    }

    // Window resize logic
    //--------------------------------------------------------------------------------

    static void glfwResize(GLFWwindow* window, int width, int height) {

        Window* self = Window::self;
        Event& p = self->p;

        // Update the OpenGL viewport
        glViewport(0, 0, width, height);

        // Update projection matrix
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, height, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        self->style.size.width = Style::Px(width);
        self->style.size.height = Style::Px(height);
        self->refresh(p);
    }

    // Window move logic (to be implemented)
    //--------------------------------------------------------------------------------

    static void glfwMove(GLFWwindow* window, int x, int y) {

        Window* self = Window::self;
        Event& p = self->p;
    }

    // Create GLFW window and point events to our (static) handler functions
    //--------------------------------------------------------------------------------

    int spawn() {

        initialized = glfwInit();
        if (!initialized) { return -1; }

        // Create a windowed mode window and its OpenGL context
        window = glfwCreateWindow(
            initialWidth,
            initialHeight,
            "Hello Wore3ld",
            NULL, NULL
        );

        // If window creation failed, terminate with error
        if (!window) {
            glfwTerminate();
            return -1;
        }

        // Set context and callback functions
        glfwMakeContextCurrent(window);
        glfwSetMouseButtonCallback(window, glfwMouseButton);
        glfwSetCursorPosCallback(window, glfwMouseMove);
        glfwSetScrollCallback(window, glfwMouseWheel);
        glfwSetKeyCallback(window, glfwKey);
        glfwSetWindowSizeCallback(window, glfwResize);
        glfwSetWindowPosCallback(window, glfwMove);

        Window::glfwResize(window, initialWidth, initialHeight);

        // Thread pauses when no events
        while (!glfwWindowShouldClose(window)) {

            glfwWaitEvents();
            glfwPollEvents();
        }

        glfwTerminate();

        return 0;
    }
};

/* 
    BAD: will need to find a better way to deal
    with the static function requirement. Considering 
    A map of initialized windows but that's just a
    stage 1 solution.
*/

Window* Window::self = nullptr;

#endif // Window_Hpp