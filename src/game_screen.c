#include "render.h"

#include <inttypes.h>
#include <math.h>
#include <ncurses.h>

#define ISCOLLIDING(ball_x, ball_y, plr_x, plr_y)                              \
  (fabs(floor(ball_x) - plr_x) < EPSILON && (int)ball_y >= plr_y &&            \
   (int)ball_y < plr_y + game->settings.pad_tiles)

static void draw_player(struct Game *game, struct Player *player,
                        enum PlayerAction player_action);

void render(struct Game *game, enum PlayerAction your_action,
            enum PlayerAction opponent_action) {
  erase();

  mvprintw(GAME_Y_OFFSET, 0, "%f, %f\n", game->ball_x, game->ball_y);
  mvprintw(GAME_Y_OFFSET - 2, game->settings.screen_width / 2 - 3,
           "%" PRIu16 "\n", game->plr_one.score);
  mvprintw(GAME_Y_OFFSET - 2, game->settings.screen_width / 2 + 3,
           "%" PRIu16 "\n", game->plr_two.score);

  draw_player(game, &game->plr_one, your_action);
  draw_player(game, &game->plr_two, opponent_action);
  mvaddch((int)game->ball_y, (int)game->ball_x, game->settings.ball_char);
  refresh();

  if (game->speed_ticks !=
      game->settings.frames_per_second / game->settings.ball_speed) {
    game->speed_ticks++;
    return;
  }

  if (fabs(floor(game->ball_x) - game->settings.screen_width) < EPSILON) {
    game->plr_one.score++;
    game->ball_y =
        (float)((int)(game->settings.screen_height / 2 + GAME_Y_OFFSET));
    game->ball_x = (float)game->settings.screen_width / 2;
  } else if (game->ball_x <= EPSILON) {
    game->plr_two.score++;
    game->ball_y =
        (float)((int)(game->settings.screen_height / 2 + GAME_Y_OFFSET));
    game->ball_x = (float)game->settings.screen_width / 2;
  }

  if (fabs(floor(game->ball_y) - (game->settings.screen_height)) < EPSILON ||
      game->ball_y <= EPSILON + GAME_Y_OFFSET) {
    game->y_ball_orientation *= -1;
  }

  // Check if next hit is going to be collision and adjust orientation
  float next_ball_y = game->ball_y + game->y_ball_orientation;
  float next_ball_x = game->ball_x + game->x_ball_orientation;

  if (ISCOLLIDING(next_ball_x, next_ball_y, game->plr_one.x, game->plr_one.y) ||
      ISCOLLIDING(next_ball_x, next_ball_y, game->plr_two.x, game->plr_two.y)) {
    game->x_ball_orientation *= -1;
  }

  game->ball_x += game->x_ball_orientation;
  game->ball_y += game->y_ball_orientation;
  game->speed_ticks = 0;
}

static void draw_player(struct Game *game, struct Player *player,
                        enum PlayerAction player_action) {
  switch (player_action) {
  case PAD_UP:
    if (player->y - 1 >= 0)
      player->y--;
    break;
  case PAD_DOWN:
    if (player->y + game->settings.pad_tiles < game->settings.screen_height)
      player->y++;
    break;
  case NONE:
  default:
    break;
  }

  for (uint8_t i = 0; i < game->settings.pad_tiles; i++)
    mvaddch(player->y + i, player->x, game->settings.pad_char);
}
