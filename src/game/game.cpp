#include "stdio.h"

#include <catalyst/structs.h>
#include <catalyst/typesdef.h>
#include <catalyst/intrinsics.h>

DLL_EXPORT void update_and_render(game_memory *memory, screen_buffer *buffer)
{
	printf("update_and_render with buffers and memory that has size %zu\n", memory->PermanentStorageSize);
}
