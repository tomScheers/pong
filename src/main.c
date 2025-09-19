#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>

#include "main.h"

void start() {

  struct Game *game = malloc(sizeof(struct Game));

  game->running = 1;
  game->game_height = 10;
  game->game_width = 30;

  setlocale(LC_ALL, "");
  initscr();
  curs_set(0);

  game->ball_y = game->game_height / 2 + 5;
  game->ball_x = game->game_width / 2;
  game->x_ball_orientation = 1;
  game->y_ball_orientation = 1;

  start_color();

  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));
  refresh();

  while (game->running) {

    mvaddch(game->ball_y, game->ball_x, '0');
    refresh();
    napms(40);
    mvdelch(game->ball_y, game->ball_x);
    refresh();

    if (game->ball_x == game->game_width) {
      game->x_ball_orientation = -1;
    } else if (game->ball_x == 0) {
      game->x_ball_orientation = 1;
    }
    if (game->ball_y == game->game_height) {
      game->y_ball_orientation = -1;
    } else if (game->ball_y == 0) {
      game->y_ball_orientation = 1;
    }
    game->ball_x += game->x_ball_orientation;
    game->ball_y += game->y_ball_orientation;
  }
  endwin();
}

int main() {

  start();
  return 0;
}
