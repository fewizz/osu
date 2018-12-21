#include "opengl/core.hpp"
#include "opengl/context.hpp"
#include "glfw/glfw.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <filesystem>

using namespace std;
using namespace gl;

namespace osu {
    extern void import_beatmap(filesystem::path);
}

int main()
{
    glfw::window window = glfw::create_window(800, 600, "osu!", {});

    glfwSetDropCallback(window, [](GLFWwindow*,int count,const char** paths) {
        for(int p = 0; p < count; p++)
            osu::import_beatmap(filesystem::path(paths[p]));
    });
    window.swap_interval(1);
    cout << "Init GL" << "\n";

    context context{wrap_context()};
    clear_color(0, 0, 0, 1);

    while (!window.should_close())
    {
        clear(clear_buffer::color);

        window.swap_buffers();
        glfw::poll_events();
    }
}