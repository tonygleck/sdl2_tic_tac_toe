// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "macro_utils/macro_utils.h"

#include "sdl2_tic_tac_toe/player_mgr.h"
#include "sdl2_tic_tac_toe/game_board.h"

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

MOCKABLE_FUNCTION(, PLAYER_MGR_HANDLE, computer_player_create, GAME_INFO*, game_info, BOARD_CELL, player_type);
MOCKABLE_FUNCTION(, void, computer_player_destroy, PLAYER_MGR_HANDLE, handle);
MOCKABLE_FUNCTION(, void, computer_player_take_turn, PLAYER_MGR_HANDLE, handle, PLAYER_TURN_COMPLETE, turn_complete, void*, user_ctx);
MOCKABLE_FUNCTION(, void, computer_process_click, PLAYER_MGR_HANDLE, player_handle, const ROW_COL_INFO*, rc_info);
MOCKABLE_FUNCTION(, PLAYER_TYPE, computer_player_get_type);
MOCKABLE_FUNCTION(, void, computer_player_reset, PLAYER_MGR_HANDLE, player_handle);

MOCKABLE_FUNCTION(, const PLAYER_INTERFACE_DESC*, computer_get_interface_description);

#ifdef __cplusplus
}
#endif
