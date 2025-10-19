#include "render.h"

#include <ctype.h>
#include <ncurses.h>
#include <string.h>

size_t get_char_len(char ch) {
  switch (ch) {
  case '.':
  case ',':
  case ' ':
  case ':':
  case '!':
    return 3;
    break;
  case '/':
  case '1':
    return 7;
    break;
  case '4':
    return 10;
    break;
  default:
    return 9;
    break;
  }
}
size_t get_ascii_text_width(const char *str) {
  size_t sum = 0;
  for (const char *c = str; *c != '\0'; c++) {
    sum += get_char_len(*c);
  }
  return sum;
}

void print_ascii(uint16_t y, const char *text) {
  size_t i = 0, y_offset = 0;
  while (text[i]) {
    size_t line_len = 0;
    size_t start = i;

    // measure how many chars fit on this line
    while (text[i] && line_len + get_char_len(text[i]) < (size_t)COLS)
      line_len += get_char_len(text[i++]);

    size_t x_cursor = (COLS - line_len) / 2;

    // render that line
    for (size_t j = start; j < i; ++j) {
      const char *ascii = get_ascii_char(text[j]);
      size_t char_w = get_char_len(text[j]);
      size_t row = y_offset, col = 0;

      for (const char *c = ascii; *c; ++c) {
        if (*c == '\n') {
          row++;
          col = 0;
          continue;
        }
        mvaddch(y + row, x_cursor + col, *c);
        col++;
      }

      x_cursor += char_w;
    }

    y_offset += 8;
  }
}

const char *get_ascii_char(char ch) {
  static const char *ascii_A = " _______ \n"
                               "(  ___  )\n"
                               "| (   ) |\n"
                               "| (___) |\n"
                               "|  ___  |\n"
                               "| (   ) |\n"
                               "| )   ( |\n"
                               "|/     \\|\n";

  static const char *ascii_B = " ______  \n"
                               "(  ___ \\ \n"
                               "| (   ) )\n"
                               "| (__/ / \n"
                               "|  __ (  \n"
                               "| (  \\ \\ \n"
                               "| )___) )\n"
                               "|/ \\___/ \n";

  static const char *ascii_C = " _______ \n"
                               "(  ____ \\\n"
                               "| (    \\/\n"
                               "| |      \n"
                               "| |      \n"
                               "| |      \n"
                               "| (____/\\\n"
                               "(_______/\n";

  static const char *ascii_D = " ______  \n"
                               "(  __  \\ \n"
                               "| (  \\  )\n"
                               "| |   ) |\n"
                               "| |   | |\n"
                               "| |   ) |\n"
                               "| (__/  )\n"
                               "(______/ \n";

  static const char *ascii_E = " _______ \n"
                               "(  ____ \\\n"
                               "| (    \\/\n"
                               "| (__    \n"
                               "|  __)   \n"
                               "| (      \n"
                               "| (____/\\\n"
                               "(_______/\n";

  static const char *ascii_F = " _______ \n"
                               "(  ____ \\\n"
                               "| (    \\/\n"
                               "| (__    \n"
                               "|  __)   \n"
                               "| (      \n"
                               "| )      \n"
                               "|/       \n";

  static const char *ascii_G = " _______ \n"
                               "(  ____ \\\n"
                               "| (    \\/\n"
                               "| |      \n"
                               "| | ____ \n"
                               "| | \\_  )\n"
                               "| (___) |\n"
                               "(_______)\n";

  static const char *ascii_H = "         \n"
                               "|\\     /|\n"
                               "| )   ( |\n"
                               "| (___) |\n"
                               "|  ___  |\n"
                               "| (   ) |\n"
                               "| )   ( |\n"
                               "|/     \\|\n";

  static const char *ascii_I = "_________\n"
                               "\\__   __/\n"
                               "   ) (   \n"
                               "   | |   \n"
                               "   | |   \n"
                               "   | |   \n"
                               "___) (___\n"
                               "\\_______/\n";

  static const char *ascii_J = "_________\n"
                               "\\__    _/\n"
                               "   )  (  \n"
                               "   |  |  \n"
                               "   |  |  \n"
                               "   |  |  \n"
                               "|\\_)  )  \n"
                               "(____/   \n";

  static const char *ascii_K = " _       \n"
                               "| \\    /\\\n"
                               "|  \\  / /\n"
                               "|  (_/ / \n"
                               "|   _ (  \n"
                               "|  ( \\ \\ \n"
                               "|  /  \\ \\\n"
                               "|_/    \\/\n";

  static const char *ascii_L = " _       \n"
                               "( \\      \n"
                               "| (      \n"
                               "| |      \n"
                               "| |      \n"
                               "| |      \n"
                               "| (____/\\\n"
                               "(_______/\n";

  static const char *ascii_M = " _______ \n"
                               "(       )\n"
                               "| () () |\n"
                               "| || || |\n"
                               "| |(_)| |\n"
                               "| |   | |\n"
                               "| )   ( |\n"
                               "|/     \\|\n";

  static const char *ascii_N = " _       \n"
                               "( (    /|\n"
                               "|  \\  ( |\n"
                               "|   \\ | |\n"
                               "| (\\ \\) |\n"
                               "| | \\   |\n"
                               "| )  \\  |\n"
                               "|/    )_)\n";

  static const char *ascii_O = " _______ \n"
                               "(  ___  )\n"
                               "| (   ) |\n"
                               "| |   | |\n"
                               "| |   | |\n"
                               "| |   | |\n"
                               "| (___) |\n"
                               "(_______)\n";

  static const char *ascii_P = " _______ \n"
                               "(  ____ )\n"
                               "| (    )|\n"
                               "| (____)|\n"
                               "|  _____)\n"
                               "| (      \n"
                               "| )      \n"
                               "|/       \n";

  static const char *ascii_Q = " _______ \n"
                               "(  ___  )\n"
                               "| (   ) |\n"
                               "| |   | |\n"
                               "| |   | |\n"
                               "| | /\\| |\n"
                               "| (_\\ \\ |\n"
                               "(____\\/_)\n";

  static const char *ascii_R = " _______ \n"
                               "(  ____ )\n"
                               "| (    )|\n"
                               "| (____)|\n"
                               "|     __)\n"
                               "| (\\ (   \n"
                               "| ) \\ \\__\n"
                               "|/   \\__/\n";

  static const char *ascii_S = " _______ \n"
                               "(  ____ \\\n"
                               "| (    \\/\n"
                               "| (_____ \n"
                               "(_____  )\n"
                               "      ) |\n"
                               "/\\____) |\n"
                               "\\_______)\n";

  static const char *ascii_T = "_________\n"
                               "\\__   __/\n"
                               "   ) (   \n"
                               "   | |   \n"
                               "   | |   \n"
                               "   | |   \n"
                               "   | |   \n"
                               "   )_(   \n";

  static const char *ascii_U = "         \n"
                               "|\\     /|\n"
                               "| )   ( |\n"
                               "| |   | |\n"
                               "| |   | |\n"
                               "| |   | |\n"
                               "| (___) |\n"
                               "(_______)\n";

  static const char *ascii_V = "         \n"
                               "|\\     /|\n"
                               "| )   ( |\n"
                               "| |   | |\n"
                               "( (   ) )\n"
                               " \\ \\_/ / \n"
                               "  \\   /  \n"
                               "   \\_/   \n";

  static const char *ascii_W = "         \n"
                               "|\\     /|\n"
                               "| )   ( |\n"
                               "| | _ | |\n"
                               "| |( )| |\n"
                               "| || || |\n"
                               "| () () |\n"
                               "(_______)\n";

  static const char *ascii_X = "         \n"
                               "|\\     /|\n"
                               "( \\   / )\n"
                               " \\ (_) / \n"
                               "  ) _ (  \n"
                               " / ( ) \\ \n"
                               "( /   \\ )\n"
                               "|/     \\|\n";

  static const char *ascii_Y = "         \n"
                               "|\\     /|\n"
                               "( \\   / )\n"
                               " \\ (_) / \n"
                               "  \\   /  \n"
                               "   ) (   \n"
                               "   | |   \n"
                               "   \\_/   \n";

  static const char *ascii_Z = " _______ \n"
                               "/ ___   )\n"
                               "\\/   )  |\n"
                               "    /   )\n"
                               "   /   / \n"
                               "  /   /  \n"
                               " /   (_/\\\n"
                               "(_______/\n";

  static const char *ascii_0 = " _______ \n"
                               "(  __   )\n"
                               "| (  )  |\n"
                               "| | /   |\n"
                               "| (/ /) |\n"
                               "|   / | |\n"
                               "|  (__) |\n"
                               "(_______)\n";

  static const char *ascii_1 = "  __   \n"
                               " /  \\  \n"
                               " \\/) ) \n"
                               "   | | \n"
                               "   | | \n"
                               "   | | \n"
                               " __) (_\n"
                               " \\____/\n";

  static const char *ascii_2 = " _______ \n"
                               "/ ___   )\n"
                               "\\/   )  |\n"
                               "    /   )\n"
                               "  _/   / \n"
                               " /   _/  \n"
                               "(   (__/\\\n"
                               "\\_______/\n";

  static const char *ascii_3 = " ______  \n"
                               "/ ___  \\ \n"
                               "\\/   \\  \\\n"
                               "   ___) /\n"
                               "  (___ ( \n"
                               "      ) \\\n"
                               "/\\___/  /\n"
                               "\\______/ \n";

  static const char *ascii_4 = "    ___   \n"
                               "   /   )  \n"
                               "  / /) |  \n"
                               " / (_) (_ \n"
                               "(____   _)\n"
                               "     ) (  \n"
                               "     | |  \n"
                               "     (_)  \n";

  static const char *ascii_5 = " _______ \n"
                               "(  ____ \\\n"
                               "| (    \\/\n"
                               "| (____  \n"
                               "(_____ \\ \n"
                               "      ) )\n"
                               "/\\____) )\n"
                               "\\______/ \n";

  static const char *ascii_6 = "  ______ \n"
                               " / ____ \\\n"
                               "( (    \\/\n"
                               "| (____  \n"
                               "|  ___ \\ \n"
                               "| (   ) )\n"
                               "( (___) )\n"
                               " \\_____/ \n";

  static const char *ascii_7 = " ______  \n"
                               "/ ___  \\ \n"
                               "\\/   )  )\n"
                               "    /  / \n"
                               "   /  /  \n"
                               "  /  /   \n"
                               " /  /    \n"
                               " \\_/     \n";

  static const char *ascii_8 = "  _____  \n"
                               " / ___ \\ \n"
                               "( (___) )\n"
                               " \\     / \n"
                               " / ___ \\ \n"
                               "( (   ) )\n"
                               "( (___) )\n"
                               " \\_____/ \n";

  static const char *ascii_9 = "  _____  \n"
                               " / ___ \\ \n"
                               "( (   ) )\n"
                               "( (___) |\n"
                               " \\____  |\n"
                               "      ) |\n"
                               "/\\____) )\n"
                               "\\______/ \n";

  static const char *ascii_dot = "   \n"
                                 "   \n"
                                 "   \n"
                                 "   \n"
                                 "   \n"
                                 "   \n"
                                 " _ \n"
                                 "(_)\n";

  static const char *ascii_slash = "       \n"
                                   "     /\\\n"
                                   "    / /\n"
                                   "   / / \n"
                                   "  / /  \n"
                                   " / /   \n"
                                   "/ /    \n"
                                   "\\/     \n";

  static const char *ascii_comma = "   \n"
                                   "   \n"
                                   "   \n"
                                   "   \n"
                                   "   \n"
                                   " _ \n"
                                   "( )\n"
                                   "|/ \n";

  static const char *ascii_excl = " _ \n"
                                  "( )\n"
                                  "| |\n"
                                  "| |\n"
                                  "| |\n"
                                  "(_)\n"
                                  " _ \n"
                                  "(_)\n";

  static const char *ascii_unknown = "  _____  \n"
                                     " / ___ \\ \n"
                                     "( (   ) )\n"
                                     " \\/  / / \n"
                                     "    ( (  \n"
                                     "    | |  \n"
                                     "    (_)  \n"
                                     "     _   \n"
                                     "    (_)  \n";

  static const char *ascii_space = "  \n"
                                   "  \n"
                                   "  \n"
                                   "  \n"
                                   "  \n"
                                   "  \n"
                                   "  \n"
                                   "  \n"
                                   "  \n";

  static const char *ascii_colon = "   \n"
                                   "   \n"
                                   " _ \n"
                                   "(_)\n"
                                   "   \n"
                                   " _ \n"
                                   "(_)\n"
                                   "   \n"
                                   "   \n";

  ch = tolower(ch);

  switch (ch) {
  case 'a':
    return ascii_A;
  case 'b':
    return ascii_B;
  case 'c':
    return ascii_C;
  case 'd':
    return ascii_D;
  case 'e':
    return ascii_E;
  case 'f':
    return ascii_F;
  case 'g':
    return ascii_G;
  case 'h':
    return ascii_H;
  case 'i':
    return ascii_I;
  case 'j':
    return ascii_J;
  case 'k':
    return ascii_K;
  case 'l':
    return ascii_L;
  case 'm':
    return ascii_M;
  case 'n':
    return ascii_N;
  case 'o':
    return ascii_O;
  case 'p':
    return ascii_P;
  case 'q':
    return ascii_Q;
  case 'r':
    return ascii_R;
  case 's':
    return ascii_S;
  case 't':
    return ascii_T;
  case 'u':
    return ascii_U;
  case 'v':
    return ascii_V;
  case 'w':
    return ascii_W;
  case 'x':
    return ascii_X;
  case 'y':
    return ascii_Y;
  case 'z':
    return ascii_Z;
  case '0':
    return ascii_0;
  case '1':
    return ascii_1;
  case '2':
    return ascii_2;
  case '3':
    return ascii_3;
  case '4':
    return ascii_4;
  case '5':
    return ascii_5;
  case '6':
    return ascii_6;
  case '7':
    return ascii_7;
  case '8':
    return ascii_8;
  case '9':
    return ascii_9;
  case '.':
    return ascii_dot;
  case '/':
    return ascii_slash;
  case '!':
    return ascii_excl;
  case ',':
    return ascii_comma;
  case ' ':
    return ascii_space;
  case ':':
    return ascii_colon;
  default:
    return ascii_unknown;
  }
}
