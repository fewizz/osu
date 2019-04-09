#include "AL/al.h"
#include "al/internal.hpp"

inline uint al::internal::get_error() {
    return alGetError();
}

