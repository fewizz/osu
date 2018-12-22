vpath %.hpp include/glfw
vpath %.cpp src

CXXFLAGS= --std=c++17 -I include

libglfw-wrapper.a : libglfw-wrapper.a(glfw.o)
glfw.o : glfw.hpp window.hpp
