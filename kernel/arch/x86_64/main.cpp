extern "C" void kmain(void *mboot_info) {
    while (true) {
        
        asm volatile("hlt");
    }    
}
