#include "map_list_screen.hpp"

using namespace std;
using namespace glm;
using namespace gl;

map_list_screen::map_list_screen() {
    auto diff_d_program = make_shared<program> (
        osu::load<vertex_shader>("shaders/passtrough_mat4_pos2_tex2.vs"),
        osu::load<fragment_shader>("shaders/passtrough_tex2.fs")
    );
    
    for(auto bm : osu::loaded_beatmaps) {
        diffs_drawers.push_back({diff_d_program, outline_drawer, bm});
    }

    choose(0, 0);

    osu::window->set_key_callback([&](int key, int scancode, int action, int mods) {
        if(action == GLFW_PRESS)
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

        background_drawer.current_tex = std::make_shared<gl::texture_2d>(jpeg::decode(
            std::filesystem::path
            {osu::loaded_beatmaps[mp].get_dir<std::string>()
            + "/"
            + *osu::loaded_beatmaps[mp].diffs[0].back
            }));
        
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

    for (auto& dd : diffs_drawers)
    {
        dd.draw(mat);
        mat = translate(mat, {0, -dd.get_h(), 0});
    }
}

void map_list_screen::background_drawer::draw(mat4 center) {
    //std::cout << "DRAW\n";
    vec2 fb_size = osu::window->framebuffer_size<uvec2>();
    vec2 tex_size = {current_tex->width(), current_tex->height()};

    //std::cout << "fb: " << tex_size[0] << " " << tex_size[1] << "\n";

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

void map_list_screen::outline_drawer_t::draw(mat4 bot_left, vec4 color, vec2 dim) {
    prog()->uniform<float, 2> (
        prog()->u_loc("u_dim"),
        dim
    );

    prog()->uniform<float, 4>(prog()->u_loc("u_color"), color);
    prog()->uniform<float, 4, 4>(prog()->u_loc("u_mat"), bot_left);

    gfx::line_loop_drawer<0, 4>::draw();
}

void map_list_screen::slot::draw(glm::mat4 top_left) {
    mat4 mat = translate(top_left, vec3{0, -get_size()[1], 0});
    text.prog()->uniform<float, 4, 4>(
        text.prog()->u_loc("u_mat"),
        translate(
            mat,
            {5, (get_h() - (osu::main_face->get_size_metrics().height() / 64.0)) / 2.0, 0}
        )
    );
    text.draw();
    od.draw(mat, outline_color, get_size());
}

map_list_screen::bm_main_drawer::bm_main_drawer(
    shared_ptr<gl::program> prog,
    outline_drawer_t& od,
    osu::beatmap_info bi
):
main{bi.artist() + " - " + bi.title(), prog, od, {0, 0, 1, 1}},
info{bi} {
    for(auto d : info.diffs) {
        diffs.emplace_back(
            *d.version,
            prog,
            od,
            vec4{1, 0, 0, 1}
        );
    }
}

void map_list_screen::bm_main_drawer::draw(mat4 top_left) {
    main.draw(top_left);
    if(!main.is_pressed())
        return;
    mat4 mat = translate(top_left, {100, -main.get_h(), 0});
    for(auto& d : diffs) {
        d.draw(mat);
        mat = translate(mat, {0, -d.get_h(), 0});
    }
}