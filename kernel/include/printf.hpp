#ifndef PRINTF_HPP
#define PRINTF_HPP

#include "string.hpp"
#include <cstdarg>
#include <cstdint>

namespace kprintf {

	constexpr uint16_t VGA_WIDTH = 80;
	constexpr uint16_t VGA_HEIGHT = 25;

	extern uint16_t* VGA_MEMORY;
	extern uint16_t terminal_row;
	extern uint16_t terminal_column;
	extern uint8_t terminal_color;

	void terminal_putchar(char c);
	void terminal_write_string(const char* str);
	void itoa(int num, char* buffer);
	void printf(const char* fmt, ...);
}

#endif
