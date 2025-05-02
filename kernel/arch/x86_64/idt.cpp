#include "include/idt.hpp"
#include "include/printf.hpp"
#include "include/port.hpp"
#include <cstdint>

extern "C" void idt_load(void* idt_descriptor);
extern "C" uint64_t isr_table[];

namespace idt {
	struct idtEntry{
		uint16_t offsetLow;
		uint16_t selector;
		uint8_t  ist;
		uint8_t  typeAttr;
		uint16_t offsetMid;
		uint32_t offsetHigh;
		uint32_t zero;
	} __attribute__((packed));

	struct idtPtr{
		uint16_t limit;
		uint64_t base;
	} __attribute__((packed));

	static idtEntry idt[256];
	static idtPtr idt_descriptor = { sizeof(idt) - 1, (uint64_t)idt };

	extern "C" void idt_handler(uint64_t* stack){
		uint64_t intnum=stack[15];
		kprintf::printf("INT %d triggered\n",intnum);

		if(intnum==14){
			uint64_t errorcode=stack[16];
			uint64_t faultAddr;
			asm volatile ("mov %%cr2, %0" : "=r" (faultAddr));
			kprintf::printf("Page fault as %p, error code: %x\n", faultAddr, errorcode);
			while (true) asm volatile("hlt");
		}

		if(intnum>=32&&intnum<=47){
			if(intnum>=40){
				port::outb(0xA0,0x20);
			}
			port::outb(0x20,0x20);
		}
	}

	void install(){
		for(int i=0;i<256;i++){
			if(!isr_table[i]){
				kprintf::printf("Error: ISR %d is null\n",i);
			}
			uint64_t isr=isr_table[i];
			idt[i].offsetLow=isr&0xFFFF;
			idt[i].selector=KERNEL_CODE_SELECTOR;
			idt[i].ist=0;
			idt[i].typeAttr=0x8E;
			idt[i].offsetMid=(isr>>16)&0xFFFF;
			idt[i].offsetHigh=(isr>>32)&0xFFFFFFFF;
			idt[i].zero=0;
		}

		idt_load(&idt_descriptor);
		kprintf::printf("%s:%d: IDT initialized\n", __FILE__, __LINE__);
	}
}
