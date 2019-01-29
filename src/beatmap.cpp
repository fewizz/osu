#include "beatmap.hpp"
#include "beatmaps.hpp"
#include <iostream>

using namespace osu;
using namespace std;
using namespace std::filesystem;

const beatmap_set& osu::beatmap::get_set() const {
    cout << "st " << "\n";
    for(auto& set : osu::beatmap_sets) {
        cout << "diffs: " << set.diffs.size() << "\n";
        cout << set.set_id() << "\n";
        if(set.set_id() == set_id)
            return set;
    }
    throw std::runtime_error("unknown beatmap set");
}