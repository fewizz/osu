vpath %.cpp src
vpath %.hpp src

CXXFLAGS+= \
-Isrc \
-Ilibs/opengl/include \
-Ilibs/utfcpp/sourc \
-Ilibs/openal \
-Ilibs/glfw \
-Ilibs/freetype \
-Ilibs/minimp3
--std=c++17
LDFLAGS+=-Llibs/opengl
LDLIBS+= -lAL -lglfw -lGL -lopengl-wrapper -lzip -lGLEW -lstdc++fs

objects = main.o import_beatmap.o

osu: $(objects) opengl-wrapper
	$(CXX) $(CXXFLAGS) -o $@ $(objects) $(LDLIBS)

.PHONY: opengl-wrapper
opengl:
	make -C libs/opengl

#.PHONY: ziplib
#ziplib:
#	make -C libs/ziplib
#	cat libs/ziplib/Bin/libzip.so > libzip.so

.PHONY: run
run:
	LD_LIBRARY_PATH=$(CURDIR) ./osu