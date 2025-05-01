#include "include/printf.hpp"
#include "include/vga/vga.hpp"

namespace kprintf {

	void terminal_putchar(char c) {
		vga::write_char(c, vga::Color::WHITE, vga::Color::BLACK);
	}

	void terminal_write_string(const char* str) {
		vga::write_string(str, vga::Color::WHITE, vga::Color::BLACK);
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

} // namespace kprintf
