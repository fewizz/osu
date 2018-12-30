#pragma once
#include <vector>
#include <array>
#include <iostream>
#include <fstream>
#include <memory>
#include <filesystem>

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

        bool next(std::array<int16_t, 1152*2>& data);

        //bool end() {
        //    return !*this;
        //}

        //operator bool() {
        //    return stream;
        //}

        info get_info();
    };

    
    inline info decode(std::istream& stream, std::vector<uint16_t>& out) {
        auto prev = stream.tellg();
        stream.seekg(0, std::ios::end);
        size_t size = stream.tellg();
        stream.seekg(prev);
        uint8_t* buffer = new uint8_t[size];
        stream.read((char*)buffer, size);
        decoder d(buffer, size);
        std::array<int16_t, 1152*2> temp;
        while(d.next(temp)) {
            out.insert(out.end(), temp.begin(), temp.end());
        }
        delete[] buffer;
        return d.get_info();
    }

    inline info decode(std::istream&& stream, std::vector<uint16_t>& out) {
        return decode(stream, out);
    }

    inline info decode(std::filesystem::path path, std::vector<uint16_t>& out) {
        return decode(std::ifstream(path, std::ios::binary), out);
    }
}