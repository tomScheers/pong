#include "net.h"
#include "render.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>

static enum PlayerAction handle_user_input(int ch);

int net_recv_msg(int opp_sock, struct DataMsg *ret_msg) {
  return recv(opp_sock, ret_msg, sizeof(*ret_msg), 0);
}

int net_send_msg(int opp_sock, struct DataMsg *data) {
  return send(opp_sock, data, sizeof(*data), 0);
}

void handle_connection(struct Game *game, int sock) {
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
    data->action = handle_user_input(getch());
    flushinp();
    data->action_time = time(NULL);

    if (net_send_msg(sock, data) == -1) {
      perror("net_send_msg");
      goto cleanup;
    }

    if (net_recv_msg(sock, rec_data) == -1) {
      perror("net_recv_msg");
      goto cleanup;
    }

    if (rec_data->action == QUIT_GAME || data->action == QUIT_GAME) {
      game->running = false;
    } else {
      render(game, data->action, rec_data->action);
    }

    if (game->settings.frames_per_second > 0)
      napms(1000 / game->settings.frames_per_second);
    else
      napms(50);
  }
cleanup:
  free(rec_data);
  free(data);
}

static enum PlayerAction handle_user_input(int ch) {
  if (IS_KEY_UP(ch))
    return PAD_UP;
  else if (IS_KEY_DOWN(ch))
    return PAD_DOWN;
  else if (ch == 'q')
    return QUIT_GAME;
  else
    return NONE;
}
