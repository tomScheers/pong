#include <ncurses.h>
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

    int sock = -1;

    if (gamemode == SERVE) {
      change_serve_settings(game);
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
    } else if (gamemode == JOIN) {
      sock = net_client_init_sock(game->settings.port);
      if (sock == -1) {
        perror("net_client_init_sock");
        break;
      }
      recv(sock, &game->settings, sizeof(game->settings), 0);
      if (game->settings.screen_width > COLS) {
        game->settings.screen_width = COLS;
      }
      if (game->settings.screen_height > LINES) {
        game->settings.screen_height = LINES;
      }
      send(sock, &game->settings, sizeof(game->settings), 0);
    } else if (gamemode == QUIT_PROGRAM) {
      break;
    } else {
      fprintf(stderr, "Gamemode not implemented yet\n");
      break;
    }

    handle_connection(game, sock);
    close(sock);
    set_game_fields(game);
    clear();
    refresh();
  }

  endwin();
  free(game);
  return EXIT_SUCCESS;
}
