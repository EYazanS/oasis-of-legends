#ifndef catalyst_structs_h
#define catalyst_structs_h

#include "typesdef.h"

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