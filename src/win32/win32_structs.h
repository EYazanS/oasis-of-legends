#ifndef win32_structs_h
#define win32_structs_h

#include <Windows.h>

#include <catalyst/typesdef.h>
#include <catalyst/structs.h>

#define GAME_UPDATE_AND_RENDER(name) void name(GameMemory *memory, GameInput *input, ScreenBuffer *buffer)
typedef GAME_UPDATE_AND_RENDER(GAMEUPDATEANDRENDER);
GAME_UPDATE_AND_RENDER(GameUpdatAndRendereStub){};

struct GameCode
{
	HMODULE LibraryHandle;
	GAMEUPDATEANDRENDER *UpdateAndRender;
	b32 IsValid;
	FILETIME LastWriteTime;
};

struct Win32BitmapBuffer
{
	BITMAPINFO Info;
	void* Memory;
	int BytesPerPixel;
	int Width;
	int Height;
	int Pitch;
};

struct Win32ProgramState
{
	b32 IsRunning;
	b32 ShowCursor;
	b32 IsFullScreen;
	i32 WindowWidth;
	i32 WindowHeight;
	Win32BitmapBuffer BitmapBuffer;
	WINDOWPLACEMENT PrevWP;

	i64 PerformanceFrequence;

	char ExeFileName[MAX_PATH];
	char* OnePastLastEXEFileNameSlash;
};
#endif