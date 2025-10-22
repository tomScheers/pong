#include "render.h"

#include <ctype.h>
#include <float.h>
#include <inttypes.h>
#include <math.h>
#include <ncurses.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define IS_BACKSPACE(ch) (ch == KEY_BACKSPACE || ch == 127 || ch == '\b')

#define PARSE_DIGIT(type, type_max)                                            \
  int64_t value = *(type *)settings[selected].setting_value_ptr;               \
  if (IS_BACKSPACE(ch)) {                                                      \
    value /= 10;                                                               \
  } else if (isdigit(ch)) {                                                    \
    value = value * 10 + (ch - '0');                                           \
    if (value > type_max)                                                      \
      value = type_max;                                                        \
  }                                                                            \
  *(type *)settings[selected].setting_value_ptr = (type)value;                 \
  erase();

enum SettingTypes {
  SETTING_UINT8,
  SETTING_UINT16,
  SETTING_INT16,
  SETTING_IP4,
  SETTING_CHAR,
  SETTING_NULL,
};

struct SettingBox {
  const char *setting_str;
  const char *setting_description;
  void *setting_value_ptr;
  enum SettingTypes setting_type;
};

static double get_char_ratio() {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
    return 2.1;
  if (ws.ws_col && ws.ws_xpixel && ws.ws_row && ws.ws_ypixel)
    return ((double)ws.ws_ypixel / ws.ws_row) /
           ((double)ws.ws_xpixel / ws.ws_col);
  return 2.1;
}

static void change_settings(struct Game *game, struct SettingBox *settings,
                            int settings_count, const char *config_msg);

void change_offline_settings(struct Game *game) {
  struct SettingBox settings[] = {
      {
          .setting_str = "Screen Height",
          .setting_description = "Adjusts the height of the pong window",
          .setting_value_ptr = &game->settings.screen_height,
          .setting_type = SETTING_UINT16,
      },
      {.setting_str = "Screen Width",
       .setting_description = "Adjusts the width of the pong window",
       .setting_value_ptr = &game->settings.screen_width,
       .setting_type = SETTING_UINT16},
      {.setting_str = "Winning Score",
       .setting_description = "Adjusts the score either one of the parties has "
                              "to reach for them to win the game",
       .setting_value_ptr = &game->settings.winning_score,
       .setting_type = SETTING_UINT16},
      {.setting_str = "FPS",
       .setting_description =
           "Adjusts at how many frames per second the game will run",
       .setting_value_ptr = &game->settings.frames_per_second,
       .setting_type = SETTING_UINT8},
      {.setting_str = "Pad Tiles",
       .setting_description =
           "Adjusts how many characters high the pads of both players will be",
       .setting_value_ptr = &game->settings.pad_tiles,
       .setting_type = SETTING_UINT8},
      {.setting_str = "Pad Char",
       .setting_description = "Dictates what char will be used for the pads",
       .setting_value_ptr = &game->settings.pad_char,
       .setting_type = SETTING_CHAR},
      {.setting_str = "Ball X Slope",
       .setting_description =
           "The speed at which the ball moves over the X-axis",
       .setting_value_ptr = &game->settings.base_ball_x_slope,
       .setting_type = SETTING_INT16},
      {.setting_str = "Ball Y Slope",
       .setting_description =
           "The speed at which the ball moves over the Y-axis",
       .setting_value_ptr = &game->settings.base_ball_y_slope,
       .setting_type = SETTING_INT16},
      {
          .setting_str = "Ball Size",
          .setting_description = "The size the ball will be",
          .setting_value_ptr = &game->settings.ball_size,
          .setting_type = SETTING_UINT16,
      },
      {.setting_str = "Ball Char",
       .setting_description = "Dictates what char will be used for the ball",
       .setting_value_ptr = &game->settings.ball_char,
       .setting_type = SETTING_CHAR},
      {.setting_str = "Start",
       .setting_value_ptr = NULL,
       .setting_type = SETTING_NULL}};
  change_settings(game, settings, sizeof(settings) / sizeof(settings[0]),
                  "offline config");
}
void change_serve_settings(struct Game *game) {
  struct SettingBox settings[] = {
      {.setting_str = "Screen Height",
       .setting_description = "Adjusts the height of the pong window",
       .setting_value_ptr = &game->settings.screen_height,
       .setting_type = SETTING_UINT16},
      {.setting_str = "Screen Width",
       .setting_description = "Adjusts the width of the pong window",
       .setting_value_ptr = &game->settings.screen_width,
       .setting_type = SETTING_UINT16},
      {.setting_str = "Port Number",
       .setting_description = "Adjusts on which port the program will run",
       .setting_value_ptr = &game->settings.port,
       .setting_type = SETTING_UINT16},
      {.setting_str = "Winning Score",
       .setting_description = "Adjusts the score either one of the parties has "
                              "to reach for them to win the game",
       .setting_value_ptr = &game->settings.winning_score,
       .setting_type = SETTING_UINT16},
      {.setting_str = "FPS",
       .setting_description =
           "Adjusts at how many frames per second the game will run",
       .setting_value_ptr = &game->settings.frames_per_second,
       .setting_type = SETTING_UINT8},
      {.setting_str = "Pad Tiles",
       .setting_description =
           "Adjusts how many characters high the pads of both players will be",
       .setting_value_ptr = &game->settings.pad_tiles,
       .setting_type = SETTING_UINT8},
      {.setting_str = "Pad Char",
       .setting_description = "Dictates what char will be used for the pads",
       .setting_value_ptr = &game->settings.pad_char,
       .setting_type = SETTING_CHAR},
      {.setting_str = "Ball X Slope",
       .setting_description =
           "The speed at which the ball moves over the X-axis",
       .setting_value_ptr = &game->settings.base_ball_x_slope,
       .setting_type = SETTING_INT16},
      {.setting_str = "Ball Y Slope",
       .setting_description =
           "The speed at which the ball moves over the Y-axis",
       .setting_value_ptr = &game->settings.base_ball_y_slope,
       .setting_type = SETTING_INT16},
      {
          .setting_str = "Ball Size",
          .setting_description = "The size the ball will be",
          .setting_value_ptr = &game->settings.ball_size,
          .setting_type = SETTING_UINT16,
      },
      {.setting_str = "Ball Char",
       .setting_description = "Dictates what char will be used for the ball",
       .setting_value_ptr = &game->settings.ball_char,
       .setting_type = SETTING_CHAR},
      {.setting_str = "Start",
       .setting_value_ptr = NULL,
       .setting_type = SETTING_NULL}};
  change_settings(game, settings, sizeof(settings) / sizeof(settings[0]),
                  "server config");
}

void change_client_settings(struct Game *game) {
  struct SettingBox settings[] = {
      {.setting_str = "Port Number",
       .setting_description =
           "Dictates on what port the client will connect to the server",
       .setting_value_ptr = &game->settings.port,
       .setting_type = SETTING_UINT16},
      {.setting_str = "IP Address",
       .setting_description =
           "IP address of the machine you wanna join the game of",
       .setting_value_ptr = game->settings.ip_octets,
       .setting_type = SETTING_IP4},
      {.setting_str = "Join",
       .setting_value_ptr = NULL,
       .setting_type = SETTING_NULL}};
  change_settings(game, settings, sizeof(settings) / sizeof(settings[0]),
                  "client config");
}

static void change_settings(struct Game *game, struct SettingBox *settings,
                            int settings_count, const char *config_msg) {
  int height, width;
  getmaxyx(stdscr, height, width);
  int start_y = height / 2 - settings_count / 2;
  int selected = 0;
  int selected_octet = 0;
  bool is_editing = false;

  int ch;
  while (true) {
    ch = getch();

    if (ch == '\n' && settings[selected].setting_type == SETTING_NULL) {
      break;
    } else if (IS_KEY_DOWN(ch) && selected < settings_count - 1 &&
               !is_editing) {
      ++selected;
      erase();
      refresh();
    } else if (IS_KEY_UP(ch) && selected > 0 && !is_editing) {
      --selected;
      erase();
      refresh();
    } else if ((ch == '\n' && (settings[selected].setting_type != SETTING_IP4 ||
                               !is_editing)) ||
               selected_octet > 3) {
      is_editing = !is_editing;
      selected_octet = 0;
      if (game->settings.screen_height < game->settings.pad_tiles)
        game->settings.pad_tiles = game->settings.screen_height;

      if (game->settings.screen_height > MAX_SCREEN_HEIGHT)
        game->settings.screen_height = MAX_SCREEN_HEIGHT;

      if (game->settings.screen_width > MAX_SCREEN_WIDTH)
        game->settings.screen_width = MAX_SCREEN_WIDTH;

      if (game->settings.screen_width < MIN_SCREEN_WIDTH)
        game->settings.screen_width = MIN_SCREEN_WIDTH;

      if (game->settings.screen_height < MIN_SCREEN_HEIGHT)
        game->settings.screen_height = MIN_SCREEN_HEIGHT;

      if (game->settings.pad_tiles > game->settings.screen_height)
        game->settings.pad_tiles = game->settings.screen_height;

      if (game->settings.pad_tiles < 1)
        game->settings.pad_tiles = 1;

      if (game->settings.frames_per_second < 1)
        game->settings.frames_per_second = 1;

      if (game->settings.ball_size < 1)
        game->settings.ball_size = 1;

      if (game->settings.ball_size > game->settings.screen_width ||
          game->settings.ball_size >
              game->settings.screen_height * get_char_ratio())
        game->settings.ball_size =
            game->settings.screen_width >
                    game->settings.screen_height * get_char_ratio()
                ? game->settings.screen_height * get_char_ratio()
                : game->settings.screen_height;

      erase();
    } else if (IS_KEY_LEFT(ch) &&
               settings[selected].setting_type == SETTING_IP4 &&
               selected_octet > 0) {
      --selected_octet;
    } else if (IS_KEY_RIGHT(ch) &&
               settings[selected].setting_type == SETTING_IP4 &&
               selected_octet < 3) {
      ++selected_octet;
    } else if (ch == '\n' && settings[selected].setting_type == SETTING_IP4 &&
               is_editing) {
      ++selected_octet;
      if (selected_octet > 3) {
        selected_octet = 0;
        is_editing = false;
      }
    }

    print_ascii(start_y / 2, config_msg);

    if (is_editing && ch != '\n' && ch != ERR) {
      if (settings[selected].setting_type == SETTING_CHAR) {
        *(char *)settings[selected].setting_value_ptr = ch;
        is_editing = false;
        erase();
      } else if (settings[selected].setting_type == SETTING_UINT8) {
        PARSE_DIGIT(uint8_t, UINT8_MAX);
      } else if (settings[selected].setting_type == SETTING_UINT16) {
        PARSE_DIGIT(uint16_t, UINT16_MAX);
      }
      //      } else if (settings[selected].setting_type == SETTING_FLOAT) {
      //        double value = *(float
      //        *)(settings[selected].setting_value_ptr); if
      //        (IS_BACKSPACE(ch)) {
      //          value /= 10;
      //        } else if (isdigit(ch)) {
      //          value = value * 10 + (ch - '0');
      //
      //          if (value > FLT_MAX)
      //            value = FLT_MAX;
      //        } else if (ch == ',') {
      //          if (fmodf(value, 1.0) == 0)
      //            return;
      //
      //          value += value + 0.0;
      //        }
      //
      //        *(uint8_t *)(settings[selected].setting_value_ptr +
      //        selected_octet) =
      //            (uint8_t)value;
      //
      //        erase();
      else if (settings[selected].setting_type == SETTING_INT16) {
        int32_t value =
            *(int16_t *)(settings[selected].setting_value_ptr + selected_octet);
        if (IS_BACKSPACE(ch)) {
          value /= 10;
        } else if (isdigit(ch)) {
          value = value * 10 + (ch - '0');

          if (value > UINT8_MAX)
            value = UINT8_MAX;
        } else if (ch == '-') {
          value *= -1;
        }

        *(int16_t *)(settings[selected].setting_value_ptr) = (int16_t)value;

        erase();
      } else if (settings[selected].setting_type == SETTING_IP4) {
        int16_t value =
            *(uint8_t *)(settings[selected].setting_value_ptr + selected_octet);
        if (IS_BACKSPACE(ch)) {
          value /= 10;
        } else if (isdigit(ch)) {
          value = value * 10 + (ch - '0');

          if (value > UINT8_MAX)
            value = UINT8_MAX;
        }

        *(uint8_t *)(settings[selected].setting_value_ptr + selected_octet) =
            (uint8_t)value;

        erase();
      }

      refresh();
    }

    if (settings[selected].setting_type != SETTING_NULL) {
      uint16_t x = (width - strlen(settings[selected].setting_description)) / 2;
      mvprintw(start_y - 2, x, "%s", settings[selected].setting_description);
    }

    for (int i = 0; i < settings_count; ++i) {
      int setting_x = width / 2 - strlen(settings[i].setting_str) - 1;
      int setting_y = start_y + i;

      if (settings[i].setting_type == SETTING_NULL)
        setting_x = (width - strlen(settings[i].setting_str)) / 2 - 1;

      int setting_value_x = width / 2 + 1;

      if (selected == i)
        attron(A_REVERSE);

      mvprintw(setting_y, setting_x, "%s", settings[i].setting_str);

      if (selected == i)
        attroff(A_REVERSE);

      if (selected == i && settings[i].setting_type != SETTING_NULL &&
          settings[i].setting_type != SETTING_IP4 && is_editing)
        attron(A_REVERSE);

      if (settings[i].setting_type == SETTING_UINT16)
        mvprintw(setting_y, setting_value_x, "%" PRIu16,
                 *(uint16_t *)settings[i].setting_value_ptr);
      else if (settings[i].setting_type == SETTING_UINT8)
        mvprintw(setting_y, setting_value_x, "%" PRIu8,
                 *(uint8_t *)settings[i].setting_value_ptr);
      else if (settings[i].setting_type == SETTING_INT16)
        mvprintw(setting_y, setting_value_x, "%" PRIi16,
                 *(int16_t *)settings[i].setting_value_ptr);
      else if (settings[i].setting_type == SETTING_CHAR)
        mvprintw(setting_y, setting_value_x, "%c",
                 *(char *)settings[i].setting_value_ptr);
      //      else if (settings[i].setting_type == SETTING_FLOAT)
      //        mvprintw(setting_y, setting_value_x, "%f",
      //                 *(float *)settings[i].setting_value_ptr);
      else if (settings[i].setting_type == SETTING_IP4) {
        for (int j = 0; j < 4; ++j) {
          if (j == selected_octet && i == selected && is_editing)
            attron(A_REVERSE);
          mvprintw(setting_y, setting_value_x + j * 4, "%" PRIu8,
                   *(uint8_t *)(settings[i].setting_value_ptr + j));
          if (j == selected_octet && i == selected && is_editing)
            attroff(A_REVERSE);
        }
      }

      if (selected == i && settings[i].setting_type != SETTING_NULL &&
          settings[i].setting_type != SETTING_IP4 && is_editing)
        attroff(A_REVERSE);
    }
    refresh();
    napms(10);
  }
  erase();
  refresh();
}
