#include "map_list_screen.hpp"
#include "png.hpp"
#include "mp3.hpp"
#include "GLFW/glfw3.h"
#include "jpeg.hpp"

using namespace std;
using namespace glm;
using namespace gl;

map_list_screen::map_list_screen() {
    auto diff_d_program = make_shared<program> (
        osu::load<vertex_shader>("shaders/passtrough_u_mat4_a_pos2_a_uv.vs"),
        osu::load<fragment_shader>("shaders/passtrough_u_tex2_u_color4_a_uv.fs")
    );
    
    for(auto bm : osu::loaded_beatmaps) {
        diffs_drawers.push_back({diff_d_program, rec_drawer, bm});
    }

    choose(0, 0);

    osu::window->set_key_callback([&](int key, int scancode, int action, int mods) {
        if(action == GLFW_REPEAT || action == GLFW_RELEASE)
            return;
        if (current_map > 0 && key == GLFW_KEY_UP)
            choose(current_map-1, 0);
        if (current_map < osu::loaded_beatmaps.size() - 1 && key == GLFW_KEY_DOWN)
            choose(current_map+1, 0);
        if (current_diff > 0 && key == GLFW_KEY_LEFT)
            choose(current_map, current_diff-1);
        if (current_diff < osu::loaded_beatmaps[current_map].diffs.size() - 1
            &&
            key == GLFW_KEY_RIGHT
            )
            choose(current_map, current_diff+1);
    });
}

void map_list_screen::choose(unsigned mp, unsigned diff) {
    if(mp != current_map) {
        vector<uint16_t> data;
        mp3::info info = mp3::decode(
            osu::loaded_beatmaps[mp].get_dir<string>()
            + "/"
            + *osu::loaded_beatmaps[mp].diffs[0].audio,
            data
        );
        std::cout << data.size() << " " <<info.channels << " " << info.frequency << "\n"; 
        src.stop();
        src.buffer(nullptr);
        buf.data(info.channels, 16, data, info.frequency);
        src.buffer(buf);
        src.play();
        std::cout << "al: " << al::internal::get_error() << "\n";

        filesystem::path back
            {osu::loaded_beatmaps[mp].get_dir<std::string>()
            + "/"
            + *osu::loaded_beatmaps[mp].diffs[0].back
            };

        background_drawer.current_tex =
            std::make_shared<gl::texture_2d>(
                png::is_png(back) ?
                png::decode(back)
                :
                jpeg::decode(back)
                );
        
        diffs_drawers[current_map].main.set_pressed(false);
        diffs_drawers[mp].main.set_pressed(true);
    }

    diffs_drawers[current_map].diffs[current_diff].set_pressed(false);
    diffs_drawers[mp].diffs[diff].set_pressed(true);

    current_map = mp;
    current_diff = diff;
}

void map_list_screen::draw() {
    vec2 fb_size = osu::window->framebuffer_size<vec2>();
    mat mat = ortho<float> (
        -fb_size[0] / 2,
        fb_size[0] / 2,
        -fb_size[1] / 2,
        fb_size[1] / 2
    );
    background_drawer.draw(mat);

    mat = ortho<float> (
        0,
        fb_size[0],
        -fb_size[1],
        0
    );

    float offY = 0;
    for(int i = 0; i < diffs_drawers.size(); i++) {
        if(current_map == i) {
            offY += diffs_drawers[i].main.get_h();
            for(int x = 0; x <= current_diff && x < diffs_drawers[i].diffs.size(); x++)
                offY += diffs_drawers[i].diffs[x].get_h() / (x == current_diff ? 2.0 : 1.0);
            break;
        }
        offY += diffs_drawers[i].get_h();
    }
    
    offY -= prevOffset;
    prevOffset=prevOffset + offY/4.0;
    if(prevOffset > osu::window->framebuffer_size<vec2>()[1] / 2.0)
        mat = translate(mat, {0, prevOffset - osu::window->framebuffer_size<vec2>()[1] / 2.0, 0});
    for (auto& dd : diffs_drawers)
    {
        dd.draw(mat);
        mat = translate(mat, {0, -dd.get_h(), 0});
    }
}

void map_list_screen::background_drawer::draw(mat4 center) {
    vec2 fb_size = osu::window->framebuffer_size<uvec2>();
    vec2 tex_size = {current_tex->width(), current_tex->height()};

    float w_ratio = fb_size[0] / fb_size[1];
    float t_ratio = tex_size[0] / tex_size[1];

    vec2 scaled_size =
        current_tex->size<vec2>()
        *
        (w_ratio > t_ratio ?
            fb_size[0] / tex_size[0]
            :
            fb_size[1] / tex_size[1]
        );
    
    prog()->uniform<float, 2>(prog()->u_loc("u_dim"), scaled_size);

    prog()->uniform<float, 4, 4>(
        prog()->u_loc("u_mat"),
        translate(center, {-scaled_size[0] / 2.0f, -scaled_size[1] / 2.0f, 0})
    );

    active_texture(*current_tex, 0);
    prog()->uniform<int, 1>(prog()->u_loc("u_tex"), 0);

    gfx::triangle_fan_drawer<0, 4>::draw();
}

void map_list_screen::rec_drawer_t::draw(
    mat4 bot_left, 
    vec2 dim,
    gl::primitive_type pt, 
    vec4 color) {
    prog()->uniform<float, 2> (
        prog()->u_loc("u_dim"),
        dim
    );

    prog()->uniform<float, 4, 4>(prog()->u_loc("u_mat"), bot_left);

    prog()->uniform<float, 4>(prog()->u_loc("u_color"), color);
    gfx::vertex_array_drawer<>::draw(pt, 0, 4);
}

void map_list_screen::slot::draw(mat4 top_left, vec4 out, vec4 back, vec4 text_color) {
    mat4 mat = translate(top_left, vec3{0, -get_size()[1], 0});
    rec.draw(mat, get_size(), gl::primitive_type::line_loop, out);
    rec.draw(mat, get_size(), gl::primitive_type::triangle_fan, back);
    text.prog()->uniform<float, 4, 4>(
        text.prog()->u_loc("u_mat"),
        translate(
            mat,
            {5, (get_h() - (osu::main_face->get_size_metrics().height() / 64.0)) / 2.0, 0}
        )
    );
    text.prog()->uniform<float, 4>(
        text.prog()->u_loc("u_color"),
        text_color
    );
    text.draw();
}

map_list_screen::bm_main_drawer::bm_main_drawer(
    shared_ptr<gl::program> prog,
    rec_drawer_t& rec,
    osu::beatmap_info bi
):
main{bi.artist() + " - " + bi.title(), prog, rec},
info{bi} {
    for(auto d : info.diffs) {
        diffs.emplace_back(
            *d.version,
            prog,
            rec
        );
    }
}

void map_list_screen::bm_main_drawer::draw(mat4 top_left) {
    main.draw(top_left, vec4{0, 0, 0, 1}, vec4{0, 0, 0, 1}, vec4{1, 1, 1, 1});
    if(!main.is_pressed())
        return;
    mat4 mat = translate(top_left, {10, -main.get_h(), 0});
    for(auto& d : diffs) {
        d.draw(
            mat,
            d.is_pressed() ? vec4{1, 0, 0, 1} : vec4{1, 1, 1, 1},
            vec4{1, 1, 1, 1},
            vec4{0, 0, 0, 1});
        mat = translate(mat, {0, -d.get_h(), 0});
    }
}