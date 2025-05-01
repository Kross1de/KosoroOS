#ifndef GDT_HPP
#define GDT_HPP

#include <cstdint>

namespace gdt {

	void install();

	struct Gdtr
	{
		uint16_t size;
		uint64_t offset;
	} __attribute__((packed));

	extern "C" Gdtr gdt_descriptor;
	extern "C" void gdt_flush();
	void install();

}

#endif
