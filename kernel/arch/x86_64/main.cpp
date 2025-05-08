#include "include/string.hpp"
#include "include/printf.hpp"
#include "include/gdt.hpp"
#include "include/idt.hpp"
#include "include/vga/vga.hpp"
#include "include/pmm.hpp"
#include "include/version.hpp"
extern "C" void kmain(void *mboot_info) {
    kprintf::printf("kernel loaded\n");

    gdt::install();
    idt::install();
    pmm::init(mboot_info);
    //kprintf::printf("mboot_info: %p\n", mboot_info);

    vga::write_string("Welcome to the KosoroOS!\n",
                      vga::Color::LIGHT_GREEN,
                      vga::Color::DARK_GRAY);
    kprintf::printf("%s %d.%d.%d %s %d %s\n",
                    kernelName, kernelVMA, kernelVMI, kernelVPA,
                    kernelArch, kernelBuildDate, kernelBuildTime);

    /*void *page1 = pmm::alloc_page();
    void *page2 = pmm::alloc_page();
    kprintf::printf("Allocated pages: %p, %p\n", page1, page2);
    pmm::free_page(page1);
    kprintf::printf("Freed page: %p\n", page1);
    kprintf::printf("Total memory: %u MB, Used: %u MB\n",
                    pmm::get_total_memory() / (1024 * 1024),
                    pmm::get_used_memory() / (1024 * 1024));*/

    while (true) {
        asm volatile("hlt");
    }
}
