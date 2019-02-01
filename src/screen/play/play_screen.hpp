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
    circles cs;
    sliders ss;

public:
    play_screen(const osu::beatmap& bm):bm{bm}
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

        cs.draw(mat);
        ss.draw(mat);
    }
    
    void update() override {

    }
};
}
}