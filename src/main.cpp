#include "opengl/core.hpp"
#include "opengl/context.hpp"
#include "opengl/debug.hpp"
#include "glfw/glfw.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <filesystem>
#include <vector>
#include <fstream>
#include "map_list_screen.hpp"
#include "freetype/library.hpp"
#include "freetype/face.hpp"
#include "main.hpp"

using namespace std;
using namespace gl;

namespace osu {
    glfw::window* window;
    freetype::library main_lib;
}

int main()
{
    freetype::face* face =
        osu::main_lib.face_from_istream(ifstream("Pacifico.ttf", iostream::binary));

    osu::window = new glfw::window(glfw::create_window(800, 600, "osu!", {}));
    cout << "window created" << "\n";

    osu::window->set_drop_callback([](vector<filesystem::path> paths) {
        for(filesystem::path p : paths)
            osu::import_beatmap(p);
    });
    cout << "Init GL" << "\n";
    osu::window->make_context_current();
    cout << "make context current" << "\n";
    osu::window->swap_interval(1);

    debug_message_callback([](string str) {
        cout << str << "\n";
    });
    cout << "debug callback set" << "\n";

    context context{wrap_context()};
    clear_color(0, 0, 0, 1);
    map_list_screen mls(*face);

    while (!osu::window->should_close())
    {
        //clear(clear_buffer::color);
        mls.render();
        osu::window->swap_buffers();
        glfw::poll_events();
    }
}