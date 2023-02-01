// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#include <cstdint>
#else
#include <stdint.h>
#include <stddef.h>
#endif /* __cplusplus */

#include "macro_utils/macro_utils.h"

#include "sdl2_tic_tac_toe/tic_tac_toe_const.h"

#include "sdl2_tic_tac_toe/sdl2_renderer.h"
#include "sdl2_tic_tac_toe/player_mgr.h"

#include "umock_c/umock_c_prod.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct BOARD_INFO_TAG* BOARD_INFO_HANDLE;

#define ROW_COL_COUNT           3

typedef void(*GAME_RESET_CLICK)(void* user_ctx);

MOCKABLE_FUNCTION(, BOARD_INFO_HANDLE, game_board_create, uint16_t, width, uint16_t, length, RENDERER_INFO_HANDLE, renderer, const PLAYER_INTERFACE_DESC*, x_player, const PLAYER_INTERFACE_DESC*, y_player);
MOCKABLE_FUNCTION(, void, game_board_destroy, BOARD_INFO_HANDLE, handle);

MOCKABLE_FUNCTION(, void, game_board_render, BOARD_INFO_HANDLE, handle, const SDL_Color*, color, BOARD_CELL, player_type);
MOCKABLE_FUNCTION(, void, game_board_process_game_loop, BOARD_INFO_HANDLE, handle);

#ifdef __cplusplus
}
#endif
