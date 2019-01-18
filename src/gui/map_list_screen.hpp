#pragma once

#include "gui.hpp"
#include "opengl/core.hpp"
#include <vector>
#include <filesystem>
#include <memory>
#include "draw/text_drawer.hpp"
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
#include "properties/with_program.hpp"
#include "draw/background_drawer.hpp"
#include <deque>
#include <utility>
#include "mp3.hpp"
#include "mp3_player.hpp"

class map_list_screen : public gui::screen<>
{
    class group;
    class beatmap_info;
    using group_cursor = std::vector<group>::iterator;
    using beatmap_cursor = std::vector<beatmap_info>::iterator;

    mp3_player player;

    gfx::background_drawer back_drawer {
        gl::program {
            osu::load<gl::vertex_shader>("shaders/rectangle_u_mat4_u_dim_uv.vs"),
            osu::load<gl::fragment_shader>("shaders/passtrough_u_tex2_a_uv.fs")
        }
    };

    struct rectangle_drawer_t :
    public prop::with_owned_program {
        rectangle_drawer_t()
        :
        prop::with_owned_program (
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
        rectangle_drawer_t& rec;
        float w;
    public:
        slot(
            std::string text,
            std::shared_ptr<gl::program> prog,
            rectangle_drawer_t& rec)
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

    class beatmap_info: public slot {
    public:
        const osu::beatmap& bm;
        beatmap_info(
            std::string text,
            std::shared_ptr<gl::program> prog,
            rectangle_drawer_t& rec,
            osu::beatmap& bm)
        :slot(text, prog, rec), bm{bm}{}
    };

    struct group : public prop::with_height<float> {
        std::string name;
        map_list_screen::slot group_slot;
        std::vector<beatmap_info> beatmaps;

        group(
            std::string name,
            std::shared_ptr<gl::program> prog,
            rectangle_drawer_t& od
        );

		float get_h() override {
            return
            group_slot.get_h()
            +
            group_slot.is_pressed()*beatmaps.size()*beatmaps[0].get_h();
        }

        void draw(glm::mat4 top_left);
    };

    std::vector<group> groups;
    group_cursor current_group = {};
    beatmap_cursor current_beatmap = {};
    float prevOffset = 0;

  public:
    map_list_screen();
    void choose(group_cursor mp, beatmap_cursor diff);
    void draw() override;
    void update() override;
};