// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifdef __cplusplus
#include <cstddef>
#include <cstdlib>
#include <cstdint>
#else
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#endif

#include "ctest.h"
#include "macro_utils/macro_utils.h"

static void* my_mem_shim_malloc(size_t size)
{
    return malloc(size);
}

static void my_mem_shim_free(void* ptr)
{
    free(ptr);
}

// Include the test tools.
//#include "testrunnerswitcher.h"
#include "macro_utils/macro_utils.h"
#include "umock_c/umock_c.h"
#include "umock_c/umocktypes_stdint.h"

#define ENABLE_MOCKS
#include "sdl2_tic_tac_toe/tic_tac_toe_const.h"
#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/thread_mgr.h"

MOCKABLE_FUNCTION(, void, process_turn_complete, const ROW_COL_INFO*, rc_info, BOARD_CELL, player_type, void*, user_ctx);

#undef ENABLE_MOCKS

#include "umock_c/umock_c_prod.h"

#include "sdl2_tic_tac_toe/computer_player.h"

//TEST_DEFINE_ENUM_TYPE(CELL_LOCATION, CELL_LOCATION_VALUES)

//static BOARD_INFO_HANDLE g_board_info = (BOARD_INFO_HANDLE)0x2345;

#define WIN_DIR_HORIZ   "Horizontal"
#define WIN_DIR_VERT    "Vertical"
#define WIN_DIR_DIAG    "Diagonal"
#define WIN_DIR_NONE    "None"

static const struct
{
    BOARD_CELL cell_matrix[9];
    CELL_LOCATION result_cell_loc;
    ROW_COL_INFO result_pos;
    const char* win_direction;
} g_test_cell_matrix[] =
{
    // 0 - 2
    { { CELL_EMPTY, CELL_X_PLAYER, CELL_X_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY }, CELL_LOC_R0_C0, {0, 0}, WIN_DIR_HORIZ },
    { { CELL_EMPTY, CELL_0_PLAYER, CELL_0_PLAYER, CELL_X_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_X_PLAYER, CELL_EMPTY, CELL_EMPTY }, CELL_LOC_R0_C0, {0, 0}, WIN_DIR_VERT },
    { { CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_X_PLAYER, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_X_PLAYER }, CELL_LOC_R0_C0, {0, 0}, WIN_DIR_DIAG },
    // 3 - 4
    { { CELL_X_PLAYER, CELL_EMPTY, CELL_X_PLAYER, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY }, CELL_LOC_R0_C1, {0, 1}, WIN_DIR_HORIZ },
    { { CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_0_PLAYER, CELL_X_PLAYER, CELL_EMPTY, CELL_0_PLAYER, CELL_X_PLAYER, CELL_EMPTY }, CELL_LOC_R0_C1, {0, 1}, WIN_DIR_VERT },
    // 5 - 7
    { { CELL_X_PLAYER, CELL_X_PLAYER, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY }, CELL_LOC_R0_C2, {0, 2}, WIN_DIR_HORIZ },
    { { CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_X_PLAYER, CELL_0_PLAYER, CELL_EMPTY, CELL_X_PLAYER }, CELL_LOC_R0_C2, {0, 2}, WIN_DIR_VERT },
    { { CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_X_PLAYER, CELL_0_PLAYER, CELL_X_PLAYER, CELL_EMPTY, CELL_EMPTY }, CELL_LOC_R0_C2, {0, 2}, WIN_DIR_DIAG },
    // 8 - 9
    { { CELL_X_PLAYER, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY,  CELL_0_PLAYER, CELL_EMPTY,  CELL_X_PLAYER, CELL_EMPTY, CELL_EMPTY }, CELL_LOC_R1_C0, {1, 0}, WIN_DIR_HORIZ },
    { { CELL_EMPTY, CELL_EMPTY, CELL_0_PLAYER,  CELL_EMPTY,  CELL_X_PLAYER, CELL_X_PLAYER,  CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY }, CELL_LOC_R1_C0, {1, 0}, WIN_DIR_VERT },
    // 10 - 13
    { { CELL_EMPTY, CELL_0_PLAYER,  CELL_EMPTY, CELL_X_PLAYER, CELL_EMPTY, CELL_X_PLAYER, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY }, CELL_LOC_R1_C1, {1, 1}, WIN_DIR_HORIZ },
    { { CELL_EMPTY, CELL_X_PLAYER, CELL_EMPTY, CELL_0_PLAYER,  CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_X_PLAYER, CELL_EMPTY }, CELL_LOC_R1_C1, {1, 1}, WIN_DIR_VERT },
    { { CELL_X_PLAYER, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_X_PLAYER }, CELL_LOC_R1_C1, {1, 1}, WIN_DIR_DIAG },
    { { CELL_0_PLAYER, CELL_EMPTY, CELL_X_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_0_PLAYER, CELL_X_PLAYER, CELL_EMPTY, CELL_EMPTY }, CELL_LOC_R1_C1, {1, 1}, WIN_DIR_DIAG },
    // 14 - 15
    { { CELL_0_PLAYER, CELL_EMPTY, CELL_X_PLAYER, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_X_PLAYER }, CELL_LOC_R1_C2, {1, 2}, WIN_DIR_VERT },
    { { CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_X_PLAYER, CELL_X_PLAYER, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY }, CELL_LOC_R1_C2, {1, 2}, WIN_DIR_HORIZ },
    // 16 - 18
    { { CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_X_PLAYER, CELL_X_PLAYER }, CELL_LOC_R2_C0, {2, 0}, WIN_DIR_HORIZ },
    { { CELL_X_PLAYER, CELL_EMPTY, CELL_0_PLAYER, CELL_X_PLAYER, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY }, CELL_LOC_R2_C0, {2, 0}, WIN_DIR_VERT },
    { { CELL_EMPTY, CELL_0_PLAYER, CELL_X_PLAYER, CELL_EMPTY, CELL_X_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY }, CELL_LOC_R2_C0, {2, 0}, WIN_DIR_DIAG },
    // 19 - 20
    { { CELL_0_PLAYER, CELL_X_PLAYER, CELL_EMPTY, CELL_0_PLAYER, CELL_X_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY }, CELL_LOC_R2_C1, {2, 1}, WIN_DIR_VERT },
    { { CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_X_PLAYER, CELL_EMPTY, CELL_X_PLAYER }, CELL_LOC_R2_C1, {2, 1}, WIN_DIR_HORIZ },
    // 21 - 23
    { { CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_0_PLAYER, CELL_EMPTY, CELL_X_PLAYER, CELL_X_PLAYER, CELL_EMPTY }, CELL_LOC_R2_C2, {2, 2}, WIN_DIR_HORIZ },
    { { CELL_EMPTY, CELL_0_PLAYER, CELL_X_PLAYER, CELL_0_PLAYER, CELL_EMPTY, CELL_X_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY }, CELL_LOC_R2_C2, {2, 2}, WIN_DIR_VERT },
    { { CELL_EMPTY, CELL_0_PLAYER, CELL_X_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_X_PLAYER, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY }, CELL_LOC_R2_C2, {2, 2}, WIN_DIR_DIAG } //,

    //{ { CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,  CELL_EMPTY,  CELL_EMPTY, CELL_EMPTY,  CELL_EMPTY,  CELL_EMPTY, CELL_EMPTY }, CELL_LOC_NA, {3, 3}, WIN_DIR_NONE }
};

static BOARD_CELL** create_board_cell_data(const BOARD_CELL cell_item[9])
{
    BOARD_CELL** result = my_mem_shim_malloc(ROW_COL_COUNT * sizeof(BOARD_CELL*));
    CTEST_ASSERT_IS_NOT_NULL(result);

    for (size_t index = 0; index < ROW_COL_COUNT; index++)
    {
        result[index] = my_mem_shim_malloc(ROW_COL_COUNT * sizeof(BOARD_CELL));
        CTEST_ASSERT_IS_NOT_NULL(result[index]);

        switch (index)
        {
            case 0:
                result[index][0] = cell_item[index];
                result[index][1] = cell_item[index + 1];
                result[index][2] = cell_item[index + 2];
                break;
            case 1:
                result[index][0] = cell_item[index + 2];
                result[index][1] = cell_item[index + 3];
                result[index][2] = cell_item[index + 4];
                break;
            case 2:
                result[index][0] = cell_item[index + 4];
                result[index][1] = cell_item[index + 5];
                result[index][2] = cell_item[index + 6];
                break;
        }
    }
    return result;
}

static void delete_cell_data(BOARD_CELL** data)
{
    for (size_t index = 0; index < ROW_COL_COUNT; index++)
    {
        free(data[index]);
    }
    free(data);
}

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

CTEST_BEGIN_TEST_SUITE(computer_player_ut)

CTEST_SUITE_INITIALIZE()
{
    CTEST_ASSERT_ARE_EQUAL(int, 0, umock_c_init(on_umock_c_error));
    //CTEST_ASSERT_ARE_EQUAL(int, 0, umocktypes_bool_register_types());
    CTEST_ASSERT_ARE_EQUAL(int, 0, umocktypes_stdint_register_types(), "umocktypes_stdint_register_types failed");

    REGISTER_UMOCK_ALIAS_TYPE(BOARD_INFO_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(PLAYER_TURN_COMPLETE, void*);

    REGISTER_UMOCK_ALIAS_TYPE(BOARD_CELL, int);

    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_malloc, my_mem_shim_malloc);
    REGISTER_GLOBAL_MOCK_FAIL_RETURN(mem_shim_malloc, NULL);
    REGISTER_GLOBAL_MOCK_HOOK(mem_shim_free, my_mem_shim_free);
}

CTEST_SUITE_CLEANUP()
{
    umock_c_deinit();
}

CTEST_FUNCTION_INITIALIZE()
{
    umock_c_reset_all_calls();
}

CTEST_FUNCTION_CLEANUP()
{
}

CTEST_FUNCTION(computer_player_create_succeed)
{
    //arrange
    PLAYER_MGR_HANDLE player_handle;
    GAME_INFO game_info = { .turn_count = 1, .game_outcome = OUTCOME_NO_RESULT, .game_board = NULL };

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    //act
    player_handle = computer_player_create(&game_info, CELL_X_PLAYER);

    //assert
    CTEST_ASSERT_IS_NOT_NULL(player_handle);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    computer_player_destroy(player_handle);
}

CTEST_FUNCTION(computer_player_destroy_succeed)
{
    //arrange
    GAME_INFO game_info = { .turn_count = 1, .game_outcome = OUTCOME_NO_RESULT, .game_board = NULL };
    PLAYER_MGR_HANDLE player_handle = computer_player_create(&game_info, CELL_X_PLAYER);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(free(IGNORED_ARG));

    //act
    computer_player_destroy(player_handle);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(computer_player_take_turn_success)
{
    //arrange
    for (size_t index = 0; index < sizeof(g_test_cell_matrix)/sizeof(g_test_cell_matrix[0]); index++)
    {
        BOARD_CELL** test_board_cell = create_board_cell_data(g_test_cell_matrix[index].cell_matrix);
        GAME_INFO game_info = { .turn_count = 4, .game_outcome = OUTCOME_NO_RESULT, .game_board = test_board_cell };
        PLAYER_MGR_HANDLE player_handle = computer_player_create(&game_info, CELL_X_PLAYER);
        umock_c_reset_all_calls();

        STRICT_EXPECTED_CALL(process_turn_complete(IGNORED_ARG, IGNORED_ARG, IGNORED_ARG));

        //act
        computer_player_take_turn(player_handle, process_turn_complete, NULL);

        //assert
        CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls(), "computer_player_take_turn_success failure in test %lu on %s", index, g_test_cell_matrix[index].win_direction);

        delete_cell_data(test_board_cell);
        computer_player_reset(player_handle);

        //cleanup
        computer_player_destroy(player_handle);
    }
}

CTEST_FUNCTION(computer_player_take_turn_stuck_issue_success)
{
    BOARD_CELL test_cell_board[9] = { CELL_X_PLAYER, CELL_0_PLAYER, CELL_X_PLAYER, CELL_0_PLAYER, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_X_PLAYER };
    BOARD_CELL** test_board_cell = create_board_cell_data(test_cell_board);

    //arrange
    GAME_INFO game_info = { .turn_count = 5, .game_outcome = OUTCOME_NO_RESULT, .game_board = test_board_cell };
    PLAYER_MGR_HANDLE player_handle = computer_player_create(&game_info, CELL_X_PLAYER);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(process_turn_complete(IGNORED_ARG, IGNORED_ARG, IGNORED_ARG));

    //act
    computer_player_take_turn(player_handle, process_turn_complete, NULL);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    delete_cell_data(test_board_cell);

    //cleanup
    computer_player_destroy(player_handle);
}

CTEST_FUNCTION(computer_process_click_handle_NULL_failure)
{
    //arrange
    ROW_COL_INFO rc_info = { 0, 0 };
    POS_INFO pos = {0, 0};

    //act
    computer_process_click(NULL, &rc_info);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(computer_process_click_success)
{
    //arrange
    ROW_COL_INFO rc_info = { 0, 0 };
    GAME_INFO game_info = { .turn_count = 1, .game_outcome = OUTCOME_NO_RESULT, .game_board = NULL };
    PLAYER_MGR_HANDLE player_handle = computer_player_create(&game_info, CELL_0_PLAYER);
    umock_c_reset_all_calls();

    //act
    computer_process_click(player_handle, &rc_info);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    computer_player_destroy(player_handle);
}

CTEST_FUNCTION(computer_player_reset_handle_NULL_success)
{
    //arrange

    //act
    computer_player_reset(NULL);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(computer_player_reset_success)
{
    //arrange
    GAME_INFO game_info = { .turn_count = 1, .game_outcome = OUTCOME_NO_RESULT, .game_board = NULL };
    PLAYER_MGR_HANDLE player_handle = computer_player_create(&game_info, CELL_0_PLAYER);
    umock_c_reset_all_calls();

    GAME_OUTCOME outcome = OUTCOME_NO_RESULT;

    //act
    computer_player_reset(player_handle);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    computer_player_destroy(player_handle);
}

CTEST_FUNCTION(computer_player_get_type_success)
{
    //arrange

    //act
    PLAYER_TYPE player_type = computer_player_get_type();

    //assert
    CTEST_ASSERT_ARE_EQUAL(int, PLAYER_TYPE_COMPUTER, player_type);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(computer_get_interface_description_success)
{
    //arrange

    //act
    const PLAYER_INTERFACE_DESC* interface_desc = computer_get_interface_description();

    //assert
    CTEST_ASSERT_IS_NOT_NULL(interface_desc);
    CTEST_ASSERT_IS_NOT_NULL(interface_desc->player_create);
    CTEST_ASSERT_IS_NOT_NULL(interface_desc->player_destroy);
    CTEST_ASSERT_IS_NOT_NULL(interface_desc->player_take_turn);
    CTEST_ASSERT_IS_NOT_NULL(interface_desc->player_process_click);
    CTEST_ASSERT_IS_NOT_NULL(interface_desc->player_get_type);
    CTEST_ASSERT_IS_NOT_NULL(interface_desc->player_reset);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_END_TEST_SUITE(computer_player_ut)
