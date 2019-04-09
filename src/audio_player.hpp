#pragma once

#include "al/source.hpp"
#include "al/buffer.hpp"
#include <memory>
#include <deque>
#include "mp3.hpp"

class audio_player {
    al::source src;
    std::deque<al::buffer> buffs = std::deque<al::buffer>(15);
    std::unique_ptr<mp3::decoder> d;

public:
    inline void begin(std::unique_ptr<mp3::decoder> dec) {
        d = std::move(dec);

        std::array<uint16_t, 1152*2> data;

        for(auto& b : buffs) {

            if(d && d->next(data)) {
                b.data(2, 16, data, d->get_info().frequency);
                src.queue_buffer(b);
            }
            else return;
        }
        src.play();
    }

    inline void stop() {
        src.stop();

        while(src.get_buffers_processed()) {
            buffs.push_back(std::move(buffs.front()));
            buffs.pop_front();
            src.unqueue_buffer(buffs.back());
        }

        d.reset();
    }

    inline void update() {
        std::array<uint16_t, 1152*2> data;

        while(src.get_buffers_processed()) {
            if(d && d->next(data)) {
                buffs.push_back(std::move(buffs.front()));
                buffs.pop_front();
                al::buffer& b = buffs.back();

                src.unqueue_buffer(b);
                b.data(2, 16, data, d->get_info().frequency);
                src.queue_buffer(b);
            }
            else break;
        }
    }
};