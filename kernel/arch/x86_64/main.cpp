#include "include/string.hpp"
#include "include/printf.hpp"
#include "include/gdt.hpp"
#include "include/idt.hpp"
#include "include/vga/vga.hpp"
#include "include/version.hpp"
extern "C" void kmain(void *mboot_info)
{
    kprintf::printf("kernel loaded\n");

    gdt::install();
    idt::install();

    vga::write_string("Welcome to the KosoroOS!\n",
                      vga::Color::LIGHT_GREEN,
                      vga::Color::DARK_GRAY);
    kprintf::printf("%s %d.%d.%d %s %d %s\n",
		    kernelName,kernelVMA,kernelVMI,kernelVPA,kernelArch,kernelBuildDate,kernelBuildTime);

    while (true)
    {
        asm volatile("hlt");
    }
}
