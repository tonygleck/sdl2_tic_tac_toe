// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "lib-util-c/sys_debug_shim.h"

#include "sdl2_tic_tac_toe/game_board.h"
#include "sdl2_tic_tac_toe/manual_player.h"

PLAYER_MGR_HANDLE player_mgr_create_player(BOARD_INFO_HANDLE board_info, BOARD_CELL player_type)
{
    PLAYER_MGR_HANDLE* result = (COMPUTER_PLAYER*)malloc(sizeof(COMPUTER_PLAYER));
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

    return result;
}
