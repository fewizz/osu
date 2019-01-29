#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include "glm/vec2.hpp"
#include "hit_object.hpp"

namespace osu {
    struct beatmap_set;
    struct beatmap;

    //struct beatmap_set_loc {
    //    beatmap_set* ptr;
    //};
    beatmap_set parse_beatmap_dir(std::filesystem::path path);

    class beatmap {
    public:
        const beatmap_set& get_set() const;

        // General
        std::string audio;

        // Metadata
        std::string title;
        std::string artist;
        std::string version;
        std::string set_id;

        // Events
        std::string back;

        // Hit Objects
        std::vector<std::unique_ptr<osu::pf_object>> objects;
    };

    struct beatmap_set {
        //std::shared_ptr<beatmap_set_loc> loc;
        std::vector<beatmap> diffs;
        std::filesystem::path dir;

        beatmap_set(std::filesystem::path dir)
        :dir{dir}{}
        //loc{std::make_shared<beatmap_set_loc>(this)}{}

        //beatmap_set(const beatmap_set&) = delete;
        //beatmap_set(beatmap_set&& r) = delete;

        inline const std::string title() {
            return diffs[0].title;
        }

        inline const std::string artist() {
            return diffs[0].artist;
        }

        inline const std::string set_id() const {
            return diffs[0].set_id;
        }

        template<class T>
        T get_dir() const {
            return dir;
        }
    };
}