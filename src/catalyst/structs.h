#ifndef catalyst_structs_h
#define catalyst_structs_h

#include "typesdef.h"

#if catalyst_internal
/* IMPORTANT:
   These are NOT for doing anything in the shipping game - they are
   blocking and the write doesn't protect against lost data!
*/
struct debug_read_file_result
{
	u32 ContentsSize;
	void *Contents;
};

debug_read_file_result debug_platform_read_entire_file(char *file_name);
void debug_platform_free_file_memory(void *memory);
b32 debug_platform_write_entire_file(char *file_name, u32 MemorySize, void *memory);

#define Debug_Platform_Free_File_Memory(name) void name(void *memory)
typedef Debug_Platform_Free_File_Memory(platform_free_file_memory);

#define Debug_Platform_Read_Entire_File(name) debug_read_file_result name(char *file_name)
typedef Debug_Platform_Read_Entire_File(platform_read_entire_file);

#define Debug_Platform_Write_Entire_File(name) b32 name(char *fileName, u32 memorySize, void *memory)
typedef Debug_Platform_Write_Entire_File(platform_write_entire_file);
#endif

struct screen_buffer
{
	void *Memory;
	int Width;
	int Height;
	int Pitch;
	int BytesPerPixel;
};

struct game_memory
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

struct game_button_state
{
	i32 HalfTransitionCount;
	b32 EndedDown;
};

struct game_controller_input
{
	b32 IsConnected;
	b32 IsAnalog;
	r32 StickAverageX;
	r32 StickAverageY;

	union
	{
		game_button_state Buttons[12];

		struct
		{
			game_button_state MoveUp;
			game_button_state MoveDown;
			game_button_state MoveLeft;
			game_button_state MoveRight;

			game_button_state ActionUp;
			game_button_state ActionDown;
			game_button_state ActionLeft;
			game_button_state ActionRight;

			game_button_state LeftShoulder;
			game_button_state RightShoulder;

			game_button_state Back;
			game_button_state Start;

			// NOTE: All buttons must be added above this line
			game_button_state Terminator;
		};
	};
};

struct game_input
{
	r64 TimeToAdvance;

	game_controller_input Controllers[5];
};

#endif