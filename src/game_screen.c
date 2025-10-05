#include "render.h"

#include <inttypes.h>
#include <ncurses.h>

static void draw_player(struct Game *game, enum PlayerAction your_action,
                        enum PlayerAction opponent_action);

void render(struct Game *game, enum PlayerAction your_action,
            enum PlayerAction opponent_action) {
  erase();
  draw_player(game, your_action, opponent_action);
  mvaddch(game->ball_y, game->ball_x, '0');
  refresh();

  if (game->ball_x == game->settings.screen_width || game->ball_x == 0) {
    game->x_ball_orientation *= -1;
  }

  if (game->ball_y == game->settings.screen_width || game->ball_y == 0) {
    game->y_ball_orientation *= -1;

    // Check if next hit is going to be collision and adjust orientation
    int next_y = game->ball_y + game->y_ball_orientation;
    int next_x = game->ball_x + game->x_ball_orientation;
    if ((mvinch(next_y, next_x) & A_CHARTEXT) == game->settings.pad_char) {
      game->x_ball_orientation *= -1;
      if ((mvinch(next_y - 1, next_x) & A_CHARTEXT) ==
              game->settings.pad_char &&
          (mvinch(next_y + 1, next_x) & A_CHARTEXT) ==
              game->settings.pad_char) { // Central hit
        game->y_ball_orientation = 0;
      } else if ((mvinch(next_y - 1, next_x) & A_CHARTEXT) ==
                 game->settings.pad_char) { // Upper hit
        game->y_ball_orientation = 1;
      } else {
        game->y_ball_orientation = -1;
      }
    }
  }
  game->ball_x += game->x_ball_orientation;
  game->ball_y += game->y_ball_orientation;
}

void draw_player(struct Game *game, enum PlayerAction your_action,
                 enum PlayerAction opponent_action) {
  mvaddch(game->plr_one.y - 1, game->plr_one.x, ' ');
  mvaddch(game->plr_one.y, game->plr_one.x, ' ');
  mvaddch(game->plr_one.y + 1, game->plr_one.x, ' ');
  mvaddch(game->plr_two.y - 1, game->plr_two.x, ' ');
  mvaddch(game->plr_two.y, game->plr_two.x, ' ');
  mvaddch(game->plr_two.y + 1, game->plr_two.x, ' ');
  switch (your_action) {
  case PAD_UP:
    game->plr_one.y--;
    break;
  case PAD_DOWN:
    game->plr_one.y++;
    break;
  case NONE:
  default:
    break;
  }
  mvaddch(game->plr_one.y - 1, game->plr_one.x, game->settings.pad_char);
  mvaddch(game->plr_one.y, game->plr_one.x, game->settings.pad_char);
  mvaddch(game->plr_one.y + 1, game->plr_one.x, game->settings.pad_char);
  switch (opponent_action) {
  case PAD_UP:
    game->plr_two.y--;
    break;
  case PAD_DOWN:
    game->plr_two.y++;
    break;
  case NONE:
  default:
    break;
  }
  mvaddch(game->plr_two.y - 1, game->plr_two.x, game->settings.pad_char);
  mvaddch(game->plr_two.y, game->plr_two.x, game->settings.pad_char);
  mvaddch(game->plr_two.y + 1, game->plr_two.x, game->settings.pad_char);
}
