#pragma once
#include "glm/vec2.hpp"

namespace osu {
    class field_object {
    public:
        virtual void update(int time, int prev) = 0;
    };

    class circle : public field_object {
        glm::ivec2 position;
    public:

        void update(int time, int prev) override {
            
        }
    };
}