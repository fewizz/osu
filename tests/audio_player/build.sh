clang++ \
-o main \
--std=c++17 -ggdb \
-DAL_INCLUDE \
-I../../src \
-I../../libs/minimp3 \
-I../../libs/openal-wrapper/include \
-L../../libs/openal-wrapper \
../../src/mp3.cpp \
main.cpp \
-lopenal \