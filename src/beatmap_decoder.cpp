#include "beatmap.hpp"
#include <algorithm>
#include <filesystem>
#include <charconv>
#include "beatmap_decoder.hpp"
#include "beatmaps.hpp"

using namespace std;
using namespace osu;

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

void osu::decode(istream& stream, beatmap& res) {
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

enum line_about {
    circle = 0b1,
    slider = 0b10,
    new_combo = 0b100,
    spinner = 0b1000,
    combo_offset = 0b1110000,
    hold = 0b10000000
};

template<class It>
std::vector<std::string_view> split(It b, It e, char c) {
    std::vector<std::string_view> res;
    
    while(true) {
        auto r = std::find(b, e, c);
        res.push_back(
            std::string_view{&*b, (size_t)(r - b)}
        );
        if(r == e)
            break;
        b = r + 1;
    }

    return res;
}

void parse_hit_objects(beatmap& res, string_view str) {
    auto s = split(str.begin(), str.end(), ',');

    glm::uvec2 pos;
    from_chars(s[0].begin(), s[0].end(), pos.x);
    from_chars(s[1].begin(), s[1].end(), pos.y);
    
    uint type = stoi(string{s[3]});

    uint combo_offset = (type & line_about::combo_offset) >> 4;
    type &= ~line_about::combo_offset;

    bool new_combo = type & line_about::new_combo;
    type &= ~line_about::new_combo;

    if(type & line_about::circle)
        res.objects.emplace_back(
            make_unique<osu::circle>(pos, combo_offset)
        );
    if(type & line_about::slider) {
        auto points_info =
            split(s[5].begin(), s[5].end(), '|');

        std::vector<glm::uvec2> points;
        points.push_back(pos);
        for_each(points_info.begin() + 1, points_info.end(),
        [&](std::string_view s){
            auto xy = ::split(s.begin(), s.end(), ':');
            glm::uvec2 p_pos;
            from_chars(xy[0].begin(), xy[0].end(), p_pos.x);
            from_chars(xy[1].begin(), xy[1].end(), p_pos.y);
            points.push_back(p_pos);
        });

        auto line_type = points_info[0];

        std::vector<glm::uvec2> positions;

        if(line_type == "L")
            positions = std::move(points);
        
        res.objects.emplace_back(
            std::make_unique<osu::slider>(std::move(positions))
        );
    }

}

/*beatmap_set parse_beatmap_dir(std::filesystem::path path) {
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
}*/

/*void osu::load_beatmap_set(filesystem::path p) {
    beatmap_sets.push_back(parse_beatmap_dir(p));
}*/
