#include "include/string.hpp"
#include "include/printf.hpp"

extern "C" void kmain(void *mboot_info)
{

    kprintf::printf("kernel loaded\n");

    // test
    const char *test = "Hello, world!";
    kprintf::printf("copied string: %s\n", test);

    size_t len = kstring::strlen(test);
    kprintf::printf("string length: %d\n", len);

    while (true) {
        asm volatile("hlt");
    }
}
