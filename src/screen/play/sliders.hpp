#pragma once

#include "slider.hpp"
#include "glm/mat4x4.hpp"

namespace osu {
    namespace playfield {
        class sliders {
            std::vector<slider> sliders;
            //slider_drawer drawer;
        public:
            void add(osu::beatmap::slider& c) {
                sliders.emplace_back(c);
            }

            void draw(glm::mat4 lb) {
                //for(auto& c : sliders)
                //    drawer.draw(c, lb);
            }
        };
    }
}