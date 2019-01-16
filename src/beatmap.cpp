#include "beatmap.hpp"
#include <algorithm>
#include <filesystem>
#include <string_view>
#include <iostream>
#include "main.hpp"

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

enum event_type {
    background = 0,
    video = 1,
    brk = 2,
    colour = 3,
    sprite = 4,
    sample = 5,
    animation = 6
};


void parse_general(beatmap& res, std::string_view str);
void parse_metadata(beatmap& res, std::string_view str);
void parse_events(beatmap& res, std::string_view str);
void parse_hit_objects(beatmap& res, std::string_view str);

void diff(istream& stream, beatmap& res) {
    enum type {
            none,
            general,
            metadata,
            events,
            hit_objects
    } current = none;

    char buff[0xFF];

    while(stream) {
        stream.getline(buff, 0xFF);
        string_view str(buff);
        if(str.back() == '\r')
            str = str.substr(0, str.length() - 1);
        // cout << str << "\n";
        // cout << str.length() << "\n";


        if(str.length() == 0) { // Delimiter only
            current = none;
            continue;
        }
        if(str.length() >= 2 && str.substr(0, 2) == "//")
            continue;
        
        if(current == general)
            parse_general(res, str);
        if(current == events)
            parse_events(res, str);
        if(current == metadata)
            parse_metadata(res, str);
        if(current == hit_objects)
            parse_hit_objects(res, str);

        if(str == "[General]")
            current = general;
        if(str == "[Events]")
            current = events;
        if(str == "[Metadata]")
            current = metadata;
        if(str == "[HitObjects]")
            current = hit_objects;
    }
}

pair<string_view, string_view>
kv_pair(string_view str) {
    auto colon = str.begin() + str.find(':');
    auto start = find_if(colon + 1, str.end(),
        [](char c) {return c != ' ';}
    );
    return pair{
        str.substr(0, colon - str.begin()),
        str.substr(start - str.begin(), str.end() - start)
    };
}

void parse_general(beatmap& res, string_view str) {
    auto [key, value] = kv_pair(str);
    //cout << "general: " << key << ":" << value << "\n";

    if(key == "AudioFilename")
        res.audio = value;
}

void parse_metadata(beatmap& res, string_view str) {
    auto [key, value] = kv_pair(str);

    if(key == "Title")
        res.title = value;
    if(key == "Artist")
        res.artist = value;
    if(key == "Version")
        res.version = value;
    if(key == "BeatmapSetID")
        res.set_id = value;
}

void parse_events(beatmap& res, string_view str) {
    using namespace string_literals;
    int type = str[0] - '0';
    switch(type) {
    case background:
        res.back = string {
            str.begin() + str.find('"') + 1,
            str.begin() + str.find_last_of('"')
        };
        break;
    }
}

void parse_hit_objects(beatmap& res, string_view str) {
    vector<string_view> split;

    auto left = 0;

    while(true) {
        auto right = str.find_first_of(',', left);
        if(right == string::npos)
            break;
        split.push_back(str.substr(left, right - left));
        left = right + 1;
    }

    //cout << "x: " << split[0] << " y: " << split[1] << "\n";
}

beatmap_set osu::parse_beatmap_dir(std::filesystem::path path) {
    using namespace std::filesystem;
    beatmap_set res(path);

    std::cout << "start parsing bm dirs" << "\n";

    directory_iterator it{path};
    std::for_each(it, directory_iterator{}, [&](directory_entry e) {
        if(e.path().extension() == ".osu") {
            res.diffs.emplace_back();

            ifstream s{e.path(), ios::binary};
            diff(s, res.diffs.back());
        }
    });

    std::cout << "end parsing bm dirs" << "\n";

    return res;
}