#include "include/string.hpp"
#include <cstdint>

namespace kstring
{
	size_t strlen(const char *str)
	{
		size_t len = 0;
		while (str[len])
		{
			len++;
		}
		return len;
	}

	void strcpy(char *dest, const char *src)
	{
		while (*src)
		{
			*dest++ = *src++;
		}
		*dest = '\0';
	}

	void strncpy(char *dest, const char *src, size_t n)
	{
		size_t i;
		for (i = 0; i < n && src[i]; i++)
		{
			dest[i] = src[i];
		}
		for (; i < n; i++)
		{
			dest[i] = '\0';
		}
	}

	int strcmp(const char *s1, const char *s2)
	{
		while (*s1 && (*s1 == *s2))
		{
			s1++;
			s2++;
		}
		return *(unsigned char *)s1 - *(unsigned char *)s2;
	}

	void memset(void *dest, int value, size_t n)
	{
		uint8_t *ptr = (uint8_t *)dest;
		for (size_t i = 0; i < n; i++)
		{
			ptr[i] = (uint8_t)value;
		}
	}

	void memcpy(void *dest, const void *src, size_t n)
	{
		uint8_t *d = (uint8_t *)dest;
		const uint8_t *s = (const uint8_t *)src;
		for (size_t i = 0; i < n; i++)
		{
			d[i] = s[i];
		}
	}
}
