#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "net.h"
#include "render.h"

int main(int argc, char **argv) {
  struct Game *game = init_game(argv, argc);
  if (!game) {
    perror("init_game");
    return EXIT_FAILURE;
  }

  int sock = -1;
  if (argc > 1 && strcmp(argv[1], "serve") == 0) {
    int serv_sock = net_serv_init_sock();

    if (serv_sock == -1) {
      perror("net_serv_init_sock");
      goto cleanup;
    }

    sock = net_serv_conn_client(serv_sock);

    close(serv_sock);

    if (sock == -1) {
      perror("net_serv_conn_client");
      goto cleanup;
    }
  } else {
    sock = net_client_init_sock();
    if (sock == -1) {
      perror("net_client_init_sock");
      goto cleanup;
    }
  }

  handle_connection(game, sock);

cleanup:
  endwin();
  close(sock);
  free(game);
  return EXIT_SUCCESS;
}
