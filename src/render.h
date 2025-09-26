#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

enum PlayerAction {
  PAD_DOWN,
  PAD_UP,
  NONE,
  QUIT,
};

struct Game {
  int x_ball_orientation;
  int y_ball_orientation;
  int ball_x;
  int ball_y;
  int game_width;
  int game_height;
  bool running;
};

void loop(struct Game *game, enum PlayerAction your_action,
          enum PlayerAction opponend_action);
struct Game *init_game();

#endif
