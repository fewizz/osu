vpath %.hpp include/openal
vpath %.cpp src

override CXXFLAGS += --std=c++17
CPPFLAGS = -I include/openal
LIBNAME := openal-wrapper
libfile := lib$(LIBNAME).a

srcs := al.cpp alc.cpp
objects := $(srcs:.cpp=.o)

$(libfile) : $(libfile)($(objects))

al.o : al.hpp
alc.o : alc.hpp

clean:
	rm -f $(objects) $(libfile)

