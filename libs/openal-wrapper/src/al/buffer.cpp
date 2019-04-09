#include "al/buffer.hpp"
#include "AL/al.h"

inline void al::internal::gen_buffers(uint n, uint* buffers) {
    alGenBuffers(n, buffers);
}

inline void al::internal::delete_buffers(uint n, uint* buffers) {
    alDeleteBuffers(n, buffers);
}

inline void al::internal::get_buffer(uint buffer, uint pname, int* value) {
    alGetBufferi(buffer, pname, value);
}

inline void al::internal::buffer_data(uint buffer, uint format, void*data, uint size, uint freq) {
    alBufferData(buffer, format, data, size, freq);
}