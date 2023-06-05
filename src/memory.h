#ifndef memory_h
#define memory_h

#include <stdlib.h>

#include "typesdef.h"

void *clone_value(u64 size, data_t source)
{
	void *target = malloc(size);

	u8 *byte = (u8 *)source;
	u8 *target_byte = (u8 *)target;

	// Copy the value
	for (memory_index i = 0; i < size; i++)
	{
		*target_byte++ = *byte++;
	}

	return target;
}

b32 compare_strings(char *str1, char *str2)
{
	memory_index index = 0;

	while (str1[index])
	{
		if (str1[index] != str2[index])
		{
			return 0;
		}

		index++;
	}

	return 1;
}

inline void clear_memory(memory_index size, void *ptr)
{
	u8 *byte = (u8 *)ptr;

	while (size--)
	{
		*byte++ = 0;
	}
}

void set_memory(memory_index data_size, data_t source, i64 val)
{
	uchar *current_byte = (uchar *)source; // Pointer to the memory block as uchar

	uchar value = (uchar)val; // Cast the value to uchar

	// Set each byte of the memory block to the value
	for (memory_index i = 0; i < data_size; i++)
	{
		current_byte[i] = value;
	}
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