#include "opengl/core.hpp"
#include "opengl/context.hpp"
#include "glfw/glfw.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <filesystem>
#include <vector>
//#include "map_list_screen.hpp"

using namespace std;
using namespace gl;

namespace osu {
    extern void import_beatmap(filesystem::path);
}

int main()
{
    glfw::window window = glfw::create_window(800, 600, "osu!", {});

    window.set_drop_callback([](vector<filesystem::path> paths) {
        for(filesystem::path p : paths)
            osu::import_beatmap(p);
    });
    window.swap_interval(1);
    cout << "Init GL" << "\n";

    context context{wrap_context()};
    clear_color(0, 0, 0, 1);
    //map_list_screen mls;

    while (!window.should_close())
    {
        //clear(clear_buffer::color);
        //mls.render();
        window.swap_buffers();
        glfw::poll_events();
    }
}