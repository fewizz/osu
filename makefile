source-containing-dirs = $(sort $(dir $(wildcard src/*/*/) $(wildcard src/*)))

$(info $(source-containing-dirs))

vpath %.cpp $(source-containing-dirs) libs/lodepng
vpath %.hpp $(source-containing-dirs)
vpath %.h libs/lodepng
vpath %.o .build/
vpath %.a .build/

objects := $(subst .cpp,.o,\
$(foreach dir,$(source-containing-dirs),$(notdir $(wildcard $(dir)*.cpp ))))
deps := $(objects:.o=.d)
CXX = clang++
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
-L.build \
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
	$(CXX) $(CXXFLAGS) -o $@ $(addprefix .build/,$(objects)) $(LDFLAGS) $(LDLIBS)

%.hpp:
	
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o .build/$@

.build/%.d : %.cpp
	#@set -e; rm -f $@;
	$(CXX) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

ifeq (,$(wildcard /.build/))
	mkdir -p .build
endif

include $(addprefix .build/,$(deps))

opengl-wrapper:
	make -C libs/opengl
openal-wrapper:
	make -C libs/openal
glfw-wrapper:
	make -C libs/glfw
freetype-wrapper:
	make -C libs/freetype
liblodepng.a: lodepng.o
	ar cr .build/liblodepng.a .build/lodepng.o

clean:
	rm -rf .build liblodepng.a osu
	make -C libs/opengl clean
	make -C libs/openal clean
	make -C libs/glfw clean
	make -C libs/freetype clean