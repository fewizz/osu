#include "al.hpp"
#include "al.h"
#include <exception>

unsigned al::internal::get_error() {
	return alGetError();
}

/* With name */
al::with_name::~with_name() {}

/* Buffer */
unsigned al::buffer::gen() {
	unsigned name;
	alGenBuffers(1, &name);
	return name;
}

void al::buffer::data(unsigned format, void* data, unsigned size, unsigned frequency) {
	alBufferData(name, format, data, size, frequency);
}
al::buffer::~buffer() {
	alDeleteBuffers(1, &name);
}

/* Source */
unsigned al::source::gen() {
	unsigned name;
	alGenSources(1, &name);
	return name;
}

void al::source::i(unsigned source, unsigned param, int value) {
	alSourcei(source, param, value);
}

void al::source::play() {
	alSourcePlay(name);
}

al::source::~source() {
	alDeleteSources(1, &name);
}