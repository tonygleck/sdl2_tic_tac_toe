#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 3.2)

set(theseTestsName game_board_ut)

set(${theseTestsName}_test_files
    ${theseTestsName}.c
)

set(${theseTestsName}_c_files
    sdl_mocked.c
)

set(${theseTestsName}_h_files
)

build_test_project(${theseTestsName} "tests/tic_tac_toe_tests")
