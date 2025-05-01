#ifndef PRINTF_HPP
#define PRINTF_HPP

#include "string.hpp"
#include <cstdarg>
#include <cstdint>

namespace kprintf {

	constexpr uint16_t VGA_WIDTH = 80;
	constexpr uint16_t VGA_HEIGHT = 25;
	uint16_t* VGA_MEMORY = (uint16_t*)0xB8000;

	static uint16_t terminal_row = 0;
	static uint16_t terminal_column = 0;
	static uint8_t terminal_color = 0x0F;

	void terminal_putchar(char c) {
		if (c == '\n') {
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT) {
				terminal_row = 0;
			}
			return;
		}

		const size_t index = terminal_row * VGA_WIDTH + terminal_column;
		VGA_MEMORY[index] = (terminal_color << 8) | c;

		if (++terminal_column == VGA_WIDTH) {
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT) {
				terminal_row = 0;
			}
		}
	}

	void terminal_write_string(const char* str) {
		for (size_t i = 0; str[i] != '\0'; i++) {
			terminal_putchar(str[i]);
		}
	}

	void itoa(int num, char* buffer) {
		if (num == 0) {
			buffer[0] = '0';
			buffer[1] = '\0';
			return;
		}

		int i = 0;
		bool is_negative = num < 0;
		if (is_negative) {
			num = -num;
		}

		while (num > 0) {
			buffer[i++] = '0' + (num % 10);
			num /= 10;
		}

		if (is_negative) {
			buffer[i++] = '-';
		}

		buffer[i] = '\0';

		// reverse the string
		for (int j = 0, k = i - 1; j < k; j++, k--) {
			char temp = buffer[j];
			buffer[j] = buffer[k];
			buffer[k] = temp;
		}
	}

	void printf(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);

		char buffer[32];

		for (size_t i = 0; fmt[i] != '\0'; i++) {
			if (fmt[i] == '%') {
				i++;
				if (fmt[i] == '\0') break;

				switch (fmt[i]) {
					case 's': {
						const char* str = va_arg(args, const char*);
						terminal_write_string(str);
						break;
					}
					case 'd': {
						int num = va_arg(args, int);
						itoa(num, buffer);
						terminal_write_string(buffer);
						break;
					}
					case 'c': {
						char c = (char)va_arg(args, int);
						terminal_putchar(c);
						break;
					}
					default:
						terminal_putchar('%');
						terminal_putchar(fmt[i]);
						break;
				}
			} else {
				terminal_putchar(fmt[i]);
			}
		}

		va_end(args);
	}
}

#endif
