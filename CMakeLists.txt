#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 3.5)

#Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(sdl2_tic_tac_toe)

option(sdl2_tic_tac_toe_ut "Include unittest in build" OFF)
option(sdl2_tic_tac_toe_samples "Include samples in build" OFF)

if (CMAKE_BUILD_TYPE MATCHES "Debug" AND NOT WIN32)
    set(DEBUG_CONFIG ON)
    set(ENABLE_COVERAGE ON)
else()
    set(ENABLE_COVERAGE OFF)
    set(DEBUG_CONFIG OFF)
endif()

include("${PROJECT_SOURCE_DIR}/cmake_configs/proj_config.cmake")

# You need to include the following dependencies
if (NOT TARGET c_build_tools)
    set(run_traceability OFF)
    set(build_traceability_tool OFF)
    add_subdirectory(deps/c-build-tools)
    set_default_build_options()
endif()
if (NOT TARGET macro_utils_c)
    add_subdirectory(deps/macro-utils-c)
    include_directories(${MACRO_UTILS_INC_FOLDER})
endif()
if (NOT TARGET c_logging)
    add_subdirectory(${PROJECT_SOURCE_DIR}/deps/c-logging)
    include_directories(${PROJECT_SOURCE_DIR}/deps/c-logging/inc)
endif()
if (NOT TARGET umock_c)
    add_subdirectory(${PROJECT_SOURCE_DIR}/deps/umock-c)
    include_directories(${PROJECT_SOURCE_DIR}/deps/umock-c/inc)
endif()
if (NOT TARGET lib-util-c)
    add_subdirectory(${PROJECT_SOURCE_DIR}/deps/lib-util-c)
    include_directories(${PROJECT_SOURCE_DIR}/deps/lib-util-c/inc)
endif()
# if (NOT TARGET patchcords)
#     add_subdirectory(${PROJECT_SOURCE_DIR}/deps/patchcords)
#     include_directories(${PROJECT_SOURCE_DIR}/deps/patchcords/inc)
# endif()

# these are the C source files
set(source_c_files
    ${PROJECT_SOURCE_DIR}/src/computer_player.c
    ${PROJECT_SOURCE_DIR}/src/game_board.c
    ${PROJECT_SOURCE_DIR}/src/main.c
    ${PROJECT_SOURCE_DIR}/src/manual_player.c
    ${PROJECT_SOURCE_DIR}/src/sdl2_renderer.c
    ${PROJECT_SOURCE_DIR}/src/sdl2_win.c
)

# these are the C headers
set(source_h_files
    ${PROJECT_SOURCE_DIR}/inc/sdl2_tic_tac_toe/computer_player.h
    ${PROJECT_SOURCE_DIR}/inc/sdl2_tic_tac_toe/game_board.h
    ${PROJECT_SOURCE_DIR}/inc/sdl2_tic_tac_toe/manual_player.h
    ${PROJECT_SOURCE_DIR}/inc/sdl2_tic_tac_toe/player_mgr.h
    ${PROJECT_SOURCE_DIR}/inc/sdl2_tic_tac_toe/sdl2_renderer.h
    ${PROJECT_SOURCE_DIR}/inc/sdl2_tic_tac_toe/sdl2_win.h
)

include_directories(${PROJECT_SOURCE_DIR}/inc)

#this is the product (a library)
add_executable(sdl2_tic_tac_toe ${source_c_files} ${source_h_files})

# TODO: Add any libs that you need
target_link_libraries(sdl2_tic_tac_toe lib-util-c SDL2 SDL2_gfx SDL2_ttf)
# if (UNIX)
#     target_link_libraries(sdl2_tic_tac_toe cord_berkley)
# elseif(WIN32)
#     target_link_libraries(sdl2_tic_tac_toe cord_winsock)
# endif (UNIX)

addCompileSettings(sdl2_tic_tac_toe)
compileTargetAsC99(sdl2_tic_tac_toe)

if (${sdl2_tic_tac_toe_ut})
    enable_testing()
    include (CTest)

    include("${CMAKE_CURRENT_LIST_DIR}/cmake_configs/proj_testing.cmake")
    enable_coverage_testing()

    # TODO: Add testing framework code
    if (NOT TARGET ctest)
        add_subdirectory(${PROJECT_SOURCE_DIR}/deps/ctest)
        include_directories(${CTEST_INC_FOLDER})
    endif()

    add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()

if (${sdl2_tic_tac_toe_samples})
    add_subdirectory(samples)
endif()
