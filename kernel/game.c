#include "vga.h"
#include "lib.h"
#include "keyboard.h"
#include "game.h"

static char board[9];
static char current;

static void draw_board(void) {
    vga_clear();
    vga_write("Tic Tac Toe (press q to quit, r to restart)\n\n");
    for (int i=0;i<9;i++) {
        char c = board[i] ? board[i] : ('1'+i);
        vga_putc(' ');
        vga_putc(c);
        vga_putc(' ');
        if (i%3 != 2) vga_putc('|');
        else if (i != 8) vga_write("\n---+---+---\n");
    }
    vga_write("\n\nTurn: "); vga_putc(current);
    vga_write("\n");
}

static int win_lines[8][3] = {
    {0,1,2},{3,4,5},{6,7,8},
    {0,3,6},{1,4,7},{2,5,8},
    {0,4,8},{2,4,6}
};

static char check_winner(void) {
    for (int i=0;i<8;i++) {
        int a=win_lines[i][0], b=win_lines[i][1], c=win_lines[i][2];
        if (board[a] && board[a]==board[b] && board[b]==board[c]) return board[a];
    }
    for (int i=0;i<9;i++) if (!board[i]) return 0;
    return 'T'; // tie
}

static void reset(void) {
    for (int i=0;i<9;i++) board[i]=0; current='X';
}

void game_tictactoe(void) {
    reset();
    draw_board();
    while (1) {
        int ch = keyboard_read_char();
        if (ch == -1) continue;
        char c = (char)ch;
        if (c=='q' || c=='Q') break;
        if (c=='r' || c=='R') { reset(); draw_board(); continue; }
        if (c>='1' && c<='9') {
            int pos = c - '1';
            if (!board[pos]) {
                board[pos] = current;
                char w = check_winner();
                draw_board();
                if (w) {
                    if (w=='T') vga_write("\nTie game! Press r to restart or q to quit.\n");
                    else { vga_write("\nWinner: "); vga_putc(w); vga_write(". Press r to restart or q to quit.\n"); }
                    // wait for r or q
                    while (1) {
                        int ch2 = keyboard_read_char();
                        if (ch2 == -1) continue;
                        char c2=(char)ch2;
                        if (c2=='q'||c2=='Q') return;
                        if (c2=='r'||c2=='R') { reset(); draw_board(); break; }
                    }
                } else {
                    current = (current=='X') ? 'O' : 'X';
                    draw_board();
                }
            }
        }
    }
}
