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
  int plr_one_x;
  int plr_one_y;
  int plr_two_x;
  int plr_two_y;
  char player_score_one;
  char player_score_two;
  bool running;
};

#endif
