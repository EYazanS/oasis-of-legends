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

#endif