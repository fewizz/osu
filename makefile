vpath %.cpp src libs/lodepng
vpath %.hpp src
vpath %.h libs/lodepng

CPPFLAGS+= \
-Isrc \
-Ilibs/opengl/include \
-Ilibs/utfcpp/source \
-Ilibs/openal/include \
-Ilibs/glfw/include \
-Ilibs/freetype/include \
-Ilibs/minimp3 \
-Ilibs/glm \
-Ilibs/stacktrace/include \
-Ilibs/lodepng
CXXFLAGS = --std=c++17
LDFLAGS += \
-L. \
-Llibs/opengl \
-Llibs/openal \
-Llibs/glfw \
-Llibs/freetype
LDLIBS+= \
-lstdc++fs \
-lopenal \
-lglfw \
-lglfw-wrapper \
-lOpenGL \
-lopengl-wrapper \
-lopenal-wrapper \
-lfreetype \
-lfreetype-wrapper \
-lzip \
-llodepng \
-lGLEW \
-ljpeg

objects := main.o import_beatmap.o beatmap.o png.o jpeg.o mp3.o
lib-targets := \
opengl-wrapper \
openal-wrapper \
glfw-wrapper \
freetype-wrapper \
liblodepng.a

.PHONY: $(lib-targets) run

osu: $(objects) $(lib-targets)
	$(CXX) $(CXXFLAGS) -o $@ $(objects) $(LDFLAGS) $(LDLIBS)
%.o: $(lib-targets)

%.d : %.cpp
	@set -e; rm -f $@; \
	$(CXX) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
include $(objects:.o=.d)

opengl-wrapper:
	make -C libs/opengl
openal-wrapper:
	make -C libs/openal
glfw-wrapper:
	make -C libs/glfw
freetype-wrapper:
	make -C libs/freetype
liblodepng.a: liblodepng.a(lodepng.o)

run:
	LD_LIBRARY_PATH=$(CURDIR) ./osu