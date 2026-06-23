#ifndef COLOR_PRINT_H
#define COLOR_PRINT_H

#include "main.h"

#define FG_RED "\x1b[31m"
#define FG_GREEN "\x1b[32m"
#define FG_YELLOW "\x1b[33m"
#define FG_BLUE "\x1b[34m"
#define FG_MAGENTA "\x1b[35m"
#define FG_CYAN "\x1b[36m"
#define FG_WHITE "\x1b[37m"
#define FG_BLACK "\x1b[30m"

#define BG_RED "\x1b[41m"
#define BG_GREEN "\x1b[42m"
#define BG_YELLOW "\x1b[43m"
#define BG_BLUE "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN "\x1b[46m"
#define BG_WHITE "\x1b[47m"
#define BG_BLACK "\x1b[40m"

#define COLOR_RESET "\x1b[0m"

#define STYLE_BOLD "\x1b[1m"
#define STYLE_UNDERLINE "\x1b[4m"

int printColor(const char *fg, const char *bg, const char *style,
               const char *string);
int printWordleGreen(const char *string);
int printWordleYellow(const char *string);
int printWordleBlack(const char *string);
int printWordleWord(const char *string, const char *correctness);

#endif // COLOR_PRINT_H