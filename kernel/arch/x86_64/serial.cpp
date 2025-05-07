#include "include/serial.hpp"
#include "include/port.hpp"
#include "include/printf.hpp"
#include "include/assert.hpp"

namespace serial
{
    bool init()
    {
        assert(COM1 != 0, "Invalid COM1 port address");

        port::outb(COM1 + 1, 0x00);
        port::outb(COM1 + 3, 0x80);
        port::outb(COM1 + 0, 0x03);
        port::outb(COM1 + 1, 0x00);
        port::outb(COM1 + 3, 0x03);
        port::outb(COM1 + 2, 0xC7);
        port::outb(COM1 + 4, 0x0B);

        port::outb(COM1, 0xAE);
        if (port::inb(COM1) != 0xAE) {
            kprintf::printf("Serial port COM1 not functional\n");
            return false;
        }

        kprintf::printf("Serial port COM1 initialized\n");
        return true;
    }

    bool is_transmit_empty()
    {
        bool result = (port::inb(COM1 + 5) & 0x20) != 0;
        assert(COM1 != 0, "Invalid COM1 port address in is_transmit_empty");
        return result;
    }

    void write_char(char c)
    {
        while (!is_transmit_empty())
            ;
        port::outb(COM1, c);
    }

    void write_string(const char *str)
    {
        assert(str != nullptr, "Null string pointer in write_string");

        while (*str)
        {
            write_char(*str++);
        }
    }
}
