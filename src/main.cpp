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
#include <csignal>
#include "map_list_screen.hpp"
#include "freetype/library.hpp"
#include "freetype/face.hpp"
#include "main.hpp"
#include "opengl/internal.hpp"
#include "stacktrace.hpp"
#include "beatmap.hpp"

using namespace std;
using namespace gl;

namespace osu {
    glfw::window* window;
    freetype::library main_lib;
}

int main0()
{
    osu::decoder::decode_diff(
        ifstream("Songs/820707 EDEN - Circles/EDEN - Circles (GreenHue) [Hard].osu", ios::binary)
    );
    freetype::face* face =
        osu::main_lib.face_from_istream(ifstream("CaviarDreams.ttf", iostream::binary));
    face->set_char_size(64*40, 0, 0, 0);

    osu::window = new glfw::window(800, 600, "osu!", {glfw::window::hints::opengl_debug_context{true}});
    cout << "window created" << "\n";

    osu::window->set_drop_callback([](vector<filesystem::path> paths) {
        for(filesystem::path p : paths) {
            cout << "callback " << p.string() << "\n";
            osu::import_beatmap(p);
        }
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

void on_error(int s) {
    cerr << get_stacktrace_string() << "\n";
    std::exit(-1);
}

int main() {
    std::signal(SIGABRT, on_error);
    std::signal(SIGSEGV, on_error);
    std::signal(SIGINT, on_error);
    try {
        return main0();
    }
    catch(const std::exception& e) {
        cerr << e.what() << "\n";
    }
    return -1;
}