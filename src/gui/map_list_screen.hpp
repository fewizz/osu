#pragma once

#include "gui.hpp"
#include "opengl/core.hpp"
#include <vector>
#include <filesystem>
#include <memory>
#include "text_drawer.hpp"
#include "opengl/shader.hpp"
#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "main.hpp"
#include "openal/al.hpp"
#include "shaders.hpp"
#include "glyph_cache.hpp"
#include "beatmap.hpp"
#include "properties/with_height.hpp"
#include "properties/with_size.hpp"
#include "properties/pressable.hpp"
#include "properties/drawable.hpp"

class map_list_screen : public gui::view<>
{
    struct background_drawer: public gfx::triangle_fan_drawer<0, 4> {
        std::shared_ptr<gl::texture_2d> current_tex;

        background_drawer()
        :
        gfx::triangle_fan_drawer<0, 4>(
            gl::program {
                osu::load<gl::vertex_shader>("shaders/rectangle_u_mat4_u_dim_uv.vs"),
                osu::load<gl::fragment_shader>("shaders/passtrough_u_tex2_a_uv.fs")
            }
        ){}

        void draw(glm::mat4 center);
    } background_drawer;

    struct rec_drawer_t : public gfx::vertex_array_drawer<> {
        rec_drawer_t()
        :
        gfx::vertex_array_drawer<> (
            gl::program {
                osu::load<gl::vertex_shader>("shaders/rectangle_u_mat4_u_dim.vs"),
                osu::load<gl::fragment_shader>("shaders/passtrough_u_color4.fs")
            }
        ){}

        void draw(
            glm::mat4 top_left,
            glm::vec2 dim,
            gl::primitive_type pt,
            glm::vec4 color
        );
    } rec_drawer;

    class slot: public gui::view<
        prop::with_size<glm::vec2, float>,
        prop::with_pressable_state<>
    >{
        gfx::text_drawer text;
        rec_drawer_t& rec;
        float w;
    public:
        slot(
            std::string text,
            std::shared_ptr<gl::program> prog,
            rec_drawer_t& rec)
            :
        text{text, *osu::glyph_cache, prog, gfx::text_drawer::origin::baseline_start},
        w{this->text.get_width() + 10},
        rec{rec}
        {}

        glm::vec2 get_size() override { return {w, 60}; }

        void draw(
            glm::mat4 top_left,
            glm::vec4 outline,
            glm::vec4 back,
            glm::vec4 text);
    };

    class bm_diff_drawer : public slot {
    public:
        using slot::slot;
    };

    struct bm_main_drawer : public prop::with_height<float> {
        osu::beatmap_info info;
        std::vector<bm_diff_drawer> diffs;
        slot main;

        bm_main_drawer(
            std::shared_ptr<gl::program> prog,
            rec_drawer_t& od,
            osu::beatmap_info bi
        );

		float get_h() override {
            return 
            main.get_h()
            +main.is_pressed()*diffs.size()*diffs[0].get_h();
        }

        void draw(glm::mat4 top_left);
    };

    std::vector<bm_main_drawer> diffs_drawers;

    unsigned current_map = 1;
    unsigned current_diff = 1;
    float prevOffset = 0;
    al::source src;
    al::buffer buf;

  public:
    map_list_screen();
    void choose(unsigned map, unsigned diff);
    void draw();
};