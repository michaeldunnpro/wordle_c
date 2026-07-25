#include "matcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper functions

/**
 * Counts occurrences of characters in a string and records unique letters.
 *
 * Parameters:
 *   chars - The source character array to analyze
 *   len_target - The number of characters to process from chars
 *   found_lets - Output array of unique characters discovered in chars
 *   found_lets_count - Output array of occurrence counts for found_lets
 */
void count_chars(const char *chars, const int len_target, char *found_lets,
                 int *found_lets_count) {
  // Counts the occurrences of each character in the given string.
  // Helper function to be used in match_word.
  // The array found_lets is filled with the unique characters,
  // and found_lets_count is filled with the corresponding counts.
  for (int i = 0; i < len_target; i++) {
    char c = chars[i];
    int in_input = 0;

    for (int j = 0; j < len_target; j++) {
      if (found_lets[j] == c) {
        found_lets_count[j]++;
        in_input = 1;
        break;
      }
    }

    if (!in_input) {
      for (int j = 0; j < len_target; j++) {
        if (found_lets[j] == '\0') {
          found_lets[j] = c;
          found_lets_count[j] = 1;
          break;
        }
      }
    }
  }
}

/**
 * Decrements the tracked count for a specific character.
 *
 * Parameters:
 *   c - Character to decrement
 *   len_target - Length of the tracking arrays
 *   found_lets - Array of tracked unique characters
 *   found_lets_count - Array of counts for each character in found_lets
 */
void decrement_count(const char c, const int len_target, const char *found_lets,
                     int *found_lets_count) {
  // Decrements the count of the character c in found_lets_count
  for (int i = 0; i < len_target; i++) {
    if (found_lets[i] == c) {
      found_lets_count[i]--;
      break;
    }
  }
}

/**
 * Gets the current tracked count for a specific character.
 *
 * Parameters:
 *   c - Character to query
 *   len_target - Length of the tracking arrays
 *   found_lets - Array of tracked unique characters
 *   found_lets_count - Array of counts for each character in found_lets
 *
 * Returns:
 *   The remaining count for c, or 0 if c is not present.
 */
int get_count(const char c, const int len_target, const char *found_lets,
              int *found_lets_count) {
  for (int i = 0; i < len_target; i++) {
    if (found_lets[i] == c) {
      return found_lets_count[i];
    }
  }
  return 0; // Character not found
}

// Main function

/**
 * Compares a target word against an input word and assigns Wordle-style match
 * states.
 *
 * Match values written to result:
 *   0 - Gray (letter not present)
 *   1 - Green (correct letter, correct position)
 *   2 - Yellow (correct letter, wrong position)
 *
 * Parameters:
 *   target - The reference word to match against
 *   input - The guessed/input word to evaluate
 *   result - Output array to receive per-character match values
 *
 * Notes:
 *   target and input must be the same length. If they are not, the function
 *   writes an error to stderr and returns without modifying result.
 */
void match_word(const char *target, const char *input, int *result) {
  // Modifies the result array in place (caller must allocate it
  // to be of size strlen(target) or strlen(input), which must be equal)
  // Check length (I know, scope creep n' all that. Sue me.)
  // Note that you must malloc explicitly for the return value.
  int len_target = strlen(target);
  int len_input = strlen(input);
  if (len_target != len_input) {
    // Print an error message to stderr and return NULL
    fprintf(stderr, "Error: target and input must be of the same length.\n");
    return;
  }
  // Declare stack array for "match"
  int match[len_target]; // Initialize all elements to 0
  // Fill the arrays with 0s
  memset(match, 0, sizeof(match));

  // Array of found letters
  char found_lets[len_target];
  int found_lets_count[len_target];
  // Safely initialize the arrays to 0
  memset(found_lets, '\0', sizeof(found_lets));
  memset(found_lets_count, 0, sizeof(found_lets_count));

  // Count available letters from the target target so duplicate letters in the
  // guess/input do not overcount as yellow.
  count_chars(target, len_target, found_lets, found_lets_count);

  // Pass 1: mark greens first and consume those letters from availability.
  for (int i = 0; i < len_target; i++) {
    if (target[i] == input[i]) {
      match[i] = 1;
      decrement_count(input[i], len_target, found_lets, found_lets_count);
    }
  }

  // Pass 2: for non-greens, mark yellow only if that letter is still available.
  for (int i = 0; i < len_target; i++) {
    if (match[i] != 0) {
      continue;
    }
    // Note because get_count returns 0 for unavailable letters, we can already
    // implicitly check for the letter's availability without need for an
    // additional check.
    if (get_count(input[i], len_target, found_lets, found_lets_count) > 0) {
      match[i] = 2;
      decrement_count(input[i], len_target, found_lets, found_lets_count);
    }
  }

  // Copy computed match values to the caller-provided result array.
  for (int i = 0; i < len_target; i++) {
    result[i] = match[i];
  }
}