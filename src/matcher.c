#include "matcher.h"
#include "word_bank.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void match_word(const char *word, const char *pattern, int *result) {
  // Modifies the result array in place (caller must allocate it
  // to be of size strlen(word) or strlen(pattern), which must be equal)
  // Check length (I know, scope creep n' all that. Sue me.)
  // Note that you must malloc explicitly for the return value.
  int len_word = strlen(word);
  int len_pattern = strlen(pattern);
  if (len_word != len_pattern) {
    // Print an error message to stderr and return NULL
    fprintf(stderr, "Error: Word and pattern must be of the same length.\n");
    return;
  }
  // Declare stack array for "match"
  int match[len_word]; // Initialize all elements to 0
  // Fill the arrays with 0s
  memset(match, 0, sizeof(match));

  // Implement the matching logic here
  // Naive implementation: check if the word matches the pattern character by
  // character
  for (int i = 0; pattern[i] != '\0'; i++) {
    if (word[i] == pattern[i]) {
      match[i] = 1;
    }
  }

  // Split non-matches into two arrays, for yellow and gray letters
  for (int i = 0; i < len_word; i++) {
    if (match[i] == 0) {
      // Check if the letter is in the word but not in the correct position
      int found = 0;
      for (int j = 0; j < len_word; j++) {
        if (word[j] == pattern[i] && i != j) {
          found = 1;
          break;
        }
      }
      if (found) {
        // Mark as yellow (2)
        match[i] = 2;
      } else {
        // Mark as gray (0)
        match[i] = 0;
      }
    }
    // Now modify result array to reflect the match status
    for (int i = 0; i < len_word; i++) {
      if (match[i] == 1) {
        result[i] = 1; // Green
      } else if (match[i] == 2) {
        result[i] = 2; // Yellow
      } else {
        result[i] = 0; // Gray
      }
    }
  }
}