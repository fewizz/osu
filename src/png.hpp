#include "lodepng.h"
#include <filesystem>
#include <vector>
#include "opengl/texture.hpp"
#include <fstream>

namespace png {
    inline gl::texture_2d decode(std::filesystem::path path) {
        using namespace std;
        using namespace gl;
        size_t size = filesystem::file_size(path);
        vector<unsigned char> out;
        vector<unsigned char> in(size);

        ifstream ifs(path, ios::binary);
        ifs.read((char*)in.data(), size);

        unsigned w, h;
        int error = lodepng::decode(out, w, h, in);
        if(error) {
            throw std::runtime_error("lodepng error: " + std::to_string(error));
        }
        texture_2d tex;

        tex.image(internal_format::rgba8, w, h, pixel_format::rgba, out);
        
        tex.mag_filter(mag_filter::nearest);
        tex.min_filter(min_filter::nearest);
        return tex;
    }
}