// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <signal.h>

#include <SDL2/SDL.h>

#include "sdl2_tic_tac_toe/tic_tac_toe_const.h"

#include "sdl2_tic_tac_toe/game_board.h"
#include "sdl2_tic_tac_toe/sdl2_renderer.h"

#include "sdl2_tic_tac_toe/player_mgr.h"

#define MAX_TURN_COUNT          ROW_COL_COUNT*ROW_COL_COUNT
#define BOTTOM_BORDER           30
#define WIN_LINE_THICKNESS      5
#define RESET_BTN_WIDTH         90
#define STATUS_AREA_HEIGHT      20
#define STATUS_AREA_WIDTH       20
#define SPACER                  5

#define min_value(first,second) (first < second ? first : second)
#define get_player_cell_type(x) x == PLAYER_X ? CELL_X_PLAYER : CELL_0_PLAYER

const SDL_Color BTN_TEXT_COLOR = { .r = 255, .g = 255, .b = 255, .a = 255 };
const SDL_Color RESET_BTN_COLOR = { .r = 33, .g = 168, .b = 60, .a = 255 };
const SDL_Color PLAYER_X_COLOR = { .r = 255, .g = 50, .b = 50, .a = 255 };
const SDL_Color PLAYER_O_COLOR = { .r = 50, .g = 100, .b = 255, .a = 255 };
const SDL_Color BLACK_COLOR = { .r = 0, .g = 0, .b = 0, .a = 255 };
const SDL_Color GRID_COLOR = { .r = 255, .g = 255, .b = 255 };
const SDL_Color TIE_COLOR = { .r = 100, .g = 100, .b = 100 };

#define PLAYER_X        0
#define PLAYER_O        1
#define MAX_FMT_MSG_LEN 32

static const char* PLAYER_TURN_MSG_FMT = "Player %s Turn";
static const char* TIE_GAME_RESULT = "The game resulted in a tie";
static const char* PLAYER_HAS_WON_FMT = "Player %s has won the game!!!";

typedef struct PLAYER_INFO_TAG
{
    BOARD_CELL player_type;
    PLAYER_MGR_HANDLE player;
    const PLAYER_INTERFACE_DESC* player_interface;
} PLAYER_INFO;

typedef struct BOARD_INFO_TAG
{
    // Screen variable
    uint16_t screen_width;
    uint16_t screen_height;
    uint16_t cell_width;
    uint16_t cell_height;

    GAME_INFO game_info;
    BOARD_CELL** game_board;
    //uint8_t turn_count;

    RENDERER_INFO_HANDLE renderer;
    GAME_OUTCOME game_outcome;
    GAME_RESET_CLICK reset_click;

    PLAYER_INFO player_list[NUM_OF_PLAYERS];
    uint8_t curr_player_index;
    bool accept_input;
} BOARD_INFO;

static void write_status_message(BOARD_INFO* board_info, const char* msg, ...)
{
    char formatted_msg[32];
    va_list args;
    va_start(args, msg);

    (void)vsnprintf(formatted_msg, MAX_FMT_MSG_LEN, msg, args);
    int xpos = SPACER;
    int ypos = board_info->screen_height+SPACER;
    int width = board_info->screen_width-RESET_BTN_WIDTH-SPACER;
    int height = STATUS_AREA_HEIGHT;

    render_draw_text(board_info->renderer, formatted_msg, &BTN_TEXT_COLOR, xpos, ypos, width, height);
    va_end(args);
}

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
        if (board_info->game_info.turn_count >= MAX_TURN_COUNT)
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
    int xpos = board_info->screen_width - (SPACER+RESET_BTN_WIDTH);
    int ypos = board_info->screen_height+SPACER;
    int width = RESET_BTN_WIDTH;
    int height = STATUS_AREA_HEIGHT;
    //render_set_draw_color(board_info->renderer, &RESET_BTN_COLOR);
    //render_draw_fill_rect(board_info->renderer, xpos, ypos, width, STATUS_AREA_HEIGHT);
    // Add the reset button text
    render_draw_bg_text(board_info->renderer, "reset", &BTN_TEXT_COLOR, &RESET_BTN_COLOR, xpos, ypos, width, height);
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
            board_info->game_info.turn_count++;
            board_info->game_board[row][col] = player_type;
            board_info->game_outcome = evaluate_board_state(board_info, player_type);
        }
        else
        {
            // Add user error statment
            raise(SIGTRAP);
        }
    }
    return board_info->game_outcome;
}

static void reset_game(BOARD_INFO* board_info)
{
    board_info->game_info.turn_count = 0;
    board_info->game_outcome = OUTCOME_NO_RESULT;
    for (size_t index = 0; index < ROW_COL_COUNT; index++)
    {
        for (size_t inner = 0; inner < ROW_COL_COUNT; inner++)
        {
            board_info->game_board[index][inner] = CELL_EMPTY;
        }
    }
    for (size_t index = 0; index < 2; index++)
    {
        board_info->player_list[index].player_interface->player_reset(board_info->player_list[index].player);
    }
    board_info->curr_player_index = 0;
    board_info->accept_input = true;
}

static void render_game_win(void* user_ctx)
{
    BOARD_INFO* board_info = (BOARD_INFO*)user_ctx;
    render_status_area(board_info);
    render_grid(board_info, &GRID_COLOR);
    render_players(board_info);
    if (board_info->game_outcome != OUTCOME_NO_RESULT)
    {
        render_winner(board_info, get_player_cell_type(board_info->curr_player_index));
    }
}

static void process_turn_complete(const ROW_COL_INFO* rc_info, BOARD_CELL player_type, void* user_ctx)
{
    BOARD_INFO* board_info = (BOARD_INFO*)user_ctx;

    (void)mark_board_play(board_info, rc_info->row, rc_info->col, player_type);

    if (board_info->game_outcome == OUTCOME_NO_RESULT)
    {
        // Change to the new player
        board_info->curr_player_index = (board_info->curr_player_index == PLAYER_X) ? PLAYER_O : PLAYER_X;
        write_status_message(board_info, PLAYER_TURN_MSG_FMT, (board_info->curr_player_index == PLAYER_X) ? "O" : "X" );
    }
    else if (board_info->game_outcome == OUTCOME_GAME_TIE)
    {
        write_status_message(board_info, TIE_GAME_RESULT);
    }
    else
    {
        write_status_message(board_info, PLAYER_HAS_WON_FMT, (board_info->curr_player_index == PLAYER_X) ? "O" : "X" );
    }
}

static void mouse_click(BOARD_INFO* board_info, const POS_INFO* pos)
{
    if (pos->y > board_info->screen_height)
    {
        // See if we're in the reset button
        if (pos->x > board_info->screen_width - (SPACER+RESET_BTN_WIDTH) &&
            pos->x < board_info->screen_width - SPACER)
        {
            if (board_info->game_outcome != OUTCOME_NO_RESULT)
            {
                // Reset the screen
                reset_game(board_info);
            }
            board_info->game_outcome = OUTCOME_NO_RESULT;
        }
    }
    else
    {
        ROW_COL_INFO rc_info;
        rc_info.row = (uint8_t)(pos->y / board_info->cell_height);
        rc_info.col = (uint8_t)(pos->x / board_info->cell_width);
        //board_info->game_outcome = mark_board_play(board_info, row, col, get_player_cell_type(board_info->curr_player_index));
        board_info->player_list[board_info->curr_player_index].player_interface->player_process_click(board_info->player_list[board_info->curr_player_index].player, &rc_info);
    }
}

static uint32_t game_board_timer_callback(uint32_t interval, void* user_ctx)
{
    (void)interval;
    BOARD_INFO* board_info = (BOARD_INFO*)user_ctx;
    if (!board_info->accept_input)
    {
        SDL_Event event;
        memset(&event, 0, sizeof(event));
        event.type = SDL_USEREVENT;
        event.user.code = EVENT_COMPUTER_TURN;
        event.user.data1 = user_ctx;
        event.user.data2 = 0;
        SDL_PushEvent(&event);
        board_info->accept_input = true;
    }
    return TIMER_CALLBACK_INTERVAL;

}

static void game_board_window_event(void* user_ctx, SDL_Event* event, void* data)
{
    BOARD_INFO* board_info = (BOARD_INFO*)user_ctx;
    switch (event->type)
    {
        case SDL_MOUSEBUTTONUP:
        {
            SDL_MouseButtonEvent* mouse_event = (SDL_MouseButtonEvent*)data;
            POS_INFO pos = { .x = mouse_event->x, .y = mouse_event->y };
            if (board_info->accept_input)
            {
                if (board_info->game_outcome == OUTCOME_NO_RESULT)
                {
                    board_info->accept_input = false;
                }
                mouse_click(board_info, &pos);
            }
            break;
        }
        case SDL_WINDOWEVENT:
            if (event->window.event == SDL_WINDOWEVENT_SHOWN)
            {
                int n = 5;
                (void)n;
            }
            break;
        case SDL_USEREVENT:
            // TODO: Need to be the same
            if (event->user.code == EVENT_COMPUTER_TURN)
            {
                board_info->player_list[board_info->curr_player_index].player_interface->player_take_turn(board_info->player_list[board_info->curr_player_index].player, process_turn_complete, user_ctx);
            }
            else if (event->user.code == BEGIN_COMPUTER_TURN)
            {
                board_info->player_list[board_info->curr_player_index].player_interface->player_take_turn(board_info->player_list[board_info->curr_player_index].player, process_turn_complete, board_info);
            }
            write_status_message(board_info, PLAYER_TURN_MSG_FMT, (board_info->curr_player_index == PLAYER_X) ? "O" : "X" );
            break;
    }
}

static int create_players(BOARD_INFO* board_info, const PLAYER_INTERFACE_DESC* x_player, const PLAYER_INTERFACE_DESC* y_player)
{
    int result;
    board_info->player_list[PLAYER_X].player_interface = x_player;
    board_info->player_list[PLAYER_X].player_type = CELL_X_PLAYER;
    board_info->player_list[PLAYER_X].player = board_info->player_list[0].player_interface->player_create(&board_info->game_info, CELL_X_PLAYER);
    if (board_info->player_list[PLAYER_X].player == NULL)
    {
        result = __LINE__;
    }
    else
    {
        board_info->player_list[PLAYER_O].player_interface = y_player;
        board_info->player_list[PLAYER_O].player_type = CELL_0_PLAYER;
        board_info->player_list[PLAYER_O].player = board_info->player_list[PLAYER_O].player_interface->player_create(&board_info->game_info, CELL_0_PLAYER);
        if (board_info->player_list[PLAYER_O].player == NULL)
        {
            board_info->player_list[PLAYER_X].player_interface->player_destroy(board_info->player_list[0].player);
            result = __LINE__;
        }
        else
        {
            board_info->curr_player_index = PLAYER_X;
            result = 0;
        }
    }
    return result;
}

BOARD_INFO_HANDLE game_board_create(uint16_t screen_width, uint16_t screen_height, RENDERER_INFO_HANDLE renderer, const PLAYER_INTERFACE_DESC* x_player, const PLAYER_INTERFACE_DESC* y_player)
{
    BOARD_INFO* result;
    if (renderer == NULL || x_player == NULL || y_player == NULL)
    {
        result = NULL;
    }
    else
    {
        result = (BOARD_INFO*)malloc(sizeof(BOARD_INFO));
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
                    result->game_info.turn_count = 0;
                    result->game_info.game_outcome = OUTCOME_NO_RESULT;
                    result->game_info.game_board = result->game_board;

                    if (create_players(result, x_player, y_player) != 0)
                    {
                        free(result);
                        result = NULL;
                    }
                    else
                    {
                        result->screen_width = screen_width;
                        result->screen_height = screen_height - BOTTOM_BORDER;
                        result->cell_width = result->screen_width / ROW_COL_COUNT;
                        result->cell_height = result->screen_height / ROW_COL_COUNT;
                        result->renderer = renderer;
                        reset_game(result);
                        result->accept_input = true;
                    }
                }
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

void game_board_process_game_loop(BOARD_INFO_HANDLE handle)
{
    SDL_Event event;
    memset(&event, 0, sizeof(event));
    event.type = SDL_USEREVENT;
    event.user.code = BEGIN_COMPUTER_TURN;
    event.user.data1 = handle;
    event.user.data2 = 0;
    SDL_PushEvent(&event);

    render_process_game_loop(handle->renderer, render_game_win, handle, game_board_window_event, handle, game_board_timer_callback, handle);
}
