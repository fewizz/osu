#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

namespace prop {
    template<int dim, class T>
    struct size : public glm::vec<dim, T> {};

    template<class T>
    struct size<2, T> : protected glm::vec<2, T> {
        using glm::vec<2, T>::vec;
        inline T width() { return this[0]; }
        inline T w() { return this[0]; }
        inline T height() { return this[0]; }
        inline T h() { return this[0]; }
    };

    using size2 = size<2, float>;
    using usize2 = size<2, uint>;
    using isize2 = size<2, int>;
}