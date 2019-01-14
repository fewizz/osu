#pragma once

namespace prop {
    template<class T>
    class with_height {
        virtual T get_h() = 0;
    };

    template<class T>
    class with_height_state
    : public with_height<T>
    {
        virtual void set_t(T val) = 0;
    };
}