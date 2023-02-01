// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "macro_utils/macro_utils.h"

#include "sdl2_tic_tac_toe/tic_tac_toe_const.h"

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* PLAYER_MGR_HANDLE;

typedef void(*PLAYER_TURN_COMPLETE)(const ROW_COL_INFO* rc_info, BOARD_CELL player_type, void* user_ctx);

typedef PLAYER_MGR_HANDLE(*PLAYER_CREATE)(GAME_INFO* game_info, BOARD_CELL player_type);
typedef void(*PLAYER_DESTROY)(PLAYER_MGR_HANDLE player_handle);
typedef void(*PLAYER_TAKE_TURN)(PLAYER_MGR_HANDLE player_handle, PLAYER_TURN_COMPLETE turn_complete, void* user_ctx);
typedef void(*PLAYER_PROCESS_CLICK)(PLAYER_MGR_HANDLE player_handle, const ROW_COL_INFO* rc_info);
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

#ifdef __cplusplus
}
#endif
