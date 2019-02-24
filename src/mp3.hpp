#pragma once

#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <memory>
#include <filesystem>
#include <functional>
#include <unsafe_iostream_operations.hpp>
#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3
#define MINIMP3_NO_STDIO
#include "minimp3.h"

namespace mp3 {
    struct info {
        int frequency;
        int channels;
    };

    class decoder {
        mp3dec_t mp3dec{};
        mp3dec_frame_info_t frame_info{};
        std::vector<uint8_t> buffer;
        std::unique_ptr< std::istream > stream;
        void skip_idv3();

    public:
        using sample_type = uint16_t;
        constexpr static uint optimal_samples_count = 1152*2;

        template<class IStream>
        decoder(IStream&& s, uint initial_buffer_size)
        : buffer(initial_buffer_size)
        {
            skip_idv3();
        }
        
        bool next(std::vector<sample_type>& data);
        bool next(std::array<sample_type, optimal_samples_count>& data);

        info get_info();
    };

    
    inline info decode(std::istream& stream, std::vector<decoder::sample_type>& out) {
        size_t size = estd::distance_to_end(stream);

        auto buffer_ptr = estd::get<uint8_t*>(stream, size);

        decoder d(buffer_ptr.get(), size);

        std::array<decoder::sample_type, decoder::optimal_samples_count> temp;

        while(d.next(temp)) {
            out.insert(out.end(), temp.begin(), temp.end());
        }
        return d.get_info();
    }

    inline info decode(std::istream&& stream, std::vector<decoder::sample_type>& out) {
        return decode(stream, out);
    }

    inline info decode(std::filesystem::path path, std::vector<decoder::sample_type>& out) {
        return decode(std::ifstream(path, std::ios::binary), out);
    }

    using frame_iter_func = std::function<bool(mp3::info, std::array<decoder::sample_type, decoder::optimal_samples_count>&)>;

    void for_each_frame(std::istream& stream, frame_iter_func);

    inline void for_each_frame(std::filesystem::path path, frame_iter_func cb) {
        std::ifstream s(path, std::ios::binary);
        for_each_frame(s, cb);
    }
}