vpath %.cpp src
vpath %.hpp src

CXXFLAGS+= \
-Isrc \
-Ilibs/opengl/include \
-Ilibs/utfcpp/sourc \
-Ilibs/openal \
-Ilibs/glfw/include \
-Ilibs/freetype \
-Ilibs/minimp3 \
-Ilibs/glm \
--std=c++17
LDFLAGS+=-Llibs/opengl -Llibs/glfw
LDLIBS+= -lopenal -lglfw -lGL -lopengl-wrapper -lglfw-wrapper -lzip -lGLEW -lstdc++fs
.PHONY: opengl-wrapper glfw-wrapper run

objects = main.o import_beatmap.o

osu: $(objects) opengl-wrapper glfw-wrapper
	$(CXX) $(CXXFLAGS) -o $@ $(objects) $(LDFLAGS) $(LDLIBS)

opengl-wrapper:
	make -C libs/opengl
glfw-wrapper:
	make -C libs/glfw

#.PHONY: ziplib
#ziplib:
#	make -C libs/ziplib
#	cat libs/ziplib/Bin/libzip.so > libzip.so

run:
	LD_LIBRARY_PATH=$(CURDIR) ./osu