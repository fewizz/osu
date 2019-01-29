#pragma once

#include <string>
#include "png.hpp"
#include "jpeg.hpp"
#include "filesystem"
#include "resources.hpp"

namespace osu {
    
    template<class Tex>
    std::enable_if_t<std::is_base_of_v<gl::texture, Tex>,
    Tex> load(std::string path);
    
    template<>
    inline gl::texture_2d load<gl::texture_2d>(std::string path) {
        auto ext = path.substr(path.find_last_of('.') + 1);

        if(ext == "png")
            return 
                png::decode(osu::get_resource_path(path));

        if(ext == "jpg")
            return
                jpeg::decode(osu::get_resource_path(path));
        throw std::runtime_error("unknown texture file extension");
    }

}