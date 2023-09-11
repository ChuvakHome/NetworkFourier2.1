
#ifndef _FOURIER_UTILS_HPP_
#define _FOURIER_UTILS_HPP_

#include <cstddef>
#include <limits>
#include <string>

#include <Fourier/Defines.hpp>

#include <SFML/Window/Keyboard.hpp>

#define DEFAULT_PRECISION 0.00001

#ifdef SFML_SYSTEM_MACOS
    #define LEFT_SPECIAL_SYSKEY sf::Keyboard::LSystem
    #define RIGHT_SPECIAL_SYSKEY sf::Keyboard::RSystem
#else
    #define LEFT_SPECIAL_SYSKEY sf::Keyboard::LControl
    #define RIGHT_SPECIAL_SYSKEY sf::Keyboard::RControl
#endif

inline bool is_special_syskey_pressed() { // short-hand for sf::Keyboard::isKeyPressed(LEFT_SPECIAL_SYSKEY) || sf::Keyboard::isKeyPressed(RIGHT_SPECIAL_SYSKEY) )
    return sf::Keyboard::isKeyPressed(LEFT_SPECIAL_SYSKEY) || sf::Keyboard::isKeyPressed(RIGHT_SPECIAL_SYSKEY);
}

inline bool is_shift_key_pressed() {
    return sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
}

void string_copy(char *dst, const char *src, std::size_t len = std::numeric_limits<size_t>::max()); // need because of necessity use special secure functions (with _s suffix) on MS Windows platfrom

int print_format(char *str, std::size_t size, const char *format, ...); // need because of necessity use special secure functions (with _s suffix) on MS Windows platfrom

bool file_exists(const std::string &path);

// util functions for doing operations with floating point numbers

bool is_equal(float x, float y, double precision = DEFAULT_PRECISION);
bool is_not_equal(float x, float y, double precision = DEFAULT_PRECISION);

bool is_equal(double x, double y, double precision = DEFAULT_PRECISION);
bool is_not_equal(double x, double y, double precision = DEFAULT_PRECISION);

#endif
