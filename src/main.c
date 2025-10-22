#include <arpa/inet.h>
#include <ifaddrs.h>
#include <inttypes.h>
#include <ncurses.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "net.h"
#include "render.h"

struct Setting {
  const char *setting;
  void *settings_field;
};

int main(int argc, char **argv) {
  struct Game *game = init_game(argv, argc);
  if (!game) {
    perror("init_game");
    return EXIT_FAILURE;
  }

  while (true) {
    enum Gamemode gamemode = loading_screen();

    if (gamemode == GM_SERVE) {
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
      srand(game->settings.seed);
      net_game_handle(game, sock);
      close(sock);
    } else if (gamemode == GM_JOIN) {
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
      srand(game->settings.seed);
      net_game_handle(game, sock);
      close(sock);
    } else if (gamemode == GM_QUIT) {
      break;
    } else if (gamemode == GM_OFFLINE) {
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
