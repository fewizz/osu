#include "al.hpp"
#include "AL/al.h"
#include <exception>
#include <vector>

std::vector<al::with_name*> objects;

unsigned al::internal::get_error() {
	return alGetError();
}

/* With name */
al::with_name::with_name(unsigned name):name{ name }
{
	objects.reserve(name + 1);
	objects[name] = this;
}
al::with_name::with_name(with_name&& o) :name{ o.name } {
	o.invalidate_name();
	objects[name] = this;
}
al::with_name::~with_name() {
	if(name != invalid_name)
		objects[name] = nullptr;
	invalidate_name();
}
al::with_name& al::with_name::operator=(with_name&& o) {
	if(name != invalid_name)this->~with_name();
	name = o.name;
	o.invalidate_name();
	objects[name] = this;
	return *this;
}

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
	if(name != invalid_name)
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

void al::source::get_i(unsigned source, unsigned param, int* value) {
	alGetSourcei(source, param, value);
}

void al::source::play() {
	alSourcePlay(name);
}

void al::source::stop() {
	alSourceStop(name);
}

void al::source::queue_buffer(al::buffer& buff) {
	alSourceQueueBuffers(name, 1, &(buff.name));
}

/*std::vector<al::buffer*> al::source::unqueue_buffers() {
	int u = get_buffers_processed();
	uint* names = new uint[u];
	alSourceUnqueueBuffers(name, u, names);
	std::vector<al::buffer*> buffs(0);
	for(int i = 0; i < u; i++) {
		buffs.push_back((al::buffer*)objects[names[i]]);
	}
	return buffs;
}*/
void al::source::unqueue_buffer(al::buffer& b) {
	alSourceUnqueueBuffers(name, 1, &b.name);
}

al::source::~source() {
	if(name != invalid_name)
		alDeleteSources(1, &name);
}
