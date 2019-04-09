#pragma once

#include <vector>
#include <array>
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
        void init();

    public:
        using sample_type = uint16_t;
        constexpr static uint optimal_samples_count = 1152*2;

        template<class IStream>
        decoder(IStream&& s, uint initial_buffer_size)
        : buffer(initial_buffer_size),
        stream{std::make_unique<IStream>(std::forward<IStream>(s))}
        {
            init();
        }
        
        bool next(std::vector<sample_type>& data);
        bool next(std::array<sample_type, optimal_samples_count>& data);

        info get_info();
    };
}