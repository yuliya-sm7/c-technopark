#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

const int LEN_BUFF = 64;
const int MUL_REBUFF = 2;

typedef struct {
    char **buff;  // массив строк (операторы и операнды)
    ssize_t top;  // индекс текущего последнего элемента
} stack_t;

// функции для работы со стеком
bool is_empty(const stack_t *stack);

bool add(stack_t *stack, char symbol[]);

char *pop(stack_t *stack);

size_t size(const stack_t *stack);

void stack_free(stack_t *stack);
// функции для работы со стеком - конец

bool get_char(char *ch);

char *get_string();

size_t rebuff(void **buff, size_t n_buff, size_t type_size);

bool data_processing(const char line[], float *p_result);

void print_result(float result);

bool is_correct_line(const char line[]);

bool is_sign(char symbol);

bool is_bracket(char ch, int *count);

bool priority(char ch_new, char ch_stack);

char *delete_space(const char line[]);

bool convert_to_polish_notation(const char line[], stack_t *out);

char *readNum(const char line[], size_t *begin);

bool calculate_polish_notation(stack_t *stack, float *result);

bool arithmetic(ssize_t top, float operands[], char element);


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


bool is_empty(const stack_t *stack) {
    if (stack == NULL)
        return false;
    return stack->top < 0;
}

bool add(stack_t *stack, char symbol[]) {
    if (stack == NULL)
        return false;
    char *str = calloc(strlen(symbol) + 1, sizeof(char));
    if (str == NULL)
        return false;
    memcpy(str, symbol, strlen(symbol) + 1);
    ++(stack->top);
    stack->buff[stack->top] = str;
    return true;
}

char *pop(stack_t *stack) {
    if (is_empty(stack))
        return NULL;
    return stack->buff[(stack->top)--];
}

size_t size(const stack_t *stack) {
    if (stack == NULL)
        return 0;
    return (size_t) (stack->top + 1);
}

void stack_free(stack_t *stack) {
    if (stack == NULL)
        return;
    for (int i = 0; i < size(stack); ++i) {
        if (stack->buff[i] != NULL)
            free(stack->buff[i]);
    }
    if (stack->buff != NULL)
        free(stack->buff);
}

bool get_char(char *ch) {
    if (ch == NULL)
        return false;
    int n = scanf("%c", ch);
    if (n != 1 || *ch == '\n' || *ch == '\0' || *ch == EOF) {
        return false;
    }
    return  true;
}


char *get_string() {
    char ch = '\0';
    size_t n = 0;
    char *str = NULL;
    size_t n_buff = rebuff((void **) &str, 0, sizeof(char));
    if (n_buff < 1)
        return NULL;

    while (get_char(&ch)) {
        str[n] = ch;
        ++n;
        if (n >= n_buff) {
            n_buff = rebuff((void **) &str, n_buff, sizeof(char));
            if (n_buff < 1)
                break;
        }
    }
    str[n] = '\0';
    return str;
}


size_t rebuff(void **buff, size_t n_buff, size_t type_size) {
    if (n_buff == 0) {
        n_buff = LEN_BUFF;
        *buff = calloc(n_buff, type_size);
        if (*buff == NULL)
            return 0;
    } else {
        if (buff == NULL || *buff == NULL)
            return 0;
        n_buff *= MUL_REBUFF;
        void *new_ptr = realloc(*buff, n_buff * type_size);
        if (new_ptr) {
            *buff = new_ptr;
        } else {
            return 0;
        }
    }
    return n_buff;  // длина нового буфера
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

    stack_t stack;
    stack.buff = calloc(strlen(line), sizeof(char *)); // колличество операторов и операндов точно не больше
    stack.top = -1;
    if (stack.buff == NULL) {
        free(clean_line);
        return false;
    }
    convert_to_polish_notation(clean_line, &stack);
    free(clean_line);

    bool is_correct = calculate_polish_notation(&stack, p_result);
    stack_free(&stack);
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
    int bracket_count = 0;

    for (size_t j = 0; j < strlen(line) && is_correct; j++) {
        if (!isdigit(line[j]) && !is_sign(line[j]) && line[j] != '.' && !is_bracket(line[j], &bracket_count))
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
    if (symbol == '+' || symbol == '-' || symbol == '/' || symbol == '*') {
        return true;
    }
    return false;
}

bool is_bracket(char ch, int *count) {
    if (ch == '(') {
        (*count)++;
    } else if (ch == ')') {
        (*count)--;
    } else {
        return false;
    }
    return true;
}

bool priority(char ch_new, char ch_stack) {
    if ((ch_new == '*' || ch_new == '/') && (ch_stack == '+' || ch_stack == '-'))
        return false;  // если в стеке приоритет ниже, чем у добавляемого элемента
    if (ch_stack == '(')
        return false;
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

bool convert_to_polish_notation(const char line[], stack_t *out) {
    if (line == NULL || out == NULL) {
        return false;
    }
    stack_t tmp;
    tmp.buff = calloc(strlen(line) - 1, sizeof(char *));
    if (tmp.buff == NULL)
        return false;
    tmp.top = -1;
    bool add_flaf = true;
    for (size_t i_line = 0; i_line < strlen(line); ++i_line) {
        char symbol = line[i_line];

        if (isdigit(symbol)) {
            char *operand = readNum(line, &i_line);
            add_flaf = add(out, operand) && add_flaf;
            free(operand);
        } else if (symbol == '(') {
            add_flaf = add(&tmp, &symbol) && add_flaf;
        } else if (symbol == ')') {
            char *ch = pop(&tmp);
            while (ch != NULL && *ch != '(') {
                add_flaf = add(out, ch) && add_flaf;
                free(ch);
                ch = pop(&tmp);
            }
            free(ch);

        } else if (is_sign(symbol)) {
            if (symbol == '-' && (i_line == 0 || (i_line > 0 && !isdigit(line[i_line - 1])))) {
                add_flaf = add(out, "0\0") && add_flaf;  // ставим 0 перед унарным минусом
            }
            while (!is_empty(&tmp) && priority(symbol, *tmp.buff[tmp.top])) {
                char *ch = pop(&tmp);
                add_flaf = add(out, ch) && add_flaf;
                free(ch);
            }
            char operator[2] = {symbol, '\0'};
            add_flaf = add(&tmp, operator) && add_flaf;
        }

    }
    while (!is_empty(&tmp)) {
        char *ch = pop(&tmp);
        add_flaf = add(out, ch) && add_flaf;
        free(ch);
    }
    stack_free(&tmp);
    return add_flaf;
}


char *readNum(const char line[], size_t *begin) {
    if (line == NULL || begin == NULL)
        return NULL;
    char *buff = calloc(LEN_BUFF, sizeof(char));
    if (buff == NULL)
        return NULL;
    size_t n = 0;
    size_t j = *begin;
    while ((isdigit(line[j]) || line[j] == '.') && n < LEN_BUFF - 1) {
        buff[n] = line[j];
        j++;
        n++;
    }
    buff[n] = '\0';
    *begin = j - 1;
    return buff;
}


bool calculate_polish_notation(stack_t *stack, float *result) {
    if (stack == NULL || result == NULL)
        return false;
    float *operands = calloc(size(stack) - 1, sizeof(float));
    if (operands == NULL)
        return false;
    ssize_t top = -1;
    bool is_correct = true;

    for (size_t i = 0; i < size(stack) && is_correct; ++i) {
        char *sym = stack->buff[i];
        if (is_sign(*sym)) {
            is_correct = arithmetic(top, operands, *sym);
            --top;
        } else {
            ++top;
            operands[top] = atof(sym);
        }
    }

    if (is_correct) {
        *result = operands[top];
    }
    free(operands);
    return is_correct;
}

bool arithmetic(ssize_t top, float operands[], char element) {
    if (top < 1 || operands == NULL)
        return false;
    switch (element) {
        case '+':
            operands[top - 1] += operands[top];
            break;
        case '*':
            operands[top - 1] *= operands[top];
            break;
        case '/':
            operands[top - 1] /= operands[top];
            break;
        case '-':
            operands[top - 1] -= operands[top];
            break;
        default:
            return false;
    }
    return true;
}