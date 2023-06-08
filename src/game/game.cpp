#include "stdio.h"

#include <catalyst/structs.h>
#include <catalyst/typesdef.h>
#include <catalyst/intrinsics.h>
#include <catalyst/math.h>

void draw_rectangle(
	ScreenBuffer *buffer,
	Vector2 vec_min, Vector2 vec_max,
	Vector3 colour)
{
	i32 min_x = round_real32_to_int32(vec_min.X);
	i32 max_x = round_real32_to_int32(vec_max.X);

	i32 min_y = round_real32_to_int32(vec_min.Y);
	i32 max_y = round_real32_to_int32(vec_max.Y);

	// Clip to the nearest valid pixel
	if (min_x < 0)
		min_x = 0;

	if (min_y < 0)
		min_y = 0;

	if (max_x > buffer->Width)
		max_x = buffer->Width;

	if (max_y > buffer->Height)
		max_y = buffer->Height;

	// Bit Pattern = x0 AA RR GG BB
	u32 final_colour =
		(round_real32_to_int32(colour.R * 255.f) << 16) |
		(round_real32_to_int32(colour.G * 255.f) << 8) |
		(round_real32_to_int32(colour.B * 255.f) << 0);

	u8 *row = (((u8 *)buffer->Memory) + (i64)min_x * buffer->BytesPerPixel + (i64)min_y * buffer->Pitch);

	for (i32 y = min_y; y < max_y; ++y)
	{
		u32 *pixel = (u32 *)row;

		for (i32 x = min_x; x < max_x; ++x)
		{
			*pixel++ = final_colour;
		}

		row += buffer->Pitch;
	}
}

DLL_EXPORT void update_and_render(GameMemory *memory, GameInput *input, ScreenBuffer *buffer)
{
	GameState *game_state = (GameState *)memory->PermanentStorage;

	if (!memory->IsInitialized)
	{
		memory->IsInitialized = true;
		game_state->colour = V3(50, 50, 50);
	}

	Vector3 *colour = &game_state->colour;

	if (input->Controllers[0].MoveLeft.EndedDown)
	{
		colour->R += 1;

		if (colour->R > 255)
		{
			colour->R = 0;
		}
	}

	if (input->Controllers[0].MoveRight.EndedDown)
	{
		colour->G += 1;

		if (colour->G > 255)
		{
			colour->G = 0;
		}
	}

	if (input->Controllers[0].MoveUp.EndedDown)
	{
		colour->B += 1;

		if (colour->B > 255)
		{
			colour->B = 0;
		}
	}

	draw_rectangle(buffer, V2(0, 0), V2(buffer->Width, buffer->Height), *colour);
}
