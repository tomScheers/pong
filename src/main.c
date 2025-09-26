#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "net.h"
#include "render.h"

#define FRAME_DELAY_MS 1000

static void handle_connection(struct Game *game, int sock);
static enum PlayerAction handle_user_input(int ch);

int main(int argc, char **argv) {
  int return_status = EXIT_SUCCESS;

  struct Game *game = init_game();
  int sock = -1; // Invalid socket

  if (strcmp(argv[1], "serve") == 0) {
    int serv_sock = net_serv_init_sock();

    if (serv_sock == -1) {
      perror("net_serv_init_sock");
      return_status = EXIT_FAILURE;
      goto cleanup;
    }

    sock = net_serv_conn_client(serv_sock);

    close(serv_sock);

    if (sock == -1) {
      perror("net_serv_conn_client");
      return_status = EXIT_FAILURE;
      goto cleanup;
    }
  } else {
    sock = net_client_init_sock();
    if (sock == -1) {
      perror("net_client_init_sock");
      return_status = EXIT_FAILURE;
      return EXIT_FAILURE;
    }
  }

  handle_connection(game, sock);

cleanup:
  close(sock);
  endwin();
  free(game);
  return return_status;
}

static void handle_connection(struct Game *game, int sock) {
  struct DataMsg *data = malloc(sizeof(*data));
  if (!data) {
    perror("malloc");
    return;
  }

  struct DataMsg *rec_data = malloc(sizeof(*rec_data));
  if (!rec_data) {
    perror("malloc");
    goto cleanup;
  }

  while (game->running) {
    data->action = handle_user_input(getchar());
    data->action_time = time(NULL);

    if (net_send_msg(sock, data) == -1) {
      perror("net_send_msg");
      goto cleanup;
    }

    if (net_recv_msg(sock, rec_data) == -1) {
      perror("net_recv_msg");
      goto cleanup;
    }

    if (rec_data->action == QUIT) {
      game->running = false;
    } else {
      loop(game, data->action, rec_data->action);
    }

    napms(FRAME_DELAY_MS);
  }
cleanup:
  free(rec_data);
  free(data);
}

static enum PlayerAction handle_user_input(int ch) {
  switch (ch) {
  case 'q':
    return QUIT;
  case 'j':
    return PAD_DOWN;
  case 'k':
    return PAD_UP;
  default:
    return NONE;
  }
}
