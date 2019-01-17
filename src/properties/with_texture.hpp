#pragma once

#include "opengl/texture.hpp"
#include "with_obj.hpp"

namespace prop {
    struct with_shared_texture_2d: public with_shared_object<gl::texture_2d> {
        using with_shared_object<gl::texture_2d>::with_shared_object;

        gl::texture_2d& texture() { return ref(); }
        gl::texture_2d& tex() { return ref(); }
    };

    struct with_unique_texture_2d: public with_unique_object<gl::texture_2d> {
        using with_unique_object<gl::texture_2d>::with_unique_object;

        gl::texture_2d& texture() { return ref(); }
        gl::texture_2d& tex() { return ref(); }
    };

    struct with_owned_texture_2d: public with_owned_object<gl::texture_2d> {
        using with_owned_object<gl::texture_2d>::with_owned_object;
        gl::texture_2d& texture() { return ref(); }
        gl::texture_2d& tex() { return ref(); }
    };
}