#include "map_list_screen.hpp"
#include "png.hpp"
#include "mp3.hpp"
#include "GLFW/glfw3.h"
#include "jpeg.hpp"
#include "beatmaps.hpp"
#include "textures.hpp"
#include "shaders.hpp"
#include <iterator>
#include <fstream>
#include "play_screen.hpp"

using namespace std;
using namespace glm;
using namespace gl;

map_list_screen::map_list_screen()
{
    std::cout << "map_list_constructor" << "\n";
    auto diff_d_program = make_shared<program> (
        osu::load<vertex_shader>("shaders/passtrough_u_mat4_a_pos2_a_uv.vs"),
        osu::load<fragment_shader>("shaders/passtrough_u_tex2_u_color4_a_uv.fs")
    );
    
    for(auto& bm : osu::beatmap_sets) {
        std::cout << "loading " << bm.title() << "\n";
        group g{bm.artist() + " - " + bm.title(), diff_d_program, rec_drawer};

        for(auto& d : bm.diffs) {
            g.beatmaps.emplace_back(
                d.version,
                diff_d_program,
                rec_drawer,
                d
            );
        }

        groups.push_back(std::move(g));
    }

    std::cout << "choosing..." << "\n";
    choose(groups.begin(), groups.begin()->beatmaps.begin());
    std::cout << "choosing end" << "\n";

    osu::window->set_key_callback([&](int key, int scancode, int action, int mods) {
        if(action == GLFW_REPEAT || action == GLFW_RELEASE)
            return;
        if (current_group != groups.begin() && key == GLFW_KEY_UP)
            choose(current_group-1, (current_group-1)->beatmaps.begin());
        if (current_group != groups.end()-1 && key == GLFW_KEY_DOWN)
            choose(current_group+1, (current_group+1)->beatmaps.begin());
        if (current_beatmap != current_group->beatmaps.begin() && key == GLFW_KEY_LEFT)
            choose(current_group, current_beatmap-1);
        if (current_beatmap != current_group->beatmaps.end()-1 && key == GLFW_KEY_RIGHT)
            choose(current_group, current_beatmap+1);
        
        if(key == GLFW_KEY_ENTER)                                                                           
            osu::current_screen = make_unique<play_screen>(current_beatmap->bm);
    });
}

void map_list_screen::choose(group_cursor g, beatmap_cursor bmi) {
    if(g != current_group) {
        std::cout << "loading sound..." << "\n";
        std::cout << "bms: " << osu::beatmap_sets.size() << "\n";
        auto path = bmi->bm.get_set().get_dir<string>()
                + "/"
                + bmi->bm.audio;
        
        osu::worker::add_task([&, path=path]
        {
            player.stop();
            player.begin(make_unique<mp3::decoder>(std::ifstream(path, ios::binary)));
        });

        std::cout << "loading back..." << "\n";
        filesystem::path back
            {bmi->bm.get_set().get_dir<std::string>()
            + "/"
            + bmi->bm.back
            };
        std::cout << "back_loc: " << back.string() << "\n";

        back_drawer.unique() =
            std::make_unique<gl::texture_2d>(
                png::is_png(back) ?
                png::decode(back)
                :
                jpeg::decode(back)
            );
    }
    std::cout << "updating cursors" << "\n";
    if(current_group != group_cursor{})
        current_group->group_slot.set_pressed(false);
    g->group_slot.set_pressed(true);
    if(current_beatmap != beatmap_cursor{})
        current_beatmap->set_pressed(false);
    bmi->set_pressed(true);

    current_group = g;
    current_beatmap = bmi;
}

void map_list_screen::draw() {
    vec2 fb_size = osu::window->framebuffer_size<vec2>();
    mat mat = ortho<float> (
        -fb_size[0] / 2,
        fb_size[0] / 2,
        -fb_size[1] / 2,
        fb_size[1] / 2
    );
    back_drawer.draw(mat);

    mat = ortho<float> (
        0,
        fb_size[0],
        -fb_size[1],
        0
    );

    float offY = 0;
    for(auto& g : groups) {
        if(&*current_group == &g) {
            offY += g.group_slot.get_h();
            for(auto& diff : g.beatmaps) {
                if(&diff == &*current_beatmap) {
                    offY += diff.get_h() / 2.0;
                    break;
                }
                offY += diff.get_h();
            }
            break;
        }
        offY += g.get_h();
    }
    
    offY -= prevOffset;
    prevOffset=prevOffset + offY/4.0;
    if(prevOffset > osu::window->framebuffer_size<vec2>()[1] / 2.0)
        mat = translate(mat, {0, prevOffset - osu::window->framebuffer_size<vec2>()[1] / 2.0, 0});
    for (auto& g : groups)
    {
        g.draw(mat);
        mat = translate(mat, {0, -g.get_h(), 0});
    }
}

void map_list_screen::rectangle_drawer_t::draw(
    mat4 bot_left,
    vec2 dim,
    gl::primitive_type pt, 
    vec4 color) {
    uniform("u_dim", dim);
    uniform("u_mat", bot_left);
    uniform("u_color", color);
    draw_arrays(pt, 0, 4);
}

void map_list_screen::slot::draw(mat4 top_left, vec4 out, vec4 back, vec4 text_color) {
    mat4 mat = translate(top_left, vec3{0, -get_size()[1], 0});
    rec.draw(mat, get_size(), gl::primitive_type::line_loop, out);
    rec.draw(mat, get_size(), gl::primitive_type::triangle_fan, back);
    text.prog().uniform(
        "u_mat",
        translate(
            mat,
            {5, (get_h() - (osu::main_face->get_size_metrics().height() / 64.0)) / 2.0, 0}
        )
    );
    text.prog().uniform(
        "u_color",
        text_color
    );
    text.draw();
}

map_list_screen::group::group(
    string name,
    shared_ptr<gl::program> prog,
    rectangle_drawer_t& rec
):
name{name},
group_slot{name, prog, rec}{}

void map_list_screen::group::draw(mat4 top_left) {
    group_slot.draw(top_left, vec4{0, 0, 0, 1}, vec4{0, 0, 0, 1}, vec4{1});
    if(!group_slot.is_pressed())
        return;
    mat4 mat = translate(top_left, {10, -group_slot.get_h(), 0});
    for(auto& d : beatmaps) {
        d.draw(
            mat,
            d.is_pressed() ? vec4{1, 0, 0, 1} : vec4{1},
            vec4{1},
            vec4{0, 0, 0, 1});
        mat = translate(mat, {0, -d.get_h(), 0});
    }
}

void map_list_screen::update() {
    osu::worker::add_task([&] {
        player.update();
    });
}