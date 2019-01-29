#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace prop {
    template<int dim, class T>
    struct size;

    template<class T>
    struct size<2, T> : public glm::vec<2, T>{
        using glm::vec<2, T>::vec;
        inline T width() { return glm::vec<2, T>::x; }
        inline T w() { return glm::vec<2, T>::x; }
        inline T height() { return glm::vec<2, T>::y; }
        inline T h() { return glm::vec<2, T>::y; }
    };

    using size2 = size<2, float>;
    using usize2 = size<2, uint>;
    using isize2 = size<2, int>;
}