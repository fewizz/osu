#include "jpeg.hpp"

#include <cstdio>
#include <iostream>
using namespace std;

#include "jpeglib.h"

gl::texture_2d jpeg::decode(char* data, size_t size) {
    cout << "reading " << size << " " << (void*)&data << "\n";
    FILE* f = fmemopen(data, size, "rb");
    jpeg_decompress_struct cinfo;

    jpeg_error_mgr err_mgr;
    err_mgr.error_exit = [](j_common_ptr cinfo) {
        abort();
    };
    err_mgr.output_message = [](j_common_ptr cinfo) {
        throw runtime_error("jpeg error");
    };

    cinfo.err = jpeg_std_error(&err_mgr);

    cout << "create_dec" << "\n";
    jpeg_create_decompress(&cinfo);
    cout << "stdio_src" << "\n";
    jpeg_stdio_src(&cinfo, f);
    cout << "read_header" << "\n";
    jpeg_read_header(&cinfo, true);
    cinfo.out_color_space = JCS_EXT_RGBA;
    cout << "start_decompress" << "\n";
    jpeg_start_decompress(&cinfo);
    std::vector<unsigned char> v(
        cinfo.output_width
        * cinfo.output_height
        * cinfo.output_components
    );
    //JSAMPARRAY buffer = cinfo.mem->alloc_sarray(cinfo, )
    cout << "read_sls" << "\n";
    vector<unsigned char*> samparray(1);
    //for(int i = 0; i < cinfo.output_height; i++) {
    //    samparray.push_back(&v[0] + i*cinfo.output_width*cinfo.output_components);
    //}
    while(cinfo.output_scanline < cinfo.output_height) {
        samparray[0] = &v[0] + cinfo.output_scanline*cinfo.output_width*cinfo.output_components;
        jpeg_read_scanlines(&cinfo, samparray.data(), 1);
    }
    //cinfo.output_scanline = cinfo.output_height - 1;
    cout << "finish_d" << "\n";
    jpeg_finish_decompress(&cinfo);
    cout << "destroy_d" << "\n";
    jpeg_destroy_decompress(&cinfo);
    cout << "close f" << "\n";
    fclose(f);
    gl::texture_2d tex;
    tex.image(gl::internal_format::rgba8, cinfo.image_width, cinfo.image_height, gl::pixel_format::rgba, v);
    tex.mag_filter(gl::mag_filter::nearest);
    tex.min_filter(gl::min_filter::nearest);
    return tex;
}