#include "stdio.h"

#include <catalyst/structs.h>
#include <catalyst/typesdef.h>
#include <catalyst/intrinsics.h>

DLL_EXPORT void update_and_render(game_memory *memory, screen_buffer *buffer)
{
	u32 finalColour =
		(150 << 16) |
		(50 << 8) |
		(50 << 0);

	u8 *row = (u8 *)buffer->Memory;

	for (i32 y = 0; y < buffer->Height; ++y)
	{
		u32 *pixel = (u32 *)row;

		for (i32 x = 0; x < buffer->Width; ++x)
		{
			*pixel++ = finalColour;
		}

		row += buffer->Pitch;
	}
}
