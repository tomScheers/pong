#include "render.h"

#include <ncurses.h>
#include <stdio.h>
#include <string.h>

static void print_ascii_art();
static void print_authors(const char *authors[], uint8_t author_amount,
                          uint8_t selected_author, bool is_selecting_author);
inline static void open_link(const char *link);

enum Gamemode loading_screen() {
  static const char *authors[] = {"tomScheers", "code-redmark"};
  static const char *author_links[] = {"https://github.com/tomScheers",
                                       "https://github.com/code-redmark"};

  static const char *options[] = {"serve", "join", "offline", "quit"};
  enum Gamemode option_value[] = {GM_SERVE, GM_JOIN, GM_OFFLINE, GM_QUIT};

  uint8_t selected_author = 0;
  uint8_t selected_option = 0;
  bool is_selecting_author = false;

  uint16_t author_amount = sizeof(authors) / sizeof(authors[0]);
  size_t options_count = sizeof(options) / sizeof(options[0]);

  uint16_t space_between_words = COLS;

  uint16_t options_y =
      (LINES + ASCII_LINE_HEIGHT) / 2; // Get the middle of the screen
                                       // accounting for the pixel art

  for (size_t i = 0; i < options_count; ++i)
    space_between_words -= strlen(options[i]);

  space_between_words /= options_count + 1;

  int ch;
  while ((ch = getch()) != '\n' || is_selecting_author) {
    print_ascii_art();
    print_authors(authors, author_amount, selected_author, is_selecting_author);

    refresh();

    if (ch == '\n' && is_selecting_author) {
      open_link(author_links[selected_author]);
    }

    if (IS_KEY_LEFT(ch) && selected_option > 0 && !is_selecting_author)
      --selected_option;

    else if (IS_KEY_LEFT(ch) && selected_author > 0 && is_selecting_author)
      --selected_author;

    else if (IS_KEY_RIGHT(ch) && selected_option < options_count - 1 &&
             !is_selecting_author)
      ++selected_option;

    else if (IS_KEY_RIGHT(ch) && selected_author < author_amount - 1 &&
             is_selecting_author)
      ++selected_author;

    else if (IS_KEY_UP(ch) && !is_selecting_author)
      is_selecting_author = !is_selecting_author;

    else if (IS_KEY_DOWN(ch) && is_selecting_author)
      is_selecting_author = !is_selecting_author;

    uint16_t option_x = space_between_words;

    for (size_t i = 0; i < options_count; ++i) {
      if (!is_selecting_author && i == selected_option) {
        attron(A_REVERSE);
      }

      mvprintw(options_y, option_x, "%s", options[i]);

      if (!is_selecting_author && i == selected_option) {
        attroff(A_REVERSE);
      }

      option_x += strlen(options[i]) + space_between_words;
    }
    refresh();
    napms(10);
  }
  erase();
  refresh();

  return option_value[selected_option];
}

static void print_ascii_art() {
  static const char *pixel_art_text = "p0ng";
  // Get pixel_art variable
  uint16_t pixel_art_height = 8;
  uint16_t pixel_art_y = (LINES - pixel_art_height) / 2 - pixel_art_height;

  print_ascii(pixel_art_y, pixel_art_text);
}

static void print_authors(const char *authors[], uint8_t author_amount,
                          uint8_t selected_author, bool is_selecting_author) {
  size_t height, width;
  getmaxyx(stdscr, height, width);
  uint16_t pixel_art_y = (height - ASCII_LINE_HEIGHT) / 2 - ASCII_LINE_HEIGHT;

  // Get author variables
  uint16_t author_print_size = 0;
  for (int i = 0; i < author_amount; ++i) {
    author_print_size += strlen(authors[i]);
  }

  if (author_amount > 2)
    author_print_size += (author_amount - 1) * 2;
  else if (author_amount == 2)
    author_print_size += 3;

  uint16_t author_y =
      pixel_art_y + ASCII_LINE_HEIGHT * 1.25; // 1.25 is magic number
  //
  uint16_t author_x = (width - author_print_size) / 2;

  for (uint16_t i = 0; i < author_amount; ++i) {
    if (is_selecting_author && i == selected_author && is_selecting_author)
      attron(A_REVERSE);

    mvprintw(author_y, author_x, "%s", authors[i]);

    if (is_selecting_author && i == selected_author && is_selecting_author)
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
}

inline static void open_link(const char *link) {
  char cmd[256];
  snprintf(cmd, sizeof(cmd), "xdg-open \"%s\"", link);
  system(cmd);
}
