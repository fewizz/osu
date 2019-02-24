#include "mp3.hpp"
#include "minimp3_ex.h"
#include <csetjmp>
#include <stdexcept>
#include <iostream>
#include <utility>
#include "unsafe_iostream_operations.hpp"

using namespace std;

void mp3::decoder::skip_idv3() {
    int global_offset = -1;
    
    estd::iterate_for_range_search_while<uint8_t>(
        [&](uint8_t* beg, size_t size){
            int rought = mp3dec_skip_id3v2(beg, size);
            if(rought != 0) {
                global_offset = rought;
                return false;
            }
            return true;
        },
        *stream,
        10,
        buffer.data(),
        buffer.size()
    );

    if(global_offset != 0)
        stream->seekg(global_offset);
}

bool mp3::decoder::next(array<sample_type, optimal_samples_count>& data) {
    int begin = -1;
    int frame_bytes = -1;

    estd::iterate_for_range_search_while<uint8_t>(
        [&](uint8_t* beg, size_t size){
            int free_format_bytes;
            int rought = 
            mp3d_find_frame(
                beg,
                size,
                &free_format_bytes,
                &frame_bytes
            );
            if(frame_bytes != 0) {
                begin = rought;
                return false;
            }
            return true;
        },
        *stream,
        4,
        buffer.data(),
        buffer.size()
    );

    if(begin == -1)
        return false;
    
    stream->seekg(begin, ios::cur);
    
    buffer.resize(frame_bytes);
    stream->read((char*)buffer.data(), frame_bytes);

    mp3dec_decode_frame(
        (mp3dec_t*)mp3dec, 
        buffer.data(), 
        buffer.size(),
        (mp3d_sample_t*)data.data(),
        (mp3dec_frame_info_t*)frame_info
    );

    return true;
}

mp3::info mp3::decoder::get_info() {
    return {
        ((mp3dec_frame_info_t*)frame_info)->hz,
        ((mp3dec_frame_info_t*)frame_info)->channels
    };
}

void mp3::for_each_frame(istream& stream, mp3::frame_iter_func cb) {
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
}