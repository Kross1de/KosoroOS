#ifndef PRINTF_HPP
#define PRINTF_HPP

#include "string.hpp"
#include <cstdarg>
#include <cstdint>

namespace kprintf {

	void terminal_putchar(char c);
	void terminal_write_string(const char* str);
	void itoa(int num, char* buffer);
	void printf(const char* fmt, ...);
}

#endif
