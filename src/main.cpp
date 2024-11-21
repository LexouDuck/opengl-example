#include "./Window.hpp"
#include "./Interface.hpp"

int main(void) {

    // Create window
    Window* window = new Window();

    // Create interface (child of window)
    Interface* interface = new Interface(window);
    
    return window->spawn();
}
