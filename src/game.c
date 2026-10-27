#include "game.h"
#include "matcher.h"
#include "word_bank.h"
#include <stdio.h>
#include <string.h>

// Helper functions

/**
 * Searches a word bank for an exact match.
 *
 * Parameters:
 *   word      - The word to look up
 *   bank      - Array of NUL-terminated words to search
 *   bank_size - Number of entries in bank
 *
 * Returns:
 *   1 if word is present in bank, 0 otherwise.
 */
static int in_bank(const char *word, const char *const bank[],
                   const int bank_size) {
  for (int i = 0; i < bank_size; i++) {
    if (strcmp(word, bank[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

/**
 * Checks whether a guess is allowed. The official allowed-guesses bank
 * excludes the answer words, so the answers bank is checked as well: every
 * answer is itself a legal guess. Matching is case sensitive and expects a
 * lower-case word.
 *
 * Parameters:
 *   input - The guess to validate
 *
 * Returns:
 *   1 if the guess is in either word bank, 0 otherwise.
 */
int check_input(const char *input) {
  if (in_bank(input, OFFICIAL_ALLOWED_GUESSES, OFFICIAL_ALLOWED_GUESSES_SIZE) ||
      in_bank(input, REAL_WORDLES, REAL_WORDLES_SIZE)) {
    return 1; // Input is valid
  }
  return 0; // Input is invalid
}

/**
 * Validates a guess, scores it against the target, and records the guess
 * text and match codes in the caller's game state.
 *
 * guesses and results are caller-allocated arrays of max_guesses pointers:
 * guesses[guess_number] must own a buffer of strlen(target) + 1 chars, and
 * results[guess_number] must own an array of strlen(target) ints.
 *
 * Parameters:
 *   target       - The answer word to score against
 *   input        - The lower-case guess to validate and record
 *   guess_number - Attempt slot to write into
 *   max_guesses  - Number of slots in guesses and results
 *   guesses      - Storage for recorded guess strings
 *   results      - Storage for per-letter match codes (see match_word)
 *
 * Returns:
 *   0 on success; -1 if the slot index is out of range, the guess is not in
 *   the word banks, or its length differs from the target. On failure no
 *   caller memory is modified.
 */
int play_round(const char *target, const char *input, const int guess_number,
               const int max_guesses, char **guesses, int **results) {
  // Check if the input is valid
  int word_length = (int)strlen(target);

  // Validate the slot index before touching any caller-owned memory.
  if (guess_number < 0 || guess_number >= max_guesses) {
    fprintf(stderr, "Error: Maximum number of guesses (%d) exceeded.\n",
            max_guesses);
    return -1; // Exceeded maximum guesses
  } else if (!check_input(input)) {
    fprintf(stderr, "Error: Input '%s' is not a valid guess.\n", input);
    return -1; // Invalid input
  } else if ((int)strlen(input) != word_length) {
    // Guard the copy loop below: input must be exactly word_length chars.
    fprintf(stderr, "Error: Input length does not match target length.\n");
    return -1; // Inconsistent word length
  }

  // Match the input against the target word

  match_word(target, input, results[guess_number]);
  for (int i = 0; i < word_length; i++) {
    guesses[guess_number][i] = input[i];
  }
  guesses[guess_number][word_length] =
      '\0'; // Null-terminate the guess string. Note that this requires us to
            // allocate an additional character for the null terminator.
  return 0; // Successful round
}
