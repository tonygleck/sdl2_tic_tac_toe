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

typedef struct RENDERER_INFO_TAG* RENDERER_INFO_HANDLE;

extern RENDERER_INFO_HANDLE create_renderer(SDL_WIN_HANDLE sdl_win, const char* exec_path);
extern void destroy_renderer(RENDERER_INFO_HANDLE handle);

extern void render_draw_fill_rect(RENDERER_INFO_HANDLE handle, int x, int y, int width, int height);
extern void render_draw_line(RENDERER_INFO_HANDLE handle, int pos_x, int pos_y, int width, int length);
extern void render_draw_thick_line(RENDERER_INFO_HANDLE handle, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t width, const SDL_Color* color);
extern void render_draw_filled_circle(RENDERER_INFO_HANDLE handle, int16_t x, int16_t y, int16_t rad, const SDL_Color* color);
extern void render_set_draw_color(RENDERER_INFO_HANDLE handle, const SDL_Color* color);
extern void render_draw_bg_text(RENDERER_INFO_HANDLE handle, const char* text, const SDL_Color* font_color, const SDL_Color* bg_color, int x, int y, int width, int height);
extern void render_draw_text(RENDERER_INFO_HANDLE handle, const char* text, const SDL_Color* font_color, int x, int y, int width, int height);
extern void render_clear(RENDERER_INFO_HANDLE handle);
extern void render_present(RENDERER_INFO_HANDLE handle);

extern void render_process_game_loop(RENDERER_INFO_HANDLE handle, RENDER_WIN_FUNC render_win, void* render_ctx, PROCESS_WIN_EVENT_FUNC win_event, void* user_ctx);
