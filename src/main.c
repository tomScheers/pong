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

#define MIN_WINDOW_HEIGHT 25
#define MIN_WINDOW_WIDTH 25

struct Setting {
  const char *setting;
  void *settings_field;
};

int main() {
  struct Game *game = init_game();
  if (!game) {
    perror("init_game");
    return EXIT_FAILURE;
  }

  if (COLS < MIN_WINDOW_WIDTH || LINES < MIN_WINDOW_HEIGHT) {
    endwin();
    free(game);
    return EXIT_FAILURE;
  }

  while (true) {
    enum Gamemode gamemode = loading_screen();

    if (gamemode == GM_SERVE) {
      enum EndScreenOption e_option = ES_RESTART;

      while (e_option == ES_RESTART) {
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

        napms(10); // Wait a little before closing the connection so that the
                   // other side has time to handle everything
        close(sock);
        e_option = end_screen();
      }

      if (e_option == ES_QUIT)
        break;
    } else if (gamemode == GM_JOIN) {
      enum EndScreenOption e_option = ES_RESTART;

      while (e_option == ES_RESTART) {
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
        game->settings.base_ball_x_slope *= -1;
        game->settings.base_ball_y_slope *= -1;

        send(sock, &game->settings, sizeof(game->settings), 0);
        srand(game->settings.seed);

        // if (game->settings.screen_width % 2 == 0 &&
        //     game->settings.ball_size % 2 == 0)
        //   --game->ball_x;

        net_game_handle(game, sock);
        napms(10); // Wait a little before closing the connection so that the
                   // other side has time to handle everything
        close(sock);
        e_option = end_screen();
      }
      if (e_option == ES_QUIT)
        break;
    } else if (gamemode == GM_QUIT) {
      break;
    } else if (gamemode == GM_OFFLINE) {
      enum EndScreenOption e_option = ES_RESTART;
      while (e_option == ES_RESTART) {
        change_offline_settings(game);
        set_game_fields(game);
        offline_mode(game);
        e_option = end_screen();
      }

      if (e_option == ES_QUIT)
        break;
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
