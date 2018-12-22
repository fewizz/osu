vpath %.cpp src
vpath %.hpp src

CXXFLAGS+= \
-Isrc \
-Ilibs/opengl/include \
-Ilibs/utfcpp/source \
-Ilibs/openal \
-Ilibs/glfw/include \
-Ilibs/freetype/include \
-Ilibs/minimp3 \
-Ilibs/glm \
--std=c++17
LDFLAGS+= \
-Llibs/opengl \
-Llibs/glfw \
-Llibs/freetype
LDLIBS+= \
-lstdc++fs \
-lopenal \
-lglfw \
-lglfw-wrapper \
-lGL \
-lopengl-wrapper \
-lfreetype \
-lfreetype-wrapper \
-lzip \
-lGLEW

.PHONY: opengl-wrapper glfw-wrapper freetype-wraper run

objects = main.o import_beatmap.o

osu: $(objects) opengl-wrapper glfw-wrapper freetype-wrapper
	$(CXX) $(CXXFLAGS) -o $@ $(objects) $(LDFLAGS) $(LDLIBS)
main.o : main.hpp gui.hpp map_list_screen.hpp

opengl-wrapper:
	make -C libs/opengl
glfw-wrapper:
	make -C libs/glfw
freetype-wrapper:
	make -C libs/freetype
run:
	LD_LIBRARY_PATH=$(CURDIR) ./osu