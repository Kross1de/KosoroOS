#include "include/serial.hpp"
#include "include/string.hpp"

extern "C" void kmain(void *mboot_info)
{
    serial::init();
    serial::write_string("Kernel initialized!\n");

    // test
    char buffer[32];
    const char *test = "Hello, world!";
    kstring::strcpy(buffer, test);
    serial::write_string("copied string: ");
    serial::write_string(buffer);
    serial::write_char('\n');

    serial::write_string("string length: ");
    char len_str[16];

    size_t len = kstring::strlen(test);
    len_str[0] = '0' + (len / 10);
    len_str[1] = '0' + (len % 10);
    serial::write_string(len_str);
    serial::write_char('\n');

    while (true)
    {
        asm volatile("hlt");
    }
}
