#include "beatmap.hpp"
#include <algorithm>
#include <filesystem>
#include <string_view>
#include <istream>

using namespace osu;
using namespace osu::decoder;
using namespace std;
using namespace std::filesystem;

namespace osu {
    namespace decoder {
        enum event_type {
            background = 0,
            video = 1,
            brk = 2,
            colour = 3,
            sprite = 4,
            sample = 5,
            animation = 6
        };
    }
}

void parse_general(osu_file_info& res, std::string_view str);
void parse_metadata(osu_file_info& res, std::string_view str);
void parse_events(osu_file_info& res, std::string_view str);
void parse_hit_objects(osu_file_info& res, std::string_view str);

osu_file_info osu::decoder::diff(istream& stream) {
    enum type {
            none,
            general,
            metadata,
            events,
            hit_objects
    } current = none;

    char buff[0xFF];
    osu_file_info res;

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

    return res;
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

void parse_general(osu_file_info& res, string_view str) {
    auto [key, value] = kv_pair(str);
    //cout << "general: " << key << ":" << value << "\n";

    if(key == "AudioFilename")
        res.audio = new string(value);
}

void parse_metadata(osu_file_info& res, string_view str) {
    auto [key, value] = kv_pair(str);
    string* val = new string(value);

    if(key == "Title")
        res.title = val;
    if(key == "Artist")
        res.artist = val;
    if(key == "BeatmapSetID")
        res.set_id = val;
}

void parse_events(osu_file_info& res, string_view str) {
    using namespace string_literals;
    int type = str[0] - '0';
    switch(type) {
    case background:
        res.back = new string {
            str.begin() + "x,x,q"s.length(),
            str.end() - "q,x,x"s.length()
        };
        break;
    }
}

void parse_hit_objects(osu_file_info& res, string_view str) {
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