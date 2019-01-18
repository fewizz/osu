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
        void* mp3dec;
        void* frame_info;
        //std::istream* stream;
        //std::unique_ptr<uint8_t[]> buffer;
        uint8_t* buffer;
        size_t buffer_size;
        size_t pointer;
    public:
        decoder(uint8_t* buffer, size_t buffer_size);
        decoder(std::ifstream&& s):decoder(s){}
        decoder(std::istream& s)
        :decoder(nullptr, 0) {
            buffer_size = estd::distance_to_end(s);
            buffer = estd::get<uint8_t*>(s, buffer_size).release();
        }

        bool next(std::array<int16_t, 1152*2>& data);

        info get_info();
    };

    
    inline info decode(std::istream& stream, std::vector<uint16_t>& out) {
        size_t size = estd::distance_to_end(stream);

        auto buffer_ptr = estd::get<uint8_t*>(stream, size);

        decoder d(buffer_ptr.get(), size);

        std::array<int16_t, 1152*2> temp;

        while(d.next(temp)) {
            out.insert(out.end(), temp.begin(), temp.end());
        }
        return d.get_info();
    }

    inline info decode(std::istream&& stream, std::vector<uint16_t>& out) {
        return decode(stream, out);
    }

    inline info decode(std::filesystem::path path, std::vector<uint16_t>& out) {
        return decode(std::ifstream(path, std::ios::binary), out);
    }

    using frame_iter_func = std::function<bool(mp3::info, std::array<uint16_t, 1152*2>&)>;

    void for_each_frame(std::istream& stream, frame_iter_func);

    inline void for_each_frame(std::filesystem::path path, frame_iter_func cb) {
        std::ifstream s(path, std::ios::binary);
        for_each_frame(s, cb);
    }
}