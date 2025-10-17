#include "render.h"

#include <ncurses.h>
#include <stdbool.h>
#include <string.h>

static bool parse_args(struct Game *game, char **args, size_t argc);
static inline bool is_flag(char *given_flag, char *expected_flag);

void set_game_fields(struct Game *game) {
  game->running = true;
  game->speed_ticks = 0;

  game->plr_one.x = 0;
  game->plr_one.y = game->settings.screen_height / 2;
  game->plr_two.x = game->settings.screen_width - 1;
  game->plr_two.y = game->settings.screen_height / 2;

  game->plr_one.score = 0;
  game->plr_two.score = 0;

  game->ball_y = (float)((int)(game->settings.screen_height / 2));
  game->ball_x = (float)game->settings.screen_width / 2;
  game->x_ball_orientation = -1;
  game->y_ball_orientation = -0.5;
}

struct Game *init_game(char **args, size_t argc) {
  struct Game *game = malloc(sizeof(struct Game));
  if (!game)
    return NULL;

  if (!parse_args(game, args, argc))
    return NULL;

  game->settings.ball_char = DEFAULT_BALL_CHAR;
  game->settings.pad_char = DEFAULT_PAD_CHAR;
  game->settings.ball_speed = DEFAULT_BALL_SPEED;
  game->settings.frames_per_second = DEFAULT_FPS;
  game->settings.pad_tiles = DEFAULT_PAD_TILES;
  game->settings.program_version = PROGRAM_VERSION;
  game->settings.winning_score = DEFAULT_WINNING_SCORE;
  game->settings.port = DEFAULT_PORT_NUM;

  for (int i = 0; i < 4; ++i)
    game->settings.ip_octets[i] = 0;

  initscr();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  cbreak();
  noecho();
  curs_set(0);

  game->settings.screen_height = LINES - 4;
  game->settings.screen_width = COLS - 4;

  set_game_fields(game);

  start_color();
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));

  return game;
}

static bool parse_args(struct Game *game, char **args, size_t argc) {
  for (uint16_t i = 1; i < argc; ++i) {
    if (is_flag(args[i], "-h") || is_flag(args[i], "--height")) {
      if (i + 1 < argc) {
        game->settings.screen_height = atoi(args[++i]);
      } else {
        fprintf(stderr, "invalid argument\n");
        return false;
      }
    } else if (is_flag(args[i], "-w") || is_flag(args[i], "--width")) {
      if (i + 1 < argc) {
        game->settings.screen_width = atoi(args[++i]);
      } else {
        fprintf(stderr, "invalid argument\n");
        return false;
      }
    } else if (is_flag(args[i], "-bc") || is_flag(args[i], "--ball-char")) {
      if (i + 1 < argc) {
        game->settings.ball_char = args[++i][0];
      } else {
        fprintf(stderr, "invalid argument\n");
        return false;
      }
    } else if (is_flag(args[i], "-pc") || is_flag(args[i], "--pad-char")) {
      if (i + 1 < argc) {
        game->settings.pad_char = args[++i][0];
      } else {
        fprintf(stderr, "invalid argument\n");
        return false;
      }
    } else if (is_flag(args[i], "-bs") || is_flag(args[i], "--ball-speed")) {
      if (i + 1 < argc) {
        game->settings.ball_speed = atoi(args[++i]);
      } else {
        fprintf(stderr, "invalid argument\n");
        return false;
      }
    } else if (is_flag(args[i], "-fps") ||
               is_flag(args[i], "--frames-per-second")) {
      if (i + 1 < argc) {
        game->settings.frames_per_second = atoi(args[++i]);
      } else {
        fprintf(stderr, "invalid argument\n");
        return false;
      }
    } else if (is_flag(args[i], "-pt") || is_flag(args[i], "--pad-tiles")) {
      if (i + 1 < argc) {
        game->settings.pad_tiles = args[++i][0];
      } else {
        fprintf(stderr, "invalid argument\n");
        return false;
      }
    } else if (is_flag(args[i], "-ws") || is_flag(args[i], "--winning-score")) {
      if (i + 1 < argc) {
        game->settings.pad_tiles = args[++i][0];
      } else {
        fprintf(stderr, "invalid argument\n");
        return false;
      }
    } else if (is_flag(args[i], "-p") || is_flag(args[i], "--port")) {
      if (i + 1 < argc) {
        game->settings.port = atoi(args[++i]);
      } else {
        fprintf(stderr, "invalid argument\n");
        return false;
      }
    } else {
      fprintf(stderr, "Invalid argument: %s\n", args[i]);
      return false;
    }
  }
  return true;
}

static inline bool is_flag(char *given_flag, char *expected_flag) {
  return strcmp(given_flag, expected_flag) == 0;
}
