#include "include/string.hpp"
#include "include/printf.hpp"
#include "include/gdt.hpp"
#include "include/idt.hpp"
#include "include/vga/vga.hpp"
extern "C" void kmain(void *mboot_info)
{
    kprintf::printf("kernel loaded\n");

    gdt::install();
    idt::install();

    vga::write_string("Welcome to the KosoroOS!\n",
                      vga::Color::LIGHT_GREEN,
                      vga::Color::DARK_GRAY);

    while (true)
    {
        asm volatile("hlt");
    }
}
