#pragma once

#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "opengl/texture.hpp"

namespace jpeg {
    gl::texture_2d decode(char* data, size_t size);

    inline gl::texture_2d decode(std::istream& s, size_t size) {
        char* data = new char[size];
        s.read(data, size);
        gl::texture_2d tex = decode(data, size);
        delete[] data;
        return tex;
    }

    inline gl::texture_2d decode(std::istream&& s, size_t size) {
        return decode(s, size);
    }

    inline gl::texture_2d decode(std::istream& s) {
        auto prev = s.tellg();
        s.seekg(0, std::ios::end);
        auto size = s.tellg() - prev;
        s.seekg(prev);
        return decode(s, size);
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
