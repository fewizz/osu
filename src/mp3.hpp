#pragma once

#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <memory>
#include <filesystem>
#include <functional>
#include <unsafe_iostream_operations.hpp>

namespace mp3 {
    struct info {
        int frequency;
        int channels;
    };

    class decoder {
        void* mp3dec{};
        void* frame_info{};
        std::vector<uint8_t> buffer;
        std::unique_ptr<std::istream > stream;
        void init_and_skip_idv3();

    public:
        using sample_type = uint16_t;
        constexpr static uint optimal_samples_count = 1152*2;

        template<class IStream>
        decoder(IStream&& s, uint initial_buffer_size)
        : buffer(initial_buffer_size),
        stream{std::make_unique<IStream>(std::forward<IStream>(s))}
        {
            init_and_skip_idv3();
        }
        
        bool next(std::vector<sample_type>& data);
        bool next(std::array<sample_type, optimal_samples_count>& data);

        info get_info();
    };

    
    /*inline info decode(std::istream&& stream, std::vector<decoder::sample_type>& out) {
        size_t size = estd::distance_to_end(stream);

        auto buffer_ptr = estd::get<uint8_t*>(stream, size);

        decoder d(buffer_ptr, size);

        std::array<decoder::sample_type, decoder::optimal_samples_count> temp;

        while(d.next(temp)) {
            out.insert(out.end(), temp.begin(), temp.end());
        }
        return d.get_info();
    }*/

    /*inline info decode(std::filesystem::path path, std::vector<decoder::sample_type>& out) {
        return decode(std::ifstream(path, std::ios::binary), out);
    }*/

    //using frame_iter_func = std::function<bool(mp3::info, std::array<decoder::sample_type, decoder::optimal_samples_count>&)>;

    //void for_each_frame(std::istream& stream, frame_iter_func);

    /*inline void for_each_frame(std::filesystem::path path, frame_iter_func cb) {
        std::ifstream s(path, std::ios::binary);
        for_each_frame(s, cb);
    }*/
}