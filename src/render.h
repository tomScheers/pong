#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define PROGRAM_VERSION 0.0
#define DEFAULT_PAD_CHAR '|'
#define DEFAULT_BALL_CHAR '0'
#define DEFAULT_WINNING_SCORE 0
#define DEFAULT_BALL_SPEED 100
#define DEFAULT_FPS 10
#define DEFAULT_PAD_TILES 3
#define DEFAULT_PORT_NUM 6767

enum PlayerAction {
  NONE,
  QUIT,
  PAD_DOWN,
  PAD_UP,
};

struct Settings {
  /*
   * screen_width and screen_height dictate the screen's dimensions.
   * screen_width can't be larger than COLS, or be smaller than 3.
   * screen_height can't be larger than ROWS, or be equal to 0.
   */
  uint16_t screen_width;
  uint16_t screen_height;

  /*
   * port represents the port number the current program is using
   *
   * This is only really useful for diagnostics and for the user to choose what
   * port they want to serve/connect a game to
   */
  uint16_t port;

  /*
   * winning_score dictates what score either player has to reach for them to
   * win.
   *
   * if winning_score is set to 0, the program will run until either player
   * breaks the connection.
   */
  uint16_t winning_score;

  /*
   * ball_speed dictates the speed at which the ball moves throught the screen.
   * It defaults to 100.
   * ball_speed scales fractionaly with time, so 110 would update the
   *
   * ball each 100/110 seconds, et cetera:
   * formula: 100 / x.
   *
   * If bal_speed is 0 the ball doesn't move at all.
   *
   * The ball_speed is capped to the FPS of the program, which naturally is
   * capped towards the network speed.
   */
  uint16_t ball_speed;

  /*
   * frames_per_second is the FPS the program will target for.
   *
   * ball_speed is capped at the frames per second, you can't move a ball twice
   * a frame for example
   *
   * frames_per_second is itself capped to the network speed, because the
   * program can't proceed if it doesn't have the movement of the opponent.
   */
  uint8_t frames_per_second;

  /*
   * Update version to make sure logic is identical on both machines.
   */
  uint8_t program_version;

  /*
   * pad_tiles dictate the number of tiles the pad will get on its vertical
   * axis.
   *
   * pad_tiles can't be larger than or equal to screen_height, or be equal to 0.
   */
  uint8_t pad_tiles;

  char pad_char;
  char ball_char;
};

struct Player {
  uint16_t x;
  uint16_t y;
  uint16_t score;
};

struct Game {
  struct Player plr_one;
  struct Player plr_two;
  struct Settings settings;
  uint16_t ball_x;
  uint16_t ball_y;
  uint8_t x_ball_orientation;
  uint8_t y_ball_orientation;
  bool running;
};

struct Game *init_game(char **args, size_t argc);
void render(struct Game *game, enum PlayerAction your_action,
            enum PlayerAction opponent_action);

#endif
