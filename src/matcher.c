#include "matcher.h"
#include "word_bank.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int match_word(const char *word, const char *pattern) {
    // Implement the matching logic here
    // For example, you can use a simple string comparison or a more complex pattern matching algorithm
    return strcmp(word, pattern) == 0; //
}