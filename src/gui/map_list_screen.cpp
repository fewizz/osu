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
        diffs_drawers.push_back({outline_drawer, bm, diff_d_program});
    }

    choose(0);

    osu::window->set_key_callback([&](int key, int scancode, int action, int mods) {
        if(action == GLFW_PRESS)
            return;
        if (current_map > 0 && key == GLFW_KEY_UP)
            choose(current_map-1);
        if (current_map < osu::loaded_beatmaps.size() - 1 && key == GLFW_KEY_DOWN)
            choose(current_map+1);
        if (current_diff > 0 && key == GLFW_KEY_LEFT)
            current_diff--;
        if (current_diff < osu::loaded_beatmaps[current_map].diffs.size() - 1
            &&
            key == GLFW_KEY_RIGHT
            )
            current_diff++;
    });
}

void map_list_screen::choose(unsigned mp) {
    cout << "choosing " << mp << "\n";
    current_map = mp;
    current_diff = 0;

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

    mat = translate(mat, {0, -100, 0});
    for (int i = 0; i < diffs_drawers.size(); i++)
    {
        mat = diffs_drawers[i].draw(mat, i == current_map ? current_diff : -1);
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

void map_list_screen::outline_drawer::draw(mat4 top_left, vec4 color, vec2 dim) {
    prog()->uniform<float, 2> (
        prog()->u_loc("u_dim"),
        dim
    );

    prog()->uniform<float, 4>(prog()->u_loc("u_color"), color);
    prog()->uniform<float, 4, 4>(prog()->u_loc("u_mat"), top_left);

    gfx::line_loop_drawer<0, 4>::draw();
}

map_list_screen::beatmap_diffs_drawer::beatmap_diffs_drawer(
    decltype(outline_drawer)& od,
    osu::beatmap_info info,
    std::shared_ptr<gl::program> p
)
:
od{od},
info{info},
map_name {
    info.artist() + " - " + info.title(),
    *osu::glyph_cache,
    p,
    gfx::text_drawer::origin::baseline_start
} {

    for(auto d : info.diffs) {
        diffs.push_back({
            *d.version,
            *osu::glyph_cache,
            p,
            gfx::text_drawer::origin::baseline_start
        });
    }
}

mat4 map_list_screen::beatmap_diffs_drawer::draw(mat4 top_left, int selected_diff) {
    const float H = 80;

    map_name.prog()->uniform<float, 4, 4>(map_name.prog()->u_loc("u_mat"), top_left);
    map_name.draw();
    od.draw(translate(top_left, {0, -10, 0}), vec4{1}, {10000, H});

    top_left = translate(top_left, {0, -H, 0});

    if(selected_diff == -1)
        return top_left;

    top_left = translate(top_left, {100, 0, 0});

    int i = 0;
    for(auto& d : diffs) {
        d.prog()->uniform<float, 4, 4>(d.prog()->u_loc("u_mat"), top_left);
        d.draw();
        od.draw(
            translate(top_left, {0, -10, 0}),
            i == selected_diff ? vec4{1, 0, 0, 1} : vec4{0, 0, 1, 1},
            vec2{600, H}
        );
        top_left = translate(top_left, {0, -H - 10, 0});
        i++;
    }
    top_left = translate(top_left, {-100, 0, 0});
    return translate(top_left, {0, -20, 0});
}