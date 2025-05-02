#include "include/vga/vga.hpp"
#include "include/port.hpp"

namespace vga {
	static uint8_t cursor_x = 0;
	static uint8_t cursor_y = 0;

	static constexpr uint16_t VGA_CTRL_PORT = 0x3D4;
	static constexpr uint16_t VGA_DATA_PORT = 0x3D5;

	static uint16_t get_buffer_index(uint8_t x, uint8_t y) {
		return y * VGA_WIDTH + x;
	}

	static void update_hardware_cursor() {
		uint16_t pos = get_buffer_index(cursor_x, cursor_y);

		// Send high byte
		port::outb(VGA_CTRL_PORT, 14);
		port::outb(VGA_DATA_PORT, pos >> 8);
		// Send low byte
		port::outb(VGA_CTRL_PORT, 15);
		port::outb(VGA_DATA_PORT, pos & 0xFF);
	}

	static void scroll() {
		for (uint8_t y = 0; y < VGA_HEIGHT - 1; y++) {
			for (uint8_t x = 0; x < VGA_WIDTH; x++) {
				VGA_BUFFER[get_buffer_index(x, y)] =
				VGA_BUFFER[get_buffer_index(x, y + 1)];
			}
		}

		for (uint8_t x = 0; x < VGA_WIDTH; x++) {
			VGA_BUFFER[get_buffer_index(x, VGA_HEIGHT - 1)] =
			(static_cast<uint16_t>(Color::BLACK) << 12) |
			(static_cast<uint16_t>(Color::WHITE) << 8) | ' ';
		}
		cursor_y--;
	}

	void init() {
		clear();
		set_cursor(0,0);
		update_hardware_cursor();
	}

	void clear(Color bg) {
		uint16_t bg_attr = static_cast<uint16_t>(bg) << 12;
		for (uint16_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
			VGA_BUFFER[i] = bg_attr | ' ';
		}
		set_cursor(0, 0);
	}

	void set_cursor(uint8_t x, uint8_t y) {
		if (x >= VGA_WIDTH) x = VGA_WIDTH - 1;
		if (y >= VGA_HEIGHT) y = VGA_HEIGHT - 1;
		cursor_x = x;
		cursor_y = y;
		update_hardware_cursor();
	}

	void get_cursor(uint8_t& x, uint8_t& y) {
		x = cursor_x;
		y = cursor_y;
	}

	void put_char(char c, uint8_t x, uint8_t y, Color fg, Color bg) {
		if (x >= VGA_WIDTH || y >= VGA_HEIGHT) return;
		uint16_t attr = (static_cast<uint16_t>(bg) << 12) |
		(static_cast<uint16_t>(fg) << 8);
		VGA_BUFFER[get_buffer_index(x, y)] = attr | c;
	}

	void write_char(char c, Color fg, Color bg) {
		if (c == '\n') {
			cursor_x = 0;
			cursor_y++;
		} else {
			put_char(c, cursor_x, cursor_y, fg, bg);
			cursor_x++;
		}

		if (cursor_x >= VGA_WIDTH) {
			cursor_x = 0;
			cursor_y++;
		}

		if (cursor_y >= VGA_HEIGHT) {
			scroll();
		}

		update_hardware_cursor();
	}

	void write_string(const char* str, Color fg, Color bg) {
		while (*str) {
			write_char(*str++, fg, bg);
		}
	}
}
