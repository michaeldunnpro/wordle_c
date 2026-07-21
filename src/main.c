#include "main.h"
#include "color_print.h"
#include "matcher.h"
#include "word_bank.h"
#include <stdio.h>

int main() {
  // Template for a main function, with a print statement.
  printf("Hello, World!\n");

  // Test color printing
  printWordleWord("SLATE", "BBBYY");
  printWordleWord("THEIR", "YBYBY");
  printWordleWord("ERUPT", "GYBBY");
  printWordleWord("ENTRY", "GGGGG");

  // Test word bank
  printf("REAL_WORDLES_SIZE: %d\n", REAL_WORDLES_SIZE);
  printf("OFFICIAL_ALLOWED_GUESSES_SIZE: %d\n", OFFICIAL_ALLOWED_GUESSES_SIZE);
  int random_index = rand() % REAL_WORDLES_SIZE;
  printf("Random REAL_WORDLES words:\n");
  for (int i = 0; i < 5; i++) {
    random_index = rand() % REAL_WORDLES_SIZE;
    printf("%s\n", REAL_WORDLES[random_index]);
  }
  random_index = rand() % OFFICIAL_ALLOWED_GUESSES_SIZE;
  printf("Random OFFICIAL_ALLOWED_GUESSES word:\n");
  for (int i = 0; i < 5; i++) {
    random_index = rand() % OFFICIAL_ALLOWED_GUESSES_SIZE;
    printf("%s\n", OFFICIAL_ALLOWED_GUESSES[random_index]);
  }

  // Test matcher
  const char *word = "SLATE";
  const char *input = "LATTE"; 
  int result[5] = {0};               // Initialize result array to 0
  match_word(word, input, result); // Call the match_word function
  // Print the array to verify the result
  printf("Match result for target '%s' and input '%s':\n", word, input);
  for (int i = 0; i < 5; i++) {
    printf("%d ", result[i]);
  }
  printf("\n");

  return 0;
}