vpath %.hpp include/glfw
vpath %.cpp src

override CXXFLAGS += --std=c++17
CPPFLAGS= -I include
LIBNAME := glfw-wrapper
libfile := lib$(LIBNAME).a

$(libfile) : $(libfile)(glfw.o)
glfw.o : glfw.hpp window.hpp

clean:
	rm -f $(libfile) glfw.o
