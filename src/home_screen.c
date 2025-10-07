#include "render.h"

#include <ncurses.h>
#include <stdio.h>
#include <string.h>

enum Gamemode loading_screen() {
  size_t height, width;
  getmaxyx(stdscr, height, width);

  uint8_t selected = 0;
  uint8_t selected_option = selected;
  bool is_selecting_author = false;

  static const char *pixel_art[] = {" _______  _______  _        _______ ",
                                    "(  ____ )(  ___  )( (    /|(  ____ \\",
                                    "| (    )|| (   ) ||  \\  ( || (    \\/",
                                    "| (____)|| |   | ||   \\ | || |      ",
                                    "|  _____)| |   | || (\\ \\) || | ____ ",
                                    "| (      | |   | || | \\   || | \\_  )",
                                    "| )      | (___) || )  \\  || (___) |",
                                    "|/       (_______)|/    )_)(_______)"};

  static const char *authors[] = {"tomScheers", "code-redmark"};
  static const char *author_links[] = {"https://github.com/tomScheers",
                                       "https://github.com/code-redmark"};

  uint16_t author_amount = sizeof(authors) / sizeof(authors[0]);
  uint16_t author_print_size = 0;
  for (int i = 0; i < author_amount; ++i) {
    author_print_size += strlen(authors[i]);
  }
  if (author_amount > 2)
    author_print_size += (author_amount - 1) * 2;
  else if (author_amount == 2)
    author_print_size += 3;

  uint16_t pixel_art_height = sizeof(pixel_art) / sizeof(pixel_art[0]);
  uint16_t pixel_art_width = strlen(pixel_art[0]);

  static const char *msgs[] = {"serve", "join", "offline", "bot", "quit"};
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
  while ((ch = getch()) != '\n' || is_selecting_author) {
    for (int i = 0; i < pixel_art_height; ++i) {
      mvprintw(pixel_art_y + i, pixel_art_x, "%s", pixel_art[i]);
    }
    uint16_t author_y = pixel_art_y + pixel_art_height * 5 / 4;
    uint16_t author_x = (width - author_print_size) / 2;

    for (uint16_t i = 0; i < author_amount; ++i) {
      if (is_selecting_author && i == selected)
        attron(A_REVERSE);

      mvprintw(author_y, author_x, "%s", authors[i]);
      if (is_selecting_author && i == selected)
        attroff(A_REVERSE);

      author_x += strlen(authors[i]);

      if (author_amount == i + 2) {
        mvprintw(author_y, author_x, " & ");
        author_x += 3;
      } else if (author_amount > i + 2) {
        mvprintw(author_y, author_x, ", ");
        author_x += 2;
      }
    }

    refresh();

    if (ch == '\n' && is_selecting_author) {
      char cmd[256];
      snprintf(cmd, sizeof(cmd), "xdg-open \"%s\"", author_links[selected]);
      system(cmd);
    }

    if (IS_KEY_LEFT(ch) && selected > 0)
      --selected;

    if (IS_KEY_RIGHT(ch) && selected < msg_count - 1)
      ++selected;

    if (IS_KEY_UP(ch) && !is_selecting_author) {
      is_selecting_author = !is_selecting_author;
      selected_option = selected;
      selected = (float)selected / msg_count * author_amount;
    }

    if (IS_KEY_DOWN(ch) && is_selecting_author) {
      is_selecting_author = !is_selecting_author;
      selected = selected_option;
    }

    uint16_t option_x = space_between_words;

    for (size_t i = 0; i < msg_count; ++i) {
      if (!is_selecting_author && i == selected) {
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
