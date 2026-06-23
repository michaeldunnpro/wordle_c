#include <stdio.h>
#include "color_print.h"

/**
 * Prints a string with the specified foreground color, background color, and style.
 * After printing, the color and style are reset to default.
 * An empty string can be passed to ignore an attribute.
 *
 * Parameters:
 *   fg    - Foreground color code (e.g., FG_RED)
 *   bg    - Background color code (e.g., BG_GREEN)
 *   style - Text style code (e.g., COLOR_BOLD)
 *   string - The string to be printed
 *
 * Returns:
 *   The number of characters printed.
 */
int printColor(const char *fg, const char *bg, const char *style, const char *string)
{
    int result = printf("%s%s%s%s%s", fg, bg, style, string, COLOR_RESET);
    return result;
}

/**
 * Prints Wordle character of correct letter and location
 */
int printWordleGreen(const char *string)
{
    return printColor(FG_WHITE, BG_GREEN, STYLE_BOLD, string);
}

/**
 * Prints Wordle character of correct letter but incorrect location
 */
int printWordleYellow(const char *string)
{
    return printColor(FG_WHITE, BG_YELLOW, STYLE_BOLD, string);
}

/**
 * Prints Wordle character of incorrect letter
 */
int printWordleBlack(const char *string)
{
    return printColor(FG_WHITE, BG_BLACK, STYLE_BOLD, string);
}

/**
 * Prints a Wordle word with each character colored according to the correctness string.
 *
 * Parameters:
 *   string - The Wordle word to be printed
 *   correctness - A string indicating the correctness of each character ('G' for green, 'Y' for yellow, 'B' for black)
 *
 * Returns:
 *   The number of characters printed.
 */
int printWordleWord(const char *string, const char *correctness)
{
    int result = 0;
    for (int i = 0; string[i] != '\0' && correctness[i] != '\0'; i++)
    {
        const char *charString = (char[]){' ', string[i], ' ', '\0'};
        switch (correctness[i])
        {
        case WORDLE_GREEN:
            result += printWordleGreen(charString);
            break;
        case WORDLE_YELLOW:
            result += printWordleYellow(charString);
            break;
        case WORDLE_BLACK:
            result += printWordleBlack(charString);
            break;
        default:
            result += printf("%c", string[i]);
            break;
        }

        printf(" ");
    }
    printf("\n");
    return result;
}
