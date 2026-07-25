#include "game.h"
#include "matcher.h"
#include "word_bank.h"
#include <stdio.h>
#include <string.h>

// Helper functions

int check_input(const char *input) {
  // Check if the input is in the allowed guesses list
  for (int i = 0; i < OFFICIAL_ALLOWED_GUESSES_SIZE; i++) {
    if (strcmp(input, OFFICIAL_ALLOWED_GUESSES[i]) == 0) {
      return 1; // Input is valid
    }
  }
  return 0; // Input is invalid
}

int len_array(const char **array) {
  // Calculate the length of an arbitrary array of arrays
  int length = sizeof(array) / sizeof(array[0]);
  return length;
}

int play_round(const char *target, const char *input, const int guess_number,
               char **guesses, int **results) {
  // Check if the input is valid
  // Note that guesses and results must be caller allocated
  int word_length = strlen(target);
  int max_guesses = len_array(guesses);
  if (!check_input(input)) {
    fprintf(stderr, "Error: Input '%s' is not a valid guess.\n", input);
    return -1; // Invalid input
  } else if (guess_number >= max_guesses) {
    fprintf(stderr, "Error: Maximum number of guesses (%d) exceeded.\n",
            max_guesses);
    return -1; // Exceeded maximum guesses
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