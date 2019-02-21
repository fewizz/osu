#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include <utility>
#include "glm/vec2.hpp"
#include <variant>
#include <chrono>

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

    // Difficulty
    double ar;
    double cs;
    double slider_multiplier = 1;
    double slider_tick_rate = 1;

    // Events
    std::string back;

    // Timing Points
    struct timing_point {
        std::chrono::milliseconds start;
        std::chrono::milliseconds beat_len;

        /*timing_point(
            std::chrono::milliseconds start,
            std::chrono::milliseconds beat_len
        ) : start{start}, beat_len{beat_len}{}*/
    };

    struct diff_point {
        std::chrono::milliseconds start;
        double speed_mult;
    };

    std::vector<timing_point> timing_points;
    std::vector<diff_point> diff_points;
    
    timing_point tp_at(std::chrono::milliseconds time) {
        for(auto& tp : timing_points)
            if(tp.start > time)
                return tp;

        throw std::runtime_error("can't find timing_point");
    }

    diff_point dp_at(std::chrono::milliseconds time) {
        for(auto& dp : diff_points)
            if(dp.start > time)
                return dp;

        throw std::runtime_error("can't find diff_point");
    }

    // Hit Objects

    struct circle {
        const glm::uvec2 pos;
        const std::chrono::milliseconds start_time;
    };

    struct slider {
        const std::chrono::milliseconds start_time;
        enum class type_t {
            linear,
            bezier,
            catmull,
            arc
        } type;
        uint repeats;

        std::vector<glm::uvec2> control_points;

        double scoring_dist(beatmap& bm) {
            return
            100.0
            * bm.slider_multiplier
            * bm.dp_at(start_time).speed_mult;
        }

        double velocity(beatmap& bm) {
            return scoring_dist(bm)
            / (double)bm.tp_at(start_time).beat_len.count();
        }

        double tick_dist(beatmap& bm) {
            return scoring_dist(bm)
            / bm.slider_tick_rate; //TODO support v8
        }
    };

    std::vector<
        std::variant<circle, slider>
    > objects;
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