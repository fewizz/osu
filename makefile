source-containing-dirs = $(sort $(dir $(wildcard src/*/)))

$(info $(source-containing-dirs))

vpath %.cpp $(source-containing-dirs) libs/lodepng
vpath %.hpp $(source-containing-dirs)
vpath %.h libs/lodepng

objects := $(subst .cpp,.o,\
$(foreach dir,$(source-containing-dirs),$(notdir $(wildcard $(dir)*.cpp ))))
deps := $(objects:.o=.d)

override CPPFLAGS += \
-Isrc \
-Ilibs/opengl/include \
-Ilibs/utfcpp/source \
-Ilibs/openal/include \
-Ilibs/glfw/include \
-Ilibs/freetype/include \
-Ilibs/minimp3 \
-Ilibs/glm \
-Ilibs/stacktrace/include \
-Ilibs/lodepng \
-I/usr/include/freetype2/
override CXXFLAGS += --std=c++17
override LDFLAGS += \
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
-ljpeg \
-lpthread

lib-targets := \
opengl-wrapper \
openal-wrapper \
glfw-wrapper \
freetype-wrapper \
liblodepng.a

.PHONY: $(lib-targets) clean osu

osu: $(objects) $(lib-targets)
	$(CXX) $(CXXFLAGS) -o $@ $(objects) $(LDFLAGS) $(LDLIBS)

%.hpp:
	
%.d : %.cpp
	@set -e; rm -f $@; \
	$(CXX) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
include $(deps)

opengl-wrapper:
	make -C libs/opengl
openal-wrapper:
	make -C libs/openal
glfw-wrapper:
	make -C libs/glfw
freetype-wrapper:
	make -C libs/freetype
liblodepng.a: liblodepng.a(lodepng.o)

clean:
	rm -f $(objects) $(deps) liblodepng.a osu
	make -C libs/opengl clean
	make -C libs/openal clean
	make -C libs/glfw clean
	make -C libs/freetype clean