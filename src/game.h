#ifndef GAME_H
#define GAME_H

/**
 * Checks whether a guess is an official Wordle-allowed guess.
 *
 * Parameters:
 *   input - Null-terminated guess (lowercase, to match the word bank)
 *
 * Returns:
 *   1 if the guess is in the allowed-guesses bank, 0 otherwise.
 */
int check_input(const char *input);

/**
 * Plays a single round: validates the guess, scores it against the target,
 * and records it in the caller-owned game state.
 *
 * Parameters:
 *   target       - The secret word (lowercase, null-terminated)
 *   input        - The guess to record
 *   guess_number - Zero-based index of the current guess
 *   max_guesses  - Number of slots available in guesses/results
 *   guesses      - Caller-allocated array of max_guesses char buffers, each
 *                  large enough for strlen(target) + 1 characters
 *   results      - Caller-allocated array of max_guesses int arrays, each of
 *                  strlen(target) integers
 *
 * Returns:
 *   0 on a successful round, -1 on exceeded guesses or invalid input.
 */
int play_round(const char *target, const char *input, const int guess_number,
               const int max_guesses, char **guesses, int **results);

#endif // GAME_H
