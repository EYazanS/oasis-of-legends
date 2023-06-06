#ifndef game_structs_h
#define game_structs_h

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
	void *PermanentStorage;
	void *TransiateStorage;
	b32 IsInitialized;
};

#endif