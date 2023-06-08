#ifndef catalyst_structs_h
#define catalyst_structs_h

#include "math.h"
#include "typesdef.h"

#if catalyst_internal
/* IMPORTANT:
   These are NOT for doing anything in the shipping game - they are
   blocking and the write doesn't protect against lost data!
*/
struct DebugReadFileResult
{
	u32 ContentsSize;
	void *Contents;
};

DebugReadFileResult debug_platform_read_entire_file(char *file_name);
void debug_platform_free_file_memory(void *memory);
b32 debug_platform_write_entire_file(char *file_name, u32 MemorySize, void *memory);

#define Debug_Platform_Free_File_Memory(name) void name(void *memory)
typedef Debug_Platform_Free_File_Memory(platform_free_file_memory);

#define Debug_Platform_Read_Entire_File(name) DebugReadFileResult name(char *file_name)
typedef Debug_Platform_Read_Entire_File(platform_read_entire_file);

#define Debug_Platform_Write_Entire_File(name) b32 name(char *fileName, u32 memorySize, void *memory)
typedef Debug_Platform_Write_Entire_File(platform_write_entire_file);
#endif

struct ScreenBuffer
{
	void *Memory;
	int Width;
	int Height;
	int Pitch;
	int BytesPerPixel;
};

struct GameMemory
{
	memory_index PermanentStorageSize;
	memory_index TransiateStorageSize;
	data_t PermanentStorage;
	data_t TransiateStorage;
	b32 IsInitialized;

	platform_free_file_memory *FreeFile;
	platform_read_entire_file *ReadFile;
	platform_write_entire_file *WriteFile;
};

struct GameButtonState
{
	i32 HalfTransitionCount;
	b32 EndedDown;
};

struct GameControllerInput
{
	b32 IsConnected;
	b32 IsAnalog;
	r32 StickAverageX;
	r32 StickAverageY;

	union
	{
		GameButtonState Buttons[12];

		struct
		{
			GameButtonState MoveUp;
			GameButtonState MoveDown;
			GameButtonState MoveLeft;
			GameButtonState MoveRight;

			GameButtonState ActionUp;
			GameButtonState ActionDown;
			GameButtonState ActionLeft;
			GameButtonState ActionRight;

			GameButtonState LeftShoulder;
			GameButtonState RightShoulder;

			GameButtonState Back;
			GameButtonState Start;

			// NOTE: All buttons must be added above this line
			GameButtonState Terminator;
		};
	};
};

struct GameInput
{
	r64 TimeToAdvance;

	GameControllerInput Controllers[5];
};

struct GameState
{
	Vector3 colour;
};

#endif