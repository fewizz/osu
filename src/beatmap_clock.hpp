#pragma once

#include <cstdint>
#include <chrono>

namespace osu {
    class beatmap_clock {
        std::chrono::system_clock::time_point begin;

    public:
        using rep = uint64_t;
        using period = std::milli;
        using duration = std::chrono::duration<rep, period>;
        using time_point = std::chrono::time_point<beatmap_clock>;
        static constexpr bool is_steady = false;

        time_point now() {
            std::chrono::system_clock::now()
            -
            begin;
        }
    };
}