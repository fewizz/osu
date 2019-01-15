#include "png.hpp"
#include "unsafe_iostream_operations.hpp"

using namespace std;
using namespace gl;

gl::texture_2d png::decode(filesystem::path path) {
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

bool png::is_png(filesystem::path path) {
    ifstream str(path, ios::binary);
    auto in = estd::get<uint8_t*>(str, 8);

    return !(in[0] != 137 || in[1] != 80 || in[2] != 78 || in[3] != 71
     || in[4] != 13 || in[5] != 10 || in[6] != 26 || in[7] != 10);
}