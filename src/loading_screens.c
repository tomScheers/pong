#include "render.h"

#include <ifaddrs.h>
#include <inttypes.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

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
