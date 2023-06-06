/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

// Using SDL and standard IO
#include <Windows.h>
#include <SDL.h>
#include <stdio.h>

#include <catalyst/strings.h>
#include <catalyst/memory.h>
#include <catalyst/structs.h>
#include <catalyst/typesdef.h>
#include <catalyst/functions.h>

#include "win32_structs.h"

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

void win32_resize_device_independent_bitmap_section(win32_bitmap_buffer *bitmap_buffer, i32 width, i32 height)
{
	if (bitmap_buffer->Memory)
	{
		VirtualFree(bitmap_buffer->Memory, NULL, MEM_RELEASE);
	}

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

void render_screen_buffer(struct screen_buffer *buffer, SDL_Texture *texture, SDL_Renderer *renderer)
{
	SDL_UpdateTexture(texture, NULL, buffer->Memory, buffer->Pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

u32 get_screen_refresh_rate()
{
	SDL_DisplayMode mode;

	if (SDL_GetCurrentDisplayMode(0, &mode) == 0)
	{
		return mode.refresh_rate;
	}

	// Failed to get the screen refresh rate, fallback to a default value
	return 60; // Default to 60 Hz
}

inline i64 win32_get_performance_frequence()
{
	LARGE_INTEGER performance_frequence_result;
	QueryPerformanceFrequency(&performance_frequence_result);
	return performance_frequence_result.QuadPart;
}

inline i64 win32_get_wall_clock()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}

inline r32 get_seconds_elapsed(u64 start, u64 end, u64 performanceFrequence)
{
	return (r32)(end - start) / (r32)performanceFrequence;
}

void SDLProcessKeyPress(game_button_state *new_state, b32 Is_down)
{
	new_state->EndedDown = Is_down;
	++new_state->HalfTransitionCount;
}

game_controller_input *get_controller(game_input *input, memory_index controller_index)
{
	game_controller_input *result = &input->Controllers[controller_index];

	return (result);
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

	game_memory game_memory = init_game_memory();

	state.PerformanceFrequence = win32_get_performance_frequence();

	state.WindowHeight = 1080;
	state.WindowWidth = 1920;

	win32_resize_device_independent_bitmap_section(&state.BitmapBuffer, state.WindowWidth, state.WindowHeight);

	// The window we'll be rendering to
	SDL_Window *window = NULL;

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

	SDL_Texture *texture = SDL_CreateTexture(renderer,
											 SDL_PIXELFORMAT_ARGB8888,
											 SDL_TEXTUREACCESS_STREAMING,
											 state.BitmapBuffer.Width,
											 state.BitmapBuffer.Height);

	// Update the surface
	SDL_UpdateWindowSurface(window);

	// Get the screen refresh rate
	u32 monitor_refresh_rate = get_screen_refresh_rate();

	u32 game_update_in_hz = monitor_refresh_rate / 2; // In HZ

	r64 target_seconds_to_advance_by = 1.0 / game_update_in_hz;

	// Get how many cycle the cpu went through
	u64 last_cycle_count = __rdtsc();
	// Get current cpu time
	i64 last_counter = win32_get_wall_clock();

	UINT desired_schedular_time_in_ms = 1;

	b32 is_sleep_granular = timeBeginPeriod(desired_schedular_time_in_ms) == TIMERR_NOERROR;

	// Event handler
	SDL_Event sdl_event;

	state.IsRunning = true;

	r64 time_to_advance;

	screen_buffer screen_bufffer = {};

	game_input inputs[2] = {};
	game_input *new_input = &inputs[0];
	game_input *old_input = &inputs[1];

	game_controller_input *new_keyboard_controller = get_controller(new_input, 0);

	while (state.IsRunning)
	{
		time_to_advance = target_seconds_to_advance_by;

		FILETIME new_last_write_time = get_file_last_write_date(source_game_code_dll_full_path);

		if (CompareFileTime(&new_last_write_time, &game.LastWriteTime) != 0)
		{
			win32_unload_game_code(&game);
			game = win32_load_game_code(source_game_code_dll_full_path, temp_game_code_dll_full_path, game_code_lock_full_path);
			game.LastWriteTime = new_last_write_time;
		}

		new_keyboard_controller = get_controller(new_input, 0);

		// Handle events on queue
		while (SDL_PollEvent(&sdl_event) != 0)
		{
			switch (sdl_event.type)
			{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			{
				SDL_Keycode KeyCode = sdl_event.key.keysym.sym;
				bool IsDown = (sdl_event.key.state == SDL_PRESSED);
				bool WasDown = false;
				if (sdl_event.key.state == SDL_RELEASED)
				{
					WasDown = true;
				}
				else if (sdl_event.key.repeat != 0)
				{
					WasDown = true;
				}

				// NOTE: In the windows version, we used "if (IsDown != WasDown)"
				// to detect key repeats. SDL has the 'repeat' value, though,
				// which we'll use.
				if (sdl_event.key.repeat == 0)
				{
					if (KeyCode == SDLK_w)
					{
						SDLProcessKeyPress(&new_keyboard_controller->MoveUp, IsDown);
					}
					else if (KeyCode == SDLK_a)
					{
						SDLProcessKeyPress(&new_keyboard_controller->MoveLeft, IsDown);
					}
					else if (KeyCode == SDLK_s)
					{
						SDLProcessKeyPress(&new_keyboard_controller->MoveDown, IsDown);
					}
					else if (KeyCode == SDLK_d)
					{
						SDLProcessKeyPress(&new_keyboard_controller->MoveRight, IsDown);
					}
					else if (KeyCode == SDLK_q)
					{
						SDLProcessKeyPress(&new_keyboard_controller->LeftShoulder, IsDown);
					}
					else if (KeyCode == SDLK_e)
					{
						SDLProcessKeyPress(&new_keyboard_controller->RightShoulder, IsDown);
					}
					else if (KeyCode == SDLK_UP)
					{
						SDLProcessKeyPress(&new_keyboard_controller->ActionUp, IsDown);
					}
					else if (KeyCode == SDLK_LEFT)
					{
						SDLProcessKeyPress(&new_keyboard_controller->ActionLeft, IsDown);
					}
					else if (KeyCode == SDLK_DOWN)
					{
						SDLProcessKeyPress(&new_keyboard_controller->ActionDown, IsDown);
					}
					else if (KeyCode == SDLK_RIGHT)
					{
						SDLProcessKeyPress(&new_keyboard_controller->ActionRight, IsDown);
					}
					else if (KeyCode == SDLK_ESCAPE)
					{
						printf("ESCAPE: ");

						if (IsDown)
						{
							printf("IsDown ");
						}
						if (WasDown)
						{
							printf("WasDown");
						}

						printf("\n");
					}
					else if (KeyCode == SDLK_SPACE)
					{
					}
					else if (KeyCode == SDLK_RETURN)
					{
						SDLProcessKeyPress(&new_keyboard_controller->Start, IsDown);
					}
				}

				b32 AltKeyWasDown = (sdl_event.key.keysym.mod & KMOD_ALT);

				if (KeyCode == SDLK_F4 && AltKeyWasDown)
				{
					state.IsRunning = false;
				}
			}
			break;

			case SDL_QUIT:
				// User requests quit
				state.IsRunning = false;
				break;

			default:

				break;
			}
		}

		screen_bufffer.Memory = state.BitmapBuffer.Memory;
		screen_bufffer.Width = state.BitmapBuffer.Width;
		screen_bufffer.Height = state.BitmapBuffer.Height;
		screen_bufffer.Pitch = state.BitmapBuffer.Pitch;
		screen_bufffer.BytesPerPixel = state.BitmapBuffer.BytesPerPixel;

		game.UpdateAndRender(&game_memory, new_input, &screen_bufffer);

		i64 workCounter = win32_get_wall_clock();

		r64 work_seconds_elapsed = get_seconds_elapsed(last_counter, workCounter, state.PerformanceFrequence);

		render_screen_buffer(&screen_bufffer, texture, renderer);

		r64 time_taken_on_frame = work_seconds_elapsed;

		// Make sure we stay at the target time for each frame.
		if (time_taken_on_frame < target_seconds_to_advance_by)
		{
			if (is_sleep_granular)
			{
				// We substract 2 ms from the sleep incase the os doesnt wake us on time
				DWORD sleep_ms = (DWORD)(1000.f * (target_seconds_to_advance_by - time_taken_on_frame)) - 2;

				if (sleep_ms > 0 && sleep_ms < 40)
					Sleep(sleep_ms);
			}

			r32 testTimeTakenOnFrame = get_seconds_elapsed(last_counter, win32_get_wall_clock(), state.PerformanceFrequence);

			// Assert(testTimeTakenOnFrame < targetSecondsPerFrams);

			while (time_taken_on_frame < target_seconds_to_advance_by)
			{
				time_taken_on_frame = get_seconds_elapsed(last_counter, win32_get_wall_clock(), state.PerformanceFrequence);
			}
		}
		else
		{
			// missed a frame
			printf("Missed a frame\n");
		}

		// Display performance counter
		i64 end_counter = win32_get_wall_clock();
		r32 ms_per_frame = 1000.0f * get_seconds_elapsed(last_counter, end_counter, state.PerformanceFrequence);
		last_counter = end_counter;

		// Register last counter we got
		r32 FPS = (1000.0f / ms_per_frame);

		printf("%.02fms/f, %.02fFPS, (%.02fws/f)\n", ms_per_frame, FPS, work_seconds_elapsed * 1000.0f);

		last_cycle_count = __rdtsc();
	}

	// Destroy window
	SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}