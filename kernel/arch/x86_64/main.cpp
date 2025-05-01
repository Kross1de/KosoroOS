#include "include/string.hpp"
#include "include/printf.hpp"
#include "include/gdt.hpp"

extern "C" void kmain(void *mboot_info)
{
    kprintf::printf("kernel loaded\n");

    gdt::install();

    while (true)
    {
        asm volatile("hlt");
    }
}