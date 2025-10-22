#ifndef RENDERER_H
#define RENDERER_H

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define PROGRAM_VERSION 0.0
#define DEFAULT_PAD_CHAR '|'
#define DEFAULT_BALL_CHAR '0'
#define DEFAULT_WINNING_SCORE 10
#define DEFAULT_BALL_SPEED 20
#define DEFAULT_FPS 60
#define DEFAULT_PAD_TILES 3
#define DEFAULT_PORT_NUM 6767
#define DEFAULT_BALL_X_SLOPE -15
#define DEFAULT_BALL_Y_SLOPE -7

#define IS_KEY_UP(ch) (ch == 'k' || ch == KEY_UP)
#define IS_KEY_LEFT(ch) (ch == 'h' || ch == KEY_LEFT)
#define IS_KEY_DOWN(ch) (ch == 'j' || ch == KEY_DOWN)
#define IS_KEY_RIGHT(ch) (ch == 'l' || ch == KEY_RIGHT)

#define EPSILON 1e-9
#define GAME_Y_OFFSET 5

#define MIN_SCREEN_HEIGHT 5
#define MIN_SCREEN_WIDTH 5
#define MAX_SCREEN_HEIGHT LINES - 6
#define MAX_SCREEN_WIDTH COLS - 2

#define SLOPE_Y_INCREASE_FACTOR 1.05
#define SLOPE_X_INCREASE_FACTOR 1.25

#define ASCII_LINE_HEIGHT 8

enum Gamemode {
  GM_SERVE,
  GM_JOIN,
  GM_OFFLINE,
  GM_BOT, // NOT IMPLEMENTED YET
  GM_QUIT,
};

enum PlayerAction {
  PA_NONE,
  PA_QUIT_GAME,
  PA_PAUSE_GAME,
  PA_PAD_DOWN,
  PA_PAD_UP,
};

enum PauseOptions {
  PO_HOME,
  PO_RESUME,
  PO_QUIT,
};

struct Settings {
  uint32_t seed;

  /*
   * screen_width and screen_height dictate the screen's dimensions.
   * screen_width can't be larger than COLS, or be smaller than 3.
   * screen_height can't be larger than ROWS, or be equal to 0.
   */
  uint16_t screen_width;
  uint16_t screen_height;

  uint16_t ball_size;

  int16_t base_ball_x_slope;
  int16_t base_ball_y_slope;

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
   * Chars/second
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
  //  uint8_t ball_speed;

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

  /*
   * ip_octets is an array of 4 unsigned 8-bit integers.
   *
   * ip_octets stores the IP address the client can connect to.
   */
  uint8_t ip_octets[4]; // Goes from most significant to least significant

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
  float x_ball_orientation;
  float y_ball_orientation;
  float ball_x;
  float ball_y;
  uint16_t ticks_y;
  uint16_t ticks_x;
  bool running;
};

struct Game *init_game(char **args, size_t argc);
void render(struct Game *game, enum PlayerAction your_action,
            enum PlayerAction opponent_action);
enum Gamemode loading_screen();
void change_serve_settings(struct Game *game);
void set_game_fields(struct Game *game);
void change_client_settings(struct Game *game);
void change_offline_settings(struct Game *game);
void end_game(struct Game *game, enum Gamemode mode);
const char *get_ascii_char(char ch);
void server_loading_screen(struct Game *game);
void print_ascii(uint16_t y, const char *text);
size_t get_ascii_char_len(char ch);
enum PauseOptions pause_screen();
enum PauseOptions pause_screen_net(int sock);
void offline_mode(struct Game *game);

#endif
