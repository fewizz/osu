#include "execinfo.h"
#include <string>

inline std::string get_stacktrace_string() {
    void *array[100];

    size_t size = backtrace (array, 10);
    char** strings = backtrace_symbols (array, size);

    std::string result;
    for (int i = 0; i < size; i++)
        result += strings[i] + std::string("\n");

    free(strings);
    return result;
}