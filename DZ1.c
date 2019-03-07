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

size_t check_correct(char *const *const input_lines, size_t n_lines, char **correct_lines);
size_t input_strings(char ***strings);
void free_input_strings(char **input_str, size_t n_lines);

int main() {
    char **input_str;
    size_t n_lines = input_strings(&input_str);  // ввод массива сток
    char *correct_str[n_lines];
    // проверка на соответствие количества открытых и закрытых строк
    size_t n_out = check_correct(input_str, n_lines, correct_str);
    for (size_t i = 0; i < n_out; i++) {  // вывод результата
        printf("%s\n", correct_str[i]);
    }
    free_input_strings(input_str, n_lines);  // очистка выделенной памяти

    return 0;
}

size_t check_correct(char *const *const input_lines, size_t n_lines, char **correct_lines) {
    if (input_lines == NULL || correct_lines == NULL)
        return 0;
    size_t n_correct = 0;
    for (size_t i = 0; i < n_lines; i++) {
        if (!input_lines[i])  // если указатель ошибочный
            continue;
        int bkt_count = 0;  // счетчик скобок
        for (size_t j = 0; j < strlen(input_lines[i]); j++) {
            if (input_lines[i][j] == '(') {
                bkt_count++;
            } else if (input_lines[i][j] == ')') {
                bkt_count--;
            }
            if (bkt_count < 0) {
                break; // если закрывающая появилась раньше открывающей - некорректно
            }
        }
        if (bkt_count == 0) {
            correct_lines[n_correct] = input_lines[i];
            n_correct++;
        }
    }
    return n_correct;
}

size_t input_strings(char ***strings) {
    if (!strings)
        return 0;
    const size_t n_max = 1024, len_max = 1024;
    *strings = calloc(n_max, sizeof(char *));
    if (!(*strings))
        return 0;
    size_t n_lines = 0;

    while (1) {  // бесконечный цикл
        char *str = calloc(len_max, sizeof(char));
        if (!str)
            return n_lines;  // оборвать считывание если нет памяти

        fgets(str, len_max, stdin);  // считываю ввод

        if (strlen(str) > 0 && str[strlen(str) - 1] == '\n') {
            str[strlen(str) - 1] = '\0';  // костыль =)
        }

        if (strlen(str) == 0) {
            free(str);
            break;  // конец считывания
        }
        (*strings)[n_lines] = str;
        n_lines++;
    }

    return n_lines;
}

void free_input_strings(char **input_str, size_t n_lines) {
    if (!input_str)
        return;
    for (size_t i = 0; i < n_lines; i++) {
        free(input_str[i]);
    }
    free(input_str);
}
