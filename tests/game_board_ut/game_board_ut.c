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

#include <SDL2/SDL.h>

// Include the test tools.
//#include "testrunnerswitcher.h"
#include "macro_utils/macro_utils.h"
#include "umock_c/umock_c.h"
#include "umock_c/umocktypes_stdint.h"

#define ENABLE_MOCKS
#include "sdl2_tic_tac_toe/sdl2_renderer.h"
#include "lib-util-c/sys_debug_shim.h"
#include "sdl_mocked.h"

#include "sdl2_tic_tac_toe/player_mgr.h"

MOCKABLE_FUNCTION(, PLAYER_MGR_HANDLE, test_player_create, GAME_INFO*, game_info, BOARD_CELL, player_type);
MOCKABLE_FUNCTION(, void, test_player_destroy, PLAYER_MGR_HANDLE, handle);
MOCKABLE_FUNCTION(, void, test_player_take_turn, PLAYER_MGR_HANDLE, handle, PLAYER_TURN_COMPLETE, turn_complete, void*, user_ctx);
MOCKABLE_FUNCTION(, void, test_process_click, PLAYER_MGR_HANDLE, player_handle, const ROW_COL_INFO*, rc_info);
MOCKABLE_FUNCTION(, PLAYER_TYPE, test_player_get_type);
MOCKABLE_FUNCTION(, void, test_player_reset, PLAYER_MGR_HANDLE, player_handle);
#undef ENABLE_MOCKS

#include "umock_c/umock_c_prod.h"

#include "sdl2_tic_tac_toe/game_board.h"

//TEST_DEFINE_ENUM_TYPE(BOARD_CELL, BOARD_CELL_VALUE);

const PLAYER_INTERFACE_DESC test_player_interface_x =
{
    test_player_create,
    test_player_destroy,
    test_player_take_turn,
    test_process_click,
    test_player_get_type,
    test_player_reset
};

const PLAYER_INTERFACE_DESC test_player_interface_y =
{
    test_player_create,
    test_player_destroy,
    test_player_take_turn,
    test_process_click,
    test_player_get_type,
    test_player_reset
};

static PLAYER_MGR_HANDLE my_test_player_create(GAME_INFO* game_info, BOARD_CELL player_type)
{
    (void)game_info;
    (void)player_type;
    return my_mem_shim_malloc(1);
}

static void my_test_player_destroy(PLAYER_MGR_HANDLE handle)
{
    my_mem_shim_free(handle);
}

#define TEST_BOARD_WIDTH        100
#define TEST_BOARD_LENGTH       200
static RENDERER_INFO_HANDLE g_renderer_handle = (RENDERER_INFO_HANDLE)0x2345;

MU_DEFINE_ENUM_STRINGS(UMOCK_C_ERROR_CODE, UMOCK_C_ERROR_CODE_VALUES)
static void on_umock_c_error(UMOCK_C_ERROR_CODE error_code)
{
    CTEST_ASSERT_FAIL("umock_c reported error :%s", MU_ENUM_TO_STRING(UMOCK_C_ERROR_CODE, error_code));
}

CTEST_BEGIN_TEST_SUITE(game_board_ut)

CTEST_SUITE_INITIALIZE()
{
    CTEST_ASSERT_ARE_EQUAL(int, 0, umock_c_init(on_umock_c_error));
    CTEST_ASSERT_ARE_EQUAL(int, 0, umocktypes_stdint_register_types(), "umocktypes_stdint_register_types failed");

    REGISTER_GLOBAL_MOCK_HOOK(test_player_create, my_test_player_create);
    REGISTER_GLOBAL_MOCK_HOOK(test_player_destroy, my_test_player_destroy);

    REGISTER_UMOCK_ALIAS_TYPE(GAME_RESET_CLICK, void*);
    REGISTER_UMOCK_ALIAS_TYPE(PLAYER_MGR_HANDLE, void*);
    REGISTER_UMOCK_ALIAS_TYPE(RENDERER_INFO_HANDLE, void*);

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

CTEST_FUNCTION(game_board_create_render_NULL_fail)
{
    //arrange
    BOARD_INFO_HANDLE board_handle;

    //act
    board_handle = game_board_create(TEST_BOARD_WIDTH, TEST_BOARD_LENGTH, NULL, &test_player_interface_x, &test_player_interface_y);

    //assert
    CTEST_ASSERT_IS_NULL(board_handle);
    CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());

    //cleanup
    game_board_destroy(board_handle);
}

CTEST_FUNCTION(game_board_create_success)
{
    //arrange
    BOARD_INFO_HANDLE board_handle;

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));
    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));
    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));
    STRICT_EXPECTED_CALL(malloc(IGNORED_ARG));

    STRICT_EXPECTED_CALL(test_player_create(IGNORED_ARG, IGNORED_ARG));
    STRICT_EXPECTED_CALL(test_player_create(IGNORED_ARG, IGNORED_ARG));

    STRICT_EXPECTED_CALL(test_player_reset(IGNORED_ARG));
    STRICT_EXPECTED_CALL(test_player_reset(IGNORED_ARG));

    //act
    board_handle = game_board_create(TEST_BOARD_WIDTH, TEST_BOARD_LENGTH, g_renderer_handle, &test_player_interface_x, &test_player_interface_y);

    //assert
    CTEST_ASSERT_IS_NOT_NULL(board_handle);
    //CTEST_ASSERT_ARE_EQUAL(char_ptr, umock_c_get_expected_calls(), umock_c_get_actual_calls());
    //printf("expect: %s\n", umock_c_get_expected_calls());
    //printf("actual: %s\n", umock_c_get_actual_calls());

    //cleanup
    game_board_destroy(board_handle);
}


CTEST_END_TEST_SUITE(game_board_ut)
