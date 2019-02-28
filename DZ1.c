#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t check_correct(char *const *const input_lines, size_t n_lines, char **correct_lines);
size_t input_strings(char ***strings);
void free_input_strigs(char **input_str, size_t n_lines);

int main() {
    char **input_str;
    size_t n_lines = input_strings(&input_str);
    char *correct_str[n_lines];
    size_t n_out = check_correct(input_str, n_lines, correct_str);

    for (size_t i = 0; i < n_out; i++) {
        printf("%s\n", correct_str[i]);
    }
    free_input_strigs(input_str, n_lines);

    return 0;
}

size_t check_correct(char *const *const input_lines, size_t n_lines, char **correct_lines) {
    if (input_lines == NULL || correct_lines == NULL)
        return 0;
    size_t count = 0;
    for (size_t i = 0; i < n_lines; i++) {
        if (!input_lines[i])
            continue;
        int bkt_count = 0;
        for (size_t j = 0; j < strlen(input_lines[i]); j++) {
            if (input_lines[i][j] == '(') {
                bkt_count++;
            } else if (input_lines[i][j] == ')') {
                bkt_count--;
            }
            if (bkt_count < 0) {
                break;
            }
        }
        if (bkt_count == 0) {
            correct_lines[count] = input_lines[i];
            count++;
        }
    }
    return count;
}

size_t input_strings(char ***strings) {
    if (!strings)
        return 0;
    const size_t n_max = 1024, len_max = 1024;
    *strings = calloc(n_max, sizeof(char *));
    if (!(*strings))
        return 0;
    size_t n_lines = 0;
    for (size_t i = 0; i < n_max; i++) {
        char *str = calloc(len_max, sizeof(char));
        if (!str)
            return 0;
        fgets(str, len_max, stdin);
	if (strlen(str) > 0 && str[strlen(str) - 1] == '\n') {
            str[strlen(str) - 1] = '\0';
        }
        if (strlen(str) == 0) {
            free(str);
            break;
        }
        (*strings)[n_lines] = str;
        n_lines++;
    }
    return n_lines;
}

void free_input_strigs(char **input_str, size_t n_lines) {
    if (!input_str)
        return;
    for (size_t i = 0; i < n_lines; i++) {
        free(input_str[i]);
    }
    free(input_str);
}
