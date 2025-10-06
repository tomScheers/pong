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

  if (game->speed_ticks ==
      game->settings.frames_per_second / game->settings.ball_speed) {
    if (game->ball_x == game->settings.screen_width || game->ball_x == 0) {
      game->x_ball_orientation *= -1;
    }

    if (game->ball_y == game->settings.screen_height || game->ball_y == 0) {
      game->y_ball_orientation *= -1;

      // Check if next hit is going to be collision and adjust orientation
      int next_y = game->ball_y + game->y_ball_orientation;
      int next_x = game->ball_x + game->x_ball_orientation;

      if (ISCOLLIDING(game->ball_x, game->ball_y, game->plr_one.x,
                      game->plr_one.y) ||
          ISCOLLIDING(game->ball_x, game->ball_y, game->plr_two.x,
                      game->plr_two.y)) {
        game->x_ball_orientation *= -1;
      }
    }
    game->ball_x += game->x_ball_orientation;
    game->ball_y += game->y_ball_orientation;
  } else
    game->speed_ticks++;
}

void draw_player(struct Game *game, enum PlayerAction your_action,
                 enum PlayerAction opponent_action) {
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
  for (uint8_t i = 0; i < game->settings.pad_tiles; i++) {
    mvaddch(game->plr_one.y + i, game->plr_one.x, game->settings.pad_char);
  }
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
  for (uint8_t i = 0; i < game->settings.pad_tiles; i++) {
    mvaddch(game->plr_one.y + i, game->plr_one.x, game->settings.pad_char);
  }
}
