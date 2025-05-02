#ifndef IDT_HPP
#define IDT_HPP

#include <cstdint>


namespace idt {
	constexpr uint16_t KERNEL_CODE_SELECTOR=0x08; //gdt entry 1
	void install();

}

#endif
