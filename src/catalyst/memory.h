#ifndef catalyst_memory_h
#define catalyst_memory_h

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

#endif