#include "render.h"
#include <ncurses.h>
#include <string.h>

enum EndScreenOption end_screen() {
  erase();

  static const char *options[] = {
      "restart",
      "home",
      "quit",
  };

  enum EndScreenOption option_values[] = {
      ES_RESTART,
      ES_HOME,
      ES_QUIT,
  };

  static const char *pixel_art_text = "GAME OVER";
  uint16_t pixel_art_height = 8;
  uint16_t pixel_art_y = (LINES - pixel_art_height) / 2 - pixel_art_height;

  uint8_t options_count = sizeof(options) / sizeof(options[0]);

  uint16_t space_between_words = COLS;

  uint16_t options_y =
      (LINES + ASCII_LINE_HEIGHT) / 2; // Get the middle of the screen
                                       // accounting for the pixel art

  for (size_t i = 0; i < options_count; ++i)
    space_between_words -= strlen(options[i]);

  space_between_words /= options_count + 1;

  uint8_t selected_option = 0;

  int ch;
  while ((ch = getch()) != '\n') {
    switch (ch) {
    case 'h':
      if (selected_option > 0)
        --selected_option;
      break;
    case 'l':
      if (selected_option + 1 < options_count)
        ++selected_option;
      break;
    }
    print_ascii(pixel_art_y, pixel_art_text);

    uint16_t option_x = space_between_words;

    for (size_t i = 0; i < options_count; ++i) {
      if (i == selected_option) {
        attron(A_REVERSE);
      }

      mvprintw(options_y, option_x, "%s", options[i]);

      if (i == selected_option) {
        attroff(A_REVERSE);
      }

      option_x += strlen(options[i]) + space_between_words;
    }
    napms(10);
  }

  erase();
  refresh();

  return option_values[selected_option];
}
