#include "include/pmm.hpp"
#include "include/printf.hpp"
#include "include/string.hpp"

namespace pmm {
    constexpr size_t pageSize = 4096;
    constexpr size_t bitmapEntrySize = 8;
    constexpr size_t maxMemory = 4ULL * 1024 * 1024 * 1024;
    static size_t bitmapSize; 
    static uint8_t bitmap[1024 * 1024] __attribute__((aligned(4096))); 
    static size_t totalPages = 0;
    static size_t usedPages = 0;
    static size_t lastUsedIndex = 0;

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
        uint32_t entrySize;
        uint32_t entryVersion;
        MultibootMmapEntry entries[0];
    };
    
    static void setBit(size_t index) {
        if (index / bitmapEntrySize < bitmapSize)
            bitmap[index / bitmapEntrySize] |= (1 << (index % bitmapEntrySize));
    }

    static void clearBit(size_t index) {
        if (index / bitmapEntrySize < bitmapSize)
            bitmap[index / bitmapEntrySize] &= ~(1 << (index % bitmapEntrySize));
    }

    static bool testBit(size_t index) {
        return (index / bitmapEntrySize < bitmapSize) &&
               (bitmap[index / bitmapEntrySize] & (1 << (index % bitmapEntrySize)));
    }

    static void markRegion(uint64_t start, uint64_t len, bool used) {
        if (start >= maxMemory || len == 0) return;

        size_t startPage = start / pageSize;
        size_t pageCount = (len + pageSize - 1) / pageSize;

        if (startPage + pageCount > maxMemory / pageSize) {
            pageCount = maxMemory / pageSize - startPage;
        }

        for (size_t i = 0; i < pageCount; i++) {
            size_t index = startPage + i;
            if (index < bitmapSize * bitmapEntrySize) {
                if (used) {
                    if (!testBit(index)) {
                        setBit(index);
                        usedPages++;
                    }
                } else {
                    if (testBit(index)) {
                        clearBit(index);
                        usedPages--;
                    }
                }
            }
        }
    }

    void init(void *mboot_info) {
    kprintf::printf("Starting PMM init, mboot_info=%p\n", mboot_info);
    bitmapSize = maxMemory / pageSize / bitmapEntrySize;
    kprintf::printf("Bitmap size: %u\n", bitmapSize);
    if (bitmapSize > sizeof(bitmap)) {
        kprintf::printf("Error: Bitmap size exceeds allocated buffer (%u > %zu)\n", bitmapSize, sizeof(bitmap));
        return;
    }
    kprintf::printf("Clearing bitmap\n");
    kstring::memset(bitmap, 0, bitmapSize);
    kprintf::printf("Bitmap cleared\n");
    usedPages = 0;
    lastUsedIndex = 0;

    // Default values in case we skip the memory map parsing
    totalPages = 0;

    if (!mboot_info) {
        kprintf::printf("Error: mboot_info is null, using default config\n");
        markRegion(0, maxMemory, false);
        totalPages = maxMemory / pageSize;
    } else {
        MultibootTag *tag = (MultibootTag *)mboot_info;
        MultibootMmapTag *mmap_tag = nullptr;

        kprintf::printf("Searching for memory map tag\n");
        for (; tag->type != 0; tag = (MultibootTag *)((uint8_t *)tag + ((tag->size + 7) & ~7))) {
            kprintf::printf("Tag type=%u, size=%u\n", tag->type, tag->size);
            if (tag->type == 6) {
                mmap_tag = (MultibootMmapTag *)tag;
                break;
            }
        }

        if (!mmap_tag || mmap_tag->entrySize == 0 || mmap_tag->entryVersion != 0) {
            kprintf::printf("Warning: Invalid or missing memory map, using default configuration\n");
            markRegion(0, maxMemory, false); 
            totalPages = maxMemory / pageSize;
        } else {
            kprintf::printf("Processing memory map, entrySize=%u, entryVersion=%u\n", mmap_tag->entrySize, mmap_tag->entryVersion);
            for (size_t i = 0; i < (mmap_tag->size - sizeof(MultibootMmapTag)) / mmap_tag->entrySize; i++) {
                MultibootMmapEntry *entry = &mmap_tag->entries[i];
                kprintf::printf("Memory region %u: addr=%llu, len=%llu, type=%u\n", i, entry->addr, entry->len, entry->type);
                if (entry->addr >= maxMemory) continue;

                if (entry->type == 1) {
                    markRegion(entry->addr, entry->len, false);
                    totalPages += entry->len / pageSize;
                } else {
                    markRegion(entry->addr, entry->len, true);
                }
            }
        }

        uint64_t kernel_start = (uint64_t)__kernel_start;
        uint64_t kernel_end = (uint64_t)__kernel_end;
        kprintf::printf("Marking kernel region: start=%llu, end=%llu\n", kernel_start, kernel_end);
        markRegion(kernel_start, kernel_end - kernel_start, true);
        kprintf::printf("Marking multiboot info: addr=%llu\n", (uint64_t)mboot_info);
        markRegion((uint64_t)mboot_info, 4096, true);
        kprintf::printf("Marking low memory\n");
        markRegion(0, 0x100000, true);
    }

    kprintf::printf("PMM initialized: %u MB total, %u MB used\n",
                   totalPages * pageSize / (1024 * 1024),
                   usedPages * pageSize / (1024 * 1024));
}
    void *alloc_page() {
        for (size_t i = lastUsedIndex; i < totalPages; i++) {
            if (!testBit(i)) {
                setBit(i);
                usedPages++;
                lastUsedIndex = i + 1;
                return (void *)(i * pageSize);
            }
        }

        for (size_t i = 0; i < lastUsedIndex; i++) {
            if (!testBit(i)) {
                setBit(i);
                usedPages++;
                lastUsedIndex = i + 1;
                return (void *)(i * pageSize);
            }
        }

        lastUsedIndex = 0; 
        kprintf::printf("Error: Out of memory\n");
        return nullptr;
    }

    void free_page(void *page) {
        size_t index = (size_t)page / pageSize;
        if (index >= totalPages || !testBit(index)) {
            kprintf::printf("Error: Invalid page to free\n");
            return;
        }
        clearBit(index);
        usedPages--;
        if (index < lastUsedIndex) {
            lastUsedIndex = index;
        }
    }

    size_t get_total_memory() {
        return totalPages * pageSize;
    }

    size_t get_used_memory() {
        return usedPages * pageSize;
    }
}
