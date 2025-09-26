#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "net.h"
#include "render.h"

void server(struct Game *game);
void client(struct Game *game);

int main(int argc, char **argv) {
  // struct Game *game = init_game();
  struct Game *game = NULL;

  if (strcmp(argv[1], "serve") == 0) {
    server(game);
  } else {
    client(game);
  }

  // endwin();
  // free(game);
  return 0;
}

void server(struct Game *game) {
  int serv_sock = net_serv_init_sock();
  int sock = net_serv_conn_client(serv_sock);
  printf("closing socket\n");
  close(serv_sock);
  int ch;
  struct DataMsg *data = malloc(sizeof(*data));
  while (true) {
    data->action = PAD_UP;
    ch = getch();
    if (ch == 'q') {
      game->running = false;
      data->action = QUIT;
    }
    data->action_time = time(NULL);
    printf("Waiting 4 seconds...\n");
    sleep(1);
    printf("1\n");
    sleep(1);
    printf("2\n");
    sleep(1);
    printf("3\n");
    sleep(1);
    printf("4\n");
    net_send_msg(sock, data);
    struct DataMsg *rec_data = net_recv_msg(sock);
    printf("Received move: %d\n", rec_data->action);
  }
  // loop(game, data->action, rec_data->action);
  free(data);
  close(sock);
  // free(rec_data);
}

void client(struct Game *game) {
  int sock = net_client_init_sock();
  if (sock == -1) {
    perror("net_client_init_sock");
    return;
  }
  int ch;
  struct DataMsg *data = malloc(sizeof(*data));
  struct DataMsg *rec_data = net_recv_msg(sock);
  while (true) {
    data->action = PAD_UP;
    ch = getch();
    if (ch == 'q') {
      game->running = false;
      data->action = QUIT;
    }
    data->action_time = time(NULL);
    if (!rec_data) {
      printf("No data receive\n");
    }
    printf("Move: %d\n", rec_data->action);
    printf("Waiting 4 seconds...\n");
    sleep(1);
    printf("1\n");
    sleep(1);
    printf("2\n");
    sleep(1);
    printf("3\n");
    sleep(1);
    printf("4\n");
    net_send_msg(sock, data);
  }
  // loop(game, data->action, rec_data->action);
  free(data);
  free(rec_data);
  close(sock);
}
