#pragma once

namespace prop {
    template<class T>
    struct with_height {
        virtual T get_h() = 0;
    };

    template<class T>
    struct with_mutable_height
    : public with_height<T>
    {
        virtual void set_t(T val) = 0;
    };
}