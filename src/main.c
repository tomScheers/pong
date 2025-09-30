#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "net.h"
#include "render.h"
#define FRAME_DELAY_MS 100

void loop(struct Game *game, enum PlayerAction your_action,
          enum PlayerAction opponent_action);

void draw_player(struct Game *game, enum PlayerAction action);
static void handle_connection(struct Game *game, int sock);
static enum PlayerAction handle_user_input(int ch);

struct Game *init_game();

int main(int argc, char **argv) {
  struct Game *game = init_game();

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

void loop(struct Game *game, enum PlayerAction your_action,
          enum PlayerAction opponent_action) {
  draw_player(game, your_action);
  mvaddch(game->ball_y, game->ball_x, '0');

  refresh();
  mvdelch(game->ball_y, game->ball_x);
  refresh();

  if (game->ball_x == game->game_width || game->ball_x == 0) {
    game->x_ball_orientation *= -1;
  }

  if (game->ball_y == game->game_height || game->ball_y == 0) {
    game->y_ball_orientation *= -1;

    // Check if next hit is going to be collision and adjust orientation
    int next_y = game->ball_y + game->y_ball_orientation;
    int next_x = game->ball_x + game->x_ball_orientation;
    if (mvinch(next_y, next_x) == '|') {
      game->x_ball_orientation *= -1;
      if (mvinch(next_y - 1, next_x + COLS) == '|' &&
          mvinch(next_y + 1, next_x - COLS) == '|') { // Central hit
        game->y_ball_orientation = 0;
      } else if (mvinch(next_y + 1, next_x) == '|') { // Upper hit
        game->y_ball_orientation = -1;
      } else {
        game->y_ball_orientation = 1;
      }
    }
    game->ball_x += game->x_ball_orientation;
    game->ball_y += game->y_ball_orientation;
  }
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

  game->plr_one_x = 1;
  game->plr_one_y = LINES / 2;
  game->plr_two_x = COLS;
  game->plr_one_y = LINES / 2;

  game->player_score_one = '0';
  game->player_score_two = '0';

  game->ball_y = game->game_height / 2 + 5;
  game->ball_x = game->game_width / 2;
  game->x_ball_orientation = -1;
  game->y_ball_orientation = 0;

  start_color();
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  bkgd(COLOR_PAIR(1));
  refresh();

  return game;
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
    data->action = handle_user_input(getch());
    data->action_time = time(NULL);

    if (net_send_msg(sock, data) == -1) {
      perror("net_send_msg");
      goto cleanup;
    }

    if (net_recv_msg(sock, rec_data) == -1) {
      perror("net_recv_msg");
      goto cleanup;
    }

    if (rec_data->action == QUIT || data->action == QUIT) {
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

void draw_player(struct Game *game, enum PlayerAction action) {
  switch (action) {
  case PAD_UP:
    mvdelch(game->plr_one_y + 1, game->plr_one_x);
    mvaddch(game->plr_one_y - 2, game->plr_one_x, '|');
    game->plr_one_y--;
    break;
  case PAD_DOWN:
    mvdelch(game->plr_one_y - 1, game->plr_one_x);
    mvaddch(game->plr_one_y + 2, game->plr_one_x, '|');
    game->plr_one_y++;
    break;
  default:
    return;
    break;
  }
}
