#ifndef PONG_H
#define PONG_H

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

void start();

#endif
