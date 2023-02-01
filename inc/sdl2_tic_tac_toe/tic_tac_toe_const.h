// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#ifdef __cplusplus
extern "C" {
#include <cstddef>
#else
#include <stdint.h>
#endif /* __cplusplus */

#include "macro_utils/macro_utils.h"
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

#define BOARD_CELL_VALUE    \
    CELL_EMPTY,             \
    CELL_X_PLAYER,          \
    CELL_0_PLAYER

MU_DEFINE_ENUM(BOARD_CELL, BOARD_CELL_VALUE)

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

typedef struct ROW_COL_INFO_TAG
{
    uint8_t row;
    uint8_t col;
} ROW_COL_INFO;

typedef struct POS_INFO_TAG
{
    int32_t x;
    int32_t y;
} POS_INFO;

typedef struct GAME_INFO_TAG
{
    int8_t turn_count;
    GAME_OUTCOME game_outcome;
    BOARD_CELL** game_board;
} GAME_INFO;

#define EVENT_COMPUTER_TURN         1
#define BEGIN_COMPUTER_TURN         2
#define NUM_OF_PLAYERS              2

#ifdef __cplusplus
}
#endif
