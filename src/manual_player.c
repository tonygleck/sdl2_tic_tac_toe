// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#include "lib-util-c/sys_debug_shim.h"

#include "sdl2_tic_tac_toe/game_board.h"
#include "sdl2_tic_tac_toe/manual_player.h"

typedef struct MANUAL_PLAYER_TAG
{
    BOARD_CELL player_type;
    BOARD_CELL opponent_type;
    BOARD_INFO_HANDLE board_info;
    PLAYER_TURN_COMPLETE turn_complete;
    void* turn_complete_ctx;
} MANUAL_PLAYER;

static void reset_variables(MANUAL_PLAYER* manual_player)
{
    (void)manual_player;
}

PLAYER_MGR_HANDLE manual_player_create(BOARD_INFO_HANDLE board_info, BOARD_CELL player_type)
{
    MANUAL_PLAYER* result = (MANUAL_PLAYER*)malloc(sizeof(MANUAL_PLAYER));
    if (result == NULL)
    {
        printf("Failure allocating board item");
    }
    else
    {
        result->player_type = player_type;
        result->opponent_type = player_type == CELL_X_PLAYER ? CELL_0_PLAYER : CELL_X_PLAYER;
        result->board_info = board_info;
        result->turn_complete = NULL;
        result->turn_complete_ctx = NULL;
    }
    return result;
}

void manual_player_destroy(PLAYER_MGR_HANDLE handle)
{
    if (handle != NULL)
    {
        free(handle);
    }
}

void manual_process_click(PLAYER_MGR_HANDLE handle, const POS_INFO* pos)
{
    MANUAL_PLAYER* manual_player = (MANUAL_PLAYER*)handle;
    GAME_OUTCOME outcome = game_board_click(manual_player->board_info, pos, manual_player->player_type);
    if (manual_player->turn_complete != NULL)
    {
        manual_player->turn_complete(outcome, manual_player->turn_complete_ctx);
    }
}

void manual_player_take_turn(PLAYER_MGR_HANDLE handle, PLAYER_TURN_COMPLETE turn_complete, void* user_ctx)
{
    MANUAL_PLAYER* manual_player = (MANUAL_PLAYER*)handle;
    manual_player->turn_complete = turn_complete;
    manual_player->turn_complete_ctx = user_ctx;
}

void manual_player_reset(PLAYER_MGR_HANDLE player_handle)
{
    reset_variables((MANUAL_PLAYER*)player_handle);
}

PLAYER_TYPE manual_player_get_type(void)
{
    return PLAYER_TYPE_MANUAL;
}

const PLAYER_INTERFACE_DESC manual_player_interface =
{
    manual_player_create,
    manual_player_destroy,
    manual_player_take_turn,
    manual_process_click,
    manual_player_get_type,
    manual_player_reset
};

const PLAYER_INTERFACE_DESC* manual_get_interface_description(void)
{
    return &manual_player_interface;
}