#include "render.h"

#include <ncurses.h>
#include <string.h>

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
  erase();
  refresh();
  return selected;
}
