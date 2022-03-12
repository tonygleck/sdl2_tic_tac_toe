// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include "sdl2_tic_tac_toe/sdl2_win.h"
#include "sdl2_tic_tac_toe/game_board.h"
#include "sdl2_tic_tac_toe/sdl2_renderer.h"
#include "sdl2_tic_tac_toe/player_mgr.h"
#include "sdl2_tic_tac_toe/manual_player.h"
#include "sdl2_tic_tac_toe/computer_player.h"

#define SCREEN_WIDTH    640
#define SCREEN_HEIGHT   480

#define PLAYER_X        0
#define PLAYER_O        1

typedef struct PLAYER_INFO_TAG
{
    BOARD_CELL player_type;
    PLAYER_MGR_HANDLE player;
    const PLAYER_INTERFACE_DESC* player_interface;
} PLAYER_INFO;

typedef struct TIC_TAC_TOE_INFO_TAG
{
    BOARD_INFO_HANDLE board;
    SDL_WIN_HANDLE game_win;
    RENDERER_INFO_HANDLE renderer_info;
    //PLAYER_INFO player_list[2];

    //uint8_t curr_player_index;
} TIC_TAC_TOE_INFO;

// static void game_reset_click(void* user_ctx)
// {
//     TIC_TAC_TOE_INFO* tic_info = (TIC_TAC_TOE_INFO*)user_ctx;
//     for (size_t index = 0; index < 2; index++)
//     {
//         tic_info->player_list[index].player_interface->player_reset(tic_info->player_list[index].player);
//     }
// }

// static void process_turn_complete(GAME_OUTCOME outcome, void* user_ctx)
// {
//     TIC_TAC_TOE_INFO* tic_info = (TIC_TAC_TOE_INFO*)user_ctx;
//     tic_info->accept_input = true;
//     PLAYER_TYPE current_player_type;

//     if (outcome == OUTCOME_NO_RESULT)
//     {
//         // Change to the new player
//         tic_info->curr_player_index = (tic_info->curr_player_index == PLAYER_X) ? PLAYER_O : PLAYER_X;
//         current_player_type = tic_info->player_list[tic_info->curr_player_index].player_interface->player_get_type();
//         if (current_player_type == PLAYER_TYPE_COMPUTER)
//         {
//             SDL_Event event;
//             SDL_memset(&event, 0, sizeof(event));
//             event.type = SDL_USEREVENT;
//             event.user.code = EVENT_COMPUTER_TURN;
//             event.user.data1 = user_ctx;
//             event.user.data2 = 0;
//             SDL_PushEvent(&event);
//         }
//     }
// }

// static void render_game_win(void* user_ctx)
// {
//     TIC_TAC_TOE_INFO* tic_info = (TIC_TAC_TOE_INFO*)user_ctx;
//     game_board_render(tic_info->board, &GRID_COLOR, get_player_cell_type(tic_info->curr_player_index) );
// }

// static void process_window_event(void* user_ctx, SDL_Event* event, void* data)
// {
//     TIC_TAC_TOE_INFO* tic_info = (TIC_TAC_TOE_INFO*)user_ctx;
//     switch (event->type)
//     {
//         case SDL_MOUSEBUTTONUP:
//         {
//             SDL_MouseButtonEvent* mouse_event = (SDL_MouseButtonEvent*)data;
//             POS_INFO pos = { .x = mouse_event->x, .y = mouse_event->y };
//             if (tic_info->accept_input)
//             {
//                 tic_info->player_list[tic_info->curr_player_index].player_interface->player_process_click(tic_info->player_list[tic_info->curr_player_index].player, &pos);
//                 tic_info->accept_input = false;
//             }
//             break;
//         }
//         case SDL_USEREVENT:
//             if (event->user.code == EVENT_COMPUTER_TURN)
//             {
//                 tic_info->player_list[tic_info->curr_player_index].player_interface->player_take_turn(tic_info->player_list[tic_info->curr_player_index].player, process_turn_complete, user_ctx);
//             }
//             break;
//     }
// }

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
            if ((tic_info->board = game_board_create(SCREEN_WIDTH, SCREEN_HEIGHT, tic_info->renderer_info, tic_info)) == NULL)
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

static void destroy_game(TIC_TAC_TOE_INFO* tic_info)
{
    game_board_destroy(tic_info->board);

    destroy_renderer(tic_info->renderer_info);
    destroy_window(tic_info->game_win);
}

// static int create_players(TIC_TAC_TOE_INFO* tic_info)
// {
//     int result;
//     tic_info->player_list[PLAYER_X].player_interface = manual_get_interface_description();
//     tic_info->player_list[PLAYER_X].player_type = CELL_X_PLAYER;
//     tic_info->player_list[PLAYER_X].player = tic_info->player_list[0].player_interface->player_create(tic_info->board, CELL_X_PLAYER);
//     if (tic_info->player_list[PLAYER_X].player == NULL)
//     {
//         result = __LINE__;
//     }
//     else
//     {
//         tic_info->player_list[PLAYER_O].player_interface = computer_get_interface_description();
//         tic_info->player_list[PLAYER_O].player_type = CELL_0_PLAYER;
//         tic_info->player_list[PLAYER_O].player = tic_info->player_list[PLAYER_O].player_interface->player_create(tic_info->board, CELL_0_PLAYER);
//         if (tic_info->player_list[PLAYER_O].player == NULL)
//         {
//             tic_info->player_list[PLAYER_X].player_interface->player_destroy(tic_info->player_list[0].player);
//             result = __LINE__;
//         }
//         else
//         {
//             tic_info->curr_player_index = PLAYER_X;
//             result = 0;
//         }
//     }
//     return result;
// }

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
            // if (create_players(&tic_info) != 0)
            // {
            //     result = EXIT_FAILURE;
            // }
            // else
            // {
                //tic_info.accept_input = true;
                //tic_info.player_list[tic_info.curr_player_index].player_interface->player_take_turn(tic_info.player_list[tic_info.curr_player_index].player, process_turn_complete, &tic_info);
                game_board_process_game_loop(tic_info.board);
                result = EXIT_SUCCESS;
            // }
            destroy_game(&tic_info);
        }
        deinitialize_system();
    }
    return result;
}