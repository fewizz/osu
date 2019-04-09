if [[ -z $CXX ]]; then
    echo you must specify CXX variable;
    exit
fi

cxxargs=\
"-c \
--std=c++17 \
-I../../include"

cxxargs=$cxxargs $CXXARGS

mkdir -p build/al
cd build/al
$CXX \
$cxxargs \
../../src/al/internal.cpp \
../../src/al/buffer.cpp \
../../src/al/source.cpp

cd ../..
mkdir -p build/alc
cd build/alc
$CXX \
$cxxargs \
../../src/alc/device.cpp

cd ..
ar r  libopenal-wrapper.a al/internal.o al/buffer.o al/source.o alc/device.o