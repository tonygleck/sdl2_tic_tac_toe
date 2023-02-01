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

#include "sdl2_tic_tac_toe/manual_player.h"

//TEST_DEFINE_ENUM_TYPE(CELL_LOCATION, CELL_LOCATION_VALUES)

#define WIN_DIR_HORIZ   "Horizontal"
#define WIN_DIR_VERT    "Vertical"
#define WIN_DIR_DIAG    "Diagonal"
#define WIN_DIR_NONE    "None"

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

CTEST_BEGIN_TEST_SUITE(manual_player_ut)

CTEST_SUITE_INITIALIZE()
{
    CTEST_ASSERT_ARE_EQUAL(int, 0, umock_c_init(on_umock_c_error));
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

CTEST_FUNCTION(manual_player_create_board_NULL_fail)
{
    //arrange
    PLAYER_MGR_HANDLE player_handle;

    //act
    player_handle = manual_player_create(NULL, CELL_X_PLAYER);

    //assert
    CTEST_ASSERT_IS_NULL(player_handle);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    manual_player_destroy(player_handle);
}

CTEST_FUNCTION(manual_player_create_succeed)
{
    //arrange
    GAME_INFO game_info = { .turn_count = 1, .game_outcome = OUTCOME_NO_RESULT, .game_board = NULL };
    PLAYER_MGR_HANDLE player_handle;

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    //act
    player_handle = manual_player_create(&game_info, CELL_X_PLAYER);

    //assert
    CTEST_ASSERT_IS_NOT_NULL(player_handle);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    manual_player_destroy(player_handle);
}

CTEST_FUNCTION(manual_player_create_malloc_fail)
{
    //arrange
    GAME_INFO game_info = { .turn_count = 1, .game_outcome = OUTCOME_NO_RESULT, .game_board = NULL };
    PLAYER_MGR_HANDLE player_handle;

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG))
        .SetReturn(NULL);

    //act
    player_handle = manual_player_create(&game_info, CELL_X_PLAYER);

    //assert
    CTEST_ASSERT_IS_NULL(player_handle);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    manual_player_destroy(player_handle);
}

CTEST_FUNCTION(manual_player_destroy_handle_NULL)
{
    //arrange

    //act
    manual_player_destroy(NULL);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(manual_player_destroy_succeed)
{
    //arrange
    GAME_INFO game_info = { .turn_count = 1, .game_outcome = OUTCOME_NO_RESULT, .game_board = NULL };
    PLAYER_MGR_HANDLE player_handle = manual_player_create(&game_info, CELL_X_PLAYER);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(free(IGNORED_ARG));

    //act
    manual_player_destroy(player_handle);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(manual_process_click_without_turn_complete_cb_success)
{
    //arrange
    GAME_INFO game_info = { .turn_count = 1, .game_outcome = OUTCOME_NO_RESULT, .game_board = NULL };
    PLAYER_MGR_HANDLE player_handle = manual_player_create(&game_info, CELL_X_PLAYER);
    umock_c_reset_all_calls();

    ROW_COL_INFO rc_info = { 0, 0 };

    //act
    manual_process_click(player_handle, &rc_info);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    manual_player_destroy(player_handle);
}

CTEST_FUNCTION(manual_process_click_success)
{
    //arrange
    ROW_COL_INFO rc_info = { 0, 0 };
    GAME_INFO game_info = { .turn_count = 1, .game_outcome = OUTCOME_NO_RESULT, .game_board = NULL };
    PLAYER_MGR_HANDLE player_handle = manual_player_create(&game_info, CELL_X_PLAYER);
    manual_player_take_turn(player_handle, process_turn_complete, NULL);
    umock_c_reset_all_calls();

    STRICT_EXPECTED_CALL(process_turn_complete(IGNORED_ARG, IGNORED_ARG, IGNORED_ARG));

    //act
    manual_process_click(player_handle, &rc_info);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    manual_player_destroy(player_handle);
}

CTEST_FUNCTION(manual_player_take_turn_player_handle_NULL_fail)
{
    //arrange

    //act
    manual_player_take_turn(NULL, process_turn_complete, NULL);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(manual_player_take_turn_success)
{
    //arrange
    GAME_INFO game_info = { .turn_count = 1, .game_outcome = OUTCOME_NO_RESULT, .game_board = NULL };
    PLAYER_MGR_HANDLE player_handle = manual_player_create(&game_info, CELL_X_PLAYER);
    umock_c_reset_all_calls();

    //act
    manual_player_take_turn(player_handle, process_turn_complete, NULL);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    manual_player_destroy(player_handle);
}

CTEST_FUNCTION(manual_player_reset_player_handle_NULL_fail)
{
    //arrange

    //act
    manual_player_reset(NULL);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(manual_player_reset_success)
{
    //arrange
    GAME_INFO game_info = { .turn_count = 1, .game_outcome = OUTCOME_NO_RESULT, .game_board = NULL };
    PLAYER_MGR_HANDLE player_handle = manual_player_create(&game_info, CELL_X_PLAYER);
    umock_c_reset_all_calls();

    //act
    manual_player_reset(player_handle);

    //assert
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    manual_player_destroy(player_handle);
}

CTEST_FUNCTION(manual_player_get_type_success)
{
    //arrange

    //act
    PLAYER_TYPE player_type = manual_player_get_type();

    //assert
    CTEST_ASSERT_ARE_EQUAL(int, PLAYER_TYPE_MANUAL, player_type);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
}

CTEST_FUNCTION(manual_get_interface_description_success)
{
    //arrange

    //act
    const PLAYER_INTERFACE_DESC* interface_desc = manual_get_interface_description();

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

CTEST_END_TEST_SUITE(manual_player_ut)
