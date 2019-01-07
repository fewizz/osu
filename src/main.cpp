#include "opengl/core.hpp"
#include "opengl/context.hpp"
#include "opengl/debug.hpp"
#include "openal/al.hpp"
#include "openal/alc.hpp"
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
#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>
#include <functional>

using namespace std;
using namespace gl;

namespace osu {
    vector<beatmap_info> loaded_beatmaps;
    glfw::window* window;
    freetype::library main_lib;

    namespace worker {
        //function<void()> end([](){});
        vector<function<void()>> tasks;
        mutex m;
        condition_variable cv;

        void add_task(function<void()> t) {
            m.lock();
            tasks.push_back(t);
            m.unlock();
            cv.notify_one();
        }

        void end() {
            cv.notify_one();
        }
    }
}

int main0()
{
    filesystem::directory_iterator it{"songs"};
    std::for_each(filesystem::directory_iterator{"songs"}, filesystem::directory_iterator{},
        [](filesystem::directory_entry entry) {
            osu::load_beatmap(entry.path());
        }
    );

    freetype::face* face =
        osu::main_lib.face_from_istream(ifstream("CaviarDreams.ttf", iostream::binary));
    face->set_char_size(64*40, 0, 0, 0);

    cout << "Init AL" << "\n";
    alc::device dev = alc::open_device();
    cout << "al: dev" << "\n";
    alc::context al_context = dev.create_context();
    cout << "al: con" << "\n";
    alc::make_context_current(al_context);
    cout << "al: make_current" << "\n";
    
    thread t([]() {
        unique_lock lock(osu::worker::m);

        while(true) {
            osu::worker::cv.wait(lock);
            if(osu::worker::tasks.empty())
                return;
            osu::worker::tasks.back();
            osu::worker::tasks.pop_back();
        }
    });

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

    osu::worker::end();
    t.join();
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