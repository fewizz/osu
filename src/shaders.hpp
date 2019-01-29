#pragma once

#include <string>
#include "opengl/shader.hpp"
#include <fstream>
#include "resources.hpp"
#include "unsafe_iostream_operations.hpp"

namespace osu {
    template<class Shader>
    std::enable_if_t<std::is_base_of_v<gl::shader, Shader>,
    Shader> load(std::string shader) {
        auto path = get_resource_path(shader);

        if(!std::filesystem::exists(path))
            throw std::runtime_error("shader \"" + path.string() + "\" not found\n");
        
        std::ifstream stream(path, std::ios::binary);
        size_t size = estd::distance_to_end(stream);
        auto data_ptr = estd::get<char*>(stream, size);
        Shader sh(data_ptr.get());
        sh.compile();
        return sh;
    }
}