#pragma once

namespace prop {
    template<class T>
    struct with_width {
        virtual T get_w() = 0;
    };

    template<class T>
    struct with_mutable_width
    : public with_width<T>
    {
        virtual void set_t(T val) = 0;
    };
}