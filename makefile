vpath %.cpp src libs/lodepng
vpath %.hpp src
vpath %.h libs/lodepng

CPPFLAGS+= \
-Isrc \
-Ilibs/opengl/include \
-Ilibs/utfcpp/source \
-Ilibs/openal \
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
-llodepng \
-lGLEW

.PHONY: opengl-wrapper glfw-wrapper freetype-wraper run

objects = main.o import_beatmap.o beatmap.o
lib-targets = opengl-wrapper glfw-wrapper freetype-wrapper liblodepng.a

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
glfw-wrapper:
	make -C libs/glfw
freetype-wrapper:
	make -C libs/freetype
liblodepng.a: liblodepng.a(lodepng.o)
liblodepng.a(lodepng.o): lodepng.o
lodepng.o: lodepng.h
run:
	LD_LIBRARY_PATH=$(CURDIR) ./osu