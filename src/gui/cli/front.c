#include "front.h"

#define _POSIX_C_SOURCE 200809L
// relying on the POSIX nanosleep()
// man 2 nanosleep
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "../../game/lib.h"
#include "../../game/tetris/defines.h"
#include "../../common/time_utils.h"

void f_init_colours(void) {
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_CYAN, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  init_pair(4, COLOR_WHITE, COLOR_BLACK);
  init_pair(5, COLOR_GREEN, COLOR_BLACK);
  init_pair(6, COLOR_BLUE, COLOR_BLACK);
  init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(8, COLOR_BLUE, COLOR_BLACK);
  init_pair(9, COLOR_BLACK, COLOR_BLACK);
}

void init_cli() {
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  if (has_colors()) {
    f_init_colours();
  }
  const int screen_center_row = getmaxy(stdscr) / 2;
  const int screen_center_column = getmaxx(stdscr) / 2;
  mvprintw(screen_center_row, screen_center_column, "s - start/restart");
  mvprintw(screen_center_row + 1, screen_center_column, "p - pause");
  mvprintw(screen_center_row + 2, screen_center_column, "space - rotate");
  mvprintw(screen_center_row + 3, screen_center_column, "q - exit");
  refresh();
}

void free_cli(void) {
  endwin();
  // valgrind still shows "possibly lost"
  // https://stackoverflow.com/questions/32410125/valgrind-shows-memory-leaks-from-ncurses-commands-after-using-appropriate-free
  // https://invisible-island.net/ncurses/ncurses.faq.html#config_leaks
}

#define IFACE_HSTRETCH_COEFF 2
#define IFACE_GAMEFIELD_WIDTH (FIELD_WIDTH * IFACE_HSTRETCH_COEFF)
void draw_game_field(const GameInfo_t *const game, const int screen_center_row);
void draw_next_figure(const GameInfo_t *const game,
                      const int screen_center_row);
void draw_game_stats(const GameInfo_t *const game, const int screen_center_row);
void draw_game_over(const int screen_center_row, const bool over);
void draw_pause(const int screen_center_row, const bool pause);

void frontend_draw_game_scene(const GameInfo_t *const game, bool game_over,
                              bool pause) {
  const int screen_center_row = getmaxy(stdscr) / 2;
  draw_game_field(game, screen_center_row);
  draw_next_figure(game, screen_center_row);
  draw_game_stats(game, screen_center_row);
  draw_game_over(screen_center_row, game_over);
  draw_pause(screen_center_row, pause);
  move(0, 0);
  refresh();
}

void draw_game_field(const GameInfo_t *const game,
                     const int screen_center_row) {
  if (!game) return;
  if (screen_center_row < FIELD_VISIBLE_HEIGHT / 2) return;
  const int start_row = screen_center_row - FIELD_VISIBLE_HEIGHT / 2 - 1;
  move(start_row, 2);
  hline(ACS_HLINE, IFACE_GAMEFIELD_WIDTH);
  move(start_row + 1, 1);
  vline(ACS_VLINE, FIELD_VISIBLE_HEIGHT);
  move(start_row + FIELD_VISIBLE_HEIGHT + 1, 2);
  hline(ACS_HLINE, IFACE_GAMEFIELD_WIDTH);
  move(start_row + 1, IFACE_GAMEFIELD_WIDTH + 2);
  vline(ACS_VLINE, FIELD_VISIBLE_HEIGHT);
  int interface_r = start_row + 1;
  for (int r = FIELD_UPPER_MARGIN; r < FIELD_TOTAL_HEIGHT; ++r) {
    move(interface_r++, 2);
    for (int c = 0; c < FIELD_WIDTH; ++c) {
      const int field_value = game->field[r][c];
      if (field_value) {
        if (has_colors()) attron(COLOR_PAIR(field_value));
        for (int q = 0; q < IFACE_HSTRETCH_COEFF; ++q) {
          addch('#');
        }
        if (has_colors()) attroff(COLOR_PAIR(field_value));
      } else {
        for (int q = 0; q < IFACE_HSTRETCH_COEFF; ++q) {
          addch(' ');
        }
      }
    }
  }
}

void draw_next_figure(const GameInfo_t *const game,
                      const int screen_center_row) {
  if (!game) return;
  if (screen_center_row < FIELD_VISIBLE_HEIGHT / 2) return;
  const int start_row = screen_center_row - FIELD_VISIBLE_HEIGHT / 2 - 1;
  move(start_row, IFACE_GAMEFIELD_WIDTH + 4);
  hline(ACS_HLINE, MAX_FIGURE_SIZE * IFACE_HSTRETCH_COEFF);
  move(start_row + 1, IFACE_GAMEFIELD_WIDTH + 3);
  vline(ACS_VLINE, MAX_FIGURE_SIZE);
  move(start_row + MAX_FIGURE_SIZE + 1, IFACE_GAMEFIELD_WIDTH + 4);
  hline(ACS_HLINE, MAX_FIGURE_SIZE * IFACE_HSTRETCH_COEFF);
  move(start_row + 1,
       IFACE_GAMEFIELD_WIDTH + 4 + MAX_FIGURE_SIZE * IFACE_HSTRETCH_COEFF);
  vline(ACS_VLINE, MAX_FIGURE_SIZE);
  int interface_r = start_row + 1;
  const int interface_c = IFACE_GAMEFIELD_WIDTH + 4;
  for (int r = 0; r < MAX_FIGURE_SIZE; ++r) {
    move(interface_r++, interface_c);
    for (int c = 0; c < MAX_FIGURE_SIZE; ++c) {
      const int field_value = game->next[r][c];
      if (field_value) {
        if (has_colors()) attron(COLOR_PAIR(field_value));
        for (int q = 0; q < IFACE_HSTRETCH_COEFF; ++q) {
          addch('#');
        }
        if (has_colors()) attroff(COLOR_PAIR(field_value));
      } else {
        for (int q = 0; q < IFACE_HSTRETCH_COEFF; ++q) {
          addch(' ');
        }
      }
    }
  }
}

void draw_game_stats(const GameInfo_t *const game,
                     const int screen_center_row) {
  if (!game) return;
  if (screen_center_row < FIELD_VISIBLE_HEIGHT / 2) return;
  const int start_row = screen_center_row - FIELD_VISIBLE_HEIGHT / 2 + 6;
  move(start_row, IFACE_GAMEFIELD_WIDTH + 4);
  hline(ACS_HLINE, MAX_FIGURE_SIZE * IFACE_HSTRETCH_COEFF);
  move(start_row + 1, IFACE_GAMEFIELD_WIDTH + 3);
  vline(ACS_VLINE, 6);
  move(start_row + 6 + 1, IFACE_GAMEFIELD_WIDTH + 4);
  hline(ACS_HLINE, MAX_FIGURE_SIZE * IFACE_HSTRETCH_COEFF);
  move(start_row + 1,
       IFACE_GAMEFIELD_WIDTH + 4 + MAX_FIGURE_SIZE * IFACE_HSTRETCH_COEFF);
  vline(ACS_VLINE, 6);
  int interface_r = start_row + 1;
  const int interface_c = IFACE_GAMEFIELD_WIDTH + 4;
  move(interface_r++, interface_c);
  printw(" score:");
  move(interface_r++, interface_c);
  printw("%7d", game->score);
  move(interface_r++, interface_c);
  printw(" level:");
  move(interface_r++, interface_c);
  printw("%7d", game->level);
  move(interface_r++, interface_c);
  printw(" best:");
  move(interface_r++, interface_c);
  printw("%7d", game->high_score);
}

void draw_game_over(const int screen_center_row, const bool over) {
  move(screen_center_row + FIELD_VISIBLE_HEIGHT / 2 - 3,
       IFACE_GAMEFIELD_WIDTH + 4);
  if (has_colors()) {
    attron(COLOR_PAIR(1));
  }
  if (over) {
    printw("GAME OVER!");
  } else {
    printw("          ");
  }
  if (has_colors()) {
    attroff(COLOR_PAIR(1));
  }
}

void draw_pause(const int screen_center_row, const bool paused) {
  move(screen_center_row + FIELD_VISIBLE_HEIGHT / 2 - 2,
       IFACE_GAMEFIELD_WIDTH + 4);
  if (has_colors()) {
    attron(COLOR_PAIR(5));
  }
  if (paused) {
    printw("PAUSED");
  } else {
    printw("      ");
  }
  if (has_colors()) {
    attroff(COLOR_PAIR(5));
  }
}

bool get_user_input(UserAction_t *const input) {
  // static clock_t prev_update;
  static struct timespec prev_update;
  bool any_input = false;
  if (get_is_time_to_operate_ms_diff(&prev_update, USERINPUT_COOLDOWN_MS)) {
    const int ch = getch();
    switch (ch) {
      case KEY_UP:
        *input = Up;
        any_input = true;
        break;
      case KEY_DOWN:
        *input = Down;
        any_input = true;
        break;
      case KEY_LEFT:
        *input = Left;
        any_input = true;
        break;
      case KEY_RIGHT:
        *input = Right;
        any_input = true;
        break;
      case 's':
        *input = Start;
        any_input = true;
        break;
      case 'p':
        *input = Pause;
        any_input = true;
        break;
      case 'q':
        *input = Terminate;
        any_input = true;
        break;
      case ' ':
        *input = Action;
        any_input = true;
        break;
      default:
        break;
    }
  }
  return any_input;
}

void frontend_interframe_delay(void) {
  struct timespec ts1 = {0};
  ts1.tv_nsec = 800000;
  struct timespec trem = {0};
  nanosleep(&ts1, &trem);
}
