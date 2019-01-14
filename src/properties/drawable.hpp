#pragma once

namespace prop {
    template<class... Args>
    struct drawable {
        virtual void draw(Args...) = 0;
    };
}