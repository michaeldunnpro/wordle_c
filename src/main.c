#include <stdio.h>
#include "main.h"

int main()
{
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
    for (int i = 0; i < 5; i++)
    {
        random_index = rand() % REAL_WORDLES_SIZE;
        printf("%s\n", REAL_WORDLES[random_index]);
    }
    random_index = rand() % OFFICIAL_ALLOWED_GUESSES_SIZE;
    printf("Random OFFICIAL_ALLOWED_GUESSES word:\n");
    for (int i = 0; i < 5; i++)
    {
        random_index = rand() % OFFICIAL_ALLOWED_GUESSES_SIZE;
        printf("%s\n", OFFICIAL_ALLOWED_GUESSES[random_index]);
    }

    return 0;
}