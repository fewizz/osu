#pragma once

#include "with_width.hpp"
#include "with_height.hpp"
#include <type_traits>

namespace prop {
    template<class Vec2, class T>
    struct with_size
    :
    public virtual with_width<T>,
    public virtual with_height<T>
    {
        virtual Vec2 get_size() {
            return Vec2{get_w(), get_h()};
        }

        virtual T get_w() override {return get_size()[0];}
        virtual T get_h() override {return get_size()[1];}
    };

    template<class Vec2, class T>
    struct with_mutable_size
    :
    public with_size<Vec2, T>,
    public with_mutable_width<T>,
    public with_mutable_height<T>
    {
        virtual void set_size(Vec2 v) {
            set_w(v[0]);
            set_h(v[1]);
        }

        virtual void get_w() {
            return with_size<Vec2, T>::get_w();
        }

        virtual void get_h() {
            return with_size<Vec2, T>::get_w();
        }

        virtual void set_w(T v) override { set_size({v, get_h()});}
        virtual void set_h(T v) override { set_size({get_w(), v});}
    };

    /*template<class Vec2, class T>
    struct with_size_state
    :
    public with_mutable_size<Vec2, T>,
    public with_width_state<T>,
    public with_height_state<T>
    {
    };*/
}