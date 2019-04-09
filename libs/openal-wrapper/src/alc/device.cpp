#include "alc/device.hpp"
#include "AL/alc.h"

inline void* alc::internal::open_device(const char* str) {
    return alcOpenDevice(str);
}

inline void alc::internal::close_device(void* device_ptr) {
    alcCloseDevice((ALCdevice*)device_ptr);
}

inline void* alc::internal::create_context(void* device_ptr, int* attribs) {
    return alcCreateContext((ALCdevice*)device_ptr, attribs);
}

inline void alc::internal::make_context_current(void* context) {
    alcMakeContextCurrent((ALCcontext*)context);
}

inline uint alc::internal::get_error(void* device) {
    return alcGetError((ALCdevice*)device);
}