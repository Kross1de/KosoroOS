#pragma once
#include <cstddef>
#include <cstdint>

namespace gdt
{
	struct Gdtr
	{
		uint16_t size;
		uint64_t offset;
	} __attribute__((packed));

	void set_entry(uint8_t index, uint16_t limit, uint32_t base, uint8_t access, uint8_t gran);
	void install();
	extern "C" void gdt_flush();
}
