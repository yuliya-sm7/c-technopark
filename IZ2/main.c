#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

const int LEN_BUFF = 64;
const int MUL_REBUFF = 2;

typedef struct Stack {
    char **buff;
    ssize_t top;
} Stack;

bool is_empty(const Stack *stack) {
    return stack->top < 0;
}

size_t size(const Stack *stack) {
    return (size_t) (stack->top + 1);
}


char *get_string();

int rebuff(void **buff, size_t *n_buff);

bool data_processing(const char line[], float *p_result);

void print_result(float result);

bool is_correct_line(const char line[]);

bool is_sign(char symbol);

bool is_bracket(char ch, ssize_t *count);

char *delete_space(const char line[]);

bool convert_to_polish_notation(const char line[], Stack *stack);

char *readNum(const char line[], size_t *i);

bool calculate_polish_notation(Stack *stack, float *result);

bool arithmetic(ssize_t top, float operands[], char element);

void add(Stack *stack, char symbol[]) {
    if (stack == NULL)
        return;
    char *str = calloc(strlen(symbol) + 1, sizeof(char));
    memcpy(str, symbol, strlen(symbol) + 1);
    ++(stack->top);
    (*stack).buff[stack->top] = str;
}

char *pop(Stack *stack) {
    if (is_empty(stack))
        return NULL;
    return stack->buff[(stack->top)--];
}


void stack_free(Stack *stack) {
    for (int i = 0; i < size(stack); ++i) {
        free(stack->buff[i]);
    }
    free(stack->buff);
}

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

    Stack stack;
    stack.buff = calloc(100, sizeof(char *));
    stack.top = -1;
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
bool convert_to_polish_notation(const char line[], Stack *out) {
    if (line == NULL) {
        return false;
    }
    Stack tmp;
    tmp.buff = calloc(10, sizeof(char *));
    tmp.top = -1;

    for (size_t i_line = 0; i_line < strlen(line); ++i_line) {

        char symbol = line[i_line];
        if (isdigit(symbol)) {
            char *operand = readNum(line, &i_line);
            add(out, operand);
            free(operand);
        } else if (symbol == '(') {
            add(&tmp, &symbol);
        } else if (symbol == ')') {
            char *ch = pop(&tmp);
            while (ch != NULL && *ch != '(') {
                add(out, ch);
                ch = pop(&tmp);
            }
            free(ch);
        } else if (is_sign(symbol)) {
            if (symbol == '-' && (i_line == 0 || (i_line > 0 && !isdigit(line[i_line - 1])))) {
                add(out, "0\0");
            }
            while (!is_empty(&tmp) && *tmp.buff[tmp.top] != '(') {
                add(out, pop(&tmp));
            }
            char operator[2];
            operator[0] = symbol;
            operator[1] = '\0';
            add(&tmp, operator);
        }
    }
    while (!is_empty(&tmp)) {
        add(out, pop(&tmp));
    }
    stack_free(&tmp);
    return true;
}


// преобразует строку в число с плавающей точкой
char *readNum(const char line[], size_t *i) {
    char *buff = calloc(32, sizeof(char));
    size_t n = 0;
    size_t j = *i;
    while ((isdigit(line[j]) || line[j] == '.') && n < 31) {
        buff[n] = line[j];
        j++;
        n++;
    }
    buff[n] = '\0';
    *i = j - 1;
    return buff;
}


bool calculate_polish_notation(Stack *stack, float *result) {
    if (stack == NULL || result == NULL)
        return false;
    float *operands = calloc(size(stack), sizeof(float));
    if (operands == NULL)
        return false;
    ssize_t stack_top = -1;

    bool is_correct = true;
    for (size_t i = 0; i < size(stack) && is_correct; i++) {
        char *sym = stack->buff[i];
        if (is_sign(*sym)) {
            is_correct = arithmetic(stack_top, operands, *sym);
            --stack_top;
        } else {
            ++stack_top;
            operands[stack_top] = atof(sym);
        }
    }
    if (is_correct) {
        *result = operands[stack_top];
    }
    free(operands);
    return is_correct;
}

bool arithmetic(ssize_t top, float operands[], char element) {
    bool is_correct = true;
    if (top < 0 || operands == NULL) {
        is_correct = false;
        return is_correct;
    }
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
            is_correct = false;
            break;
    }
    return is_correct;
}