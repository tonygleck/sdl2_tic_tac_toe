// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#include <cstdint>
#else
#include <stdint.h>
#include <stddef.h>
#endif /* __cplusplus */

#include "sdl2_tic_tac_toe/sdl2_win.h"
#include <SDL2/SDL.h>

typedef struct SDL_WINDOW_INFO_TAG* SDL_WIN_HANDLE;

typedef void(*PROCESS_WIN_EVENT_FUNC)(void* user_ctx, SDL_Event* event, void* data);
typedef void(*RENDER_WIN_FUNC)(void* user_ctx);


extern int initialize_system(void);
extern void deinitialize_system(void);

extern SDL_WIN_HANDLE create_window(const char* title, int left, int top, int width, int height, uint32_t flag);
extern void destroy_window(SDL_WIN_HANDLE handle);

extern SDL_Window* get_game_window(SDL_WIN_HANDLE handle);
