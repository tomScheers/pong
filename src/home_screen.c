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

  static const char *options[] = {"serve", "join", "offline", "bot", "quit"};

  // Get pixel_art variable
  uint16_t pixel_art_height = sizeof(pixel_art) / sizeof(pixel_art[0]);
  uint16_t pixel_art_width = strlen(pixel_art[0]);
  uint16_t pixel_art_y = (height - pixel_art_height) / 2 - pixel_art_height;
  uint16_t pixel_art_x = (width - pixel_art_width) / 2;

  // Get author variables
  uint16_t author_amount = sizeof(authors) / sizeof(authors[0]);
  uint16_t author_print_size = 0;

  for (int i = 0; i < author_amount; ++i) {
    author_print_size += strlen(authors[i]);
  }

  if (author_amount > 2)
    author_print_size += (author_amount - 1) * 2;
  else if (author_amount == 2)
    author_print_size += 3;

  // Get option variables
  size_t options_count = sizeof(options) / sizeof(options[0]);
  uint16_t gap_amount =
      options_count +
      1; // + 1 for the gap between the last option and the end of the screen

  uint16_t space_between_words = width;

  for (size_t i = 0; i < options_count; ++i)
    space_between_words -= strlen(options[i]);

  space_between_words /= gap_amount;

  uint16_t options_y =
      (height + pixel_art_height) / 2; // Get the middle of the screen
                                       // accounting for the pixel art

  int ch;
  while ((ch = getch()) != '\n' || is_selecting_author) {
    for (int i = 0; i < pixel_art_height; ++i) {
      mvprintw(pixel_art_y + i, pixel_art_x, "%s", pixel_art[i]);
    }
    uint16_t author_y =
        pixel_art_y + pixel_art_height * 1.25; // 1.25 is magic number
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

    if (IS_KEY_RIGHT(ch) && selected < options_count - 1)
      ++selected;

    if (IS_KEY_UP(ch) && !is_selecting_author) {
      is_selecting_author = !is_selecting_author;
      selected_option = selected;
      selected = (float)selected / options_count * author_amount;
    }

    if (IS_KEY_DOWN(ch) && is_selecting_author) {
      is_selecting_author = !is_selecting_author;
      selected = selected_option;
    }

    uint16_t option_x = space_between_words;

    for (size_t i = 0; i < options_count; ++i) {
      if (!is_selecting_author && i == selected) {
        attron(A_REVERSE);
        mvprintw(options_y, option_x, "%s", options[i]);
        attroff(A_REVERSE);
      } else {
        mvprintw(options_y, option_x, "%s", options[i]);
      }
      option_x += strlen(options[i]) + space_between_words;
    }
    refresh();
  }
  erase();
  refresh();
  return selected;
}
