#include "render.h"

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

void offline_mode(struct Game *game) {
  while (game->running) {
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
    case 'p':
      your_action = PAUSE_GAME;
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
    case 'p':
      your_action = PAUSE_GAME;
      break;
    default:
      opponent_action = NONE;
      break;
    }

    if (your_action == QUIT_GAME || opponent_action == QUIT_GAME) {
      game->running = false;
      break;
    } else if (your_action == PAUSE_GAME || opponent_action == PAUSE_GAME) {
      enum PauseOptions option = pause_screen();
      switch (option) {
      case PO_QUIT:
        game->running = false;
        exit(0);
      case PO_HOME:
        game->running = false;
        break;
      case PO_RESUME:
      default:
        break;
      }
    }

    if (game->running)
      render(game, your_action, opponent_action);
    napms(1000 / game->settings.frames_per_second);
  }
}
