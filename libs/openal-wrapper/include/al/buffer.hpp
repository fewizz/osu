#pragma once

#include "al/with_name.hpp"
#include <cstdint>
#include "al/check_error_macro.hpp"

namespace al {

namespace internal {
    inline void gen_buffers(uint n, uint* buffers);
	inline void delete_buffers(uint n, uint* buffers);
	inline void get_buffer(uint buffer, uint pname, int* value);
	inline void buffer_data(uint buffer, uint format, void*data, uint size, uint freq);
}

class buffer : public with_name {
	inline static uint gen() {
		uint name;
		internal::gen_buffers(1, &name);
		AL_CHECK_FOR_ERROR_FN
		return name;
	}

	enum class attrib {
		frequency = 0x2001,
		bits,
		channels,
		size
	};

	int get_attrib(attrib a) {
		int i;
		internal::get_buffer(name, (uint)a, &i);
		AL_CHECK_FOR_ERROR_FN
		return i;
	}
    
	inline void data(uint format, void* data, uint size, uint frequency) {
		internal::buffer_data(name, format, data, size, frequency);
		AL_CHECK_FOR_ERROR_FN
	}
public:
    enum class format {
		mono8 = 0x1100,
		mono16,
		stereo8,
		stereo16
	};

	buffer(buffer&& r) = default;
	buffer():with_name(gen()) {}
	buffer& operator=(buffer&& b) = default;
	~buffer() {
		if(name != invalid_name) {
			internal::delete_buffers(1, &name);
			AL_CHECK_FOR_ERROR_FN
			invalidate_name();
		}
	}

	template<class RAI>
	void data(format fmt, RAI begin, uint size, uint frequency) {
		data((uint)fmt, &*begin, size, frequency);
	}

	template<class RAI>
	void data(uint8_t channels, uint8_t bits, RAI begin, uint size, uint frequency) {
		data(
			static_cast<al::buffer::format>
			(0x1100 + (channels - 1) * 2 + bits / 8 - 1),
			begin,
			size,
			frequency
		);
	}

	template<class Container>
	void data(format fmt, Container& c, uint frequency) {
		data(
			fmt,
			c.data(),
			c.size()*sizeof(typename Container::value_type),
			frequency
		);
	}

	template<class Container>
	void data(uint8_t channels, uint8_t bits, Container& c, uint frequency) {
		data(channels, bits, c.begin(), c.size()*sizeof(typename Container::value_type), frequency);
	}

	uint get_frequency() { return get_attrib(attrib::frequency); }
	uint get_bits() { return get_attrib(attrib::bits); }
	uint get_channels() { return get_attrib(attrib::channels); }
	uint get_size() { return get_attrib(attrib::size); }
};

}

#ifdef AL_INCLUDE
#include "../../src/al/buffer.cpp"
#endif