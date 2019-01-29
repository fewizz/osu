#include "zip.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <memory>
#include "beatmaps.hpp"
#include "import_beatmap.hpp"

using namespace std;

void osu::import_beatmap(filesystem::path path) {
    filesystem::create_directory("songs");

    cout << "Importing " << path << "\n";

    filesystem::path song_path(path);

    string song_name = string(path);
    song_name = string(
        song_name.begin() + song_name.find_last_of('/') + 1,
        song_name.begin() + song_name.find_last_of('.')
    );
    cout << "song name: " << song_name << "\n";

    ifstream file{song_path, fstream::binary};
    size_t size = filesystem::file_size(song_path);

    cout << "reading. size: " << size << "\n";

    char* ar_data = new char[size];
    file.read(ar_data, size);

    zip_error_t* err = nullptr;
    auto zip_source = zip_source_buffer_create(ar_data, size, 0, err);
    if(err != nullptr)
        cout << "zip_src: " << err->str << "\n";
    err = nullptr;
    auto zip = zip_open_from_source(zip_source, ZIP_RDONLY, err);
    if(err != nullptr)
        cout << "zip_open: " <<err->str << "\n";

    int entries = zip_get_num_entries(zip, 0);
    cout << "entries: " << entries << "\n";

    char* buff0 = new char[0xFFFF];

    filesystem::path song_dir("songs/" + song_name);
    filesystem::create_directory(song_dir);

    for(int e = 0; e < entries; e++) {
        auto zip_file = zip_fopen_index(zip, e, 0);
        string out_dir = 
            song_dir.string() + "/" + zip_get_name(zip, e, 0);

        cout << "out: " << out_dir << "\n";

        ofstream out {
            out_dir,
            fstream::binary
        };
        int read;
        while((read = zip_fread(zip_file, buff0, 0xFFFF)) > 0) {
            out.write(buff0, read);
            cout << "write: " << read << "\n";
        }
    }

    osu::load_beatmap_set(song_dir);
    delete[] ar_data;
    delete[] buff0;
}