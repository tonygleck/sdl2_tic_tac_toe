// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdint.h>
#include <stddef.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include "sdl2_tic_tac_toe/sdl2_renderer.h"
#include "sdl2_tic_tac_toe/sdl2_win.h"

typedef struct RENDERER_INFO_TAG
{
    SDL_Renderer* renderer;
    SDL_WIN_HANDLE sdl_win;
    TTF_Font* font;
} RENDERER_INFO;

static const char* FONT_FILENAME = "./res/new_roman.ttf";

static int load_font(RENDERER_INFO* render_info)
{
    int result;
    render_info->font = TTF_OpenFont(FONT_FILENAME, 24);
    if (render_info->font == NULL)
    {
        result = __LINE__;
    }
    else
    {
        result = 0;
    }
    return result;
}

RENDERER_INFO_HANDLE create_renderer(SDL_WIN_HANDLE sdl_win, const char* exec_path)
{
    (void)exec_path;
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
            load_font(result);
            result->sdl_win = sdl_win;
        }
    }
    return result;
}

void destroy_renderer(RENDERER_INFO_HANDLE handle)
{
    if (handle != NULL)
    {
        TTF_CloseFont(handle->font);
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

void render_draw_bg_text(RENDERER_INFO_HANDLE handle, const char* text, const SDL_Color* font_color, const SDL_Color* bg_color, int x, int y, int width, int height)
{
    if (handle == NULL)
    {
        printf("Invalid parameter %p\n", handle);
    }
    else
    {
        SDL_Surface* text_surface = TTF_RenderText_Shaded(handle->font, text, *font_color, *bg_color);
        if (text_surface == NULL)
        {

        }
        else
        {
            // Create the texture
            SDL_Texture* texture_msg = SDL_CreateTextureFromSurface(handle->renderer, text_surface);
            SDL_Rect rect = { .x = x, .y = y, .w = width, .h = height };

            SDL_RenderCopy(handle->renderer, texture_msg, NULL, &rect);
            SDL_FreeSurface(text_surface);
            SDL_DestroyTexture(texture_msg);
        }
    }
}

void render_draw_text(RENDERER_INFO_HANDLE handle, const char* text, const SDL_Color* font_color, int x, int y, int width, int height)
{
    if (handle == NULL)
    {
        printf("Invalid parameter %p\n", handle);
    }
    else
    {
        SDL_Surface* text_surface = TTF_RenderText_Blended(handle->font, text, *font_color);
        if (text_surface == NULL)
        {

        }
        else
        {
            // Create the texture
            SDL_Texture* texture_msg = SDL_CreateTextureFromSurface(handle->renderer, text_surface);
            SDL_Rect src_rc = { .x = 0, .y = 0, .w = text_surface->w, .h = text_surface->h };
            SDL_Rect rect = { .x = x, .y = y, .w = width, .h = height };

            SDL_RenderCopy(handle->renderer, texture_msg, &src_rc, &rect);
            SDL_FreeSurface(text_surface);
            SDL_DestroyTexture(texture_msg);
        }
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
            //SDL_Delay(10);
        }
    }
}
