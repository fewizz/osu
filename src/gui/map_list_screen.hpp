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

class map_list_screen : public gui::screen<>
{
    class group;
    class beatmap_info;
    using group_cursor = std::vector<group>::iterator;
    using beatmap_cursor = std::vector<beatmap_info>::iterator;

    class mp3_player {
        al::source src;
        std::deque<al::buffer> buffs = std::deque<al::buffer>(10);
        std::unique_ptr<mp3::decoder> d;
    public:
        //player(std::unique_ptr<mp3::decoder> d):d{d}{}

        inline void begin(std::unique_ptr<mp3::decoder>&& dec) {
            std::cout << "player: beginning" << "\n";
            d = std::move(dec);
            std::array<int16_t, 1152*2> data;

            for(auto& b : buffs) {
                d->next(data);
                b.data(2, 16, data, d->get_info().frequency);
                src.queue_buffer(b);    
            }

            src.play();
        }

        inline void stop() {
            src.stop();
            for(auto& b : buffs) {
                src.unqueue_buffer(b);   
            }
            d.reset();
        }

        inline void update() {
            /*if(src.get_buffers_processed() == 0)
                return;
            array<int16_t, 1152*2> data;
            d.next(data);
            buffers[0].data(2, 16, data, d.get_info().frequency);
            src.queue_buffer(buffers[0]);
            al::buffer t0 = std::move(buffers[0]);
            al::buffer t1 = std::move(buffers[1]);
            buffers[1] = std::move(t0);
            buffers[0] = std::move(t1);*/
            //std::cout << "update:al: " << al::internal::get_error() << "\n";
            for(int i = 0; i < src.get_buffers_processed(); i++) {
                //std::cout << "b:al: " << al::internal::get_error() << "\n";
                std::array<int16_t, 1152*2> data;
                //std::cout << "next" << "\n";
                if(d->next(data)) {
                    //std::cout << "moving" << "\n";
                    al::buffer b = std::move(buffs.front());
                    //std::cout << "poping" << "\n";
                    buffs.pop_front();
                    src.unqueue_buffer(b);
                    //std::cout << "before_data:al: " << al::internal::get_error() << "\n";
                    //std::cout << "data freq: " <<d->get_info().frequency << "\n";
                    b.data(2, 16, data, d->get_info().frequency);
                    //std::cout << "queue" << "\n";
                    src.queue_buffer(b);
                    buffs.push_back(std::move(b));
                    //std::cout << "queue:al: " << al::internal::get_error() << "\n";
                }
            }

            //if(src.get_state() != al::source::state::playing)
            //    src.play();
        }
    } player;

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
    std::deque<al::buffer> unused_buffers = std::deque<al::buffer>(10);
    std::deque<al::buffer> used_buffers;
    //std::unique_ptr<std::ifstream> sample_stream;
    std::unique_ptr<mp3::decoder> sample_decoder;

  public:
    map_list_screen();
    void choose(group_cursor mp, beatmap_cursor diff);
    void draw() override;
    void update() override;
};