#include "mp3.hpp"
#include <csetjmp>
#include <stdexcept>
#include <iostream>
#include <utility>
#include "unsafe_iostream_operations.hpp"
#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3
#define MINIMP3_NO_STDIO
#include "minimp3.h"
#include "minimp3_ex.h"

using namespace std;

void mp3::decoder::init() {
    mp3dec = new mp3dec_t();
    frame_info = new mp3dec_frame_info_t();
}

#define mp3dec (*((mp3dec_t*)mp3dec))
#define frame_info (*((mp3dec_frame_info_t*)frame_info))

bool mp3::decoder::next(array<sample_type, optimal_samples_count>& data) {

    bool found = estd::iterate_for_range_search_while<uint8_t>(
        [&](uint8_t* beg, size_t size){
            int rought = 
            mp3dec_decode_frame(
                &mp3dec,
                beg,
                size,
                (mp3d_sample_t*)data.data(),
                &frame_info
            );
            if(rought && frame_info.frame_bytes) {
                return true;
            }
            return false;
        },
        *stream,
        4,
        buffer.data(),
        buffer.size()
    );

    if(!found)
        return false;

    stream->seekg(-stream->gcount() + frame_info.frame_bytes, ios::cur);

    return true;
}

mp3::info mp3::decoder::get_info() {
    return {
        frame_info.hz,
        frame_info.channels
    };
}