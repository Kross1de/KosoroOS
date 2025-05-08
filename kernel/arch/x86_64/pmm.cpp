#include "include/pmm.hpp"
#include "include/printf.hpp"
#include "include/string.hpp"

namespace pmm {
    constexpr size_t PAGE_SIZE = 4096;
    constexpr size_t BITMAP_ENTRY_BITS = 8;
    constexpr size_t MAX_MEMORY = 4ULL * 1024 * 1024 * 1024; 
    constexpr size_t BITMAP_MAX_SIZE = 128 * 1024; 

    static uint8_t bitmap[BITMAP_MAX_SIZE] __attribute__((aligned(PAGE_SIZE)));
    static size_t bitmap_size = 0;
    static size_t total_pages = 0;
    static size_t free_pages = 0;
    static size_t next_free_page = 0;

    extern "C" {
        extern uint8_t __kernel_start[];
        extern uint8_t __kernel_end[];
    }

    struct MultibootTag {
        uint32_t type;
        uint32_t size;
    };

    struct MultibootMmapEntry {
        uint64_t addr;
        uint64_t len;
        uint32_t type;
        uint32_t reserved;
    };

    struct MultibootMmapTag {
        uint32_t type;
        uint32_t size;
        uint32_t entry_size;
        uint32_t entry_version;
        MultibootMmapEntry entries[0];
    };

    static inline void set_bit(size_t index) {
        if (index / BITMAP_ENTRY_BITS < bitmap_size) {
            bitmap[index / BITMAP_ENTRY_BITS] |= (1 << (index % BITMAP_ENTRY_BITS));
        }
    }

    static inline void clear_bit(size_t index) {
        if (index / BITMAP_ENTRY_BITS < bitmap_size) {
            bitmap[index / BITMAP_ENTRY_BITS] &= ~(1 << (index % BITMAP_ENTRY_BITS));
        }
    }

    static inline bool test_bit(size_t index) {
        if (index / BITMAP_ENTRY_BITS >= bitmap_size) {
            return true;
        }
        return bitmap[index / BITMAP_ENTRY_BITS] & (1 << (index % BITMAP_ENTRY_BITS));
    }

    static void mark_region(uint64_t base, uint64_t length, bool used) {
        uint64_t start = (base + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
        uint64_t end = base + length;
        size_t page_count = (end - start + PAGE_SIZE - 1) / PAGE_SIZE;

        if (start >= MAX_MEMORY || page_count == 0) {
            return;
        }

        size_t start_page = start / PAGE_SIZE;
        if (start_page + page_count > MAX_MEMORY / PAGE_SIZE) {
            page_count = MAX_MEMORY / PAGE_SIZE - start_page;
        }

        for (size_t i = 0; i < page_count; i++) {
            size_t index = start_page + i;
            if (index >= bitmap_size * BITMAP_ENTRY_BITS) {
                break;
            }
            bool current_state = test_bit(index);
            if (used && !current_state) {
                set_bit(index);
                free_pages--;
            } else if (!used && current_state) {
                clear_bit(index);
                free_pages++;
            }
        }
    }

    void init(void *mboot_info) {
        kprintf::printf("Initializing PMM...\n");

        bitmap_size = (MAX_MEMORY / PAGE_SIZE + BITMAP_ENTRY_BITS - 1) / BITMAP_ENTRY_BITS;
        if (bitmap_size > BITMAP_MAX_SIZE) {
            bitmap_size = BITMAP_MAX_SIZE;
        }
        kstring::memset(bitmap, 0xFF, bitmap_size); 
        total_pages = bitmap_size * BITMAP_ENTRY_BITS;
        free_pages = 0;
        next_free_page = 0;

        if (!mboot_info) {
            kprintf::printf("No multiboot info, using default 1GB configuration\n");
            mark_region(0x100000, 1024 * 1024 * 1024, false);
        } else {
            MultibootTag *tag = (MultibootTag *)mboot_info;
            MultibootMmapTag *mmap_tag = nullptr;
            size_t tag_count = 0;

            while (tag->type != 0 && tag_count < 100) {
                kprintf::printf("Checking tag: type=%u, size=%u\n", tag->type, tag->size);
                if (tag->type == 6) {
                    mmap_tag = (MultibootMmapTag *)tag;
                    kprintf::printf("Found memory map tag\n");
                    break;
                }
                tag = (MultibootTag *)((uint8_t *)tag + ((tag->size + 7) & ~7));
                tag_count++;
            }

            if (!mmap_tag || mmap_tag->entry_size == 0) {
                kprintf::printf("Invalid memory map, using default 1GB configuration\n");
                mark_region(0x100000, 1024 * 1024 * 1024, false);
            } else {
                size_t entry_count = (mmap_tag->size - sizeof(MultibootMmapTag)) / mmap_tag->entry_size;
                kprintf::printf("Processing %u memory map entries\n", entry_count);
                for (size_t i = 0; i < entry_count && i < 100; i++) {
                    MultibootMmapEntry *entry = &mmap_tag->entries[i];
                    if (entry->addr >= MAX_MEMORY) {
                        continue;
                    }
                    kprintf::printf("Region %u: addr=0x%llx, len=0x%llx, type=%u\n",
                        i, entry->addr, entry->len, entry->type);
                    if (entry->type == 1) {
                        mark_region(entry->addr, entry->len, false);
                    }
                }
            }
        }

        uint64_t kernel_start = (uint64_t)__kernel_start;
        uint64_t kernel_end = (uint64_t)__kernel_end;
        mark_region(kernel_start, kernel_end - kernel_start, true);
        mark_region((uint64_t)mboot_info, PAGE_SIZE, true);
        mark_region(0, 0x100000, true);
        mark_region((uint64_t)bitmap, bitmap_size, true);

        kprintf::printf("PMM initialized: %u MB total, %u MB free\n",
            (total_pages * PAGE_SIZE) / (1024 * 1024),
            (free_pages * PAGE_SIZE) / (1024 * 1024));
    }

    void *alloc_page() {
        for (size_t i = next_free_page; i < total_pages; i++) {
            if (!test_bit(i)) {
                set_bit(i);
                free_pages--;
                next_free_page = i + 1;
                return (void *)(i * PAGE_SIZE);
            }
        }

        for (size_t i = 0; i < next_free_page; i++) {
            if (!test_bit(i)) {
                set_bit(i);
                free_pages--;
                next_free_page = i + 1;
                return (void *)(i * PAGE_SIZE);
            }
        }

        kprintf::printf("Error: Out of physical memory\n");
        return nullptr;
    }

    void free_page(void *page) {
        if (!page) return;

        size_t index = (size_t)page / PAGE_SIZE;
        if (index >= total_pages) {
            kprintf::printf("Error: Invalid page address to free: %p\n", page);
            return;
        }

        if (!test_bit(index)) {
            kprintf::printf("Warning: Double free attempt on page: %p\n", page);
            return;
        }

        clear_bit(index);
        free_pages++;
        if (index < next_free_page) {
            next_free_page = index;
        }
    }

    size_t get_total_memory() {
        return total_pages * PAGE_SIZE;
    }

    size_t get_used_memory() {
        return (total_pages - free_pages) * PAGE_SIZE;
    }
}
