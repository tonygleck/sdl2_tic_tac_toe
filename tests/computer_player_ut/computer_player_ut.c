// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>

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
#include "umock_c/umocktypes_bool.h"

#define ENABLE_MOCKS
#include "sdl2_tic_tac_toe/game_board.h"
#include "lib-util-c/sys_debug_shim.h"
#undef ENABLE_MOCKS

#include "umock_c/umock_c_prod.h"

#include "sdl2_tic_tac_toe/computer_player.h"

//TEST_DEFINE_ENUM_TYPE(CELL_LOCATION, CELL_LOCATION_VALUES)

static BOARD_INFO_HANDLE g_board_info = (BOARD_INFO_HANDLE)0x2345;

BOARD_CELL** create_board_cell_data(void)
{
    BOARD_CELL** result = malloc(ROW_COL_COUNT * sizeof(BOARD_CELL*));
    CTEST_ASSERT_IS_NOT_NULL(result);

    for (size_t index = 0; index < ROW_COL_COUNT; index++)
    {
        result[index] = malloc(ROW_COL_COUNT * sizeof(BOARD_CELL));
        CTEST_ASSERT_IS_NOT_NULL(result[index]);

        switch (index)
        {
            case 0:
                result[index][0] = CELL_EMPTY;
                result[index][1] = CELL_X_PLAYER;
                result[index][2] = CELL_X_PLAYER;
                break;
            case 1:
                result[index][0] = CELL_EMPTY;
                result[index][1] = CELL_EMPTY;
                result[index][2] = CELL_0_PLAYER;
                break;
            case 2:
                result[index][0] = CELL_EMPTY;
                result[index][1] = CELL_0_PLAYER;
                result[index][2] = CELL_EMPTY;
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
    CTEST_ASSERT_ARE_EQUAL(int, 0, umocktypes_bool_register_types());

    REGISTER_UMOCK_ALIAS_TYPE(BOARD_INFO_HANDLE, void*);

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
    COMPUTER_PLAYER_HANDLE player_handle;

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    //act
    player_handle = computer_player_create(g_board_info, CELL_X_PLAYER);

    //assert
    CTEST_ASSERT_IS_NOT_NULL(player_handle);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    computer_player_destroy(player_handle);
}

CTEST_FUNCTION(computer_player_destroy_succeed)
{
    //arrange
    COMPUTER_PLAYER_HANDLE player_handle = computer_player_create(g_board_info, CELL_X_PLAYER);
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
    COMPUTER_PLAYER_HANDLE player_handle = computer_player_create(g_board_info, CELL_X_PLAYER);
    umock_c_reset_all_calls();

    BOARD_CELL** test_board_cell = create_board_cell_data();

    STRICT_EXPECTED_CALL(game_board_get_board(IGNORED_ARG))
        .SetReturn(test_board_cell);

    //act
    CELL_LOCATION cell_loc = computer_player_take_turn(player_handle);

    //assert
    //CTEST_ASSERT_ARE_EQUAL(CELL_LOCATION, CELL_LOC_R0_C0, cell_loc);
    CTEST_ASSERT_ARE_EQUAL(int, CELL_LOC_R0_C0, cell_loc);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    computer_player_destroy(player_handle);
    delete_cell_data(test_board_cell);
}

CTEST_END_TEST_SUITE(computer_player_ut)
