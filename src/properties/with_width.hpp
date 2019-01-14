#pragma once

namespace prop {
    template<class T>
    class with_width {
        virtual T get_w() = 0;
    };

    template<class T>
    class with_width_state
    : public with_height<T>
    {
        virtual void set_t(T val) = 0;
    };
}