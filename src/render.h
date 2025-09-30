#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include <stdint.h>

#define PAD_CHAR '|'

enum PlayerAction {
  PAD_DOWN,
  PAD_UP,
  NONE,
  QUIT,
};

struct Player {
  uint16_t x;
  uint16_t y;
  uint8_t score;
};

struct Game {
  int8_t x_ball_orientation;
  int8_t y_ball_orientation;
  int16_t ball_x;
  int16_t ball_y;
  int16_t game_width;
  int16_t game_height;
  struct Player plr_one;
  struct Player plr_two;
  bool running;
};

void render(struct Game *game, enum PlayerAction your_action,
            enum PlayerAction opponent_action);

#endif
