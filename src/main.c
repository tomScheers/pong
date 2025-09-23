#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "net.h"
#include "pong.h"

void loop(struct Game *game, enum PlayerAction your_action,
          enum PlayerAction opponend_action);
struct Game *init_game();

int main(int argc, char **argv) {
  /*  struct Game *game = init_game();
    int ch;
    while (game->running) {
      ch = getch();
      if (ch == 'q') {
        game->running = false;
        break;
      }
      loop(game, NONE, NONE);
    }

    endwin();
    free(game);
  */
  if (strcmp(argv[1], "serve") == 0) {
    int serv_sock = net_serv_init_sock();
    printf("socket initialized\n");
    int client_sock = net_serv_conn_client(serv_sock);
    printf("Client socket found\n");
    struct DataMsg *data = malloc(sizeof(*data));
    data->action = PAD_UP;
    data->action_time = time(NULL);
    net_send_msg(client_sock, data);
    printf("Message sent\n");
    free(data);
  } else {
    int sock = net_client_init_sock();
    printf("Socket initialized\n");
    struct DataMsg *data = net_recv_msg(sock);
    printf("Messaged received\n");
    printf("Action: %d, Time: %ld\n", data->action, data->action_time);
    free(data);
  }
  return 0;
}

void loop(struct Game *game, enum PlayerAction your_action,
          enum PlayerAction opponend_action) {
  mvaddch(game->ball_y, game->ball_x, '0');
  refresh();
  napms(40);
  mvdelch(game->ball_y, game->ball_x);
  refresh();

  if (game->ball_x == game->game_width) {
    game->x_ball_orientation = -1;
  } else if (game->ball_x == 0) {
    game->x_ball_orientation = 1;
  }
  if (game->ball_y == game->game_height) {
    game->y_ball_orientation = -1;
  } else if (game->ball_y == 0) {
    game->y_ball_orientation = 1;
  }
  game->ball_x += game->x_ball_orientation;
  game->ball_y += game->y_ball_orientation;
}

struct Game *init_game() {
  struct Game *game = malloc(sizeof(struct Game));

  game->running = true;
  game->game_height = 10;
  game->game_width = 30;

  initscr();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  cbreak();
  noecho();
  curs_set(0);

  game->ball_y = game->game_height / 2 + 5;
  game->ball_x = game->game_width / 2;
  game->x_ball_orientation = 1;
  game->y_ball_orientation = 1;

  start_color();
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));
  refresh();

  return game;
}
