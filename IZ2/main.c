#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

char *read_line();

bool data_processing(const char line[], float *p_result);

void print_result(float result);

bool is_correct_line(const char line[]);

bool is_sign(char symbol);

char *delete_space(const char line[]);

char *convert_to_polish_notation(const char line[]);

size_t readNum(const char line[], size_t i, float *number);

bool calculate_polish_notation(const char line[], float *result);

bool arithmetic(size_t *top, float stack[], char element);

int main() {
    char *line = read_line(); // считывание входной последовательности
    if (line == NULL && strlen(line) <= 0)
        printf("[error]");
    else {
        float result = 0;
        bool is_correct = data_processing(line, &result);
        if (is_correct) print_result(result);
        else printf("[error]");
    }
    free(line);
    return 0;
}

// считываем входную строку
char *read_line() {
    char *str = calloc(1024, sizeof(char));
    fgets(str, 1024, stdin); // считываение входной строки
    if (errno == EINVAL || errno == ENOMEM) { // если ошибка, то освобождаем память и завершаем программу
        return NULL;
    }
    if (strlen(str) > 0 && str[strlen(str) - 1] == '\n') {
        str[strlen(str) - 1] = '\0';  // костыль =)
    }
    return str;
}

// проверка корректности входной строки: корректные скобки, отсутствие лишних символов
bool data_processing(const char line[], float *p_result) {
    if (!is_correct_line(line)) {
        return false;
    }
    // удаление всех пробелов
    char *inter_line = delete_space(line);
    if (inter_line == NULL) return false;
    // перевод выражения в обратную польскую запись
    char *new_line = convert_to_polish_notation(inter_line);
    free(inter_line);
    if (new_line == NULL) return false;

    // расчет выражения, представленного в польской записи
    bool is_correct = calculate_polish_notation(new_line, p_result);
    free(new_line);

    return is_correct;
}

void print_result(float result) {
    printf("%.2lf\n", result);
}

// функция проверки корректности скобок, а также отсутствия символов отличных от цифр и арифметических операций во входной строке
bool is_correct_line(const char line[]) {
    if (line == NULL) return false;
    bool is_correct = true;

    int bkt_count = 0;  // счетчик скобок
    for (size_t j = 0; j < strlen(line); j++) {
        if (line[j] == '(') bkt_count++;
        else if (line[j] == ')') bkt_count--;
        else if (!isdigit(line[j]) && !is_sign(line[j]))
            is_correct = false;
        if (bkt_count < 0) {
            is_correct = false; // если закрывающая появилась раньше открывающей - некорректно
        }
    }
    if (bkt_count != 0) is_correct = false;
    return is_correct;
}

// проверяет, относится ли символ к допустимым в арифметических операциях (+, -, * итд)
bool is_sign(char symbol) {
    bool is_correct = false;
    if (symbol >= 42 && symbol <= 47) is_correct = true;
    //if (symbol ==32) is_correct = true;
    return is_correct;
}

// удаление всех пробелов из входной последовательности
char *delete_space(const char line[]) {
    char *result = calloc(strlen(line) + 1, sizeof(char));
    if (errno == ENOMEM || line == NULL) return NULL;

    for (size_t i = 0, j = 0; i < strlen(line) + 1; ++i) {
        if (line[i] != ' ')
            result[j++] = line[i];
    }
    return result;
}

// перевод входной последовательности выражения (очищенной от пробелов) в запись в обратной польской нотации
char *convert_to_polish_notation(const char line[]) {
    if (line == NULL) {
        return NULL;
    }
    // выделяем память вдвое большую, так как неможем гарантировать, что запись в польской нотации не превысит
    // размеров записи в обычной нотации (из-за вставки пробелов внутри этой функции для различия разных чисел),
    // но можем гарантировать, что не превысит вдвое увеличенного буффера
    char *result = calloc(2 * strlen(line) + 1, sizeof(char));
    if (errno == ENOMEM) {
        return NULL;
    }

    char *temp = calloc(strlen(line) + 1,
                        sizeof(char)); // временный буфер, нужен для хранения скобок и знаков операций, цифры попадают сразу в конечный буфер
    if (errno == ENOMEM) {
        return NULL;
    }
    size_t line_index = 0;
    size_t res_index = 0;
    size_t temp_index = 0;

    while (line[line_index] != '\0') {
        // если цифра, то записываем её в конечный буффер
        if (isdigit(line[line_index]) || line[line_index] == '.') {
            result[res_index] = line[line_index];
            ++res_index;
        }
            // проверям, относится ли этот минус к знаку числа, если относится, то тоже его в конечную последовательность
        else if (line[line_index] == '-' && (line_index == 0 || (line_index > 0 && !isdigit(line[line_index - 1])))) {
            result[res_index] = line[line_index];
            ++res_index;
        }
            // все остальные символы во временный буффер
        else {
            if (line[line_index] == '(') { // и открывающуюся скобку тоже
                temp[temp_index] = line[line_index];
                ++temp_index;
            } else if (line[line_index] ==
                       ')') { // если видим закрывающуюся скобку, то идем по временному буферу в обратном порядке до откр. скобки и копируем все знаки операций в конечный буфер
                while (temp_index > 0 && temp[temp_index] != '(') {
                    --temp_index;
                    if (is_sign(temp[temp_index])) {
                        result[res_index] = temp[temp_index];
                        ++res_index;
                    }
                }
            } else {
                result[res_index] = ' ';  // добавляем пробел для разделения чисел
                ++res_index;
                temp[temp_index] = line[line_index];
                ++temp_index;
            }
        }
        ++line_index;
    }

    while (temp_index >
           0) { // когда обошли всю входную пследовательность, но во временном буффере еще остались символы, то переносим их от туда в конечный буффер
        --temp_index;
        if (is_sign(temp[temp_index])) {
            result[res_index] = temp[temp_index];
            ++res_index;
        }
    }
    result[res_index] = '\0';
    free(temp);

    return result;
}


// преобразует строку в число с плавающей точкой
// возвращает 0 в случае ошибок и 1 в их отсутствии
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
// возвращает 0 в случае ошибок и 1 в их отсутствии
bool calculate_polish_notation(const char line[], float *result) {
    bool is_correct = true;
    if (line == NULL || result == NULL) {
        is_correct = false;
        return is_correct;
    }
    float stack[strlen(line) + 1];
    if (errno == ENOMEM) {
        is_correct = false;
        return is_correct;
    }
    size_t stack_top = 0;
    for (size_t i = 0; i < strlen(line) && is_correct; ++i) {
        if (is_sign(line[i])) { // если знак операции
            is_correct = arithmetic(&stack_top, stack, line[i]);
            if (line[i+1] == ' ') ++i;
        } else {
            float digit;
            size_t n = readNum(line, i, &digit);
            is_correct = (n > 0);
            i += n;
            if (line[i] != ' ') --i;
            stack[stack_top] = digit;
            ++stack_top;
        }
    }
    if (is_correct) {
        *result = stack[stack_top - 1];
    }
    return is_correct;
}

bool arithmetic(size_t *top, float stack[], char element) {
    bool is_correct = true;
    if (top == NULL || *top < 0 || stack == NULL) {
        is_correct = false;
        return is_correct;
    }
    size_t stack_index = *top;
    switch (element) {
        case '+':
            stack[stack_index - 2] += stack[stack_index - 1];
            break;
        case '*':
            stack[stack_index - 2] *= stack[stack_index - 1];
            break;
        case '/':
            stack[stack_index - 2] /= stack[stack_index - 1];
            break;
        case '-':
            stack[stack_index - 2] -= stack[stack_index - 1];
            break;
        default:
            is_correct = false;
            break;
    }
    if (is_sign(element) && element != ' ') {
        --stack_index;
    }
    *top = stack_index;
    return is_correct;
}