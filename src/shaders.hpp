#pragma once

#include <string>
#include "opengl/shader.hpp"
#include <fstream>
#include "main.hpp"
#include "unsafe_iostream_operations.hpp"

namespace osu {

    template<class Shader>
    Shader load(std::string shader) {
        auto path = get_resource_path(shader);
        auto ext = path.extension();
        //std::unique_ptr<gl::shader> sh;

        //if(ext == ".vs")
        //    sh = std::make_unique<gl::vertex_shader>();
        //else if(ext == ".fs")
        //    sh = std::make_unique<gl::fragment_shader()>;
        //else 
        //    throw std::runtime_error("unknown shader file extension: " + shader);
        
        std::ifstream stream(path, std::ios::binary);
        size_t size = estd::distance_to_end(stream);
        auto data_ptr = estd::get<char*>(stream, size);
        Shader sh(data_ptr.get());
        sh.compile();
        return sh;
    }
}