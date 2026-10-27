#include "main.h"
#include "color_print.h"
#include "game.h"
#include "tui.h"
#include "word_bank.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * Reads one line from stdin into buffer, which holds up to size bytes.
 *
 * Parameters:
 *   buffer - Destination for the line, stored without the newline
 *   size   - Size of buffer in bytes
 *
 * Returns:
 *   1 - a newline-terminated line was read
 *   2 - the line was longer than buffer; the rest of the line is drained so
 *       it cannot leak into the next read
 *   0 - end of input or read error (buffer is set to the empty string)
 */
static int read_line(char *buffer, size_t size) {
  buffer[0] = '\0';
  if (fgets(buffer, (int)size, stdin) == NULL) {
    return 0;
  }
  int terminated = (strchr(buffer, '\n') != NULL);
  if (!terminated) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
  }
  size_t len = strlen(buffer);
  while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
    buffer[--len] = '\0';
  }
  return terminated ? 1 : 2;
}

/**
 * Checks whether a string is non-empty and made only of alphabetic
 * characters. This also guarantees that echoing the input inside status
 * messages can never emit terminal escape codes.
 *
 * Parameters:
 *   s - The string to check
 *
 * Returns:
 *   1 if every character of s is a letter, 0 otherwise.
 */
static int is_all_letters(const char *s) {
  if (*s == '\0') {
    return 0;
  }
  for (size_t i = 0; s[i] != '\0'; i++) {
    if (!isalpha((unsigned char)s[i])) {
      return 0;
    }
  }
  return 1;
}

/**
 * Converts a word to lower case in place (the word banks are lower case).
 *
 * Parameters:
 *   s - The string to convert
 */
static void downcase(char *s) {
  for (size_t i = 0; s[i] != '\0'; i++) {
    s[i] = (char)tolower((unsigned char)s[i]);
  }
}

/**
 * Allocates one guess buffer and one result array per attempt slot. All
 * slots are initialised to NULL first so a failed allocation can still be
 * cleaned up safely by free_state.
 *
 * Parameters:
 *   guesses     - Array of attempt slots receiving guess buffers
 *   results     - Array of attempt slots receiving match-code arrays
 *   max_guesses - Number of slots to allocate
 *   word_length - Word length; each buffer holds word_length + 1 chars
 *
 * Returns:
 *   0 on success, -1 on allocation failure.
 */
static int init_state(char *guesses[], int *results[], int max_guesses,
                      int word_length) {
  for (int i = 0; i < max_guesses; i++) {
    guesses[i] = NULL;
    results[i] = NULL;
  }
  for (int i = 0; i < max_guesses; i++) {
    guesses[i] = calloc((size_t)word_length + 1, sizeof(char));
    results[i] = calloc((size_t)word_length, sizeof(int));
    if (guesses[i] == NULL || results[i] == NULL) {
      return -1;
    }
  }
  return 0;
}

/**
 * Frees every guess buffer and result array allocated by init_state. Slots
 * holding NULL are harmless to free.
 *
 * Parameters:
 *   guesses     - Array of guess buffers to free
 *   results     - Array of match-code arrays to free
 *   max_guesses - Number of slots to free
 */
static void free_state(char *guesses[], int *results[], int max_guesses) {
  for (int i = 0; i < max_guesses; i++) {
    free(guesses[i]);
    free(results[i]);
  }
}

/**
 * Plays one complete game against target, driving the TUI: full-screen
 * repaints, input validation, scoring through play_round, and win/loss
 * detection. The game state it allocates is freed before returning.
 *
 * Parameters:
 *   target       - The lower-case answer word
 *   attempts_out - Receives the number of guesses recorded before the game
 *                  ended
 *
 * Returns:
 *   1 on a win, 0 on a loss, -1 on quit or end of input.
 */
static int play_game(const char *target, int *attempts_out) {
  char *guesses[MAX_ATTEMPTS];
  int *results[MAX_ATTEMPTS];
  int key_state[26];
  char line[128];
  char echo_buf[96];
  memset(key_state, 0, sizeof(key_state));

  if (init_state(guesses, results, MAX_ATTEMPTS, WORD_LENGTH) != 0) {
    free_state(guesses, results, MAX_ATTEMPTS);
    fprintf(stderr, "Error: could not allocate game state.\n");
    exit(EXIT_FAILURE);
  }

  const char *message = "Type a 5-letter guess ('q' to quit).";
  int n_guesses = 0;
  for (;;) {
    // Draw the whole frame, then wait for input.
    tui_clear();
    tui_banner();
    tui_board(guesses, results, n_guesses);
    tui_keyboard(key_state);
    tui_status(n_guesses + 1, message);

    int rc = read_line(line, sizeof(line));
    if (rc == 0) {
      *attempts_out = n_guesses;
      free_state(guesses, results, MAX_ATTEMPTS);
      return -1; // End of input
    }
    if (!is_all_letters(line)) {
      if (rc == 2) {
        message = "That guess is too long.";
      } else if (line[0] == '\0') {
        message = "Type a 5-letter guess.";
      } else {
        message = "Letters only, please.";
      }
      continue;
    }
    downcase(line);
    if (strcmp(line, "q") == 0 || strcmp(line, "quit") == 0) {
      *attempts_out = n_guesses;
      free_state(guesses, results, MAX_ATTEMPTS);
      return -1; // Quit
    }
    if (strlen(line) != (size_t)WORD_LENGTH) {
      message = "Guesses must be exactly 5 letters.";
      continue;
    }

    // Record and score the round through the existing game logic.
    if (play_round(target, line, n_guesses, MAX_ATTEMPTS, guesses, results) !=
        0) {
      // Reached only with a 5-letter guess, hence the %.5s precision bound.
      snprintf(echo_buf, sizeof(echo_buf), "'%.5s' is not in the word list.",
               line);
      message = echo_buf;
      continue;
    }

    tui_key_state_update(key_state, guesses[n_guesses], results[n_guesses]);
    n_guesses++;

    int win = 1;
    for (int c = 0; c < WORD_LENGTH; c++) {
      if (results[n_guesses - 1][c] != SLOT_GREEN) {
        win = 0;
        break;
      }
    }
    if (win || n_guesses >= MAX_ATTEMPTS) {
      *attempts_out = n_guesses;
      free_state(guesses, results, MAX_ATTEMPTS);
      return win ? 1 : 0;
    }
  }
}

/**
 * Draws the end-of-game screen: the result message, the answer revealed as
 * green tiles, and a replay prompt.
 *
 * Parameters:
 *   target   - The answer word to reveal
 *   outcome  - Play outcome: 1 win, 0 loss, anything else quit or EOF
 *   attempts - Number of guesses used, shown on the win message
 *
 * Returns:
 *   1 to replay, 0 to exit.
 */
static int end_screen(const char *target, int outcome, int attempts) {
  char upper[WORD_LENGTH + 1];
  for (int i = 0; i < WORD_LENGTH; i++) {
    upper[i] = (char)toupper((unsigned char)target[i]);
  }
  upper[WORD_LENGTH] = '\0';

  tui_clear();
  tui_banner();
  printf("\n");
  if (outcome == 1) {
    char msg[48];
    snprintf(msg, sizeof(msg), "Solved in %d/%d guesses!", attempts,
             MAX_ATTEMPTS);
    tui_center(FG_WORDLE_GREEN, msg);
  } else if (outcome == 0) {
    tui_center(FG_WORDLE_YELLOW, "Out of guesses. The word was:");
  } else {
    tui_center(FG_WORDLE_YELLOW, "Thanks for playing!");
    tui_center(FG_WORDLE_GRAY, "The word was:");
  }
  printf("\n");
  tui_pad(11); // " X X X X X" is 15 wide; centres it in the 34-wide layout.
  printWordleWord(upper, "GGGGG");
  printf("\n");

  char line[128];
  tui_pad(6);
  printColor(FG_WORDLE_GRAY, "", "", "Press r + Enter to play again: ");
  fflush(stdout);
  if (read_line(line, sizeof(line)) == 0) {
    return 0;
  }
  return (line[0] == 'r' || line[0] == 'R');
}

/**
 * Program entry point. Plays games against fresh targets until the player
 * declines to replay.
 *
 * Parameters:
 *   argc - Argument count
 *   argv - Optional testing hook: argv[1] as a five-letter word pins the
 *          same target for every game
 *
 * Returns:
 *   EXIT_SUCCESS, or EXIT_FAILURE if the target override is malformed.
 */
int main(int argc, char *argv[]) {
  srand((unsigned int)time(NULL));

  // Optional testing hook: ./wordle_c slate forces a fixed target.
  char forced_target[WORD_LENGTH + 1];
  int force_target = 0;
  if (argc > 1) {
    if (strlen(argv[1]) != (size_t)WORD_LENGTH || !is_all_letters(argv[1])) {
      fprintf(stderr, "Usage: %s [5-letter target override]\n", argv[0]);
      return EXIT_FAILURE;
    }
    memcpy(forced_target, argv[1], WORD_LENGTH);
    forced_target[WORD_LENGTH] = '\0';
    downcase(forced_target);
    force_target = 1;
  }

  int replay = 1;
  while (replay) {
    // Pick a fresh target for every game; the testing hook pins the word.
    const char *target =
        force_target ? forced_target : REAL_WORDLES[rand() % REAL_WORDLES_SIZE];
    int attempts = 0;
    int outcome = play_game(target, &attempts);
    replay = end_screen(target, outcome, attempts);
  }
  tui_clear();
  printColor(FG_WORDLE_GRAY, "", "", "See you next time!\n");
  return 0;
}
