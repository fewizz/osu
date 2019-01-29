#include "resources.hpp"
#include "resourcepack.hpp"
#include <vector>
#include <algorithm>

using namespace std;

namespace osu {
    vector<resourcepack> resourcepacks;
}

void osu::load_resourcepacks() {
    for_each (
        filesystem::directory_iterator{"resourcepacks"},
        filesystem::directory_iterator{},
        [](filesystem::directory_entry entry)
        {
            resourcepacks.push_back(entry.path());
        }
    );
}

filesystem::path osu::get_resource_path(string p) {
    for(auto r : resourcepacks) {
        auto path = r.get_relative_path(p);
        if(std::filesystem::exists(path))
            return path;
    }
    throw std::runtime_error("resource \"" + p + "\" not found");
}
