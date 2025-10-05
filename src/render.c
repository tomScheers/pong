#include "render.h"

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

static void draw_player(struct Game *game, enum PlayerAction your_action,
                        enum PlayerAction opponent_action);
static inline bool is_flag(char *given_flag, char *expected_flag) {
  return strcmp(given_flag, expected_flag) == 0;
}

void render(struct Game *game, enum PlayerAction your_action,
            enum PlayerAction opponent_action) {
  draw_player(game, your_action, opponent_action);
  mvaddch(game->ball_y, game->ball_x, '0');

  refresh();
  mvdelch(game->ball_y, game->ball_x);
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
    game->ball_x += game->x_ball_orientation;
    game->ball_y += game->y_ball_orientation;
  }
}

struct Game *init_game(char **args, size_t argc) {
  struct Game *game = malloc(sizeof(struct Game));
  if (!game)
    return NULL;

  game->settings.screen_height = 0;
  game->settings.screen_width = 0;
  game->settings.ball_char = DEFAULT_BALL_CHAR;
  game->settings.pad_char = DEFAULT_PAD_CHAR;
  game->settings.ball_speed = DEFAULT_BALL_SPEED;
  game->settings.frames_per_second = DEFAULT_FPS;
  game->settings.pad_tiles = DEFAULT_PAD_TILES;
  game->settings.program_version = PROGRAM_VERSION;
  game->settings.winning_score = DEFAULT_WINNING_SCORE;
  game->settings.port = DEFAULT_PORT_NUM;

  for (uint16_t i = 1; i < argc; ++i) {
    if (is_flag(args[i], "-h") || is_flag(args[i], "--height")) {
      if (i + 1 < argc) {
        game->settings.screen_height = atoi(args[++i]);
      } else {
        fprintf(stderr, "invalid argument\n");
        return NULL;
      }
    } else if (is_flag(args[i], "-w") || is_flag(args[i], "--width")) {
      if (i + 1 < argc) {
        game->settings.screen_width = atoi(args[++i]);
      } else {
        fprintf(stderr, "invalid argument\n");
        return NULL;
      }
    } else if (is_flag(args[i], "-bc") || is_flag(args[i], "--ball-char")) {
      if (i + 1 < argc) {
        game->settings.ball_char = args[++i][0];
      } else {
        fprintf(stderr, "invalid argument\n");
        return NULL;
      }
    } else if (is_flag(args[i], "-pc") || is_flag(args[i], "--pad-char")) {
      if (i + 1 < argc) {
        game->settings.pad_char = args[++i][0];
      } else {
        fprintf(stderr, "invalid argument\n");
        return NULL;
      }
    } else if (is_flag(args[i], "-bs") || is_flag(args[i], "--ball-speed")) {
      if (i + 1 < argc) {
        game->settings.ball_speed = atoi(args[++i]);
      } else {
        fprintf(stderr, "invalid argument\n");
        return NULL;
      }
    } else if (is_flag(args[i], "-fps") ||
               is_flag(args[i], "--frames-per-second")) {
      if (i + 1 < argc) {
        game->settings.frames_per_second = atoi(args[++i]);
      } else {
        fprintf(stderr, "invalid argument\n");
        return NULL;
      }
    } else if (is_flag(args[i], "-pt") || is_flag(args[i], "--pad-tiles")) {
      if (i + 1 < argc) {
        game->settings.pad_tiles = args[++i][0];
      } else {
        fprintf(stderr, "invalid argument\n");
        return NULL;
      }
    } else if (is_flag(args[i], "-ws") || is_flag(args[i], "--winning-score")) {
      if (i + 1 < argc) {
        game->settings.pad_tiles = args[++i][0];
      } else {
        fprintf(stderr, "invalid argument\n");
        return NULL;
      }
    } else if (is_flag(args[i], "-p") || is_flag(args[i], "--port")) {
      if (i + 1 < argc) {
        game->settings.port = atoi(args[++i]);
      } else {
        fprintf(stderr, "invalid argument\n");
        return NULL;
      }
    } else if (is_flag(args[i], "serve")) {
      continue;
    } else {
      fprintf(stderr, "Invalid argument: %s\n", args[i]);
      return NULL;
    }
  }
  initscr();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  cbreak();
  noecho();
  curs_set(0);

  if (game->settings.screen_height == 0 ||
      game->settings.screen_height > LINES) {
    game->settings.screen_height = LINES;
  }

  if (game->settings.screen_width == 0 || game->settings.screen_width > COLS) {
    game->settings.screen_width = COLS;
  }

  game->running = true;

  game->plr_one.x = 1;
  game->plr_one.y = LINES / 2;
  game->plr_two.x = COLS - 1;
  game->plr_two.y = LINES / 2;

  game->plr_one.score = 0;
  game->plr_two.score = 0;

  game->ball_y = game->settings.screen_height / 2 + 5;
  game->ball_x = game->settings.screen_width / 2;
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
    break;
  }
  mvaddch(game->plr_one.y - 1, game->plr_one.x, DEFAULT_PAD_CHAR);
  mvaddch(game->plr_one.y, game->plr_one.x, DEFAULT_PAD_CHAR);
  mvaddch(game->plr_one.y + 1, game->plr_one.x, DEFAULT_PAD_CHAR);
  switch (opponent_action) {
  case PAD_UP:
    game->plr_two.y--;
    break;
  case PAD_DOWN:
    game->plr_two.y++;
    break;
  case NONE:
    break;
  }
  mvaddch(game->plr_two.y - 1, game->plr_two.x, DEFAULT_PAD_CHAR);
  mvaddch(game->plr_two.y, game->plr_two.x, DEFAULT_PAD_CHAR);
  mvaddch(game->plr_two.y + 1, game->plr_two.x, DEFAULT_PAD_CHAR);
}
