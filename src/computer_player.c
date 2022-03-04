// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
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

static CELL_LOCATION convert_row_col_to_location(uint8_t row, uint8_t col)
{
    CELL_LOCATION result;
    if (row == 0 && col == 0)
        result = CELL_LOC_R0_C0;
    else if (row == 0 && col == 1)
        result = CELL_LOC_R0_C1;
    else if (row == 0 && col == 2)
        result = CELL_LOC_R0_C2;
    else if (row == 1 && col == 0)
        result = CELL_LOC_R1_C0;
    else if (row == 1 && col == 1)
        result = CELL_LOC_R1_C1;
    else if (row == 1 && col == 2)
        result = CELL_LOC_R1_C2;
    else if (row == 2 && col == 0)
        result = CELL_LOC_R2_C0;
    else if (row == 2 && col == 1)
        result = CELL_LOC_R2_C1;
    else if (row == 2 && col == 2)
        result = CELL_LOC_R2_C2;
    else
        result = CELL_LOC_NA;
    return result;
}

static CELL_LOCATION check_col_row_win_scenarios(BOARD_CELL** board, uint8_t row, uint8_t col)
{
    CELL_LOCATION result = CELL_LOC_NA;
    // Test the Horizontal
    switch (row)
    {
        case 0:
            if (board[1][col] == board[row][col])
                result = convert_row_col_to_location(2, col);
            else if (board[2][col] == board[row][col])
                result = convert_row_col_to_location(1, col);
            break;
        case 1:
            if (board[0][col] == board[row][col])
                result = convert_row_col_to_location(2, col);
            else if (board[2][col] == board[row][col])
                result = convert_row_col_to_location(0, col);
            break;
        case 2:
            if (board[0][col] == board[row][col])
                result = convert_row_col_to_location(1, col);
            else if (board[1][col] == board[row][col])
                result = convert_row_col_to_location(0, col);
            break;
    }
    if (result == CELL_LOC_NA)
    {
        // Test the Vertical
        switch (col)
        {
            case 0:
                if (board[row][1] == board[row][col])
                    result = convert_row_col_to_location(row, 2);
                else if (board[row][2] == board[row][col])
                    result = convert_row_col_to_location(row, 1);
                break;
            case 1:
                if (board[row][0] == board[row][col])
                    result = convert_row_col_to_location(row, 2);
                else if (board[row][2] == board[row][col])
                    result = convert_row_col_to_location(row, 0);
                break;
            case 2:
                if (board[row][0] == board[row][col])
                    result = convert_row_col_to_location(row, 1);
                else if (board[row][1] == board[row][col])
                    result = convert_row_col_to_location(row, 0);
                break;
        }
        if (result == CELL_LOC_NA)
        {
            // Check Diagonal
            if ((row == 0 && col == 0) && (board[1][1] == board[row][col] || board[2][2] == board[row][col]) )
            {
                if (board[1][1] == board[row][col])
                    result = convert_row_col_to_location(2, 2);
                else
                    result = convert_row_col_to_location(1, 1);
            }
            else if ((row == 0 && col == 2) && (board[1][1] == board[row][col] || board[2][0] == board[row][col]))
            {
                if (board[1][1] == board[row][col])
                    result = convert_row_col_to_location(2, 0);
                else
                    result = convert_row_col_to_location(1, 1);
            }
            else if ((row == 1 && col == 1) && (board[2][2] == board[row][col] || board[2][0] == board[row][col]))
            {
                if (board[2][2] == board[row][col])
                    result = convert_row_col_to_location(0, 0);
                else
                    result = convert_row_col_to_location(0, 2);
            }
        }
    }
    return result;
}

static CELL_LOCATION check_win_scenarios(COMPUTER_PLAYER* player, BOARD_CELL** board)
{
    CELL_LOCATION result = CELL_LOC_NA;
    for (uint8_t col = 0; col < ROW_COL_COUNT && result == CELL_LOC_NA; col++)
    {
        for (uint8_t row = 0; row < ROW_COL_COUNT && result == CELL_LOC_NA; row++)
        {
            // Test only spaces occupied by our player
            if (board[col][row] == player->player_type)
            {
                result = check_col_row_win_scenarios(board, col, row);
            }
        }
    }
    return result;
}
/*static CELL_LOCATION check_win_scenarios(COMPUTER_PLAYER* player, BOARD_CELL** board)
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
            // Check the Vertical Column
            else if (board[2][0] == player->player_type)
            {
                result = CELL_LOC_R0_C0;
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
            // Check the vertical
            else if (board[1][1] == player->player_type && board[2][1] == player->player_type)
            {
                result = CELL_LOC_R0_C1;
            }
            else if (board[1][2] == player->player_type && board[2][2] == player->player_type)
            {
                result = CELL_LOC_R0_C2;
            }
            // Check diagonal
            else if (board[1][1] == player->player_type && board[0][2] == player->player_type)
            {
                result = CELL_LOC_R2_C0;
            }
        }
    }
    return result;
}*/

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
