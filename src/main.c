// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include "sdl2_tic_tac_toe/sdl2_win.h"
#include "sdl2_tic_tac_toe/game_board.h"
#include "sdl2_tic_tac_toe/sdl2_renderer.h"

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

const SDL_Color GRID_COLOR = { .r = 255, .g = 255, .b = 255 };
const SDL_Color TIE_COLOR = { .r = 100, .g = 100, .b = 100 };

typedef struct TIC_TAC_TOE_INFO_TAG
{
    BOARD_INFO_HANDLE board;
    SDL_WIN_HANDLE game_win;
    RENDERER_INFO_HANDLE renderer_info;
} TIC_TAC_TOE_INFO;

static void render_game_win(void* user_ctx)
{
    TIC_TAC_TOE_INFO* tic_info = (TIC_TAC_TOE_INFO*)user_ctx;
    game_board_render(tic_info->board, &GRID_COLOR);
}

static void process_window_event(void* user_ctx, uint16_t event, void* data)
{
    TIC_TAC_TOE_INFO* tic_info = (TIC_TAC_TOE_INFO*)user_ctx;
    switch (event)
    {
        case SDL_MOUSEBUTTONUP:
        {
            SDL_MouseButtonEvent* mouse_event = (SDL_MouseButtonEvent*)data;
            POS_INFO pos = { .x = mouse_event->x, .y = mouse_event->y };
            game_board_click(tic_info->board, &pos);
            break;
        }
    }
}

static int create_game_window(TIC_TAC_TOE_INFO* tic_info)
{
    int result;
    tic_info->game_win = create_window("SDL Project", 10, 10,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (tic_info->game_win == NULL)
    {
        printf("SDL_CreateWindow failure: %s\n", SDL_GetError());
        result = EXIT_FAILURE;
    }
    else
    {
        if ((tic_info->renderer_info = create_renderer(tic_info->game_win)) == NULL)
        {
            printf("failure create renderer\n");
            result = EXIT_FAILURE;
            destroy_window(tic_info->game_win);
        }
        else
        {
            if ((tic_info->board = game_board_create(SCREEN_WIDTH, SCREEN_HEIGHT, tic_info->renderer_info)) == NULL)
            {
                printf("failure create game board\n");
                result = EXIT_FAILURE;
                destroy_renderer(tic_info->renderer_info);
                destroy_window(tic_info->game_win);
            }
            else
            {
                result = 0;
            }
        }
    }
    return result;
}

void destroy_game(TIC_TAC_TOE_INFO* tic_info)
{
    game_board_destroy(tic_info->board);

    destroy_renderer(tic_info->renderer_info);
    destroy_window(tic_info->game_win);
}

int main(int argc, char* argv[])
{
    int result;
    (void)argc;
    (void)argv;

    TIC_TAC_TOE_INFO tic_info = {0};

    if (initialize_system() != 0)
    {
        printf("Could not initialize SDL2: %s\n", SDL_GetError());
        result = EXIT_FAILURE;
    }
    else
    {
        if (create_game_window(&tic_info) != 0)
        {
            result = EXIT_FAILURE;
        }
        else
        {
            // Create the computer player if necessary
            //if (create_players)

            render_process_game_loop(tic_info.renderer_info, render_game_win, &tic_info, process_window_event, &tic_info);
            result = EXIT_SUCCESS;

            destroy_game(&tic_info);
        }
        deinitialize_system();
    }

    return result;
}