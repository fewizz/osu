#pragma once
#include <string>

namespace al {
	namespace internal {
		unsigned get_error();
	}

	class with_name {
	protected:
		static constexpr unsigned invalid_name = std::numeric_limits<unsigned>::max();

		void invalidate_name() {
			name = invalid_name;
		}

		virtual ~with_name() = 0;
	public:
		with_name(unsigned name) :name{ name } {}
		with_name(with_name&& o) :name{ o.name } {
			o.invalidate_name();
		}

		with_name& operator=(with_name&& o) {
			name = o.name;
			o.invalidate_name();
			return *this;
		}

		unsigned name{ invalid_name };
	};
	namespace internal {
		enum source_param {
			source_relative = 0x202,
			cone_inner_angle = 0x1001,
			cone_outer_angle = 0x1002,
			looping = 0x1007,
			buffer = 0x1009,
			source_state = 0x1010
		};
	}
	class buffer : public with_name {
		static unsigned gen();
		void data(unsigned format, void* data, unsigned size, unsigned frequency);
	public:
		enum class format {
			mono8 = 0x1100,
			mono16,
			stereo8,
			stereo16
		};
		buffer():with_name(gen()) {}
		~buffer();

		template<class RAI>
		void data(format fmt, RAI begin, unsigned size, unsigned frequency) {
			data(static_cast<unsigned>(fmt), &*begin, size, frequency);
		}

		template<class RAI>
		void data(uint8_t channels, uint8_t bits, RAI begin, unsigned size, unsigned frequency) {
			data(static_cast<al::buffer::format>
				(0x1100 + (channels - 1) * 2 + bits / 8 - 1), begin, size, frequency);
		}

		template<class Container>
		void data(uint8_t channels, uint8_t bits, Container c, unsigned frequency) {
			data(channels, bits, c.begin(), c.size(), frequency);
		}
	};

	class source : public with_name {
		static unsigned gen();
		static void i(unsigned source, unsigned param, int value);
	public:
		source() :with_name{ gen() } {}
		~source();

		void buffer(al::buffer& buf) {
			i(name, internal::source_param::buffer, buf.name);
		}

		void play();
	};
}