#include "render.h"

#include <inttypes.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

enum SettingTypes {
  SETTING_UINT8,
  SETTING_UINT16,
  SETTING_CHAR,
  SETTING_NULL,
};

struct SettingBox {
  const char *setting_str;
  void *setting_value_ptr;
  enum SettingTypes setting_type;
};

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

void set_game_fields(struct Game *game) {
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

  set_game_fields(game);

  start_color();
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));

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

enum Gamemode loading_screen() {
  int height, width;
  getmaxyx(stdscr, height, width);

  uint8_t selected = 0;

  const char *pixel_art[] = {" _______  _______  _        _______ ",
                             "(  ____ )(  ___  )( (    /|(  ____ \\",
                             "| (    )|| (   ) ||  \\  ( || (    \\/",
                             "| (____)|| |   | ||   \\ | || |      ",
                             "|  _____)| |   | || (\\ \\) || | ____ ",
                             "| (      | |   | || | \\   || | \\_  )",
                             "| )      | (___) || )  \\  || (___) |",
                             "|/       (_______)|/    )_)(_______)"};
  int pixel_art_height = sizeof(pixel_art) / sizeof(pixel_art[0]);
  int pixel_art_width = strlen(pixel_art[0]);

  const char *msgs[] = {"serve", "join", "offline", "bot", "quit"};
  int msg_count = sizeof(msgs) / sizeof(msgs[0]);

  int total_len = 0;
  for (int i = 0; i < msg_count; ++i)
    total_len += strlen(msgs[i]);

  int gaps = msg_count + 1;
  int space = (width - total_len) / gaps;

  int y = height / 2 + pixel_art_height / 2;
  int pixel_art_y = height / 2 - pixel_art_height / 2 - pixel_art_height;
  int pixel_art_x = (width - pixel_art_width) / 2;

  int ch;
  while ((ch = getch()) != '\n') {
    for (int i = 0; i < pixel_art_height; ++i) {
      mvprintw(pixel_art_y + i, pixel_art_x, "%s", pixel_art[i]);
    }

    if (ch == 'h' && selected > 0)
      --selected;
    if (ch == 'l' && selected < msg_count - 1)
      ++selected;
    int x = space;
    for (int i = 0; i < msg_count; ++i) {
      if (i == selected) {
        attron(A_REVERSE);
        mvprintw(y, x, "%s", msgs[i]);
        attroff(A_REVERSE);
      } else {
        mvprintw(y, x, "%s", msgs[i]);
      }
      x += strlen(msgs[i]) + space;
    }
    refresh();
  }
  clear();
  refresh();
  return selected;
}

void change_serve_settings(struct Game *game) {
  int height, width;
  getmaxyx(stdscr, height, width);
  struct SettingBox settings[] = {
      {.setting_str = "Screen Height",
       .setting_value_ptr = &game->settings.screen_height,
       .setting_type = SETTING_UINT16},
      {.setting_str = "Screen Width",
       .setting_value_ptr = &game->settings.screen_width,
       .setting_type = SETTING_UINT16},
      {.setting_str = "Port Number",
       .setting_value_ptr = &game->settings.port,
       .setting_type = SETTING_UINT16},
      {.setting_str = "Winning Score",
       .setting_value_ptr = &game->settings.winning_score,
       .setting_type = SETTING_UINT16},
      {.setting_str = "Ball Speed",
       .setting_value_ptr = &game->settings.ball_speed,
       .setting_type = SETTING_UINT16},
      {.setting_str = "FPS",
       .setting_value_ptr = &game->settings.frames_per_second,
       .setting_type = SETTING_UINT8},
      {.setting_str = "Pad Tiles",
       .setting_value_ptr = &game->settings.pad_tiles,
       .setting_type = SETTING_UINT8},
      {.setting_str = "Pad Char",
       .setting_value_ptr = &game->settings.pad_char,
       .setting_type = SETTING_CHAR},
      {.setting_str = "Ball Char",
       .setting_value_ptr = &game->settings.ball_char,
       .setting_type = SETTING_CHAR},
      {.setting_str = "Start",
       .setting_value_ptr = NULL,
       .setting_type = SETTING_NULL}};

  int settings_count = sizeof(settings) / sizeof(settings[0]);
  int start_y = height / 2 - settings_count / 2;
  int selected = 0;
  bool is_editing = false;

  int ch;
  while (true) {
    ch = getch();

    if (ch == '\n' && settings[selected].setting_type == SETTING_NULL)
      break;

    if (ch == 'j' && selected < settings_count - 1 && !is_editing)
      ++selected;

    if (ch == 'k' && selected > 0 && !is_editing)
      --selected;

    if (ch == '\n')
      is_editing = !is_editing;

    if (is_editing && ch != '\n' && ch != ERR) {
      if (settings[selected].setting_type == SETTING_CHAR) {
        *(char *)settings[selected].setting_value_ptr = ch;
        is_editing = false;
        clear();
      } else if (settings[selected].setting_type == SETTING_UINT8) {
        uint16_t value = *(uint8_t *)settings[selected].setting_value_ptr;
        if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
          value /= 10;
        } else if (ch >= '0' && ch <= '9') {
          value = value * 10 + (ch - '0');
          if (value > UINT8_MAX)
            value = UINT8_MAX;
        }
        *(uint8_t *)settings[selected].setting_value_ptr = (uint8_t)value;
        clear();
      } else if (settings[selected].setting_type == SETTING_UINT16) {
        uint32_t value = *(uint16_t *)settings[selected].setting_value_ptr;
        if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
          value /= 10;
        } else if (ch >= '0' && ch <= '9') {
          value = value * 10 + (ch - '0');
          if (value > UINT16_MAX)
            value = UINT16_MAX;
        }
        *(uint16_t *)settings[selected].setting_value_ptr = (uint16_t)value;
        clear();
      }
    }

    for (int i = 0; i < settings_count; ++i) {
      int setting_x = width / 2 - strlen(settings[i].setting_str) - 1;

      if (settings[i].setting_type == SETTING_NULL)
        setting_x = width / 2 - strlen(settings[i].setting_str) / 2 - 1;

      int setting_value_x = width / 2 + 1;
      int setting_y = start_y + i;

      if (selected == i && settings[i].setting_type == SETTING_NULL)
        attron(A_REVERSE);

      mvwprintw(stdscr, setting_y, setting_x, "%s", settings[i].setting_str);

      if (selected == i && settings[i].setting_type == SETTING_NULL)
        attroff(A_REVERSE);

      if (selected == i && settings[i].setting_type != SETTING_NULL)
        attron(A_REVERSE);

      if (settings[i].setting_type == SETTING_UINT16)
        mvwprintw(stdscr, setting_y, setting_value_x, "%" PRIu16,
                  *(uint16_t *)settings[i].setting_value_ptr);
      else if (settings[i].setting_type == SETTING_UINT8)
        mvwprintw(stdscr, setting_y, setting_value_x, "%" PRIu8,
                  *(uint8_t *)settings[i].setting_value_ptr);
      else if (settings[i].setting_type == SETTING_CHAR)
        mvwprintw(stdscr, setting_y, setting_value_x, "%c",
                  *(char *)settings[i].setting_value_ptr);

      if (selected == i && settings[i].setting_type != SETTING_NULL)
        attroff(A_REVERSE);
    }
    refresh();
  }
  clear();
  refresh();
}
