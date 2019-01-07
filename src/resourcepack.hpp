#pragma once

#include <string>
#include <filesystem>

namespace osu {
    class resourcepack {
        std::filesystem::path path;

    public:
        resourcepack(std::filesystem::path p):path{p}{}

        template<class T>
        T get_dir();

        std::filesystem::path get_relative_path(std::string p);
    };

    template<>
    inline std::string resourcepack::get_dir<std::string>() {
        return path.string();
    }

    template<>
    inline std::filesystem::path resourcepack::get_dir<std::filesystem::path>() {
        return path;
    }

    inline std::filesystem::path resourcepack::get_relative_path(std::string p) {
        return get_dir<std::string>() + "/" + p;
    }
}