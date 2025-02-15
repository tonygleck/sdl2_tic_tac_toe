// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// References here https://www.geeksforgeeks.org/minimax-algorithm-in-game-theory-set-3-tic-tac-toe-ai-finding-optimal-move/?ref=rp

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <signal.h>

#include "lib-util-c/sys_debug_shim.h"

#include "sdl2_tic_tac_toe/tic_tac_toe_const.h"

#include "sdl2_tic_tac_toe/player_mgr.h"
//#include "sdl2_tic_tac_toe/game_board.h"
#include "sdl2_tic_tac_toe/computer_player.h"

#define TURN_DELAY      1000

typedef struct COMPUTER_PLAYER_TAG
{
    uint8_t play_count;
    uint8_t play_record[ROW_COL_COUNT*ROW_COL_COUNT];
    BOARD_CELL player_type;
    BOARD_CELL opponent_type;
    GAME_INFO* game_info;
    PLAYER_TURN_COMPLETE turn_complete;
    ROW_COL_INFO next_move;
} COMPUTER_PLAYER;

static int evaluate_move(BOARD_CELL** board, COMPUTER_PLAYER* player)
{
    int result = 0;
    // Check Rows for Victory
    for (uint8_t row = 0; row < ROW_COL_COUNT; row++)
    {
        if (board[row][0] == board[row][1] && board[row][1] == board[row][2])
        {
            if (board[row][0] == player->player_type)
            {
                result = 10;
            }
            else if (board[row][0] == player->opponent_type)
            {
                result = -10;
            }
        }
    }

    // Check Col for Victory
    if (result == 0)
    {
        for (uint8_t col = 0; col < ROW_COL_COUNT; col++)
        {
            if (board[0][col] == board[1][col] && board[1][col] == board[2][col])
            {
                if (board[0][col] == player->player_type)
                {
                    result = 10;
                }
                else if (board[0][col] == player->opponent_type)
                {
                    result = -10;
                }
            }
        }
    }

    // Check Diaginal for Victory
    if (result == 0)
    {
        if ((board[0][0] == board[1][1] && board[1][1] == board[2][2]) ||
            (board[0][2] == board[1][1] && board[1][1] == board[2][0])
         )
        {
            if (board[1][1] == player->player_type)
            {
                result = 10;
            }
            else if (board[1][1] == player->opponent_type)
            {
                result = -10;
            }
        }
    }
    return result;
}

static void reset_variables(COMPUTER_PLAYER* player)
{
    player->play_count = 0;
    memset(player->play_record, 0, sizeof(int8_t)*(ROW_COL_COUNT*ROW_COL_COUNT));
    player->turn_complete = 0;
    player->next_move.row = player->next_move.col = ROW_COL_COUNT;
}

PLAYER_MGR_HANDLE computer_player_create(GAME_INFO* game_info, BOARD_CELL player_type)
{
    COMPUTER_PLAYER* result = (COMPUTER_PLAYER*)malloc(sizeof(COMPUTER_PLAYER));
    if (result == NULL)
    {
        printf("Failure allocating board item");
    }
    else
    {
        result->player_type = player_type;
        result->opponent_type = player_type == CELL_X_PLAYER ? CELL_0_PLAYER : CELL_X_PLAYER;
        result->game_info = game_info;
        reset_variables(result);
    }
    return result;
}

void computer_player_destroy(PLAYER_MGR_HANDLE handle)
{
    if (handle != NULL)
    {
        free(handle);
    }
}

void computer_player_take_turn(PLAYER_MGR_HANDLE handle, PLAYER_TURN_COMPLETE turn_complete, void* user_ctx)
{
    CELL_LOCATION cell_loc;
    if (handle != NULL)
    {
        COMPUTER_PLAYER* player = (COMPUTER_PLAYER*)handle;
        player->turn_complete = turn_complete;

        // See if we can win
        // Check the diagonal
        cell_loc = check_win_scenarios(player, player->game_info->game_board);
        if (cell_loc == CELL_LOC_NA)
        {
            // Block opponent winning
            cell_loc = check_opponent_win(player, player->game_info->game_board);
            if (cell_loc == CELL_LOC_NA)
            {
                cell_loc = determine_next_move(player, player->game_info);
            }
        }
        ROW_COL_INFO rc_info = { 0, 0 };
        if (cell_loc != CELL_LOC_NA)
        {
            rc_info = convert_location_row_col(cell_loc);
            player->play_record[player->play_count++] = rc_info.row + rc_info.col;
        }
        else
        {
            raise(SIGTRAP);
        }

        player->turn_complete(&rc_info, player->player_type, user_ctx);
    }
}

void computer_process_click(PLAYER_MGR_HANDLE player_handle, const ROW_COL_INFO* rc_info)
{
    (void)player_handle;
    (void)rc_info;
}

PLAYER_TYPE computer_player_get_type(void)
{
    return PLAYER_TYPE_COMPUTER;
}

void computer_player_reset(PLAYER_MGR_HANDLE handle)
{
    if (handle != NULL)
    {
        reset_variables((COMPUTER_PLAYER*)handle);
    }
}

const PLAYER_INTERFACE_DESC computer_player_interface =
{
    computer_player_create,
    computer_player_destroy,
    computer_player_take_turn,
    computer_process_click,
    computer_player_get_type,
    computer_player_reset
};

const PLAYER_INTERFACE_DESC* computer_get_interface_description(void)
{
    return &computer_player_interface;
}