#include "render.h"

#include <ncurses.h>
#include <string.h>

enum Gamemode loading_screen() {
  size_t height, width;
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
  uint16_t pixel_art_height = sizeof(pixel_art) / sizeof(pixel_art[0]);
  uint16_t pixel_art_width = strlen(pixel_art[0]);

  const char *msgs[] = {"serve", "join", "offline", "bot", "quit"};
  size_t msg_count = sizeof(msgs) / sizeof(msgs[0]);

  int total_len = 0;
  for (size_t i = 0; i < msg_count; ++i)
    total_len += strlen(msgs[i]);

  uint16_t gap_amount = msg_count + 1;
  uint16_t space_between_words = (width - total_len) / gap_amount;

  uint16_t options_y =
      height / 2 +
      pixel_art_height /
          2; // Get the middle of the screen accounting for the pixel art

  uint16_t pixel_art_y = height / 2 - pixel_art_height / 2 - pixel_art_height;
  uint16_t pixel_art_x = (width - pixel_art_width) / 2;

  int ch;
  while ((ch = getch()) != '\n') {
    for (int i = 0; i < pixel_art_height; ++i) {
      mvprintw(pixel_art_y + i, pixel_art_x, "%s", pixel_art[i]);
    }

    if (IS_KEY_LEFT(ch) && selected > 0)
      --selected;

    if (IS_KEY_RIGHT(ch) && selected < msg_count - 1)
      ++selected;

    uint16_t option_x = space_between_words;

    for (size_t i = 0; i < msg_count; ++i) {
      if (i == selected) {
        attron(A_REVERSE);
        mvprintw(options_y, option_x, "%s", msgs[i]);
        attroff(A_REVERSE);
      } else {
        mvprintw(options_y, option_x, "%s", msgs[i]);
      }
      option_x += strlen(msgs[i]) + space_between_words;
    }
    refresh();
  }
  erase();
  refresh();
  return selected;
}
