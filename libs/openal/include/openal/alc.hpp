#pragma once
#include <string>

namespace alc {
	class context;
	class device;

	device open_device(std::string device_name = "");
	void make_context_current(context& c);

	class device {
		friend device open_device(std::string);
		void* device_ptr;
		device(void* device_ptr) :device_ptr{ device_ptr } {}
	public:
		context create_context();
	};

	class context {
		friend context device::create_context();
		friend void make_context_current(context&);
		void* alc_context_ptr_;
		context(void* alc_context_ptr_) :alc_context_ptr_{ alc_context_ptr_ } {}
	};
}