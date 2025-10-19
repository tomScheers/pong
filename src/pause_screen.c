#include "render.h"

#include <ncurses.h>
#include <string.h>
#include <sys/select.h>

typedef bool (*PauseUpdateFunc)(enum PauseOptions *rec_opt, void *ctx);

static bool pause_update_func(enum PauseOptions *rec_opt, void *ctx);
static enum PauseOptions pause_screen_base(PauseUpdateFunc update_fn,
                                           void *ctx);

enum PauseOptions pause_screen() { return pause_screen_base(NULL, NULL); }

enum PauseOptions pause_screen_net(int sock) {
  return pause_screen_base(pause_update_func, &sock);
}

static bool pause_update_func(enum PauseOptions *rec_opt, void *ctx) {
  int sock = *(int *)ctx;

  fd_set readfds;
  struct timeval tv = {0, 0}; // non-blocking check

  FD_ZERO(&readfds);
  FD_SET(sock, &readfds);

  int ready = select(sock + 1, &readfds, NULL, NULL, &tv);
  if (ready > 0 && FD_ISSET(sock, &readfds)) {
    ssize_t bytes = recv(sock, &rec_opt, sizeof(rec_opt), 0);
    if (bytes > 0)
      return true;
  }
  return false;
}

static enum PauseOptions pause_screen_base(PauseUpdateFunc update_fn,
                                           void *ctx) {
  erase();
  size_t height, width;
  getmaxyx(stdscr, height, width);

  uint8_t selected_option = 0;

  static const char *pixel_art_text = "game paused";
  static const char *options[] = {"home", "resume", "quit"};
  static enum PauseOptions option_value[] = {PO_HOME, PO_RESUME, PO_QUIT};

  // Get pixel_art variable
  uint16_t pixel_art_height = 8;
  uint16_t pixel_art_y = (height - pixel_art_height) / 2 - pixel_art_height;

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
  while ((ch = getch()) != '\n') {
    enum PauseOptions rec_opt;

    if (update_fn && update_fn(&rec_opt, ctx))
      return rec_opt;

    print_ascii(pixel_art_y, pixel_art_text);

    if (IS_KEY_LEFT(ch) && selected_option > 0)
      --selected_option;

    else if (IS_KEY_RIGHT(ch) && selected_option < options_count - 1)
      ++selected_option;

    uint16_t option_x = space_between_words;

    for (size_t i = 0; i < options_count; ++i) {
      if (i == selected_option)
        attron(A_REVERSE);

      mvprintw(options_y, option_x, "%s", options[i]);

      if (i == selected_option)
        attroff(A_REVERSE);

      option_x += strlen(options[i]) + space_between_words;
    }
    refresh();
    napms(10);
  }

  erase();
  refresh();
  return option_value[selected_option];
}
