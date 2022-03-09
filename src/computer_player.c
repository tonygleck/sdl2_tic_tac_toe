// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "lib-util-c/sys_debug_shim.h"

#include "sdl2_tic_tac_toe/game_board.h"
#include "sdl2_tic_tac_toe/computer_player.h"

typedef struct COMPUTER_PLAYER_TAG
{
    uint8_t play_count;
    uint8_t play_record[ROW_COL_COUNT*ROW_COL_COUNT];
    BOARD_CELL player_type;
    BOARD_CELL opponent_type;
    BOARD_INFO_HANDLE board_info;
    PLAYER_TURN_COMPLETE turn_complete;
} COMPUTER_PLAYER;

static ROW_COL_INFO convert_location_row_col(CELL_LOCATION cell_loc)
{
    ROW_COL_INFO rc;
    switch (cell_loc)
    {
        case CELL_LOC_R0_C0:
            rc.col = rc.row = 0;
            break;
        case CELL_LOC_R0_C1:
            rc.row = 0;
            rc.col = 1;
            break;
        case CELL_LOC_R0_C2:
            rc.row = 0;
            rc.col = 2;
            break;
        case CELL_LOC_R1_C0:
            rc.row = 1;
            rc.col = 0;
            break;
        case CELL_LOC_R1_C1:
            rc.row = rc.col = 1;
            break;
        case CELL_LOC_R1_C2:
            rc.row = 1;
            rc.col = 2;
            break;
        case CELL_LOC_R2_C0:
            rc.row = 2;
            rc.col = 0;
            break;
        case CELL_LOC_R2_C1:
            rc.row = 2;
            rc.col = 1;
            break;
        case CELL_LOC_R2_C2:
            rc.row = rc.col = 2;
            break;
        default:
        case CELL_LOC_NA:
            rc.col = rc.row = 3;
            break;
    }
    return rc;
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

static CELL_LOCATION check_col_row_scenarios(BOARD_CELL** board, uint8_t row, uint8_t col)
{
    CELL_LOCATION result = CELL_LOC_NA;
    // Test the Vertical
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
        // Test the Horizontal
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
    for (uint8_t row = 0; row < ROW_COL_COUNT && result == CELL_LOC_NA; row++)
    {
        for (uint8_t col = 0; col < ROW_COL_COUNT && result == CELL_LOC_NA; col++)
        {
            // Test only spaces occupied by our player
            if (board[row][col] == player->player_type)
            {
                result = check_col_row_scenarios(board, row, col);
            }
        }
    }
    return result;
}

static CELL_LOCATION check_opponent_win(COMPUTER_PLAYER* player, BOARD_CELL** board)
{
    CELL_LOCATION result = CELL_LOC_NA;
    for (uint8_t row = 0; row < ROW_COL_COUNT && result == CELL_LOC_NA; row++)
    {
        for (uint8_t col = 0; col < ROW_COL_COUNT && result == CELL_LOC_NA; col++)
        {
            // Test only spaces occupied by our player
            if (board[row][col] == player->opponent_type)
            {
                result = check_col_row_scenarios(board, row, col);
            }
        }
    }
    return result;
}

static CELL_LOCATION determine_next_move(COMPUTER_PLAYER* player, BOARD_CELL** board)
{
    (void)player;
    CELL_LOCATION result = CELL_LOC_NA;
    for (uint8_t row = 0; row < ROW_COL_COUNT && result == CELL_LOC_NA; row++)
    {
        for (uint8_t col = 0; col < ROW_COL_COUNT && result == CELL_LOC_NA; col++)
        {
            // Test only spaces occupied by our player
            if (board[row][col] == CELL_EMPTY) //player->player_type)
            {
                result = convert_row_col_to_location(row, col);
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
}

PLAYER_MGR_HANDLE computer_player_create(BOARD_INFO_HANDLE board_info, BOARD_CELL player_type)
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
        result->board_info = board_info;
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

void computer_player_reset(PLAYER_MGR_HANDLE handle)
{
    if (handle != NULL)
    {
        reset_variables((COMPUTER_PLAYER*)handle);
    }
}

void computer_player_take_turn(PLAYER_MGR_HANDLE handle, PLAYER_TURN_COMPLETE turn_complete, void* user_ctx)
{
    CELL_LOCATION cell_loc;
    if (handle != NULL)
    {
        COMPUTER_PLAYER* player = (COMPUTER_PLAYER*)handle;
        player->turn_complete = turn_complete;

        BOARD_CELL** board = game_board_get_board(player->board_info);
        // See if we can win
        // Check the diagonal
        cell_loc = check_win_scenarios(player, board);
        if (cell_loc == CELL_LOC_NA)
        {
            // Block opponent winning
            cell_loc = check_opponent_win(player, board);
            if (cell_loc == CELL_LOC_NA)
            {
                cell_loc = determine_next_move(player, board);
            }
        }
        GAME_OUTCOME outcome;
        if (cell_loc != CELL_LOC_NA)
        {
            ROW_COL_INFO rc = convert_location_row_col(cell_loc);
            outcome = game_board_play(player->board_info, rc.row, rc.col, player->player_type);
            player->play_record[player->play_count++] = rc.row + rc.col;
        }
        else
        {
            outcome = OUTCOME_NO_RESULT;
        }

        player->turn_complete(outcome, user_ctx);
    }
}

void computer_process_click(PLAYER_MGR_HANDLE player_handle, const POS_INFO* pos)
{
    (void)player_handle;
    (void)pos;
}

PLAYER_TYPE computer_player_get_type(void)
{
    return PLAYER_TYPE_COMPUTER;
}

const PLAYER_INTERFACE_DESC computer_player_interface =
{
    computer_player_create,
    computer_player_destroy,
    computer_player_take_turn,
    computer_process_click,
    computer_player_get_type
};

const PLAYER_INTERFACE_DESC* computer_get_interface_description(void)
{
    return &computer_player_interface;
}