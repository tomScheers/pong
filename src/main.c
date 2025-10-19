#include <arpa/inet.h>
#include <ifaddrs.h>
#include <inttypes.h>
#include <math.h>
#include <ncurses.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "net.h"
#include "render.h"

struct Setting {
  const char *setting;
  void *settings_field;
};

void offline_mode(struct Game *game);

int main(int argc, char **argv) {
  struct Game *game = init_game(argv, argc);
  if (!game) {
    perror("init_game");
    return EXIT_FAILURE;
  }

  while (true) {
    enum Gamemode gamemode = loading_screen();

    if (gamemode == SERVE) {
      int sock = -1;
      change_serve_settings(game);
      server_loading_screen(game);
      int serv_sock = net_serv_init_sock(game->settings.port);

      if (serv_sock == -1) {
        perror("net_serv_init_sock");
        break;
      }

      sock = net_serv_conn_client(serv_sock);

      close(serv_sock);

      if (sock == -1) {
        perror("net_serv_conn_client");
        break;
      }

      send(sock, &game->settings, sizeof(game->settings), 0);
      recv(sock, &game->settings, sizeof(game->settings), 0);
      set_game_fields(game);
      handle_connection(game, sock);
      close(sock);
    } else if (gamemode == JOIN) {
      if (game->settings.screen_width % 2 == 1)
        ++game->ball_x;
      change_client_settings(game);
      int sock = -1;
      uint32_t packed_ip_address = (game->settings.ip_octets[0] << 24) |
                                   (game->settings.ip_octets[1] << 16) |
                                   (game->settings.ip_octets[2] << 8) |
                                   game->settings.ip_octets[3];
      sock = net_client_init_sock(game->settings.port, packed_ip_address);
      if (sock == -1) {
        perror("net_client_init_sock");
        break;
      }
      recv(sock, &game->settings, sizeof(game->settings), 0);
      if (game->settings.screen_width > COLS - 2) {
        game->settings.screen_width = COLS - 2;
      }
      if (game->settings.screen_height > LINES - 2) {
        game->settings.screen_height = LINES - 2;
      }
      set_game_fields(game);
      game->x_ball_orientation *= -1;

      send(sock, &game->settings, sizeof(game->settings), 0);
      handle_connection(game, sock);
      close(sock);
    } else if (gamemode == QUIT_PROGRAM) {
      break;
    } else if (gamemode == OFFLINE) {
      change_offline_settings(game);
      set_game_fields(game);
      offline_mode(game);
    } else {
      fprintf(stderr, "Gamemode not implemented yet\n");
      break;
    }

    set_game_fields(game);
    erase();
    refresh();
  }

  endwin();
  free(game);
  return EXIT_SUCCESS;
}

void offline_mode(struct Game *game) {
  while (true) {
    char ch1 = 0, ch2 = 0;

    clock_t start = time(NULL);

    while (difftime(time(NULL), start) <
               1.0 / game->settings.frames_per_second &&
           (!ch1 || !ch2)) {
      int ch = getch();
      if (!ch1 && !IS_KEY_DOWN(ch) && !IS_KEY_UP(ch))
        ch1 = ch;
      else if (!ch2)
        ch2 = ch;
    }

    flushinp();

    enum PlayerAction your_action, opponent_action;

    switch (ch1) {
    case 'w':
      your_action = PAD_UP;
      break;
    case 's':
      your_action = PAD_DOWN;
      break;
    case 'q':
      your_action = QUIT_GAME;
      break;
    default:
      your_action = NONE;
      break;
    }

    switch (ch2) {
    case 'k':
      opponent_action = PAD_UP;
      break;
    case 'j':
      opponent_action = PAD_DOWN;
      break;
    case 'q':
      opponent_action = QUIT_GAME;
      break;
    default:
      opponent_action = NONE;
      break;
    }

    if (your_action == QUIT_GAME || opponent_action == QUIT_GAME) {
      game->running = false;
      break;
    }

    render(game, your_action, opponent_action);
    napms(1000 / game->settings.frames_per_second);
  }
}
