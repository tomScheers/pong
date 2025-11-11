#include "render.h"

#include <ncurses.h>
#include <stdbool.h>
#include <time.h>

void set_game_fields(struct Game *game) {
  game->running = true;
  game->ticks_x = 0;
  game->ticks_y = 0;

  game->plr_one.x = 0;
  game->plr_one.y = game->settings.screen_height / 2;
  game->plr_two.x = game->settings.screen_width - 1;
  game->plr_two.y = game->settings.screen_height / 2;

  game->plr_one.score = 0;
  game->plr_two.score = 0;

  game->ball_y = 0;
  game->ball_x = (float)game->settings.screen_width / 2;
  game->x_ball_orientation = game->settings.base_ball_x_slope;
  game->y_ball_orientation = game->settings.base_ball_y_slope;
}

struct Game *init_game() {
  struct Game *game = malloc(sizeof(struct Game));
  if (!game)
    return NULL;

  game->settings.ball_char = DEFAULT_BALL_CHAR;
  game->settings.pad_char = DEFAULT_PAD_CHAR;
  game->settings.frames_per_second = DEFAULT_FPS;
  game->settings.pad_tiles = DEFAULT_PAD_TILES;
  game->settings.program_version = PROGRAM_VERSION;
  game->settings.winning_score = DEFAULT_WINNING_SCORE;
  game->settings.port = DEFAULT_PORT_NUM;
  game->settings.seed = (uint32_t)time(NULL);
  game->settings.base_ball_x_slope = DEFAULT_BALL_X_SLOPE;
  game->settings.base_ball_y_slope = DEFAULT_BALL_Y_SLOPE;
  game->settings.ball_size = 3;

  for (int i = 0; i < 4; ++i)
    game->settings.ip_octets[i] = 0;

  initscr();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  cbreak();
  noecho();
  curs_set(0);

  game->settings.screen_height = PREFERED_SCREEN_HEIGHT > MAX_SCREEN_HEIGHT
                                     ? MAX_SCREEN_HEIGHT
                                     : PREFERED_SCREEN_HEIGHT;
  game->settings.screen_width = PREFERED_SCREEN_WIDTH > MAX_SCREEN_WIDTH
                                    ? MAX_SCREEN_WIDTH
                                    : PREFERED_SCREEN_WIDTH;

  set_game_fields(game);

  start_color();
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));

  return game;
}
