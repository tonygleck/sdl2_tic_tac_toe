// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "macro_utils/macro_utils.h"

#include "sdl2_tic_tac_toe/game_board.h"
#include "sdl2_tic_tac_toe/player_mgr.h"

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

MOCKABLE_FUNCTION(, PLAYER_MGR_HANDLE, manual_player_create, BOARD_INFO_HANDLE, board_info, BOARD_CELL, player_type);
MOCKABLE_FUNCTION(, void, manual_player_destroy, PLAYER_MGR_HANDLE, handle);
MOCKABLE_FUNCTION(, void, manual_player_take_turn, PLAYER_MGR_HANDLE, handle, PLAYER_TURN_COMPLETE, turn_complete, void*, user_ctx);
MOCKABLE_FUNCTION(, void, manual_process_click, PLAYER_MGR_HANDLE, player_handle, const POS_INFO*, pos);
MOCKABLE_FUNCTION(, PLAYER_TYPE, manual_player_get_type);
MOCKABLE_FUNCTION(, void, manual_player_reset, PLAYER_MGR_HANDLE, player_handle);

MOCKABLE_FUNCTION(, const PLAYER_INTERFACE_DESC*, manual_get_interface_description);

#ifdef __cplusplus
}
#endif
