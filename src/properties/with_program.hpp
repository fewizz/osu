#pragma once

#include "opengl/program.hpp"
#include "with_obj.hpp"

namespace prop {
    struct with_shared_program: public with_shared_object<gl::program> {
        using with_shared_object<gl::program>::with_shared_object;

        gl::program& program() { return ref(); }
        gl::program& prog() { return ref(); }
    };

    struct with_owned_program: public with_owned_object<gl::program> {
        using with_owned_object<gl::program>::with_owned_object;

        gl::program& program() { return ref(); }
        gl::program& prog() { return ref(); }
    };
}