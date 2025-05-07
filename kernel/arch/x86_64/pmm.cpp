#include "include/pmm.hpp"
#include "include/printf.hpp"
#include "include/string.hpp"

namespace pmm {
	constexpr size_t pageSize = 4096;
	constexpr size_t bitmapEntrySize = 8;
	constexpr size_t maxMemory = 4ULL * 1024 * 1024 * 1024;
	constexpr size_t bitmapSize = maxMemory / pageSize / bitmapEntrySize;
	
	static uint8_t bitmap[bitmapSize] __attribute__((aligned(4096)));
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
		bitmap[index / bitmapEntrySize] |= (1 << (index % bitmapEntrySize));
	}
	
	static void clearBit(size_t index) {
		bitmap[index / bitmapEntrySize] &= ~(1 << (index % bitmapEntrySize));
	}
	
	static bool testBit(size_t index) {
		return bitmap[index / bitmapEntrySize] & (1 << (index % bitmapEntrySize));
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
	
	void init(void *mboot_info) {
		kstring::memset(bitmap, 0, bitmapSize);
		usedPages = 0; 
		lastUsedIndex = 0;
		
		MultibootTag *tag = (MultibootTag *)mboot_info;
		MultibootMmapTag *mmap_tag = nullptr;
		
		for (; tag->type != 0; tag = (MultibootTag *)((uint8_t *)tag + ((tag->size + 7) & ~7))) {
			if (tag->type == 6) {
				mmap_tag = (MultibootMmapTag *)tag;
				break;
			}
		}
		
		if (!mmap_tag) {
			kprintf::printf("Error: No memory map found\n");
			return;
		}
		
		for (size_t i = 0; i < (mmap_tag->size - sizeof(MultibootMmapTag)) / mmap_tag->entrySize; i++) {
			MultibootMmapEntry *entry = &mmap_tag->entries[i];
			if (entry->addr == maxMemory) continue;
			
			if (entry->type == 1) {
				markRegion(entry->addr, entry->len, false);
				totalPages += entry->len / pageSize;
			} else {
				markRegion(entry->addr, entry->len, true);
			}
		}
		
		uint64_t kernel_start = (uint64_t)__kernel_start;
        uint64_t kernel_end = (uint64_t)__kernel_end;
        markRegion(kernel_start, kernel_end - kernel_start, true);
        markRegion((uint64_t)mboot_info, 4096, true);
        markRegion(0, 0x100000, true);
        kprintf::printf("PMM initialized: %u MB total, %u MB used\n", totalPages * pageSize / (1024 * 1024), usedPages * pageSize / (1024 * 1024));
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
