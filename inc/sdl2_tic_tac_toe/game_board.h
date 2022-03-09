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

#include "sdl2_tic_tac_toe/sdl2_renderer.h"

#include "umock_c/umock_c_prod.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct BOARD_INFO_TAG* BOARD_INFO_HANDLE;

#define ROW_COL_COUNT           3

typedef enum GAME_OUTCOME_TAG
{
    OUTCOME_WIN_ROW_0,
    OUTCOME_WIN_ROW_1,
    OUTCOME_WIN_ROW_2,
    OUTCOME_WIN_COL_0,
    OUTCOME_WIN_COL_1,
    OUTCOME_WIN_COL_2,
    OUTCOME_WIN_DIAG_1,
    OUTCOME_WIN_DIAG_2,
    OUTCOME_GAME_TIE,
    OUTCOME_NO_RESULT
} GAME_OUTCOME;

typedef enum BOARD_CELL_TAG
{
    CELL_EMPTY,
    CELL_X_PLAYER,
    CELL_0_PLAYER
} BOARD_CELL;

typedef struct POS_INFO_TAG
{
    uint8_t x;
    uint8_t y;
} POS_INFO;

MOCKABLE_FUNCTION(, BOARD_INFO_HANDLE, game_board_create, uint16_t, width, uint16_t, length, RENDERER_INFO_HANDLE, renderer);
MOCKABLE_FUNCTION(, void, game_board_destroy, BOARD_INFO_HANDLE, handle);

MOCKABLE_FUNCTION(, void, game_board_render, BOARD_INFO_HANDLE, handle, const SDL_Color*, color, BOARD_CELL, player_type);
MOCKABLE_FUNCTION(, GAME_OUTCOME, game_board_click, BOARD_INFO_HANDLE, handle, const POS_INFO*, pos, BOARD_CELL, player_type);
MOCKABLE_FUNCTION(, GAME_OUTCOME, game_board_play, BOARD_INFO_HANDLE, handle, uint8_t, row, uint8_t, col, BOARD_CELL, player_type);
MOCKABLE_FUNCTION(, BOARD_CELL**, game_board_get_board, BOARD_INFO_HANDLE, handle);

#ifdef __cplusplus
}
#endif
