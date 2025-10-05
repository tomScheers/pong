#include "render.h"

#include <inttypes.h>
#include <ncurses.h>
#include <stdbool.h>
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

static void change_settings(struct SettingBox *settings, int settings_count);

void change_serve_settings(struct Game *game) {
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
  change_settings(settings, sizeof(settings) / sizeof(settings[0]));
}

void change_client_settings(struct Game *game) {
  struct SettingBox settings[] = {{.setting_str = "Port Number",
                                   .setting_value_ptr = &game->settings.port,
                                   .setting_type = SETTING_UINT16},
                                  {.setting_str = "Join",
                                   .setting_value_ptr = NULL,
                                   .setting_type = SETTING_NULL}};
  change_settings(settings, sizeof(settings) / sizeof(settings[0]));
}

static void change_settings(struct SettingBox *settings, int settings_count) {
  int height, width;
  getmaxyx(stdscr, height, width);
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
        erase();
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
        erase();
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
        erase();
      }
    }

    for (int i = 0; i < settings_count; ++i) {
      int setting_x = width / 2 - strlen(settings[i].setting_str) - 1;

      if (settings[i].setting_type == SETTING_NULL)
        setting_x = width / 2 - strlen(settings[i].setting_str) / 2 - 1;

      int setting_value_x = width / 2 + 1;
      int setting_y = start_y + i;

      if (selected == i)
        attron(A_REVERSE);

      mvwprintw(stdscr, setting_y, setting_x, "%s", settings[i].setting_str);

      if (selected == i)
        attroff(A_REVERSE);

      if (selected == i && settings[i].setting_type != SETTING_NULL &&
          is_editing)
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

      if (selected == i && settings[i].setting_type != SETTING_NULL &&
          is_editing)
        attroff(A_REVERSE);
    }
    refresh();
  }
  erase();
  refresh();
}
