#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "net.h"
#include "pong.h"

void loop(struct Game *game, enum PlayerAction your_action,
          enum PlayerAction opponent_action);

void draw_player(struct Game *game, enum PlayerAction action);

struct Game *init_game();

int main(int argc, char **argv) {

  struct Game *game = init_game();
  enum PlayerAction action;
  int ch;

  while (game->running) {
    ch = getch();
    switch (ch) {
    case KEY_UP:
      action = PAD_UP;
      loop(game, action, NONE);
      break;
    case KEY_DOWN:
      action = PAD_DOWN;
      loop(game, action, NONE);
      break;
    case 'q':
      action = QUIT;
      exit(0); // Can't really do much with the PlayerAction
    default:
      action = NONE;
      break;
    }
    loop(game, action, NONE);
  }
  free(game);
  return 0;
}

void loop(struct Game *game, enum PlayerAction your_action,
          enum PlayerAction opponent_action) {

  draw_player(game, your_action);
  mvaddch(game->ball_y, game->ball_x, '0');

  refresh();
  napms(40);
  mvdelch(game->ball_y, game->ball_x);
  refresh();

  if (game->ball_x == game->game_width || game->ball_x == 0) {
    game->x_ball_orientation *= -1;
  }

  if (game->ball_y == game->game_height || game->ball_y == 0) {
    game->y_ball_orientation *= -1;

    // Check if next hit is going to be collision and adjust orientation
    int next_y = game->ball_y + game->y_ball_orientation;
    int next_x = game->ball_x + game->x_ball_orientation;
    if (mvinch(next_y, next_x) == '|') {
      game->x_ball_orientation *= -1;
      if (mvinch(next_y - 1, next_x + COLS) == '|' &&
          mvinch(next_y + 1, next_x - COLS) == '|') { // Central hit
        game->y_ball_orientation = 0;
      } else if (mvinch(next_y + 1, next_x) == '|') { // Upper hit
        game->y_ball_orientation = -1;
      } else {
        game->y_ball_orientation = 1;
      }
    }
    game->ball_x += game->x_ball_orientation;
    game->ball_y += game->y_ball_orientation;
  }
}
struct Game *init_game() {
  struct Game *game = malloc(sizeof(struct Game));

  game->running = true;
  game->game_height = 10;
  game->game_width = 30;

  initscr();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  cbreak();
  noecho();
  curs_set(0);

  game->plr_one_x = 1;
  game->plr_one_y = LINES / 2;
  game->plr_two_x = COLS;
  game->plr_one_y = LINES / 2;

  game->player_score_one = '0';
  game->player_score_two = '0';

  game->ball_y = game->game_height / 2 + 5;
  game->ball_x = game->game_width / 2;
  game->x_ball_orientation = -1;
  game->y_ball_orientation = 0;

  start_color();
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));
  refresh();

  return game;
}

void draw_player(struct Game *game, enum PlayerAction action) {
  switch (action) {
  case PAD_UP:
    mvdelch(game->plr_one_y + 1, game->plr_one_x);
    mvaddch(game->plr_one_y - 2, game->plr_one_x, '|');
    game->plr_one_y--;
    break;
  case PAD_DOWN:
    mvdelch(game->plr_one_y - 1, game->plr_one_x);
    mvaddch(game->plr_one_y + 2, game->plr_one_x, '|');
    game->plr_one_y++;
    break;
  case NONE:
    return;
    break;
  }
}
