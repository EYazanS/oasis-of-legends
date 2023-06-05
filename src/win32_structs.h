#ifndef win32_structs_h
#define win32_structs_h

#define GAME_UPDATE_AND_RENDER(name) void name()
typedef GAME_UPDATE_AND_RENDER(GAMEUPDATEANDRENDER);
GAME_UPDATE_AND_RENDER(GameUpdatAndRendereStub){};

#include <Windows.h>
#include <typesdef.h>

struct game_code
{
	HMODULE LibraryHandle;
	GAMEUPDATEANDRENDER *UpdateAndRender;
	b32 IsValid;
	FILETIME LastWriteTime;
};

struct win32_program_state
{
	b32 IsRunning;
	b32 ShowCursor;
	b32 IsFullScreen;
	i32 WindowWidth;
	i32 WindowHeight;
	WINDOWPLACEMENT PrevWP;

	i64 PerformanceFrequence;

	char ExeFileName[MAX_PATH];
	char* OnePastLastEXEFileNameSlash;
};
#endif