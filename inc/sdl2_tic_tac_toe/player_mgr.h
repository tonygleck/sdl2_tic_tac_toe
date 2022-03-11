// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "macro_utils/macro_utils.h"

#include "sdl2_tic_tac_toe/game_board.h"

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CELL_LOCATION_VALUES    \
    CELL_LOC_R0_C0,             \
    CELL_LOC_R0_C1,             \
    CELL_LOC_R0_C2,             \
    CELL_LOC_R1_C0,             \
    CELL_LOC_R1_C1,             \
    CELL_LOC_R1_C2,             \
    CELL_LOC_R2_C0,             \
    CELL_LOC_R2_C1,             \
    CELL_LOC_R2_C2,             \
    CELL_LOC_NA

MU_DEFINE_ENUM(CELL_LOCATION, CELL_LOCATION_VALUES)

#define PLAYER_TYPE_VALUES  \
    PLAYER_TYPE_MANUAL,     \
    PLAYER_TYPE_COMPUTER

MU_DEFINE_ENUM(PLAYER_TYPE, PLAYER_TYPE_VALUES)

#define EVENT_COMPUTER_TURN         1

typedef void* PLAYER_MGR_HANDLE;

typedef void(*PLAYER_TURN_COMPLETE)(GAME_OUTCOME outcome, void* user_ctx);

typedef PLAYER_MGR_HANDLE(*PLAYER_CREATE)(BOARD_INFO_HANDLE board_info, BOARD_CELL player_type);
typedef void(*PLAYER_DESTROY)(PLAYER_MGR_HANDLE player_handle);
typedef void(*PLAYER_TAKE_TURN)(PLAYER_MGR_HANDLE player_handle, PLAYER_TURN_COMPLETE turn_complete, void* user_ctx);
typedef void(*PLAYER_PROCESS_CLICK)(PLAYER_MGR_HANDLE player_handle, const POS_INFO* pos);
typedef PLAYER_TYPE(*PLAYER_GET_TYPE)(void);
typedef void(*PLAYER_RESET)(PLAYER_MGR_HANDLE player_handle);

typedef struct PLAYER_INTERFACE_DESC_TAG
{
    PLAYER_CREATE player_create;
    PLAYER_DESTROY player_destroy;
    PLAYER_TAKE_TURN player_take_turn;
    PLAYER_PROCESS_CLICK player_process_click;
    PLAYER_GET_TYPE player_get_type;
    PLAYER_RESET player_reset;
} PLAYER_INTERFACE_DESC;

/*MOCKABLE_FUNCTION(, PLAYER_MGR_HANDLE, player_mgr_create_player, BOARD_INFO_HANDLE, board_info, BOARD_CELL, player_type);
MOCKABLE_FUNCTION(, void, player_mgr_destroy_player, PLAYER_MGR_HANDLE, handle);

MOCKABLE_FUNCTION(, CELL_LOCATION, player_mgr_take_turn, PLAYER_MGR_HANDLE, handle);
MOCKABLE_FUNCTION(, CELL_LOCATION, player_mgr_process_click, PLAYER_MGR_HANDLE, handle, const POS_INFO*, pos);*/

#ifdef __cplusplus
}
#endif
