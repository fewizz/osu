#include "alc.hpp"
#include "alc.h"

alc::device alc::open_device(std::string device_name) {
	ALCdevice* device = alcOpenDevice(device_name.length() == 0 ? nullptr : device_name.c_str());
	if (device == nullptr) {
		throw std::exception("Error when opening AL device");
	}
	return device;
}

alc::context alc::device::create_context() {
	return alcCreateContext(static_cast<ALCdevice*>(device_ptr), nullptr);
}

void alc::make_context_current(context& c) {
	alcMakeContextCurrent(static_cast<ALCcontext*>(c.alc_context_ptr_));
}