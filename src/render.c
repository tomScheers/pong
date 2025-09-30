#include "render.h"

#include <ncurses.h>
#include <stdlib.h>

static void draw_player(struct Game *game, enum PlayerAction your_action,
                        enum PlayerAction opponent_action);

void render(struct Game *game, enum PlayerAction your_action,
            enum PlayerAction opponent_action) {
  draw_player(game, your_action, opponent_action);
  mvaddch(game->ball_y, game->ball_x, '0');

  refresh();
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
    if ((mvinch(next_y, next_x) & A_CHARTEXT) == PAD_CHAR) {
      game->x_ball_orientation *= -1;
      if ((mvinch(next_y - 1, next_x) & A_CHARTEXT) == PAD_CHAR &&
          (mvinch(next_y + 1, next_x) & A_CHARTEXT) ==
              PAD_CHAR) { // Central hit
        game->y_ball_orientation = 0;
      } else if ((mvinch(next_y - 1, next_x) & A_CHARTEXT) ==
                 PAD_CHAR) { // Upper hit
        game->y_ball_orientation = 1;
      } else {
        game->y_ball_orientation = -1;
      }
    }
    game->ball_x += game->x_ball_orientation;
    game->ball_y += game->y_ball_orientation;
  }
}

struct Game *init_game() {
  struct Game *game = malloc(sizeof(struct Game));
  if (!game)
    return NULL;

  game->running = true;
  game->game_height = 10;
  game->game_width = 30;

  initscr();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  cbreak();
  noecho();
  curs_set(0);

  game->plr_one.x = 1;
  game->plr_one.y = LINES / 2;
  game->plr_two.x = COLS - 1;
  game->plr_two.y = LINES / 2;

  game->plr_one.score = 0;
  game->plr_two.score = 0;

  game->ball_y = game->game_height / 2 + 5;
  game->ball_x = game->game_width / 2;
  game->x_ball_orientation = -1;
  game->y_ball_orientation = 0;

  start_color();
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));

  draw_player(game, NONE, NONE);

  refresh();

  return game;
}

void draw_player(struct Game *game, enum PlayerAction your_action,
                 enum PlayerAction opponent_action) {
  switch (your_action) {
  case PAD_UP:
    mvdelch(game->plr_one.y + 1, game->plr_one.x);
    mvaddch(game->plr_one.y - 2, game->plr_one.x, PAD_CHAR);
    game->plr_one.y--;
    break;
  case PAD_DOWN:
    mvdelch(game->plr_one.y - 1, game->plr_one.x);
    mvaddch(game->plr_one.y + 2, game->plr_one.x, PAD_CHAR);
    game->plr_one.y++;
    break;
  case NONE:
  default:
    mvdelch(game->plr_one.y - 1, game->plr_one.x);
    mvaddch(game->plr_one.y - 1, game->plr_one.x, PAD_CHAR);
    mvdelch(game->plr_one.y, game->plr_one.x);
    mvaddch(game->plr_one.y, game->plr_one.x, PAD_CHAR);
    mvdelch(game->plr_one.y + 1, game->plr_one.x);
    mvaddch(game->plr_one.y + 1, game->plr_one.x, PAD_CHAR);
    return;
    break;
  }

  switch (opponent_action) {
  case PAD_UP:
    mvdelch(game->plr_two.y + 1, game->plr_two.x);
    mvaddch(game->plr_two.y - 2, game->plr_two.x, PAD_CHAR);
    game->plr_two.y--;
    break;
  case PAD_DOWN:
    mvdelch(game->plr_two.y - 1, game->plr_two.x);
    mvaddch(game->plr_two.y + 2, game->plr_two.x, PAD_CHAR);
    game->plr_two.y++;
    break;
  case NONE:
  default:
    mvdelch(game->plr_two.y - 1, game->plr_two.x);
    mvaddch(game->plr_two.y - 1, game->plr_two.x, PAD_CHAR);
    mvdelch(game->plr_two.y, game->plr_two.x);
    mvaddch(game->plr_two.y, game->plr_two.x, PAD_CHAR);
    mvdelch(game->plr_two.y + 1, game->plr_two.x);
    mvaddch(game->plr_two.y + 1, game->plr_two.x, PAD_CHAR);
    return;
    break;
  }
}
