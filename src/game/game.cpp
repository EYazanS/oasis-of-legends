#include "stdio.h"

#include <catalyst/structs.h>
#include <catalyst/typesdef.h>
#include <catalyst/intrinsics.h>
#include <catalyst/math.h>
#include <catalyst/functions.h>

GameSprite debug_load_Bmp(platform_read_entire_file *read_file, char *file_name)
{
	GameSprite result = {};

	DebugReadFileResult bmp = read_file(file_name);

	if (bmp.ContentsSize != 0)
	{
		BitmapHeader *header = (BitmapHeader *)bmp.Memory;

		u32 *pixels = (u32 *)((u8 *)bmp.Memory + header->BitmapOffset);

		u32 *source = pixels;

		BitScanResult alpha_scan = find_least_sigifigant_set_bit(header->AlphaMask);
		BitScanResult red_scan = find_least_sigifigant_set_bit(header->RedMask);
		BitScanResult green_scan = find_least_sigifigant_set_bit(header->GreeenMask);
		BitScanResult blue_scan = find_least_sigifigant_set_bit(header->BlueMask);

		i32 alpha_shift = 24 - (i32)alpha_scan.Index;
		i32 red_shift = 16 - (i32)red_scan.Index;
		i32 green_shift = 8 - (i32)green_scan.Index;
		i32 blue_shift = 0 - (i32)blue_scan.Index;

		for (i32 y = 0; y < header->Height; y++)
		{
			for (i32 x = 0; x < header->Width; x++)
			{
				u32 pixel = *source;

				u32 alpha = rotate_left(pixel & header->AlphaMask, alpha_shift);
				u32 red = rotate_left(pixel & header->RedMask, red_shift);
				u32 green = rotate_left(pixel & header->GreeenMask, green_shift);
				u32 blue = rotate_left(pixel & header->BlueMask, blue_shift);

				u32 combined_color = alpha | red | green | blue;

				*source++ = combined_color;
			}
		}

		result.Data = pixels;
		result.Width = header->Width;
		result.Height = header->Height;
	}

	return result;
}

void draw_bitmap(ScreenBuffer *buffer, GameSprite *sprite, r32 real_min_x, r32 real_min_y, r32 alpha)
{
	i64 minX = round_real32_to_int32(real_min_x);
	i64 minY = round_real32_to_int32(real_min_y);
	i64 maxX = minX + sprite->Width;
	i64 maxY = minY + sprite->Height;

	// Clip to the nearest valid pixel
	i64 sourceOffsetX = 0;

	if (minX < 0)
	{
		sourceOffsetX = -minX;
		minX = 0;
	}

	i64 sourceOffsetY = 0;

	if (minY < 0)
	{
		sourceOffsetY = -minY - 1;
		minY = 0;
	}

	if (maxX > buffer->Width)
	{
		maxX = buffer->Width;
	}

	if (maxY > buffer->Height)
	{
		maxY = buffer->Height;
	}

	u32 *sourceRow = sprite->Data + sprite->Width * (sprite->Height - 1);
	sourceRow += (-sourceOffsetY * sprite->Width) + sourceOffsetX;

	u8 *destRow = (((u8 *)buffer->Memory) + minX * buffer->BytesPerPixel + minY * buffer->Pitch);

	for (i64 y = minY; y < maxY; y++)
	{

		u32 *dest = (u32 *)destRow;
		u32 *source = sourceRow;

		for (i64 x = minX; x < maxX; x++)
		{
			// Enabling this cause game to crash becasue too much calculation for the cpu on 60fps
#if 1
			// Linear blend
			r32 a = (r32)((*source >> 24) & 0xFF) / 255.0f;
			a *= alpha;

			r32 sr = (r32)((*source >> 16) & 0xFF);
			r32 sg = (r32)((*source >> 8) & 0xFF);
			r32 sb = (r32)((*source >> 0) & 0xFF);

			r32 dr = (r32)((*dest >> 16) & 0xFF);
			r32 dg = (r32)((*dest >> 8) & 0xFF);
			r32 db = (r32)((*dest >> 0) & 0xFF);

			r32 r = (1.0f - a) * dr + a * sr;
			r32 g = (1.0f - a) * dg + a * sg;
			r32 b = (1.0f - a) * db + a * sb;

			u32 result = (((u32)(r + 0.5f) << 16) | ((u32)(g + 0.5f) << 8) | ((u32)(b + 0.5f)) << 0);

			*dest = result;
#else
			// Alpha Test
			if (*source >> 24 > 124)
			{
				*dest = *source;
			}
#endif // 0

			dest++;
			source++;
		}

		destRow += buffer->Pitch;
		sourceRow -= sprite->Width;
	}
}

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

		game_state->Background = debug_load_Bmp(memory->ReadFile, "assets/test/test_background.bmp");

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
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

	Vector3 tile_colour;
	tile_colour.G = 0.5f;
	tile_colour.B = 0.5f;
	tile_colour.R = 0.5f;

	r32 tile_width = 100.0f;
	r32 tile_height = 100.0f;

	Vector2 min;
	Vector2 max;

	draw_rectangle(buffer, min, max, tile_colour);

	draw_bitmap(buffer, &game_state->Background, 0, 0, 1);

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

				min.X = column * tile_width;
				min.Y = row * tile_height;

				max.X = min.X + tile_width;
				max.Y = min.Y + tile_height;

				draw_rectangle(buffer, min, max, tile_colour);
			}
		}
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
