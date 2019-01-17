#pragma once

#include "with_obj.hpp"
#include "opengl/vertex_array.hpp"

namespace prop {
    struct with_owned_vertex_array: public with_owned_object<gl::vertex_array> {
        using with_owned_object<gl::vertex_array>::with_owned_object;
        gl::vertex_array& vertex_array() { return ref(); }
    };

}