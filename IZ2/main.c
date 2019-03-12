#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

const int LEN_BUFF = 64;
const int MUL_REBUFF = 2;

char *get_string();

int rebuff(void **buff, size_t *n_buff);

bool data_processing(const char line[], float *p_result);

void print_result(float result);

bool is_correct_line(const char line[]);

bool is_sign(char symbol);

bool is_bracket(char ch, ssize_t *count);

char *delete_space(const char line[]);

char *convert_to_polish_notation(const char line[]);

size_t readNum(const char line[], size_t i, float *number);

bool calculate_polish_notation(const char line[], float *result);

bool arithmetic(ssize_t top, float stack[], char element);


int main() {
    char *line = get_string();
    if (line == NULL || strlen(line) < 1)
        printf("[error]");
    else {
        float result = 0.0;
        bool is_correct = data_processing(line, &result);
        if (is_correct)
            print_result(result);
        else
            printf("[error]");
    }
    free(line);
    return 0;
}


char *get_string() {
    char ch = '\0';
    size_t n = 0;
    size_t n_buff = 0;
    char *str = NULL;
    if (rebuff((void **) &str, &n_buff) < 0)
        return NULL;

    while (scanf("%c", &ch) == 1 && ch != '\n' && ch != EOF) {
        str[n] = ch;
        ++n;
        if (n >= n_buff) {
            if (rebuff((void **) &str, &n_buff) < 0)
                break;
        }
        str[n] = '\0';
    }
    return str;
}


int rebuff(void **buff, size_t *n_buff) {
    if (n_buff == NULL)
        return -1;
    if (*n_buff == 0) {
        *n_buff = LEN_BUFF;
        *buff = calloc(*n_buff, sizeof(char));
    } else {
        if (buff == NULL || *buff == NULL)
            return -1;
        *n_buff *= MUL_REBUFF;
        void *new_ptr = realloc(*buff, *n_buff * sizeof(char));
        if (new_ptr) {
            *buff = new_ptr;
        } else {
            return -1;  // false
        }
    }
    return 0;  // true
}


bool data_processing(const char line[], float *p_result) {
    if (line == NULL || p_result == NULL)
        return false;
    char *clean_line = delete_space(line);
    if (clean_line == NULL)
        return false;
    if (is_correct_line(clean_line) == false) {
        free(clean_line);
        return false;
    }

    char *polish_line = convert_to_polish_notation(clean_line);
    free(clean_line);
    if (polish_line == NULL)
        return false;

    // расчет выражения, представленного в польской записи
    bool is_correct = calculate_polish_notation(polish_line, p_result);
    free(polish_line);
    return is_correct;
}

void print_result(float result) {
    printf("%.2lf\n", result);
}

// функция проверки корректности скобок, а также отсутствия символов отличных от цифр и арифметических операций во входной строке
bool is_correct_line(const char line[]) {
    if (line == NULL)
        return false;
    bool is_correct = true;
    ssize_t bracket_count = 0;

    for (size_t j = 0; j < strlen(line) && is_correct; j++) {
        if (!isdigit(line[j]) && !is_sign(line[j]) && !is_bracket(line[j], &bracket_count))
            is_correct = false;
        if (bracket_count < 0)
            is_correct = false;
    }
    if (bracket_count != 0) {
        is_correct = false;
    }
    return is_correct;
}

// проверяет, относится ли символ к допустимым в арифметических операциях (+, -, * итд)
bool is_sign(char symbol) {
    if (symbol >= 42 && symbol <= 47) {
        return true;
    }
    return false;
}

bool is_bracket(char ch, ssize_t *count) {
    if (ch == '(') {
        (*count)++;
    } else if (ch == ')') {
        (*count)--;
    } else {
        return false;
    }
    return true;
}


// удаление всех пробелов из входной последовательности
char *delete_space(const char line[]) {
    if (line == NULL)
        return NULL;
    char *result = calloc(strlen(line) + 1, sizeof(char));
    if (result == NULL)
        return NULL;

    for (size_t i = 0, j = 0; i < strlen(line) + 1; ++i) {
        if (line[i] != ' ')
            result[j++] = line[i];
    }
    return result;
}

// перевод входной последовательности выражения (очищенной от пробелов) в запись в обратной польской нотации
char *convert_to_polish_notation(const char line[]) {
    if (line == NULL)
        return NULL;
    char *result = calloc(2 * strlen(line) + 1, sizeof(char));
    if (result == NULL)
        return NULL;
    char *temp = calloc(strlen(line) + 1, sizeof(char));
    if (temp == NULL)
        return NULL;
    size_t res_index = 0;
    size_t temp_index = 0;

    for (size_t i_line = 0; i_line < strlen(line); ++i_line) {
        if (isdigit(line[i_line]) || line[i_line] == '.') {
            result[res_index] = line[i_line];
            ++res_index;
        } else if (line[i_line] == ')') {
            // если видим закрывающуюся скобку, то идем по временному буферу в обратном порядке до откр. скобки и копируем все знаки операций в конечный буфер
            while (temp_index > 0 && temp[temp_index] != '(') {
                --temp_index;
                if (is_sign(temp[temp_index])) {
                    result[res_index] = ' ';  // добавляем пробел для разделения чисел
                    ++res_index;
                    result[res_index] = temp[temp_index];
                    ++res_index;
                }
            }
        } else {
            if (line[i_line] == '-' && (i_line == 0 || (i_line > 0 && !isdigit(line[i_line - 1])))) {
                result[res_index] = '0';  // добавляем 0
                ++res_index;
            }
            if (is_sign(line[i_line])) {
                result[res_index] = ' ';
                ++res_index;
            }
            temp[temp_index] = line[i_line];
            ++temp_index;
        }
    }
    while (temp_index > 0) {
        --temp_index;
        if (is_sign(temp[temp_index])) {
            result[res_index] = ' ';  // добавляем пробел для разделения чисел
            ++res_index;
            result[res_index] = temp[temp_index];
            ++res_index;
        }
    }
    result[res_index] = '\0';
    free(temp);
    return result;
}


// преобразует строку в число с плавающей точкой
size_t readNum(const char line[], size_t i, float *number) {
    char buff[32] = "";
    size_t n = 0;
    while ((isdigit(line[i]) || line[i] == '.') && n < 31) {
        buff[n] = line[i];
        i++;
        n++;
    }
    buff[n] = '\0';
    *number = (float) atof(buff);
    return n;
}

// функция вычисляет значение выражения записанного в обратной польской нотации
bool calculate_polish_notation(const char line[], float *result) {
    if (line == NULL || result == NULL)
        return false;
    float stack[strlen(line) + 1];
    if (stack == NULL)
        return false;
    ssize_t stack_top = -1;

    bool is_correct = true;
    for (size_t i = 0; i < strlen(line) && is_correct; i += 2) {
        if (is_sign(line[i])) {
            is_correct = arithmetic(stack_top, stack, line[i]);
            --stack_top;
        } else {
            float digit;
            size_t n = readNum(line, i, &digit);
            is_correct = (n > 0);
            i += n - 1;
            ++stack_top;
            stack[stack_top] = digit;
        }
    }
    if (is_correct) {
        *result = stack[stack_top];
    }
    return is_correct;
}

bool arithmetic(ssize_t top, float stack[], char element) {
    bool is_correct = true;
    if (top < 0 || stack == NULL) {
        is_correct = false;
        return is_correct;
    }
    switch (element) {
        case '+':
            stack[top - 1] += stack[top];
            break;
        case '*':
            stack[top - 1] *= stack[top];
            break;
        case '/':
            stack[top - 1] /= stack[top];
            break;
        case '-':
            stack[top - 1] -= stack[top];
            break;
        default:
            is_correct = false;
            break;
    }
    return is_correct;
}