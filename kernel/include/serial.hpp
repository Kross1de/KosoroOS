#pragma once
#include <cstdint>

namespace serial {
	constexpr uint16_t COM1 = 0x3F8;

	bool init();
	bool is_transmit_empty();
	void write_char(char c);
	void write_string(const char* str);
}
