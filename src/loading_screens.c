#include "render.h"

#include <ifaddrs.h>
#include <inttypes.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

void server_loading_screen(struct Game *game) {
  struct ifaddrs *ifaddr, *ifa;
  char ip[INET_ADDRSTRLEN];

  if (getifaddrs(&ifaddr) == -1) {
    perror("getifaddrs");
    exit(EXIT_FAILURE);
  }

  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr == NULL)
      continue;
    if (ifa->ifa_addr->sa_family == AF_INET) { // IPv4
      struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
      inet_ntop(AF_INET, &sa->sin_addr, ip, sizeof(ip));
    }
  }

  freeifaddrs(ifaddr);

  char msg[128];
  snprintf(msg, sizeof(msg), "%s:%d", ip, game->settings.port);
  print_ascii(LINES / 2 - 12, "Waiting on client...");
  print_ascii(LINES / 2 + 4, msg);
  refresh();
}

enum PauseOptions pause_screen() {
  erase();
  size_t height, width;
  getmaxyx(stdscr, height, width);

  uint8_t selected_option = 0;

  static const char *pixel_art_text = "game paused";

  static const char *options[] = {"home", "resume", "quit"};

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
    print_ascii(pixel_art_y, pixel_art_text);

    refresh();

    if (IS_KEY_LEFT(ch) && selected_option > 0)
      --selected_option;

    else if (IS_KEY_RIGHT(ch) && selected_option < options_count - 1)
      ++selected_option;

    uint16_t option_x = space_between_words;

    for (size_t i = 0; i < options_count; ++i) {
      if (i == selected_option) {
        attron(A_REVERSE);
        mvprintw(options_y, option_x, "%s", options[i]);
        attroff(A_REVERSE);
      } else {
        mvprintw(options_y, option_x, "%s", options[i]);
      }
      option_x += strlen(options[i]) + space_between_words;
    }
    refresh();
    napms(10);
  }
  erase();
  refresh();
  return selected_option;
}

enum PauseOptions pause_screen_net(int sock) {
  erase();
  size_t height, width;
  getmaxyx(stdscr, height, width);

  uint8_t selected_option = 0;

  static const char *pixel_art_text = "game paused";

  static const char *options[] = {"home", "resume", "quit"};

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

    fd_set readfds;
    struct timeval tv = {0, 0}; // non-blocking check

    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);

    int ready = select(sock + 1, &readfds, NULL, NULL, &tv);
    if (ready > 0 && FD_ISSET(sock, &readfds)) {
      ssize_t bytes = recv(sock, &rec_opt, sizeof(rec_opt), 0);
      if (bytes > 0)
        return rec_opt;
    }

    print_ascii(pixel_art_y, pixel_art_text);

    if (IS_KEY_LEFT(ch) && selected_option > 0)
      --selected_option;

    else if (IS_KEY_RIGHT(ch) && selected_option < options_count - 1)
      ++selected_option;

    uint16_t option_x = space_between_words;

    for (size_t i = 0; i < options_count; ++i) {
      if (i == selected_option) {
        attron(A_REVERSE);
        mvprintw(options_y, option_x, "%s", options[i]);
        attroff(A_REVERSE);
      } else {
        mvprintw(options_y, option_x, "%s", options[i]);
      }
      option_x += strlen(options[i]) + space_between_words;
    }
    refresh();
    napms(10);
  }
  return selected_option;
}
