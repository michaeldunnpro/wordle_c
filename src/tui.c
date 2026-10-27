#include "tui.h"
#include "color_print.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// Nominal width used to centre each element of the layout.
#define LAYOUT_WIDTH 34

/**
 * Writes n space characters to stdout.
 *
 * Parameters:
 *   n - Number of spaces to write (nothing is written if n <= 0)
 */
void tui_pad(int n) {
  for (int i = 0; i < n; i++) {
    fputc(' ', stdout);
  }
}

/**
 * Draws a single tile: a 3-character block containing an upper-case letter
 * (or a blank when letter is NUL) between two spaces.
 *
 * Parameters:
 *   letter - Character to draw, or '\0' for an empty tile
 *   fg     - Foreground colour escape code
 *   bg     - Background colour escape code
 */
static void print_tile(int letter, const char *fg, const char *bg) {
  // Built on a stack buffer with an explicit terminator, so no buffer can
  // overrun.
  char cell[4];
  cell[0] = ' ';
  cell[1] = (letter != '\0') ? (char)toupper(letter) : ' ';
  cell[2] = ' ';
  cell[3] = '\0';
  printColor(fg, bg, STYLE_BOLD, cell);
}

/**
 * Clears the whole screen and homes the cursor.
 */
void tui_clear(void) { printf("\x1b[2J\x1b[H"); }

/**
 * Draws the title block: the coloured WORDLE wordmark, the tagline, and the
 * green/yellow/gray tile legend, centred within the layout.
 */
void tui_banner(void) {
  static const char title[] = "WORDLE";
  static const char *title_fg[] = {FG_WORDLE_GREEN,  FG_WORDLE_YELLOW,
                                   FG_WORDLE_GRAY,   FG_WORDLE_GREEN,
                                   FG_WORDLE_YELLOW, FG_WORDLE_GRAY};

  tui_pad((LAYOUT_WIDTH - 11) / 2); // "W O R D L E" is 11 characters wide.
  for (int i = 0; title[i] != '\0'; i++) {
    char ch[2] = {title[i], '\0'};
    printColor(title_fg[i], "", STYLE_BOLD, ch);
    if (title[i + 1] != '\0') {
      fputc(' ', stdout);
    }
  }
  printf("\n");

  tui_pad((LAYOUT_WIDTH - 24) / 2);
  printColor(FG_WORDLE_GRAY, "", "", "Guess the word in six tries.");
  printf("\n\n");

  tui_pad(6);
  print_tile('G', FG_WHITE, BG_WORDLE_GREEN);
  printColor(FG_WORDLE_GRAY, "", "", "  right letter, right spot\n");
  tui_pad(6);
  print_tile('Y', FG_WHITE, BG_WORDLE_YELLOW);
  printColor(FG_WORDLE_GRAY, "", "", "  right letter, wrong spot\n");
  tui_pad(6);
  print_tile('A', FG_WHITE, BG_WORDLE_GRAY);
  printColor(FG_WORDLE_GRAY, "", "", "  wrong letter\n");
}

/**
 * Draws the attempt grid: recorded guesses up to n_guesses are shown with
 * each tile coloured from its match code, and the remaining rows are drawn
 * as empty slots.
 *
 * Parameters:
 *   guesses   - Guess strings, one per completed attempt
 *   results   - Match codes per attempt (see match_word)
 *   n_guesses - Number of completed attempts to display
 */
void tui_board(char **guesses, int **results, const int n_guesses) {
  printf("\n");
  for (int r = 0; r < MAX_ATTEMPTS; r++) {
    tui_pad((LAYOUT_WIDTH - (3 + 3 * WORD_LENGTH)) / 2);
    char label[4];
    snprintf(label, sizeof(label), "%d ", r + 1);
    printColor(FG_WORDLE_GRAY, "", "", label);
    for (int c = 0; c < WORD_LENGTH; c++) {
      if (r < n_guesses) {
        const char *bg;
        switch (results[r][c]) {
        case SLOT_GREEN:
          bg = BG_WORDLE_GREEN;
          break;
        case SLOT_YELLOW:
          bg = BG_WORDLE_YELLOW;
          break;
        default:
          bg = BG_WORDLE_GRAY;
          break;
        }
        print_tile(guesses[r][c], FG_WHITE, bg);
      } else {
        print_tile('\0', FG_WORDLE_GRAY, BG_WORDLE_EMPTY);
      }
    }
    printf("\n");
  }
}

/**
 * Draws the QWERTY keyboard legend, shading every key according to its
 * current state (see tui_key_state_update).
 *
 * Parameters:
 *   key_state - Array of 26 key states indexed by letter, starting at 'a'
 */
void tui_keyboard(const int *key_state) {
  static const char *rows[] = {"QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};
  static const int row_widths[] = {10, 9, 7}; // Tiles are 3 columns wide.
  printf("\n");
  for (int r = 0; r < 3; r++) {
    tui_pad((LAYOUT_WIDTH - 3 * row_widths[r]) / 2);
    for (const char *k = rows[r]; *k != '\0'; k++) {
      int state = key_state[tolower((unsigned char)*k) - 'a'];
      const char *fg = FG_WHITE;
      const char *bg = "";
      switch (state) {
      case KEY_GREEN:
        bg = BG_WORDLE_GREEN;
        break;
      case KEY_YELLOW:
        bg = BG_WORDLE_YELLOW;
        break;
      case KEY_GRAY:
        fg = FG_WORDLE_GRAY;
        bg = BG_WORDLE_EMPTY;
        break;
      default:
        break;
      }
      print_tile(*k, fg, bg);
    }
    printf("\n");
  }
}

/**
 * Draws the status line: the current attempt counter and a feedback
 * message.
 *
 * Parameters:
 *   attempt - Attempt number to display (1-based)
 *   message - Feedback text shown next to the counter
 */
void tui_status(const int attempt, const char *message) {
  printf("\n");
  tui_pad(6);
  char counter[32];
  snprintf(counter, sizeof(counter), "Guess %d/%d", attempt, MAX_ATTEMPTS);
  printColor(FG_WORDLE_GRAY, "", "", counter);
  printf("  ");
  printColor(FG_WORDLE_YELLOW, "", STYLE_BOLD, message);
  printf("\n");
  fflush(stdout);
}

/**
 * Prints a line of text centred within the layout width.
 *
 * Parameters:
 *   fg   - Foreground colour escape code
 *   text - Text to centre and print
 */
void tui_center(const char *fg, const char *text) {
  int width = (int)strlen(text);
  tui_pad((LAYOUT_WIDTH - width) / 2);
  printColor(fg, "", STYLE_BOLD, text);
  printf("\n");
}

/**
 * Updates the keyboard legend states from a scored guess, mapping
 * match_word codes onto KEY_GRAY/KEY_GREEN/KEY_YELLOW. A key never loses a
 * stronger state it already holds (green > yellow > gray).
 *
 * Parameters:
 *   key_state - Array of 26 key states indexed by letter, starting at 'a'
 *   guess     - The guess that was scored
 *   result    - Match codes for that guess (see match_word)
 */
void tui_key_state_update(int *key_state, const char *guess,
                          const int *result) {
  static const int to_key[] = {KEY_GRAY, KEY_GREEN, KEY_YELLOW};
  for (int i = 0; guess[i] != '\0'; i++) {
    if (result[i] < SLOT_GRAY || result[i] > SLOT_YELLOW) {
      continue; // Ignore out-of-range codes defensively.
    }
    int idx = tolower((unsigned char)guess[i]) - 'a';
    if (idx < 0 || idx >= 26) {
      continue; // Not a legend key.
    }
    if (to_key[result[i]] > key_state[idx]) {
      key_state[idx] = to_key[result[i]];
    }
  }
}
