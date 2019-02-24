#pragma once

#include "gui.hpp"
#include "size.hpp"
#include "beatmap.hpp"
#include "opengl/program.hpp"
#include "png.hpp"
#include "shaders.hpp"
#include "glm/mat4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "textures.hpp"
#include "opengl/texture.hpp"
#include "size.hpp"
#include "sliders.hpp"
#include "circles.hpp"
#include <memory>
#include <variant>
#include <cmath>
#include "mp3_player.hpp"

template<class...Ts>
struct hand : public Ts... {
    using Ts::operator()...;
};
template<class... Ts>
hand(Ts...) -> hand<Ts...>;

namespace osu {
namespace playfield {

class play_screen : public gui::screen<> {
    const osu::beatmap& bm;
    float scale;
    circles cs;
    sliders ss;

    double diff_range(double diff, double min, double mid, double max) {
        return
        min + (mid - min)/5*std::min(5.0, diff)
        +
        (max - mid)/5*std::max(0.0, diff-5);
    }
public:
    std::chrono::system_clock::time_point start;
    std::chrono::milliseconds preempt;
    std::chrono::milliseconds fade_in;

    play_screen(const osu::beatmap& bm)
    :bm{bm},
    preempt{(uint64_t)diff_range(bm.ar, 1800, 1200, 450)},
    fade_in{(uint64_t)diff_range(bm.ar, 1200, 800, 300)},
    scale{(1.0f - 0.7f * (float(bm.cs) - 5.0f) / 5.0f) / 2.0f},
    ss{scale},
    cs{scale},
    start{std::chrono::system_clock::now()}
    {
        for(auto& o : bm.objects) {
            std::visit( hand {
            [&](const osu::beatmap::circle& c){
                cs.add(c);
            },
            [&](const osu::beatmap::slider& s){
                ss.add(s);
            }
            }
            ,
            o
            );
        }
    }

    void draw() override {
        auto ws = osu::window->framebuffer_size<prop::size2>();
        auto mat = glm::ortho<float>(
            -ws.w() / 2, ws.w() / 2,
            -ws.h() / 2, ws.h() / 2
        );

        mat = glm::translate(
            mat,
            {-256, -192, 0}
        );

        auto dur =
            std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now() - start);
        cs.draw(mat, dur, preempt, fade_in);
        ss.draw(mat, dur);
    }
    
    void update() override {

    }
};
}
}