#ifndef VGA_HPP
#define VGA_HPP

#include <cstdint>

namespace vga{
	static  uint16_t* const VGA_BUFFER=(uint16_t*)0xB8000;
	static constexpr uint8_t VGA_WIDTH=80;
	static constexpr uint8_t VGA_HEIGHT=25;

	enum class Color : uint8_t {
		BLACK=0,
		BLUE=1,
		GREEN=2,
		CYAN=3,
		RED=4,
		MAGENTA=5,
		BROWN=6,
		LIGHT_GRAY=7,
		DARK_GRAY=8,
		LIGHT_BLUE=9,
		LIGHT_GREEN=10,
		LIGHT_CYAN=11,
		LIGHT_RED=12,
		LIGHT_MAGENTA=13,
		YELLOW=14,
		WHITE=15
	};
	void init();
	void clear(Color bg=Color::BLACK);
	void set_cursor(uint8_t x,uint8_t y);
	void get_cursor(uint8_t& x,uint8_t& y);
	void put_char(char c, uint8_t x, uint8_t y,
			Color fg = Color::WHITE, Color bg = Color::BLACK);
	void write_char(char c, Color fg = Color::WHITE, Color bg = Color::BLACK);
	void write_string(const char* str,
			    Color fg = Color::WHITE, Color bg = Color::BLACK);
}

#endif
