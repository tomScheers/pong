#ifndef MAIN
#define MAIN

struct Game {

  int x_ball_orientation;
  int y_ball_orientation;
  int ball_x;
  int ball_y;

  int game_width;
  int game_height;
  int running;
};

void start();

#endif