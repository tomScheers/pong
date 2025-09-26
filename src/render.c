#include "render.h"

#include <ncurses.h>
#include <stdlib.h>

void loop(struct Game *game, enum PlayerAction your_action,
          enum PlayerAction opponend_action) {
  printw("Your move: %d\nOpponend Move: %d\n", your_action, opponend_action);
  refresh();
  // mvaddch(game->ball_y, game->ball_x, '0');
  // refresh();
  // mvdelch(game->ball_y, game->ball_x);
  // refresh();

  // if (game->ball_x == game->game_width) {
  //   game->x_ball_orientation = -1;
  // } else if (game->ball_x == 0) {
  //   game->x_ball_orientation = 1;
  // }
  // if (game->ball_y == game->game_height) {
  //   game->y_ball_orientation = -1;
  // } else if (game->ball_y == 0) {
  //   game->y_ball_orientation = 1;
  // }
  // game->ball_x += game->x_ball_orientation;
  // game->ball_y += game->y_ball_orientation;
}

struct Game *init_game() {
  struct Game *game = malloc(sizeof(struct Game));

  game->running = true;
  game->game_height = 10;
  game->game_width = 30;

  initscr();
  cbreak();
  noecho();
  // nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  curs_set(0);

  game->ball_y = game->game_height / 2 + 5;
  game->ball_x = game->game_width / 2;
  game->x_ball_orientation = 1;
  game->y_ball_orientation = 1;

  start_color();
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));
  refresh();

  return game;
}
