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
-Ilibs/opengl-wrapper/include \
-Ilibs/utfcpp/source \
-Ilibs/openal-wrapper/include \
-Ilibs/glfw-wrapper/include \
-Ilibs/freetype-wrapper/include \
-Ilibs/minimp3 \
-Ilibs/glm \
-Ilibs/stacktrace/include \
-Ilibs/lodepng \
-I/usr/include/freetype2/

override CXXFLAGS += --std=c++17

override LDFLAGS += \
-L.build \
-L. \
-Llibs/opengl-wrapper \
-Llibs/openal-wrapper \
-Llibs/glfw-wrapper \
-Llibs/freetype-wrapper

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

.PHONY: $(lib-targets) clean framework

framework: $(objects) $(lib-targets)
	ar cr .build/libframework.a $(addprefix .build/,$(objects))

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
	make -C libs/opengl-wrapper
openal-wrapper:
	make -C libs/openal-wrapper
glfw-wrapper:
	make -C libs/glfw-wrapper
freetype-wrapper:
	make -C libs/freetype-wrapper
liblodepng.a: lodepng.o
	ar cr .build/liblodepng.a .build/lodepng.o

clean:
	rm -rf .build
	make -C libs/opengl-wrapper clean
	make -C libs/openal-wrapper clean
	make -C libs/glfw-wrapper clean
	make -C libs/freetype-wrapper clean