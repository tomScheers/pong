#include <ncurses.h>
#include "render.h"

void end_game(struct Game *game, enum Gamemode mode) {
    erase();
    game->running = false;

    if (mode == SERVE || mode == JOIN || mode == BOT){
        if (game->plr_one.score == game->settings.winning_score){
            mvprintw(LINES / 2, COLS / 2, "YOU WON!");
        } else {
            mvprintw(LINES / 2, COLS / 2, "YOU LOST :(");
            mvprintw(LINES / 2 + 4, COLS / 2, "Press any key to continue...");
            getch();
        }
    } else if (mode == OFFLINE) { 
        if (game->plr_one.score == game->settings.winning_score){
            mvprintw(LINES / 2, COLS / 2, "Player 1 Won!");  
        }
    }    
}

 
    





                            