#ifndef win32_structs_h
#define win32_structs_h

#include <Windows.h>

#include <catalyst/typesdef.h>
#include <catalyst/structs.h>

#define GAME_UPDATE_AND_RENDER(name) void name(game_memory *memory, screen_buffer *buffer)
typedef GAME_UPDATE_AND_RENDER(GAMEUPDATEANDRENDER);
GAME_UPDATE_AND_RENDER(GameUpdatAndRendereStub){};

struct game_code
{
	HMODULE LibraryHandle;
	GAMEUPDATEANDRENDER *UpdateAndRender;
	b32 IsValid;
	FILETIME LastWriteTime;
};

struct win32_bitmap_buffer
{
	BITMAPINFO Info;
	void* Memory;
	int BytesPerPixel;
	int Width;
	int Height;
	int Pitch;
};

struct win32_program_state
{
	b32 IsRunning;
	b32 ShowCursor;
	b32 IsFullScreen;
	i32 WindowWidth;
	i32 WindowHeight;
	win32_bitmap_buffer BitmapBuffer;
	WINDOWPLACEMENT PrevWP;

	i64 PerformanceFrequence;

	char ExeFileName[MAX_PATH];
	char* OnePastLastEXEFileNameSlash;
};
#endif