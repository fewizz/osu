#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glfw/glfw.hpp"
#include <string>
#include <functional>
#include "glfw/window.hpp"

void glfw::internal::init() {
	glfwInit();
}

void glfw::window::hints::hint::set_hint() {
	glfwWindowHint(code, value);
}

glfw::window::window(int width, int height, std::string title, std::initializer_list<glfw::window::hints::hint> hints) {
	internal::init();

	for (auto hint : hints)
		hint.set_hint();

	GLFWwindow* w = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(w, this);
	window_ptr = w;
}

glfw::window::window(window&& r):window_ptr{r.window_ptr} {
	glfwSetWindowUserPointer((GLFWwindow*)window_ptr, this);
}

void glfw::window::make_context_current() {
	glfwMakeContextCurrent(static_cast<GLFWwindow*>(window_ptr));
}

bool glfw::window::should_close() {
	return glfwWindowShouldClose(static_cast<GLFWwindow*>(window_ptr));
}

void glfw::window::swap_buffers() {
	glfwSwapBuffers(static_cast<GLFWwindow*>(window_ptr));
}

void glfw::internal::framebuffer_size(void* window, int& w, int& h) {
	glfwGetFramebufferSize((GLFWwindow*)window, &w, &h);
}

void glfw::window::swap_interval(int interval) {
	glfwSwapInterval(interval);
}

void glfw::window::set_drop_callback(std::function<glfw::window::drop_callback> callback) {
	drop_callback_ = callback;
	
	glfwSetDropCallback((GLFWwindow*)window_ptr, [](GLFWwindow* win_ptr,int count,const char** paths_) {
		std::vector<std::filesystem::path> paths;
		for(int i = 0; i < count; i++) {
			paths.push_back(std::filesystem::path(paths_[i]));
		}
		((glfw::window*)glfwGetWindowUserPointer(win_ptr))->drop_callback_(paths);
	});
}

void glfw::window::set_key_callback(std::function<glfw::window::key_callback> callback) {
	key_callback_ = callback;
	glfwSetKeyCallback((GLFWwindow*)window_ptr, [](GLFWwindow* win_ptr,int a,int b,int c,int d) {
		((glfw::window*)glfwGetWindowUserPointer(win_ptr))->key_callback_(a, b, c, d);
	});
}

void glfw::poll_events() {
	glfwPollEvents();
}
