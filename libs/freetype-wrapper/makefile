vpath %.hpp include/freetype
vpath %.cpp src
LIBNAME = freetype-wrapper
libfile = lib$(LIBNAME).a

CXXFLAGS = --std=c++17
CPPFLAGS = \
-Iinclude \
-I/usr/include/freetype2

objects := $(subst .cpp,.o,$(notdir $(wildcard src/*.cpp )))
deps := $(objects:.o=.d)

$(libfile): $(libfile)($(objects))

%.d : %.cpp
	@set -e; rm -f $@; \
	$(CXX) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
include $(deps)

.PHONY: clean
clean:
	rm -f $(libfile) $(objects) $(deps)
