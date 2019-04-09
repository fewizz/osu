#pragma once

#include <string>
#include "alc/context.hpp"
#include <stdexcept>
#include <sstream>

namespace alc {
namespace internal {
    inline void* open_device(const char* str);
    inline void close_device(void* device_ptr);
    inline void* create_context(void* device_ptr, int* attribs);
    inline void make_context_current(void* context);
    inline uint get_error(void* device);
}

class device;

class device {
    friend device open_device(std::string name);
    friend device open_default_device();
    void* device_ptr;

    device(void* device_ptr): device_ptr{ device_ptr } {}

    struct alc_error : std::runtime_error {
        alc_error(std::string what):std::runtime_error(what) {}
    };
    void check_for_error() {
        uint error = internal::get_error(device_ptr);
        if(error) {
            std::ostringstream o;
            o << std::hex << std::showbase << error;
            throw alc_error{o.str()};
        }
    }
#ifdef AL_CHECK_FOR_ERROR
#define ALC_CHECK_FOR_ERROR_FN check_for_errors();
#else
#define ALC_CHECK_FOR_ERROR_FN
#endif

public:
    ~device() {
        internal::close_device(device_ptr);
        ALC_CHECK_FOR_ERROR_FN
    }

    context create_context() {
        return internal::create_context(device_ptr, nullptr);
        ALC_CHECK_FOR_ERROR_FN
    }
};

inline device open_default_device() {
    return device(internal::open_device(nullptr));
    ALC_CHECK_FOR_ERROR_FN
}

inline device open_device(std::string name) {
    return device(internal::open_device(name.data()));
    ALC_CHECK_FOR_ERROR_FN
}

inline void make_context_current(context& c) {
    internal::make_context_current(c);
    ALC_CHECK_FOR_ERROR_FN
}

}

#ifdef AL_INCLUDE
#include "../../src/alc/device.cpp"
#endif