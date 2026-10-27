#ifndef TUI_H
#define TUI_H

#include "main.h"

// Board slot states. These share the encoding produced by match_word:
// 0 = gray, 1 = green, 2 = yellow.
#define SLOT_GRAY 0
#define SLOT_GREEN 1
#define SLOT_YELLOW 2

// Keyboard legend key states (ordered from least to most significant; a key
// is never downgraded once a stronger state has been seen).
#define KEY_UNKNOWN 0
#define KEY_GRAY 1
#define KEY_YELLOW 2
#define KEY_GREEN 3

// Clears the whole screen and homes the cursor. Call once before drawing a
// frame (banner, board, keyboard, status), then read input.
void tui_clear(void);

// Draws the title banner and the colour legend.
void tui_banner(void);

// Draws the 6x5 board. guesses/results hold n_guesses scored guesses; the
// remaining rows are drawn as empty slots.
void tui_board(char **guesses, int **results, const int n_guesses);

// Draws a QWERTY keyboard legend coloured by key_state[26].
void tui_keyboard(const int *key_state);

// Draws the attempt counter and the current status message, then flushes.
void tui_status(const int attempt, const char *message);

// Prints n spaces (used to line up custom output with the layout).
void tui_pad(int n);

// Prints one line of text, centred in the layout, in the given foreground
// colour (with a trailing newline).
void tui_center(const char *fg, const char *text);

// Folds a scored guess into the keyboard legend state (key_state[26]).
void tui_key_state_update(int *key_state, const char *guess, const int *result);

#endif // TUI_H
