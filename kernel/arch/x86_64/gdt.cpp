#include "include/gdt.hpp"
#include <cstdint>
#include "include/printf.hpp"

namespace gdt
{
    struct GdtEntry
    {
        uint16_t limit;
        uint16_t base_low;
        uint8_t base_mid;
        uint8_t access;
        uint8_t gran;
        uint8_t base_high;
    } __attribute__((packed));

    constexpr size_t GDT_ENTRIES_COUNT = 9;
    static GdtEntry gdt_entries[GDT_ENTRIES_COUNT];

    Gdtr gdt_descriptor;

    void set_entry(uint8_t index, uint16_t limit, uint32_t base, uint8_t access, uint8_t gran)
    {
        gdt_entries[index].limit = limit;
        gdt_entries[index].base_low = static_cast<uint16_t>(base & 0xFFFF);
        gdt_entries[index].base_mid = static_cast<uint8_t>((base >> 16) & 0xFF);
        gdt_entries[index].access = access;
        gdt_entries[index].gran = gran;
        gdt_entries[index].base_high = static_cast<uint8_t>((base >> 24) & 0xFF);
    }

    void install()
    {
        set_entry(0, 0x0000, 0x00000000, 0x00, 0x00);
        set_entry(1, 0x0000, 0x00000000, 0x9A, 0x20);
        set_entry(2, 0x0000, 0x00000000, 0x92, 0x00);
        set_entry(3, 0x0000, 0x00000000, 0xFA, 0x20);
        set_entry(4, 0x0000, 0x00000000, 0xF2, 0x00);

        gdt_descriptor = Gdtr{
            .size = static_cast<uint16_t>(sizeof(GdtEntry) * GDT_ENTRIES_COUNT - 1),
            .offset = reinterpret_cast<uint64_t>(&gdt_entries)};

        gdt_flush();

        kprintf::printf("%s:%d: GDT initialized\n", __FILE__, __LINE__);
    }
}
