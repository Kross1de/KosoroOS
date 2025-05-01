#include "include/serial.hpp"
#include "include/port.hpp"

namespace serial
{

	void init()
	{
		port::outb(COM1 + 1, 0x00);
		port::outb(COM1 + 3, 0x80);
		port::outb(COM1 + 0, 0x03);
		port::outb(COM1 + 1, 0x00);
		port::outb(COM1 + 3, 0x03);
		port::outb(COM1 + 2, 0xC7);
		port::outb(COM1 + 4, 0x0B);
	}

	bool is_transmit_empty()
	{
		return (port::inb(COM1 + 5) & 0x20) != 0;
	}

	void write_char(char c)
	{
		while (!is_transmit_empty())
			;
		port::outb(COM1, c);
	}

	void write_string(const char *str)
	{
		while (*str)
		{
			write_char(*str++);
		}
	}
}
