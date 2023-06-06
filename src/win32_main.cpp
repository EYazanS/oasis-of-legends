/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

// Using SDL and standard IO
#include <Windows.h>
#include <SDL.h>
#include <stdio.h>

#include "typesdef.h"
#include "memory.h"
#include "functions.h"
#include "wind32_string.h"
#include "win32_structs.h"
#include "game_structs.h"

win32_program_state state = {};

void win32_build_exe_path_file_name(win32_program_state *state, char *file_name, int dest_count, char *dest)
{
	con_cat_strings(state->OnePastLastEXEFileNameSlash - state->ExeFileName, state->ExeFileName,
					str_length(file_name), file_name,
					dest_count, dest);
}

void win32_get_exe_file_name(win32_program_state *State)
{
	DWORD size_of_file_name = GetModuleFileNameA(0, State->ExeFileName, sizeof(State->ExeFileName));

	State->OnePastLastEXEFileNameSlash = State->ExeFileName;

	for (char *scan = State->ExeFileName; *scan; ++scan)
	{
		if (*scan == '\\')
		{
			State->OnePastLastEXEFileNameSlash = scan + 1;
		}
	}
}

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

void win32_unload_game_code(game_code *code)
{
	if (code->LibraryHandle)
	{
		FreeLibrary(code->LibraryHandle);
	}

	code->IsValid = false;
	code->UpdateAndRender = GameUpdatAndRendereStub;
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

game_memory init_game_memory()
{
	game_memory game_memory = {};

#if catalyst_internal
	LPVOID base_address = (LPVOID)TERABYTES(2);
#else
	LPVOID base_address = 0;
#endif

	game_memory.PermanentStorageSize = MEGABYTES(256);
	game_memory.TransiateStorageSize = GIGABYTES(1);

	size_t total_size = game_memory.PermanentStorageSize + game_memory.TransiateStorageSize;

	game_memory.PermanentStorage = VirtualAlloc(base_address, total_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	game_memory.TransiateStorage = (u8 *)game_memory.PermanentStorage + game_memory.PermanentStorageSize;

	return game_memory;
}

void win32_resize_dib_section(win32_bitmap_buffer *bitmap_buffer, i32 width, i32 height)
{
	if (bitmap_buffer->Memory)
		VirtualFree(bitmap_buffer->Memory, NULL, MEM_RELEASE);

	bitmap_buffer->Width = width;
	bitmap_buffer->Height = height;
	bitmap_buffer->BytesPerPixel = 4;
	bitmap_buffer->Pitch = bitmap_buffer->Width * bitmap_buffer->BytesPerPixel;

	// If the height is negative, it goes top down, instead of down up
	bitmap_buffer->Info.bmiHeader.biSize = sizeof(bitmap_buffer->Info.bmiHeader);
	bitmap_buffer->Info.bmiHeader.biWidth = bitmap_buffer->Width;
	bitmap_buffer->Info.bmiHeader.biHeight = -bitmap_buffer->Height;
	bitmap_buffer->Info.bmiHeader.biPlanes = 1;
	bitmap_buffer->Info.bmiHeader.biBitCount = 32;
	bitmap_buffer->Info.bmiHeader.biCompression = BI_RGB;

	int bitmap_memory_size = bitmap_buffer->Width * bitmap_buffer->Height * bitmap_buffer->BytesPerPixel;

	bitmap_buffer->Memory = VirtualAlloc(0, bitmap_memory_size, MEM_COMMIT, PAGE_READWRITE);
}

// Windows entry point
int WINAPI wWinMain(
	_In_ HINSTANCE instance,
	_In_opt_ HINSTANCE prevInstance,
	_In_ LPWSTR cmdLine,
	_In_ int showCmd)
{
	win32_get_exe_file_name(&state);

	char source_game_code_dll_full_path[MAX_PATH];

	win32_build_exe_path_file_name(&state, "game.dll", sizeof(source_game_code_dll_full_path), source_game_code_dll_full_path);

	char temp_game_code_dll_full_path[MAX_PATH];

	win32_build_exe_path_file_name(&state, "game_temp.dll", sizeof(temp_game_code_dll_full_path), temp_game_code_dll_full_path);

	char game_code_lock_full_path[MAX_PATH];
	win32_build_exe_path_file_name(&state, "lock.tmp", sizeof(game_code_lock_full_path), game_code_lock_full_path);

	game_code game = win32_load_game_code(source_game_code_dll_full_path, temp_game_code_dll_full_path, game_code_lock_full_path);

	game_memory g_memory = init_game_memory();

	state.WindowHeight = 1080;
	state.WindowWidth = 1920;

	win32_resize_dib_section(&state.BitmapBuffer, state.WindowWidth, state.WindowHeight);

	// The window we'll be rendering to
	SDL_Window *window = NULL;

	// The surface contained by the window
	SDL_Surface *screen_surface = NULL;

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

	// Create a "renderer" for our window.
	SDL_Renderer *renderer = SDL_CreateRenderer(window,
												-1,
												SDL_RENDERER_PRESENTVSYNC);

	// Get window surface
	screen_surface = SDL_GetWindowSurface(window);

	// Fill the surface white
	SDL_FillRect(screen_surface, NULL, SDL_MapRGB(screen_surface->format, 0xFF, 0xFF, 0xFF));

	// Update the surface
	SDL_UpdateWindowSurface(window);

	// Event handler
	SDL_Event sdl_event;

	state.IsRunning = true;

	while (state.IsRunning)
	{
		FILETIME new_last_write_time = get_file_last_write_date(source_game_code_dll_full_path);

		if (CompareFileTime(&new_last_write_time, &game.LastWriteTime) != 0)
		{
			win32_unload_game_code(&game);
			game = win32_load_game_code(source_game_code_dll_full_path, temp_game_code_dll_full_path, game_code_lock_full_path);
			game.LastWriteTime = new_last_write_time;
		}

		// Handle events on queue
		while (SDL_PollEvent(&sdl_event) != 0)
		{
			switch (sdl_event.type)
			{
			case SDL_QUIT:
				// User requests quit
				state.IsRunning = false;
				break;

			default:

				break;
			}
		}

		screen_buffer screen_buf = {};

		screen_buf.Memory = state.BitmapBuffer.Memory;
		screen_buf.Width = state.BitmapBuffer.Width;
		screen_buf.Height = state.BitmapBuffer.Height;
		screen_buf.Pitch = state.BitmapBuffer.Pitch;
		screen_buf.BytesPerPixel = state.BitmapBuffer.BytesPerPixel;

		game.UpdateAndRender(&g_memory, &screen_buf);
	}

	// Destroy window
	SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}