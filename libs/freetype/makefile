vpath %.hpp include/freetype
vpath %.cpp src
LIBNAME = libfreetype-wrapper
SLN = $(LIBNAME).a

CXXFLAGS = \
-Iinclude \
-I/usr/include/freetype2 \
--std=c++17

$(SLN): $(SLN)(error.o face.o glyph.o library.o)

face.o: face.hpp
glyph.o: glyph.hpp
library.o: library.hpp
error.o: error.cpp