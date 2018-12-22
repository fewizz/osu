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
#include "opengl/internal.hpp"

using namespace std;
using namespace gl;

namespace osu {
    glfw::window* window;
    freetype::library main_lib;
}

int main0()
{
    freetype::face* face =
        osu::main_lib.face_from_istream(ifstream("Pacifico.ttf", iostream::binary));
    face->set_char_size(64*60, 0, 0, 0);

    osu::window = new glfw::window(glfw::create_window(800, 600, "osu!", {}));
    cout << "window created" << "\n";

    osu::window->set_drop_callback([](vector<filesystem::path> paths) {
        for(filesystem::path p : paths)
            osu::import_beatmap(p);
    });
    osu::window->swap_interval(1);
    cout << "Init GL" << "\n";
    osu::window->make_context_current();
    cout << "make context current" << "\n";
    wrap_context();

    cout << "GL: " << current_context()->get_major() << "." << current_context()->get_minor() << "\n";
    debug_message_callback([](string str) {
        cout << str << "\n";
    });
    cout << "debug callback set" << "\n";

    clear_color(0, 0, 0, 1);
    cout << "set clear color" << "\n";
    map_list_screen mls(*face);

    while (!osu::window->should_close())
    {
        viewport(
            0,
            0,
            osu::window->get_framebuffer_size().first,
            osu::window->get_framebuffer_size().second
        );
        clear(clear_buffer::color);
        mls.render();
        osu::window->swap_buffers();
        glfw::poll_events();
    }
    return 0;
}

int main() {
    try {
        return main0();
    }
    catch(const std::exception& e) {
        cerr << e.what() << "\n";
    }
    return -1;
}