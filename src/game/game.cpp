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
	{
		min_x = 0;
	}

	if (min_y < 0)
	{
		min_y = 0;
	}

	if (max_x > buffer->Width)
	{
		max_x = buffer->Width;
	}

	if (max_y > buffer->Height)
	{
		max_y = buffer->Height;
	}

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

		game_state->PlayerPosition.X = 40;
		game_state->PlayerPosition.Y = 30;
	}

	Vector2 player_momentum = {0, 0};

	// Player movement
	if (input->Controllers[0].MoveLeft.EndedDown)
	{
		player_momentum.X = -1;
	}

	if (input->Controllers[0].MoveRight.EndedDown)
	{
		player_momentum.X = 1;
	}

	if (input->Controllers[0].MoveDown.EndedDown)
	{
		player_momentum.Y = 1;
	}

	if (input->Controllers[0].MoveUp.EndedDown)
	{
		player_momentum.Y = -1;
	}

	r32 player_speed = 64.f;

	if (input->Controllers[0].ActionLeft.EndedDown)
	{
		player_speed = 128.f;
	}

	player_momentum.X *= player_speed;
	player_momentum.Y *= player_speed;

	// Movement in seconds
	game_state->PlayerPosition.X += player_momentum.X * input->DeltaTime;
	game_state->PlayerPosition.Y += player_momentum.Y * input->DeltaTime;

	u32 tileMap[9][17] = {
		{1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1},
		{1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
		{0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0},
		{1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1}};

	Vector3 tile_colour;
	tile_colour.G = 0.5f;
	tile_colour.B = 0.5f;
	tile_colour.R = 0.5f;

	r32 tile_width = 100.0f;
	r32 tile_height = 100.0f;

	Vector2 min;
	Vector2 max;

	draw_rectangle(buffer, min, max, tile_colour);

	for (i32 row = 0; row < 9; row++)
	{
		for (i32 column = 0; column < 17; column++)
		{
			u32 tileId = tileMap[row][column];

			if (tileId == 1)
			{
				tile_colour.G = 1;
				tile_colour.B = 1;
				tile_colour.R = 1;
			}
			else
			{
				tile_colour.G = 0.5f;
				tile_colour.B = 0.5f;
				tile_colour.R = 0.5f;
			}

			min.X = column * tile_width;
			min.Y = row * tile_height;

			max.X = min.X + tile_width;
			max.Y = min.Y + tile_height;

			draw_rectangle(buffer, min, max, tile_colour);
		}

		u32 player_width = tile_width * 0.75f;
		u32 player_height = tile_height;

		min.X = game_state->PlayerPosition.X - (0.5f * player_width);
		min.Y = game_state->PlayerPosition.Y - player_height;

		max.X = min.X + player_width;
		max.Y = min.Y + player_height;

		tile_colour.G = 0.0f;
		tile_colour.B = 0.0f;
		tile_colour.R = 0.0f;

		draw_rectangle(buffer, min, max, tile_colour);
	}
}
