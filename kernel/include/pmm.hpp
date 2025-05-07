#pragma once

#include <cstdint>
#include <cstddef>

namespace pmm {
	void init(void *mboot_info);
	void *alloc_page();
	void free_page(void *page);
	size_t get_total_memory();
	size_t get_used_memory();
}
