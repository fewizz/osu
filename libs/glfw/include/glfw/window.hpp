#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <functional>

namespace glfw {

	class window {

		//class GLFWwindow;
		//typedef void (* GLFWdropfun)(GLFWwindow*,int,const char**);
		typedef void(drop_callback)(std::vector<std::filesystem::path>);
		typedef void(key_callback) (int key, int scancode, int action, int mods);
	public:
		std::function<drop_callback> drop_callback_;
		std::function<key_callback> key_callback_;

		class hints {
			hints() {} // You can't create it, it's like fake namespace

		public:
			class hint {
			protected:
				int code;
				int value;
			public:
				hint(int code, int value) :code{ code }, value{ value }{}
				void set_hint();
			};

			class bool_hint : public hint {
			public:
				bool_hint(unsigned code, bool value) :hint(code, value) {}
			};
			class opengl_debug_context : public bool_hint {
			public:
				opengl_debug_context(bool value) :bool_hint(0x00022007, value) {}
			};
		};

		void* window_ptr;
		window(int width, int height, std::string title, std::initializer_list<window::hints::hint> hints);
		window(void* window_ptr) :window_ptr{ window_ptr } {}
		window(window&& r);

		void make_context_current();
		bool should_close();
		void swap_buffers();
		std::pair<int, int> get_framebuffer_size();
		void swap_interval(int interval);
		void set_drop_callback(std::function<drop_callback>);
		void set_key_callback(std::function<key_callback>);
	};
}