/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

// Using SDL and standard IO
#include <Windows.h>
#include <SDL.h>
#include <stdio.h>

#include "typesdef.h"
#include "memory.h"
#include "functions.h"

// Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

typedef void (*update_and_render)(int);
update_and_render UpdateAndRender;

// Windows entry point
int main(int argc, char *args[])
{
	// The window we'll be rendering to
	SDL_Window *window = NULL;

	// The surface contained by the window
	SDL_Surface *screenSurface = NULL;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		// Create window
		window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			// Get window surface
			screenSurface = SDL_GetWindowSurface(window);

			// Fill the surface white
			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

			// Update the surface
			SDL_UpdateWindowSurface(window);

			HMODULE dllHandle = LoadLibraryA("game.dll"); // Load the DLL

			if (dllHandle != NULL)
			{
				UpdateAndRender = (update_and_render)GetProcAddress(dllHandle, "update_and_render"); // Get the function pointer

				if (UpdateAndRender != NULL)
				{
					// Call the function from the DLL
					UpdateAndRender(42);
				}
				else
				{
					printf("Failed to get function pointer.\n");
				}

				// Cleanup: Release the DLL handle
				FreeLibrary(dllHandle);
			}
			else
			{
				// Get the last error code
				DWORD errorCode = GetLastError();

				// Get the error message associated with the last error code
				LPSTR errorMessage = NULL;
				FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
							   NULL,
							   errorCode,
							   0,
							   (LPSTR)&errorMessage,
							   0,
							   NULL);

				printf("Failed to load DLL. Error: %s\n", errorMessage);

				// Cleanup: Release the error message buffer
				LocalFree(errorMessage);
			}

			// Wait two seconds
			SDL_Delay(2000);
		}
	}

	// Destroy window
	SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}