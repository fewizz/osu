#include "mp3.hpp"
#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3
#include "minimp3_ex.h"
#include <csetjmp>
#include <stdexcept>
#include <iostream>

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
/*mp3::decoder::decoder(istream* stream, size_t buffer_size):
    stream{stream},
    pointer{0},
    buffer{new uint8_t[buffer_size]},
    buffer_size{buffer_size} {
        frame_info = new mp3dec_frame_info_t;
        mp3dec = new mp3dec_t;
        mp3dec_init((mp3dec_t*)mp3dec);
    };*/

bool mp3::decoder::next(array<int16_t, 1152*2>& data) {
    //cout << "next" << "\n";
    while(true) {
    /*while(true) {
        int ffb = 0;
        int fb = 0;
        int n = mp3d_find_frame(buffer + pointer, buffer_size - pointer, &ffb, &fb);

        cout << "fb: " << fb << "\n";
        cout << "n: " << n << "\n";

        if(!fb && n) {
            pointer += n;
            continue;
        }
        if(!fb)
            return false;

        int samples = mp3dec_decode_frame(
            (mp3dec_t*)mp3dec,
            buffer + pointer,
            fb,
            data.data(),
            (mp3dec_frame_info_t*)frame_info
        );

        cout << "samples: " << samples << "\n";

        if(!samples)
            return false;
        else {
            pointer += ((mp3dec_frame_info_t*)frame_info)->frame_bytes;
            return true;
        }
    }*/
    //mp3dec_t* preserv = new mp3dec_t(*(mp3dec_t*)mp3dec);

    if(pointer == buffer_size)
        return false;
    //if(!*stream)
    //    return false;

    //if(pointer == buffer_size) {
    //    cout << "try read: " << (buffer_size) << "\n";
    //    stream->read((char*)buffer.get(), buffer_size);
    //    buffer_size = stream->gcount();
    //    cout << "new bs: " << (buffer_size) << "\n";
    //    pointer = 0;
    //}

    //cout << "decode: to_read=" << (buffer_size - pointer) << "\n";
    int samples = mp3dec_decode_frame(
        (mp3dec_t*)mp3dec,
        buffer + pointer,
        buffer_size - pointer,
        data.data(),
        (mp3dec_frame_info_t*)frame_info
    );

    //cout << "post-decode: samples="<<samples << "\n";
    int to_add = ((mp3dec_frame_info_t*)frame_info)->frame_bytes;

    //cout << "check_for_insuff" << "\n";
    if(to_add == 0 && samples == 0) {
        //cout << "insuff" << "\n";
        //stream->seekg(pointer - buffer_size, std::ios::cur);
        //mp3dec = preserv;
        //pointer = buffer_size;
        //continue;
        throw runtime_error("minimp3: insufficient data");
    }
        //throw runtime_error("minimp3: insufficient data");

    pointer += to_add;
    
    if(samples == 1152)
        return true;
    else if(samples != 0)
        throw runtime_error("is this mp3?");

    //cout << "again: pointer=" << (pointer) <<"\n";
    }
}

mp3::info mp3::decoder::get_info() {
    return {
        ((mp3dec_frame_info_t*)frame_info)->hz,
        ((mp3dec_frame_info_t*)frame_info)->channels
    };
}