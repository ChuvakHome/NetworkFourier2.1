
#include <Fourier/Utils.hpp>

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#ifdef SFML_SYSTEM_WINDOWS
    #include <sys/types.h>
#else
    #include <sys/stat.h>
#endif

void string_copy(char *dst, const char *src, std::size_t len) {
    #ifdef SFML_SYSTEM_WINDOWS
        strncpy_s(dst, len, src, len);
    #else
        strncpy(dst, src, len);
    #endif
}

int print_format(char *str, std::size_t size, const char *format, ...) {
    va_list args_list;
    va_start(args_list, format);
    int print_result;

    #ifdef SFML_SYSTEM_WINDOWS
        print_result = vsnprintf_s(str, size, size, format, args_list);
    #else
        print_result = vsnprintf(str, size, format, args_list);
    #endif

    va_end(args_list);

    return print_result;
}

bool file_exists(const std::string &path) {
    struct stat buf;

    return stat(path.c_str(), &buf) == 0;
}

bool is_equal(float x, float y, double precision) {
    return std::fabs(x - y) <= precision;
}

bool is_not_equal(float x, float y, double precision) {
    return std::fabs(x - y) > precision;
}

bool is_equal(double x, double y, double precision) {
    return std::abs(x - y) <= precision;
}

bool is_not_equal(double x, double y, double precision) {
    return std::abs(x - y) > precision;
}
