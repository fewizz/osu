#include "mp3.hpp"
#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3
#include "minimp3_ex.h"
#include <csetjmp>
#include <stdexcept>
#include <iostream>
#include <utility>

using namespace std;

mp3::decoder::decoder(uint8_t* buffer, size_t buffer_size):
    buffer{buffer},
    pointer{0},
    buffer_size{buffer_size}
{
    frame_info = new mp3dec_frame_info_t;
    mp3dec = new mp3dec_t;
    mp3dec_init((mp3dec_t*)mp3dec);
}

bool mp3::decoder::next(array<int16_t, 1152*2>& data) {
    MP3D_ITERATE_CB fun = [](
            void *user_data,
            const uint8_t *frame,
            int frame_size,
            size_t offset,
            mp3dec_frame_info_t *info) -> int
        {
            //std::cout << "found frame, decoding" << "\n";
            std::pair<array<int16_t, 1152*2>*, mp3::decoder*>* data
            = (std::pair<array<int16_t, 1152*2>*, mp3::decoder*>*) user_data;
            int bytes = mp3dec_decode_frame(
                (mp3dec_t*)data->second->mp3dec,
                frame,
                frame_size,
                data->first->data(),
                info); 
            data->second->pointer += offset + frame_size;
            cout << bytes << "\n";
            //data->second->frame_info = info;
            //memset(data->second->frame_info, sizeof(mp3dec_frame_info_t), 1)
            memcpy(data->second->frame_info, info, sizeof(mp3dec_frame_info_t));
            return 1;
        };
    
    std::pair<array<int16_t, 1152*2>*, mp3::decoder*> user_data{&data, this};
    size_t prev = pointer;
    //std::cout << "mp3::decoder::next: buffer: "
    //    << &buffer << " pointer: " << pointer
    //    << " size:" << buffer_size << "\n";
    mp3dec_iterate_buf(
        buffer + pointer,
        buffer_size - pointer,
        fun,
        &user_data
    );
    return prev != pointer;
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
            mp3dec_frame_info_t *info) -> int
        {
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
        &info);
}