#include "render.h"

#include <inttypes.h>
#include <math.h>
#include <ncurses.h>

#define ISCOLLIDING(ball_x, ball_y, plr_x, plr_y)                              \
  (fabs(floor(ball_x) - plr_x) < EPSILON && (int)ball_y >= plr_y &&            \
   (int)ball_y < plr_y + game->settings.pad_tiles)

static void draw_player(struct Game *game, struct Player *player,
                        enum PlayerAction player_action, size_t offset_y,
                        size_t offset_x);

void draw_border(int y1, int x1, int y2, int x2) {
  // Ensure top-left and bottom-right ordering
  if (y1 > y2) {
    int tmp = y1;
    y1 = y2;
    y2 = tmp;
  }
  if (x1 > x2) {
    int tmp = x1;
    x1 = x2;
    x2 = tmp;
  }

  // Draw horizontal lines
  mvhline(y1, x1, ACS_HLINE, x2 - x1);
  mvhline(y2, x1, ACS_HLINE, x2 - x1);

  // Draw vertical lines
  mvvline(y1, x1, ACS_VLINE, y2 - y1);
  mvvline(y1, x2, ACS_VLINE, y2 - y1);

  // Draw corners
  mvaddch(y1, x1, ACS_ULCORNER);
  mvaddch(y1, x2, ACS_URCORNER);
  mvaddch(y2, x1, ACS_LLCORNER);
  mvaddch(y2, x2, ACS_LRCORNER);
}

void render(struct Game *game, enum PlayerAction your_action,
            enum PlayerAction opponent_action) {
  erase();

  mvprintw(2, COLS / 2 - 3, "%" PRIu16 "\n", game->plr_one.score);
  mvprintw(2, COLS / 2 + 3, "%" PRIu16 "\n", game->plr_two.score);

  int x_offset = (COLS - game->settings.screen_width) / 2;
  int y_offset = (LINES - game->settings.screen_height) / 2;

  draw_player(game, &game->plr_one, your_action, y_offset, x_offset);
  draw_player(game, &game->plr_two, opponent_action, y_offset, x_offset);

  mvaddch((int)game->ball_y + (LINES - game->settings.screen_height) / 2,
          (int)game->ball_x + (COLS - game->settings.screen_width) / 2,
          game->settings.ball_char);

  draw_border((LINES - game->settings.screen_height) / 2 - 1,
              (COLS - game->settings.screen_width) / 2 - 1,
              (LINES + game->settings.screen_height) / 2,
              (COLS + game->settings.screen_width) / 2);

  refresh();

  if (game->speed_ticks !=
      game->settings.frames_per_second / game->settings.ball_speed) {
    game->speed_ticks++;
    return;
  }

  if (fabs(floor(game->ball_x) - game->settings.screen_width) < EPSILON) {
    game->plr_one.score++;
    game->ball_y = (float)((int)(game->settings.screen_height / 2));
    game->ball_x = (float)game->settings.screen_width / 2;
  } else if (game->ball_x <= EPSILON) {
    game->plr_two.score++;
    game->ball_y = (float)((int)(game->settings.screen_height / 2));
    game->ball_x = (float)game->settings.screen_width / 2;
  }

  // Check if next hit is going to be collision and adjust orientation
  float next_ball_y = game->ball_y + game->y_ball_orientation;
  float next_ball_x = game->ball_x + game->x_ball_orientation;

  if (next_ball_y <= 0 || next_ball_y >= game->settings.screen_height) {
    game->y_ball_orientation *= -1;
  }

  if (ISCOLLIDING(next_ball_x, next_ball_y, game->plr_one.x, game->plr_one.y) ||
      ISCOLLIDING(next_ball_x, next_ball_y, game->plr_two.x, game->plr_two.y)) {
    game->x_ball_orientation *= -1;
  }

  game->ball_x += game->x_ball_orientation;
  game->ball_y += game->y_ball_orientation;
  game->speed_ticks = 0;
}

static void draw_player(struct Game *game, struct Player *player,
                        enum PlayerAction player_action, size_t offset_y,
                        size_t offset_x) {
  switch (player_action) {
  case PAD_UP:
    if (player->y > 0)
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
    mvaddch(player->y + i + offset_y, player->x + offset_x,
            game->settings.pad_char);
}
