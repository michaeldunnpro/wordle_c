#ifndef MATCHER_H
#define MATCHER_H

void count_chars(const char *chars, int len_target, char *found_lets,
                 int *found_lets_count);
void match_word(const char *target, const char *input, int *result);

#endif // MATCHER_H