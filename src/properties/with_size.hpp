#pragma once

#include "with_width.hpp"
#include "with_height.hpp"
#include <type_traits>

namespace prop {
    template<class Vec2, class T>
    struct with_size
    :
    public with_width<T>,
    public with_height<T>
    {
        virtual Vec2 get_size() {
            return Vec2{get_w(), get_h()};
        }

        virtual T get_w() override {return get_size()[0];}
        virtual T get_h() override {return get_size()[1];}
    };

    template<class Vec2, class T>
    struct with_size_state
    :
    public with_size<Vec2, T>,
    public with_width_state<T>,
    public with_height_state<T>
    {
        virtual Vec2 get_size() = 0;
    };
}