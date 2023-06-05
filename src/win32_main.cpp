/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

// Using SDL and standard IO
#include <Windows.h>
#include <SDL.h>
#include <stdio.h>

#include "typesdef.h"
#include "memory.h"
#include "functions.h"

#include "win32_structs.h"

win32_program_state state = {};

FILETIME get_file_last_write_date(const char *file_name)
{
	WIN32_FILE_ATTRIBUTE_DATA result = {};

	FILETIME last_write_time = {};

	if (GetFileAttributesExA(file_name, GetFileExInfoStandard, &result))
	{
		last_write_time = result.ftLastWriteTime;
	}

	return last_write_time;
}

// Game
game_code win32_load_game_code(char *source_dll_name, char *temp_dll_name, char *lock_file_name)
{
	game_code result = {};

	WIN32_FILE_ATTRIBUTE_DATA Ignored;

	if (!GetFileAttributesExA(lock_file_name, GetFileExInfoStandard, &Ignored))
	{
		result.LastWriteTime = get_file_last_write_date(source_dll_name);

		CopyFileA(source_dll_name, temp_dll_name, FALSE);

		result.LibraryHandle = LoadLibraryA(temp_dll_name);

		if (result.LibraryHandle)
		{
			result.UpdateAndRender = (GAMEUPDATEANDRENDER *)GetProcAddress(result.LibraryHandle, "update_and_render");
			result.IsValid = !!result.UpdateAndRender;
		}
	}

	if (!result.IsValid)
	{
		result.UpdateAndRender = 0;
	}

	return result;
}

// Windows entry point
int WINAPI wWinMain(
	_In_ HINSTANCE instance,
	_In_opt_ HINSTANCE prevInstance,
	_In_ LPWSTR cmdLine,
	_In_ int showCmd)
{
	game_code game = win32_load_game_code("game.dll", "game_temp.dll", "lock.tmp");

	state.WindowHeight = 480;
	state.WindowWidth = 640;
	state.IsRunning = true;

	// The window we'll be rendering to
	SDL_Window *window = NULL;

	// The surface contained by the window
	SDL_Surface *screenSurface = NULL;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());

		return -1;
	}

	// Create window
	window = SDL_CreateWindow("Oasis of legends", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, state.WindowWidth, state.WindowHeight, SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());

		SDL_Quit();

		return -1;
	}

	// Get window surface
	screenSurface = SDL_GetWindowSurface(window);

	// Fill the surface white
	SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

	// Update the surface
	SDL_UpdateWindowSurface(window);

	// Event handler
	SDL_Event sdl_event;

	while (state.IsRunning)
	{
		// Handle events on queue
		while (SDL_PollEvent(&sdl_event) != 0)
		{
			// User requests quit
			if (sdl_event.type == SDL_QUIT)
			{
				state.IsRunning = false;
			}
		}
	}

	// Destroy window
	SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}