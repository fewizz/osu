#pragma once

#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "opengl/texture.hpp"
#include "unsafe_iostream_operations.hpp"

namespace jpeg {
    gl::texture_2d decode(char* data, size_t size);

    inline gl::texture_2d decode(std::istream& s, size_t size) {
        auto data_ptr = estd::get<char*>(s, size);
        gl::texture_2d tex = decode(data_ptr.get(), size);
        return tex;
    }

    inline gl::texture_2d decode(std::istream&& s, size_t size) {
        return decode(s, size);
    }

    inline gl::texture_2d decode(std::istream& s) {
        return decode(s, estd::distance_to_end(s));
    }

    inline gl::texture_2d decode(std::istream&& s) {
        return decode(s);
    }

    inline gl::texture_2d decode(std::filesystem::path p) {
        return decode(
            std::ifstream(p, std::ios::binary),
            std::filesystem::file_size(p)
        );
    }
}
