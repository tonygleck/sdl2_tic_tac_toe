// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stddef.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "sdl2_tic_tac_toe/sdl2_renderer.h"
#include "sdl2_tic_tac_toe/sdl2_win.h"

typedef struct RENDERER_INFO_TAG
{
    SDL_Renderer* renderer;
    SDL_WIN_HANDLE sdl_win;
} RENDERER_INFO;

RENDERER_INFO_HANDLE create_renderer(SDL_WIN_HANDLE sdl_win)
{
    RENDERER_INFO* result = (RENDERER_INFO*)malloc(sizeof(RENDERER_INFO));
    if (result == NULL)
    {
        printf("Failure allocating item map item");
    }
    else
    {
        result->renderer = SDL_CreateRenderer(get_game_window(sdl_win), -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (result->renderer == NULL)
        {
            printf("SDL_CreateRenderer failure: %s\n", SDL_GetError());
            free(result);
            result = NULL;
        }
        else
        {
            result->sdl_win = sdl_win;
        }
    }
    return result;
}

void destroy_renderer(RENDERER_INFO_HANDLE handle)
{
    if (handle != NULL)
    {
        free(handle);
    }

}

void render_draw_fill_rect(RENDERER_INFO_HANDLE handle, int x, int y, int width, int height)
{
    if (handle == NULL)
    {
        printf("Invalid parameter %p\n", handle);
    }
    else
    {
        SDL_Rect rect = { .x = x, .y = y, .w = width, .h = height };
        SDL_RenderFillRect(handle->renderer, &rect);
    }
}

void render_draw_line(RENDERER_INFO_HANDLE handle, int pos_x, int pos_y, int width, int length)
{
    if (handle == NULL)
    {
        printf("Invalid parameter %p\n", handle);
    }
    else
    {
        SDL_RenderDrawLine(handle->renderer, pos_x, pos_y, width, length);
    }
}

void render_draw_thick_line(RENDERER_INFO_HANDLE handle, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t width, const SDL_Color* color)
{
    if (handle == NULL)
    {
        printf("Invalid parameter %p\n", handle);
    }
    else
    {
        thickLineRGBA(handle->renderer, x1, y1, x2, y2, width, color->r, color->g, color->b, 255);
    }
}

void render_draw_filled_circle(RENDERER_INFO_HANDLE handle, int16_t x, int16_t y, int16_t rad, const SDL_Color* color)
{
    if (handle == NULL)
    {
        printf("Invalid parameter %p\n", handle);
    }
    else
    {
        filledCircleRGBA(handle->renderer, x, y, rad, color->r, color->g, color->b, 255);
    }
}

void render_set_draw_color(RENDERER_INFO_HANDLE handle, const SDL_Color* color)
{
    if (handle == NULL)
    {
        printf("Invalid parameter %p\n", handle);
    }
    else
    {
        SDL_SetRenderDrawColor(handle->renderer, color->r, color->g, color->b, color->a);
    }
}

void render_clear(RENDERER_INFO_HANDLE handle)
{
    if (handle == NULL)
    {
        printf("Invalid parameter %p\n", handle);
    }
    else
    {
        SDL_RenderClear(handle->renderer);
    }

}

void render_present(RENDERER_INFO_HANDLE handle)
{
    if (handle == NULL)
    {
        printf("Invalid parameter %p\n", handle);
    }
    else
    {
        SDL_RenderPresent(handle->renderer);
    }
}

void render_process_game_loop(RENDERER_INFO_HANDLE handle, RENDER_WIN_FUNC render_win, void* render_ctx, PROCESS_WIN_EVENT_FUNC win_event, void* user_ctx)
{
    if (handle != NULL)
    {
        SDL_Event event;
        int quit = 0;

        while (!quit)
        {
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_QUIT:
                        quit = 1;
                        break;
                    default:
                        if (win_event != NULL)
                        {
                            win_event(user_ctx, &event, &event.button);
                        }
                        break;
                }
            }
            SDL_SetRenderDrawColor(handle->renderer, 0, 0, 0, 255);
            SDL_RenderClear(handle->renderer);
            if (render_win != NULL)
            {
                render_win(render_ctx);
            }
            SDL_RenderPresent(handle->renderer);
        }
    }
}
