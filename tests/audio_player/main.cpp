#include "audio_player.hpp"
#include <fstream>
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>
#include "openal/al.hpp"
#include "openal/alc.hpp"

using namespace std;

int main() {
    auto device = alc::open_device();
    auto context = device.create_context();
    alc::make_context_current(context);

    auto decoder {
        make_unique<mp3::decoder>(
            mp3::decoder(ifstream("audio.mp3", ios::binary), 0x1000)
        )
    };

    audio_player player{};
    player.begin(move(decoder));
    for(int i = 0; i < 10000; i++) {
        player.update();
        cout << "al: " << al::internal::get_error() << "\n";
        this_thread::sleep_for(1ms);
    }

    //using namespace std::chrono_literals;
    //this_thread::sleep_for(10s);

    player.stop();
}