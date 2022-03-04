// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include "sdl2_tic_tac_toe/sdl2_win.h"

typedef struct SDL_WINDOW_INFO_TAG
{
    SDL_Window* game_win;
} SDL_WINDOW_INFO;


int initialize_system(void)
{
    int result;
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Could not initialize SDL2: %s\n", SDL_GetError());
        result = __LINE__;
    }
    else
    {
        result = 0;
    }
    return result;
}

void deinitialize_system(void)
{
    SDL_Quit();
}

SDL_WIN_HANDLE create_window(const char* title, int left, int top, int width, int height, uint32_t flag)
{
    SDL_WINDOW_INFO* result = (SDL_WINDOW_INFO*)malloc(sizeof(SDL_WINDOW_INFO));
    if (result == NULL)
    {
        printf("Failure allocating item map item");
    }
    else
    {
        result->game_win = SDL_CreateWindow(title, left, top,width, height, flag);
        if (result->game_win == NULL)
        {
            printf("SDL_CreateWindow failure: %s\n", SDL_GetError());
            free(result);
            result = NULL;
        }
    }
    return result;
}

void destroy_window(SDL_WIN_HANDLE handle)
{
    if (handle != NULL)
    {
        SDL_DestroyWindow(handle->game_win);
        free(handle);
    }
}

SDL_Window* get_game_window(SDL_WIN_HANDLE handle)
{
    SDL_Window* result;
    if (handle == NULL)
    {
        result = NULL;
    }
    else
    {
        result = handle->game_win;
    }
    return result;
}