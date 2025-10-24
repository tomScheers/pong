#include "render.h"

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include <time.h>
#include <unistd.h>

void offline_mode(struct Game *game) {
  while (game->running) {
    clock_t start = time(NULL);
    char ch1 = 0, ch2 = 0;

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
      your_action = PA_PAD_UP;
      break;
    case 's':
      your_action = PA_PAD_DOWN;
      break;
    case 'q':
      your_action = PA_QUIT_GAME;
      break;
    case 'p':
      your_action = PA_PAUSE_GAME;
      break;
    default:
      your_action = PA_NONE;
      break;
    }

    switch (ch2) {
    case 'k':
      opponent_action = PA_PAD_UP;
      break;
    case 'j':
      opponent_action = PA_PAD_DOWN;
      break;
    case 'q':
      opponent_action = PA_QUIT_GAME;
      break;
    case 'p':
      your_action = PA_PAUSE_GAME;
      break;
    default:
      opponent_action = PA_NONE;
      break;
    }

    if (your_action == PA_QUIT_GAME || opponent_action == PA_QUIT_GAME) {
      game->running = false;
      break;
    } else if (your_action == PA_PAUSE_GAME ||
               opponent_action == PA_PAUSE_GAME) {
      enum PauseOptions option = pause_screen();
      switch (option) {
      case PO_QUIT:
        game->running = false;
        endwin();
        free(game);
        exit(0);
      case PO_HOME:
        game->running = false;
        break;
      case PO_RESUME:
      default:
        break;
      }
    }

    if (game->running) {
      render(game, your_action, opponent_action);
      napms(1000 / game->settings.frames_per_second);
    }
  }
}
