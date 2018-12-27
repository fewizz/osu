#pragma once

#include <filesystem>
#include <string>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <string_view>

namespace osu {
    struct beatmap_info {
    };

    struct osu_file_info {
        // General
        std::filesystem::path* audio;

        // Events
        std::filesystem::path* back;

        // Meta
        std::string* title;
    };

    namespace decoder {

        enum type {
            none,
            general,
            events
        } current;

        enum event_type {
            background = 0,
            video = 1,
            brk = 2,
            colour = 3,
            sprite = 4,
            sample = 5,
            animation = 6
        };

        osu_file_info decode_diff(std::istream& stream) {
            
            char buff[0xFF];
            osu_file_info res;

            while(stream) {
                stream.getline(buff, 0xFF);
                std::string_view str{buff};
                std::cout<< str << "\n";
    
                if(str.length() == 0) { // Delimiter only
                    current = none;
                    continue;
                }
                if(str.length() >= 2 && str.substr(2, str.length() - 2) == "//")
                    continue;
                if(str == "[General]")
                    current = general;
                if(str == "[Events]")
                
                if(current == general) {
                    auto colon = str.begin() + str.find(':');
                    auto start = std::find_if(colon, str.end(),
                        [](char c) {return c != ' ';}
                    );

                    if(str == "AudioFilename")
                        res.audio = new std::filesystem::path {
                            start,
                            str.end()
                        };
                }
                if(current == events) {
                    int type = std::stoi({str.begin(), str.find('.')});
                    switch(type) {
                    case background:
                        res.back = new std::filesystem::path {
                            str.begin() + "x,x,q"s.length(),
                            str.end() - "q,x,x"s.length()
                        };
                    }
                }
            }

            return res;
        }



        osu_file_info decode_diff(std::istream&& stream) {
            decode_diff(stream);
        }

    }
}