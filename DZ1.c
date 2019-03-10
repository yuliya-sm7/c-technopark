// Смирнова Юлия АПО-12 весна 2019
// Submit a solution for A- (Variant 7)
//Задача A-7. Задача о скобочной последовательности
//Time limit:	14 s
//Memory limit:	64 M
//Составить программу построчной обработки текста. Суть обработки - отбор строк, содержащих одинаковое количество открывающих и закрывающих круглых скобок.
//
//Программа считывает входные данные со стандартного ввода, и печатает результат в стандартный вывод.
//
//Процедура отбора нужных строк должна быть оформлена в виде отдельной функции, которой на вход подается массив строк (который необходимо обработать), количество переданных строк, а также указатель на переменную, в которой необходимо разместить результат - массив отобранных строк.
//В качестве возвращаемого значения функция должна возвращать количество строк, содержащихся в результирующем массиве.
//
//Программа должна уметь обрабатывать ошибки - такие как неверные входные данные(отсутствие входных строк) или ошибки выделения памяти и т.п.
//В случае возникновения ошибки нужно выводить об этом сообщение "[error]" и завершать выполнение программы.
//
//ВАЖНО! Программа в любом случае должна возвращать 0. Не пишите return -1, exit(1) и т.п. Даже если обнаружилась какая-то ошибка, все равно необходимо вернуть 0! (и напечатать [error] в stdout).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int LEN_BUFF = 64;
const int MUL_REBUFF = 2;

char **input_strings(size_t *n_lines);

char *get_string();

int rebuff(char *buff[], size_t *n_buff);

char **check_correct_bracket(char *const *const input_lines, size_t n_lines, size_t *n_out);

void free_input_strings(char **input_str, size_t n_lines);

int main() {
    size_t n_lines = 0;
    char **input_str = input_strings(&n_lines);
    if (input_str == NULL) {
        printf("[error]");
        return 0;
    }

    size_t n_out = 0;
    char **correct_str = check_correct_bracket(input_str, n_lines, &n_out);
    if (correct_str == NULL) {
        free_input_strings(input_str, n_lines);
        printf("[error]");
        return 0;
    }

    for (size_t i = 0; i < n_out; i++) {
        printf("%s\n", correct_str[i]);
    }

    free_input_strings(input_str, n_lines);
    free(correct_str);
    return 0;
}

char **check_correct_bracket(char *const *const input_lines, size_t n_lines, size_t *n_out) {
    if (input_lines == NULL || n_out == NULL)
        return NULL;
    char **correct_lines = calloc(n_lines, sizeof(char *));
    if (correct_lines == NULL)
        return NULL;

    size_t n_correct = 0;
    for (size_t i = 0; i < n_lines; i++) {
        if (!input_lines[i])
            continue;  // если указатель ошибочный
        int bracket_count = 0;

        for (size_t j = 0; j < strlen(input_lines[i]) && bracket_count >= 0; j++) {
            if (input_lines[i][j] == '(') {
                bracket_count++;
            } else if (input_lines[i][j] == ')') {
                bracket_count--;
            }
        }

        if (bracket_count == 0) {
            correct_lines[n_correct] = input_lines[i];
            n_correct++;
        }
    }
    *n_out = n_correct;
    return correct_lines;
}


char *get_string() {
    char ch = '\0';
    size_t n = 0;
    size_t n_buff = 0;
    char *str = NULL;
    if (rebuff(&str, &n_buff) < 0)
        return NULL;

    while (scanf("%c", &ch) == 1 && ch != '\n' && ch != EOF) {
        str[n] = ch;
        ++n;
        if (n >= n_buff) {
            if (rebuff(&str, &n_buff) < 0)
                break;
        }
        str[n] = '\0';
    }
    return str;
}

int rebuff(char *buff[], size_t *n_buff) {
    if (n_buff == NULL)
        return -1;
    if (*n_buff == 0) {
        *n_buff = LEN_BUFF;
        *buff = calloc(*n_buff, sizeof(char));
    } else {
        if (buff == NULL || *buff == NULL)
            return -1;
        *n_buff *= MUL_REBUFF;
        char *new_ptr = realloc(*buff, *n_buff * sizeof(char));
        if (new_ptr) {
            *buff = new_ptr;
        } else {
            return -1;  // false
        }
    }
    return 0;  // true
}

char **input_strings(size_t *n_lines) {
    if (n_lines == NULL)
        return NULL;
    char **strings = malloc(sizeof(char *));
    if (strings == NULL)
        return NULL;
    size_t n = 0;
    size_t n_buff = 1;
    char *str = get_string();

    while (str != NULL && strlen(str) > 0) {
        if(n >= n_buff) {
            n_buff *= MUL_REBUFF;
            char **new_ptr = realloc(strings, n_buff * sizeof(char *));
            if (new_ptr)
                strings = new_ptr;
            else
                break;
        }
        strings[n] = str;
        ++n;
        str = get_string();  // считываем новую строку
    }
    free(str);
    *n_lines = n;
    return strings;
}


void free_input_strings(char **input_str, size_t n_lines) {
    if (!input_str)
        return;
    for (size_t i = 0; i < n_lines; i++) {
        free(input_str[i]);
    }
    free(input_str);
}
