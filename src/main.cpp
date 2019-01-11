#include "main.hpp"
#include "opengl/core.hpp"
#include "opengl/context.hpp"
#include "opengl/debug.hpp"
#include "openal/al.hpp"
#include "openal/alc.hpp"
#include "glfw/glfw.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <filesystem>
#include <vector>
#include <fstream>
#include <csignal>
#include "map_list_screen.hpp"
#include "freetype/library.hpp"
#include "freetype/face.hpp"
#include "freetype/face.hpp"
#include "opengl/internal.hpp"
#include "stacktrace.hpp"
#include "beatmap.hpp"
#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>
#include <functional>
#include "resourcepack.hpp"
#include "glyph_cache.hpp"

using namespace std;
using namespace gl;
using namespace freetype;

namespace osu {
    vector<beatmap_info> loaded_beatmaps;
    vector<resourcepack> loaded_resourcepacks;
    unique_ptr<glfw::window> window;
    freetype::library main_lib;
    unique_ptr<reference_wrapper<freetype::face>> main_face;
    unique_ptr<gfx::glyph_cache> glyph_cache;

    namespace worker {
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
    for_each (
        filesystem::directory_iterator{"resourcepacks"},
        filesystem::directory_iterator{},
        [](filesystem::directory_entry entry)
        {
            osu::loaded_resourcepacks.push_back(entry.path());
        }
    );

    for_each (
        filesystem::directory_iterator{"songs"},
        filesystem::directory_iterator{},
        [](filesystem::directory_entry entry)
        {
            osu::load_beatmap(entry.path());
        }
    );

    cout << "reading font..." << "\n";
    osu::main_face = make_unique<reference_wrapper<freetype::face>> (
        osu::main_lib.face_from_istream (
            ifstream (
                "CaviarDreams.ttf",
                iostream::binary
            )
        )
        
    );
    osu::get_main_face().set_char_size(64*40, 0, 0, 0);

    cout << "Init AL" << "\n";
    alc::device dev = alc::open_device();
    cout << "al: dev" << "\n";
    alc::context al_context = dev.create_context();
    cout << "al: con" << "\n";
    alc::make_context_current(al_context);
    cout << "al: make_current" << "\n";

    osu::window = make_unique<glfw::window> (
        800,
        600,
        "osu!",
        initializer_list<glfw::window::hints::hint> {
            glfw::window::hints::opengl_debug_context{true}
        }
    );

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

    std::cout << "calculate size" << "\n";
    auto max_size = gfx::get_max_bbox_size(*osu::main_face);

    auto atlas = gfx::texture_atlas {
        {1024, 1024},
        gfx::fixed_slot_container {
            max_size,
            1024 / max_size[0],
            1024 / max_size[1]
        }
    };
    atlas.mag_filter(gl::mag_filter::nearest);
    atlas.min_filter(gl::min_filter::nearest);
    std::cout << "creating cache" << "\n";
    osu::glyph_cache = make_unique<gfx::glyph_cache> (
        osu::get_main_face(),
        std::move(atlas)
    );

    clear_color(0, 0, 0, 1);
    cout << "set clear color" << "\n";
    map_list_screen mls;

    while (!osu::window->should_close())
    {
        viewport(
            0,
            0,
            osu::window->framebuffer_size<glm::uvec2>()[0],
            osu::window->framebuffer_size<glm::uvec2>()[1]
        );
        clear(clear_buffer::color);
        mls.draw();
        osu::window->swap_buffers();
        glfw::poll_events();
    }

    return 0;
}

void on_error(int s) {
    cerr << get_stacktrace_string() << "\n";
    exit(-1);
}

int main() {
    signal(SIGABRT, on_error);
    signal(SIGSEGV, on_error);
    signal(SIGINT, on_error);
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

    int res = 0;
    try {
        res = main0();
    }
    catch(const exception& e) {
        cerr << e.what() << "\n";
        res = -1;
    }
    osu::worker::end();
    t.join();
    return res;
}