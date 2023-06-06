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


#endif