// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "sdl2_tic_tac_toe/game_board.h"
#include "sdl2_tic_tac_toe/sdl2_renderer.h"

#define MAX_TURN_COUNT          ROW_COL_COUNT*ROW_COL_COUNT
#define BOTTOM_BORDER           30
#define WIN_LINE_THICKNESS      5
#define RESET_BTN_WIDTH         60
#define RESET_BTN_HEIGHT        20
#define SPACER                  5

#define min_value(first,second) (first < second ? first : second)

const SDL_Color RESET_BTN_COLOR = { .r = 33, .g = 168, .b = 60, .a = 255 };
const SDL_Color PLAYER_X_COLOR = { .r = 255, .g = 50, .b = 50, .a = 255 };
const SDL_Color PLAYER_O_COLOR = { .r = 50, .g = 100, .b = 255, .a = 255 };
const SDL_Color BLACK_COLOR = { .r = 0, .g = 0, .b = 0, .a = 255 };

typedef struct BOARD_INFO_TAG
{
    BOARD_CELL** game_board;
    uint16_t screen_width;
    uint16_t screen_height;
    uint16_t cell_width;
    uint16_t cell_height;
    RENDERER_INFO_HANDLE renderer;
    uint8_t turn_count;
    GAME_OUTCOME game_outcome;
    GAME_RESET_CLICK reset_click;
    void* reset_user_ctx;
} BOARD_INFO;

static GAME_OUTCOME evaluate_diagonal_win(BOARD_INFO* board_info, BOARD_CELL player_type)
{
    GAME_OUTCOME result;
    if (((board_info->game_board[0][0] == player_type) &&
        (board_info->game_board[1][1] == player_type) &&
        (board_info->game_board[2][2] == player_type)))
    {
        result = OUTCOME_WIN_DIAG_1;
    }
    else if (
        ((board_info->game_board[0][2] == player_type) &&
        (board_info->game_board[1][1] == player_type) &&
        (board_info->game_board[2][0] == player_type)) )
    {
        result = OUTCOME_WIN_DIAG_2;
    }
    else
    {
        result = OUTCOME_NO_RESULT;
    }
    return result;
}

static GAME_OUTCOME evaluate_horizontal_win(BOARD_INFO* board_info, BOARD_CELL player_type)
{
    GAME_OUTCOME current_col = OUTCOME_WIN_ROW_0;
    bool winner_found = false;
    for (int index = 0; index < ROW_COL_COUNT; index++)
    {
        for (int inner = 0; inner < ROW_COL_COUNT; inner++)
        {
            if (board_info->game_board[index][inner] == player_type)
            {
                winner_found = true;
            }
            else
            {
                winner_found = false;
                break;
            }
        }
        if (winner_found)
        {
            // We have a winner
            break;
        }
        current_col += 1;
    }
    return winner_found ? current_col : OUTCOME_NO_RESULT;
}

static GAME_OUTCOME evaluate_vertical_win(BOARD_INFO* board_info, BOARD_CELL player_type)
{
    GAME_OUTCOME current_row = OUTCOME_WIN_COL_0;
    bool winner_found = false;
    for (int index = 0; index < ROW_COL_COUNT; index++)
    {
        for (int inner = 0; inner < ROW_COL_COUNT; inner++)
        {
            if (board_info->game_board[inner][index] == player_type)
            {
                winner_found = true;
            }
            else
            {
                winner_found = false;
                break;
            }
        }
        if (winner_found)
        {
            // We have a winner
            break;
        }
        current_row += 1;
    }
    return winner_found ? current_row : OUTCOME_NO_RESULT;
}

static GAME_OUTCOME evaluate_board_state(BOARD_INFO* board_info, BOARD_CELL player_type)
{
    GAME_OUTCOME result;
    // Check winning combinations
    if ((result = evaluate_vertical_win(board_info, player_type)) != OUTCOME_NO_RESULT ||
        (result = evaluate_horizontal_win(board_info, player_type)) != OUTCOME_NO_RESULT ||
        (result = evaluate_diagonal_win(board_info, player_type)) != OUTCOME_NO_RESULT
       )
    {
        // We have a Winner
        board_info->game_outcome = result;
    }
    else
    {
        if (board_info->turn_count >= MAX_TURN_COUNT)
        {
            result = OUTCOME_GAME_TIE;
        }
        else
        {
            result = OUTCOME_NO_RESULT;
        }
    }
    return result;
}

static void render_x_player(BOARD_INFO* board_info, uint8_t row, uint8_t col, const SDL_Color* color)
{
    const float half_box_side = (float)(min_value(board_info->cell_width, board_info->cell_height) * 0.25);
    const float center_x = (float)(board_info->cell_width * 0.5 + col * board_info->cell_width);
    const float center_y = (float)(board_info->cell_height * 0.5 + row * board_info->cell_height);

    render_draw_thick_line(board_info->renderer, (int16_t)(center_x - half_box_side),
        (int16_t)(center_y - half_box_side), (int16_t)(center_x + half_box_side), (int16_t)(center_y + half_box_side), 10, color);
    render_draw_thick_line(board_info->renderer, (int16_t)(center_x + half_box_side),
        (int16_t)(center_y - half_box_side), (int16_t)(center_x - half_box_side), (int16_t)(center_y + half_box_side), 10, color);
}

static void render_y_player(BOARD_INFO* board_info, uint8_t row, uint8_t col, const SDL_Color* color)
{
    const float half_box_side = (float)(min_value(board_info->cell_width, board_info->cell_height) * 0.25);
    const float center_x = (float)(board_info->cell_width * 0.5 + col * board_info->cell_width);
    const float center_y = (float)(board_info->cell_height * 0.5 + row * board_info->cell_height);

    render_draw_filled_circle(board_info->renderer, (int16_t)center_x, (int16_t)center_y, (int16_t)(half_box_side + 5), color);
    render_draw_filled_circle(board_info->renderer, (int16_t)center_x, (int16_t)center_y, (int16_t)(half_box_side - 5), &BLACK_COLOR);
}

static void render_status_area(BOARD_INFO* board_info)
{
    // Draw Reset button
    render_set_draw_color(board_info->renderer, &RESET_BTN_COLOR);

    render_draw_fill_rect(board_info->renderer, board_info->screen_width - (SPACER+RESET_BTN_WIDTH),
        board_info->screen_height+SPACER, RESET_BTN_WIDTH, RESET_BTN_HEIGHT);
}

static void render_players(BOARD_INFO* board_info)
{
    for (uint8_t index = 0; index < ROW_COL_COUNT; index++)
    {
        for (uint8_t inner = 0; inner < ROW_COL_COUNT; inner++)
        {
            switch (board_info->game_board[index][inner])
            {
                case CELL_X_PLAYER:
                    render_x_player(board_info, index, inner, &PLAYER_X_COLOR);
                    break;
                case CELL_0_PLAYER:
                    render_y_player(board_info, index, inner, &PLAYER_O_COLOR);
                    break;
                case CELL_EMPTY:
                default:
                    break;
            }
        }
    }
}

static void render_grid(BOARD_INFO* board_info, const SDL_Color* color)
{
    render_set_draw_color(board_info->renderer, color);

    for (int index = 1; index < ROW_COL_COUNT; index++)
    {
        render_draw_line(board_info->renderer, index*board_info->cell_width, 0, index*board_info->cell_width, board_info->screen_height);
        render_draw_line(board_info->renderer, 0, index*board_info->cell_height, board_info->screen_width, index*board_info->cell_height);
    }
}

static void render_winner(BOARD_INFO* board_info, BOARD_CELL player_type)
{
    const SDL_Color* line_color = player_type == CELL_X_PLAYER ? &PLAYER_X_COLOR : &PLAYER_O_COLOR;
    int16_t xpos;
    int16_t ypos;
    int16_t endx;
    int16_t endy;
    switch (board_info->game_outcome)
    {
        case OUTCOME_WIN_COL_0:
            xpos = endx = board_info->cell_width/2;
            ypos = SPACER;
            endy = board_info->screen_height - SPACER;
            break;
        case OUTCOME_WIN_COL_1:
            xpos = endx = (board_info->cell_width/2)+board_info->cell_width;
            ypos = SPACER;
            endy = board_info->screen_height - SPACER;
            break;
        case OUTCOME_WIN_COL_2:
            xpos = endx = (board_info->cell_width/2)+(board_info->cell_width*2);
            ypos = SPACER;
            endy = board_info->screen_height - SPACER;
            break;
        case OUTCOME_WIN_ROW_0:
            xpos = SPACER;
            endx = board_info->screen_width - SPACER;
            ypos = endy = board_info->cell_height/2;
            break;
        case OUTCOME_WIN_ROW_1:
            xpos = SPACER;
            endx = board_info->screen_width - SPACER;
            ypos = endy = board_info->cell_height/2+board_info->cell_height;
            break;
        case OUTCOME_WIN_ROW_2:
            xpos = SPACER;
            endx = board_info->screen_width - SPACER;
            ypos = endy = board_info->cell_height/2+(board_info->cell_height*2);
            break;
        case OUTCOME_WIN_DIAG_1:
            ypos = xpos = SPACER;
            endx = board_info->screen_width - SPACER;
            endy = board_info->screen_height - SPACER;
            break;
        case OUTCOME_WIN_DIAG_2:
            xpos = board_info->screen_width - SPACER;
            ypos = SPACER;
            endx = SPACER;
            endy = board_info->screen_height - SPACER;
            break;
        default:
            return;
            break;
    }
    render_draw_thick_line(board_info->renderer, xpos, ypos, endx, endy, WIN_LINE_THICKNESS, line_color);
}

static GAME_OUTCOME mark_board_play(BOARD_INFO* board_info, int row, int col, BOARD_CELL player_type)
{
    if (board_info->game_outcome == OUTCOME_NO_RESULT)
    {
        if (board_info->game_board[row][col] == CELL_EMPTY)
        {
            board_info->turn_count++;
            board_info->game_board[row][col] = player_type;
            if (evaluate_board_state(board_info, player_type) == OUTCOME_NO_RESULT)
            {
            }
        }
        else
        {
            // Add user error statment
        }
    }
    return board_info->game_outcome;
}

static void reset_game(BOARD_INFO* board_info)
{
    board_info->turn_count = 0;
    board_info->game_outcome = OUTCOME_NO_RESULT;
    for (size_t index = 0; index < ROW_COL_COUNT; index++)
    {
        for (size_t inner = 0; inner < ROW_COL_COUNT; inner++)
        {
            board_info->game_board[index][inner] = CELL_EMPTY;
        }
    }
}

BOARD_INFO_HANDLE game_board_create(uint16_t screen_width, uint16_t screen_height, RENDERER_INFO_HANDLE renderer, GAME_RESET_CLICK reset_click, void* user_ctx)
{
    BOARD_INFO* result = (BOARD_INFO*)malloc(sizeof(BOARD_INFO));
    if (result == NULL)
    {
        printf("Failure allocating board item");
    }
    else
    {
        //size_t alloc_len = ROW_COL_COUNT * sizeof(BOARD_CELL*) + sizeof(BOARD_CELL)*ROW_COL_COUNT*ROW_COL_COUNT;
        if ((result->game_board = malloc(ROW_COL_COUNT * sizeof(BOARD_CELL*))) == NULL)
        {
            free(result);
            result = NULL;
        }
        else
        {
            for (size_t index = 0; index < ROW_COL_COUNT; index++)
            {
                if ((result->game_board[index] = malloc(ROW_COL_COUNT * sizeof(BOARD_CELL))) == NULL)
                {
                    free(result);
                    result = NULL;
                    break;
                }
            }
            if (result != NULL)
            {
                result->screen_width = screen_width;
                result->screen_height = screen_height - BOTTOM_BORDER;
                result->cell_width = result->screen_width / ROW_COL_COUNT;
                result->cell_height = result->screen_height / ROW_COL_COUNT;
                result->renderer = renderer;
                reset_game(result);
                result->reset_click = reset_click;;
                result->reset_user_ctx = user_ctx;
            }
        }
    }
    return result;
}

void game_board_destroy(BOARD_INFO_HANDLE handle)
{
    if (handle != NULL)
    {
        for (size_t index = 0; index < ROW_COL_COUNT; index++)
        {
            free(handle->game_board[index]);
        }
        free(handle->game_board);

        free(handle);
    }
}

void game_board_render(BOARD_INFO_HANDLE handle, const SDL_Color* color, BOARD_CELL player_type)
{
    if (handle != NULL)
    {
        render_status_area(handle);
        render_grid(handle, color);
        render_players(handle);
        if (handle->game_outcome != OUTCOME_NO_RESULT)
        {
            render_winner(handle, player_type);
        }
    }
}

GAME_OUTCOME game_board_play(BOARD_INFO_HANDLE handle, uint8_t row, uint8_t col, BOARD_CELL player_type)
{
    GAME_OUTCOME result;
    if (handle != NULL)
    {
        result = mark_board_play(handle, row, col, player_type);
    }
    else
    {
        result = OUTCOME_NO_RESULT;
    }
    return result;
}

GAME_OUTCOME game_board_click(BOARD_INFO_HANDLE handle, const POS_INFO* pos, BOARD_CELL player_type)
{
    if (handle != NULL)
    {
        if (pos->y > handle->screen_height)
        {
            // See if we're in the reset button
            if (pos->x > handle->screen_width - (SPACER+RESET_BTN_WIDTH) &&
                pos->x < handle->screen_width - SPACER)
            {
                if (handle->game_outcome != OUTCOME_NO_RESULT)
                {
                    // Reset the screen
                    reset_game(handle);
                    handle->reset_click(handle->reset_user_ctx);
                }
            }
            handle->game_outcome = OUTCOME_NO_RESULT;
        }
        else
        {
            uint8_t row = (uint8_t)(pos->y / handle->cell_height);
            uint8_t col = (uint8_t)(pos->x / handle->cell_width);
            handle->game_outcome = mark_board_play(handle, row, col, player_type);
        }
    }
    else
    {
        handle->game_outcome = OUTCOME_NO_RESULT;
    }
    return handle->game_outcome;
}

BOARD_CELL** game_board_get_board(BOARD_INFO_HANDLE handle)
{
    return handle->game_board;
}