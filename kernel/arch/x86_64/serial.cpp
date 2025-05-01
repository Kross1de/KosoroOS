#include "serial.hpp"

namespace serial {
	//basic I/O port operations
	static inline void outb(uint16_t port,uint8_t value){
		asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
	}

	static inline uint8_t inb(uint16_t port){
		uint8_t value;
		asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
		return value;
	}

	void init(){
		outb(COM1+1,0x00);
		outb(COM1+3,0x80);
		outb(COM1+0,0x03);
		outb(COM1+1,0x00);
		outb(COM1+3,0x03);
		outb(COM1+2,0xC7);
		outb(COM1+4,0x0B);
	}

	bool is_transmit_empty(){
		return (inb(COM1+5)&0x20)!=0;
	}

	void write_char(char c){
		while(!is_transmit_empty());
		outb(COM1,c);
	}

	void write_string(const char* str){
		while(*str){
			write_char(*str++);
		}
	}
}
