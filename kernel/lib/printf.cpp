#include "include/printf.hpp"
#include "include/string.hpp"
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

        bool is_negative = num < 0;
        if (is_negative) {
            num = -num;
        }

        size_t index = 0;
        while (num > 0) {
            buffer[index++] = '0' + (num % 10);
            num /= 10;
        }

        if (is_negative) {
            buffer[index++] = '-';
        }

        buffer[index] = '\0';

        for (size_t i = 0; i < index / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[index - i - 1];
            buffer[index - i - 1] = temp;
        }
    }

    static void utoa(uint64_t num, char* buffer, int base = 10, bool uppercase = false) {
        if (num == 0) {
            buffer[0] = '0';
            buffer[1] = '\0';
            return;
        }

        size_t index = 0;
        const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

        while (num > 0) {
            buffer[index++] = digits[num % base];
            num /= base;
        }

        buffer[index] = '\0';

        for (size_t i = 0; i < index / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[index - i - 1];
            buffer[index - i - 1] = temp;
        }
    }

    void printf(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        char buffer[64];

        while (*fmt) {
            if (*fmt != '%') {
                terminal_putchar(*fmt++);
                continue;
            }

            fmt++;

            int width = 0;
            while (*fmt >= '0' && *fmt <= '9') {
                width = width * 10 + (*fmt - '0');
                fmt++;
            }

            switch (*fmt) {
                case 'd': {
                    int num = va_arg(args, int);
                    itoa(num, buffer);
                    int len = kstring::strlen(buffer);
                    while (len++ < width) terminal_putchar('0');
                    terminal_write_string(buffer);
                    break;
                }
                case 'u': {
                    unsigned int num = va_arg(args, unsigned int);
                    utoa(num, buffer, 10, false);
                    int len = kstring::strlen(buffer);
                    while (len++ < width) terminal_putchar('0');
                    terminal_write_string(buffer);
                    break;
                }
                case 'x':
                case 'X': {
                    uint64_t num = va_arg(args, uint64_t);
                    utoa(num, buffer, 16, (*fmt == 'X'));
                    int len = kstring::strlen(buffer);
                    while (len++ < width) terminal_putchar('0');
                    terminal_write_string(buffer);
                    break;
                }
                case 'p': {
                    uint64_t ptr = (uint64_t)va_arg(args, void*);
                    terminal_write_string("0x");
                    utoa(ptr, buffer, 16, false);
                    int len = kstring::strlen(buffer);
                    while (len++ < width) terminal_putchar('0');
                    terminal_write_string(buffer);
                    break;
                }
                case 's': {
                    const char* str = va_arg(args, const char*);
                    int len = kstring::strlen(str);
                    while (len++ < width) terminal_putchar(' ');
                    terminal_write_string(str ? str : "(null)");
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    terminal_putchar(c);
                    break;
                }
                case '%':
                    terminal_putchar('%');
                    break;
                default:
                    terminal_putchar('%');
                    terminal_putchar(*fmt);
                    break;
            }
            fmt++;
        }

        va_end(args);
    }
}
