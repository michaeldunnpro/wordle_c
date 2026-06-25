#include "matcher.h"
#include "word_bank.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper functions

void count_chars(const char *chars, int len_word, char *found_lets,
                 int *found_lets_count) {
  // Counts the occurrences of each character in the given string.
  // Helper function to be used in match_word.
  // The array found_lets is filled with the unique characters,
  // and found_lets_count is filled with the corresponding counts.
  for (int i = 0; i < len_word; i++) {
    char c = chars[i];
    int in_pattern = 0;

    for (int j = 0; j < len_word; j++) {
      if (found_lets[j] == c) {
        found_lets_count[j]++;
        in_pattern = 1;
        break;
      }
    }

    if (!in_pattern) {
      for (int j = 0; j < len_word; j++) {
        if (found_lets[j] == '\0') {
          found_lets[j] = c;
          found_lets_count[j] = 1;
          break;
        }
      }
    }
  }
}

void decrement_count(char c, int len_word, char *found_lets,
                     int *found_lets_count) {
  // Decrements the count of the character c in found_lets_count
  for (int i = 0; i < len_word; i++) {
    if (found_lets[i] == c) {
      found_lets_count[i]--;
      break;
    }
  }
}

// Find current count of a character in the found_lets_count array
int get_count(char c, int len_word, char *found_lets, int *found_lets_count) {
  for (int i = 0; i < len_word; i++) {
    if (found_lets[i] == c) {
      return found_lets_count[i];
    }
  }
  return 0; // Character not found
}

// Main function

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

  // Array of found letters
  char found_lets[len_word];
  int found_lets_count[len_word];
  // Safely initialize the arrays to 0
  memset(found_lets, '\0', sizeof(found_lets));
  memset(found_lets_count, 0, sizeof(found_lets_count));

  // Count available letters from the target word so duplicate letters in the
  // guess/pattern do not overcount as yellow.
  count_chars(word, len_word, found_lets, found_lets_count);

  // Pass 1: mark greens first and consume those letters from availability.
  for (int i = 0; i < len_word; i++) {
    if (word[i] == pattern[i]) {
      match[i] = 1;
      decrement_count(pattern[i], len_word, found_lets, found_lets_count);
    }
  }

  // Pass 2: for non-greens, mark yellow only if that letter is still available.
  for (int i = 0; i < len_word; i++) {
    if (match[i] != 0) {
      continue;
    }
    // Note because get_count returns 0 for unavailable letters, we can already
    // implicitly check for the letter's availability without need for an
    // additional check.
    if (get_count(pattern[i], len_word, found_lets, found_lets_count) > 0) {
      match[i] = 2;
      decrement_count(pattern[i], len_word, found_lets, found_lets_count);
    }
  }

  // Copy computed match values to the caller-provided result array.
  for (int i = 0; i < len_word; i++) {
    result[i] = match[i];
  }
}