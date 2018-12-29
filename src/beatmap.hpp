#pragma once

#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

namespace osu {
    struct osu_file_info;

    struct beatmap_info {
        std::vector<osu_file_info> diffs;

        std::filesystem::path get_dir_path();
    };

    struct osu_file_info {
        // General
        std::filesystem::path* audio;

        // Metadata
        std::string* title;
        std::string* artist;
        std::string* beatmap_set_id;

        // Events
        std::filesystem::path* back;
    };

    namespace decoder {
        osu_file_info diff(std::istream& stream);

        inline osu_file_info diff(std::istream&& stream) {
            return diff(stream);
        }

        inline osu_file_info diff(std::filesystem::path path) {
            return diff(std::ifstream(path, std::ios::binary));
        }
    }

    inline beatmap_info parse_beatmap_dir(std::filesystem::path path) {
        using namespace std::filesystem;
        beatmap_info res;

        directory_iterator it{path};
        std::for_each(it, directory_iterator{}, [&](directory_entry e) {
            if(e.path().extension() == ".osu")
                res.diffs.push_back(decoder::diff(e.path()));
        });

        return res;
    }
}