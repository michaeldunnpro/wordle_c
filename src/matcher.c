#include "matcher.h"
#include "word_bank.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int* match_word(const char *word, const char *pattern, int *result) {
    // Heavily WIP
    // Modifies the result array in place (caller must allocate it)
    // Check length (I know, scope creep n' all that. Sue me.)
    // Note that you must malloc explicitly for the return value.
    int len_word = strlen(word);
    int len_pattern = strlen(pattern);
    if (len_word != len_pattern) {
        // Print an error message to stderr and return NULL
        fprintf(stderr, "Error: Word and pattern must be of the same length.\n");
        return NULL;
    }
    //



    // Implement the matching logic here
    // Naive implementation: check if the word matches the pattern character by character
    for (int i = 0; pattern[i] != '\0'; i++) {
    }
}