#pragma once

#include <filesystem>
#include <string>
#include "resourcepack.hpp"

namespace osu {
    //extern std::vector<resourcepack> resourcepacks;

    void load_resourcepacks();
    std::filesystem::path get_resource_path(std::string p);
}