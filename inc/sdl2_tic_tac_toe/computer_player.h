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

typedef struct COMPUTER_PLAYER_TAG* COMPUTER_PLAYER_HANDLE;

MOCKABLE_FUNCTION(, COMPUTER_PLAYER_HANDLE, computer_player_create, BOARD_INFO_HANDLE, board_info, BOARD_CELL, player_type);
MOCKABLE_FUNCTION(, void, computer_player_destroy, COMPUTER_PLAYER_HANDLE, handle);
MOCKABLE_FUNCTION(, CELL_LOCATION, computer_player_take_turn, COMPUTER_PLAYER_HANDLE, handle);

#ifdef __cplusplus
}
#endif
