# openal-wrapper
### for c++17

## Usage
Options:
- You can compile this wrapper to static lib. You don't need to specify AL headers.
- Use only headers. For this, you should add ```AL_INCLUDE``` definition, for example, as compiler option (-D).

Also, you can add ```AL_CHECK_FOR_ERROR``` definition, so after all funtion calls ```alGetError``` will be called to check the state. If there's error, al::al_error will be thrown, with error code.

Open device and create context
```c++
//#include "alc/device.hpp"
auto device = alc::open_default_device();
auto context = device.create_context();
alc::make_context_current(context);
```
Or you can use ```alc::open_device(std::string)``` to open specidied device

### Buffer
#### Creating
```c++
//#include "al/buffer.hpp"
al::buffer buff;
```

#### Feeding
```c++
std::vector<char> data;
buff.data(
    2, // channels
    16, // bits
    data, // passing vector by reference
    40000 // frequency
);
```
or
```c++
buff.data(
    al::buffer::format::stereo16,
    data, // passing vector by reference
    40000 // frequency
);
```
for arrays
```c++
char* data = ...;
size_t size = ...;
...
buff.data(
    al::buffer::format::stereo16,
    data,
    size,
    40000 // frequency
);
```
### Source
#### Creating
```c++
//#include "al/source.hpp"
al::source src;
```
#### Setting buffer
```c++
src.buffer(buff);
```

#### Quering state
```al::source::state get_state()```
or specified versions: ```bool stopped()```, ```bool playing()```, etc