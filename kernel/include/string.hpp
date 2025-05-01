#pragma once
#include <cstddef>

namespace kstring {
	size_t strlen(const char* str);
	void strcpy(char* dest, const char* src);
	void strncpy(char* dest, const char* src, size_t n);
	int strcmp(const char* s1, const char* s2);
	void memset(void* dest, int value, size_t n);
	void memcpy(void* dest, const void* src, size_t n);
}
