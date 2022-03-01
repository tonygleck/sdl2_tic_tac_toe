// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stddef.h>
#include <stdint.h>

#include "lib-util-c/sys_debug_shim.h"

#include "sdl2_tic_tac_toe/game_board.h"
#include "sdl2_tic_tac_toe/computer_player.h"

typedef struct COMPUTER_PLAYER_TAG
{
    BOARD_CELL player_type;
    BOARD_CELL opponent_type;
    BOARD_INFO_HANDLE board_info;
} COMPUTER_PLAYER;

static CELL_LOCATION check_opponent_win(COMPUTER_PLAYER* player, BOARD_CELL** board)
{
    CELL_LOCATION result = CELL_LOC_NA;
    // Check First Row
    if (board[0][0] == player->opponent_type)
    {
    }
    return result;
}

static CELL_LOCATION check_win_scenarios(COMPUTER_PLAYER* player, BOARD_CELL** board)
{
    CELL_LOCATION result = CELL_LOC_NA;
    // Check First Row
    if (board[0][0] == player->player_type)
    {
        if (board[0][1] == player->player_type)
        {
            result = CELL_LOC_R0_C2;
        }
        else if (board[0][2] == player->player_type)
        {
            result = CELL_LOC_R0_C1;
        }
        // Check first the Vertical Column
        else if (board[1][0] == player->player_type)
        {
            result = CELL_LOC_R2_C0;
        }
        else if (board[2][0] == player->player_type)
        {
            result = CELL_LOC_R1_C0;
        }
        // Now check diagonal
        else if (board[1][1] == player->player_type)
        {
            result = CELL_LOC_R2_C2;
        }
        else if (board[2][2] == player->player_type)
        {
            result = CELL_LOC_R1_C1;
        }
    }
    if (result == CELL_LOC_NA)
    {
        if (board[0][1] == player->player_type && board[0][2] == player->player_type)
        {
            result = CELL_LOC_R0_C0;
        }
        // Diagonal
        else if (board[1][1] == player->player_type && board[2][2] == player->player_type)
        {
            result = CELL_LOC_R0_C0;
        }
        // Check Second Row
        else if (board[1][0] == player->player_type)
        {
            if (board[1][1] == player->player_type)
            {
                result = CELL_LOC_R1_C2;
            }
            else if (board[1][2] == player->player_type)
            {
                result = CELL_LOC_R1_C1;
            }
            // Check second the Vertical Column
            else if (board[1][1] == player->player_type)
            {
                result = CELL_LOC_R2_C1;
            }
            else if (board[2][1] == player->player_type)
            {
                result = CELL_LOC_R1_C1;
            }
        }
        if (result == CELL_LOC_NA)
        {
            if (board[1][1] == player->player_type && board[1][2] == player->player_type)
            {
                result = CELL_LOC_R1_C0;
            }
            // Check Third Row
            else if (board[2][0] == player->player_type)
            {
                if (board[2][1] == player->player_type)
                {
                    result = CELL_LOC_R2_C2;
                }
                else if (board[2][2] == player->player_type)
                {
                    result = CELL_LOC_R2_C1;
                }
                // Check second the Vertical Column
                else if (board[1][2] == player->player_type)
                {
                    result = CELL_LOC_R2_C2;
                }
                else if (board[2][1] == player->player_type)
                {
                    result = CELL_LOC_R1_C1;
                }
                // Now check diagonal
                else if (board[1][1] == player->player_type)
                {
                    result = CELL_LOC_R0_C2;
                }
                else if (board[0][2] == player->player_type)
                {
                    result = CELL_LOC_R1_C1;
                }
            }
            else if (board[2][1] == player->player_type && board[2][2] == player->player_type)
            {
                result = CELL_LOC_R1_C0;
            }
            // Check diagonal
            else if (board[1][1] == player->player_type && board[0][2] == player->player_type)
            {
                result = CELL_LOC_R2_C0;
            }
        }
    }
    return result;
}

COMPUTER_PLAYER_HANDLE computer_player_create(BOARD_INFO_HANDLE board_info, BOARD_CELL player_type)
{
    COMPUTER_PLAYER* result = (COMPUTER_PLAYER*)malloc(sizeof(COMPUTER_PLAYER));
    if (result == NULL)
    {
        printf("Failure allocating board item");
    }
    else
    {
        result->player_type = player_type;;
        result->opponent_type = player_type == CELL_X_PLAYER ? CELL_0_PLAYER : CELL_X_PLAYER;
        result->board_info = board_info;
    }
    return result;
}

void computer_player_destroy(COMPUTER_PLAYER_HANDLE handle)
{
    if (handle != NULL)
    {
        free(handle);
    }
}

CELL_LOCATION computer_player_take_turn(COMPUTER_PLAYER_HANDLE handle)
{
    CELL_LOCATION result;
    if (handle != NULL)
    {
        BOARD_CELL** board = game_board_get_board(handle->board_info);
        // See if we can win
        // Check the diagonal
        result = check_win_scenarios(handle, board);
        if (result == CELL_LOC_NA)
        {
            // Block opponent winning
            result = check_opponent_win(handle, board);
            if (result == CELL_LOC_NA)
            {
                // Find a place to move
            }
        }
    }
    else
    {
        result = CELL_LOC_NA;
    }
    return result;
}
