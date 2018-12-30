vpath %.hpp include/openal
vpath %.cpp src

CXXFLAGS = --std=c++17
CPPFLAGS = -I include/openal

srcs := al.cpp alc.cpp
objects := $(srcs:.cpp=.o)

libopenal-wrapper.a : libopenal-wrapper.a($(objects))

al.o : al.hpp
alc.o : alc.hpp