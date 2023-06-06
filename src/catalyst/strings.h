#ifndef catalyst_strings_h
#define catalyst_strings_h

#include "typesdef.h"

void con_cat_strings(
	size_t first_source_count, char *first_source,
	size_t sec_source_count, char *sec_source,
	size_t dest_count, char *dest)
{
	for (size_t index = 0; index < first_source_count; ++index)
	{
		*dest++ = *first_source++;
	}

	for (size_t index = 0; index < sec_source_count; ++index)
	{
		*dest++ = *sec_source++;
	}

	*dest++ = 0;
}

b32 compare_strings(char *first_str, char *sec_str)
{
	memory_index index = 0;

	while (first_str[index])
	{
		if (first_str[index] != sec_str[index])
		{
			return 0;
		}

		index++;
	}

	return 1;
}

u32 str_length(char *source)
{
	u32 size = 0;

	while (source[size])
	{
		size++;
	}

	// Calculate the null terminator
	return size + 1;
}
#endif