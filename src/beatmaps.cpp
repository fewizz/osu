#include "beatmaps.hpp"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "beatmap.hpp"
#include "beatmap_decoder.hpp"

using namespace std;

namespace osu {
    std::vector<beatmap_set> beatmap_sets;
}

void osu::load_beatmap_set(std::filesystem::path p) {
    using namespace filesystem;
    beatmap_set res(p);

    std::cout << "start parsing bm dirs" << "\n";
    std::for_each(
        directory_iterator{p},
        directory_iterator{},
        [&](directory_entry e) {
            if(e.path().extension() == ".osu") {
                res.diffs.emplace_back();

            ifstream s{e.path(), ios::binary};
            decode(s, res.diffs.back());
        }
    });

    beatmap_sets.push_back(move(res));

    std::cout << "end parsing bm dirs" << "\n";
}

void osu::load_beatmaps() {
    std::vector<beatmap_set> beatmap_sets;

    for_each (
        filesystem::directory_iterator{"songs"},
        filesystem::directory_iterator{},
        [](filesystem::directory_entry entry)
        {
            load_beatmap_set(entry.path());
        }
    );
}