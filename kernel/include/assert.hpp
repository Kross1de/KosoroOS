#ifndef ASSERT_HPP
#define ASSERT_HPP

#include "printf.hpp"

#define assert(condition, message) \
    do { \
        if (!(condition)) { \
            kprintf::printf("Assertion failed: %s, file %s, line %d\n", \
                            message, __FILE__, __LINE__); \
            while (true) { \
                asm volatile("cli; hlt"); \
            } \
        } \
    } while (0)

#endif
