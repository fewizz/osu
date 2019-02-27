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

void mp3::decoder::init_and_skip_idv3() {
    mp3dec = new mp3dec_t();
    frame_info = new mp3dec_frame_info_t();

#define mp3dec (*((mp3dec_t*)mp3dec))
#define frame_info (*((mp3dec_frame_info_t*)frame_info))

    int global_offset = -1;
    
    estd::iterate_for_range_search_while<uint8_t>(
        [&](uint8_t* beg, size_t size){
            int rought = mp3dec_skip_id3v2(beg, size);
            if(rought != 0) {
                global_offset = rought;
                return true;
            }
            return false;
        },
        *stream,
        10,
        buffer.data(),
        buffer.size()
    );

    if(global_offset != -1)
        stream->seekg(global_offset);
    else
        stream->seekg(0, ios::beg);
}

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

/*void mp3::for_each_frame(istream& stream, mp3::frame_iter_func cb) {
    size_t size = estd::distance_to_end(stream);
    auto buff = estd::get<uint8_t*>(stream, size);
    mp3dec_t dec;
    using pcm_array_t = array<uint16_t, 1152*2>;
    pcm_array_t arr;
    auto info = make_tuple(buff.get(), &dec, &arr, cb);

    MP3D_ITERATE_CB fun = [](
        void *user_data,
        const uint8_t *frame,
        int frame_size,
        size_t offset,
        mp3dec_frame_info_t *info
    ) -> int {
        auto p =
            (tuple<uint8_t*, mp3dec_t*, pcm_array_t*, mp3::frame_iter_func>*) user_data;
        uint8_t* buff = get<0>(*p);
        mp3dec_t* dec = get<1>(*p);
        pcm_array_t* pcm = get<2>(*p);
        mp3::frame_iter_func cb = get<3>(*p);
        mp3dec_decode_frame(dec, frame, frame_size, (mp3d_sample_t*)pcm->data(), info);
        return ! cb(mp3::info{ info->hz, info->channels }, *pcm);
    };
    mp3dec_iterate_buf(
        buff.get(),
        size,
        fun,
        &info
    );
}*/