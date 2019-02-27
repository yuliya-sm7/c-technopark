#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t funn(char **input_lines, size_t n_lines, char **correct_lines);

int main()
{
    size_t n_max = 1024;
    char *A[n_max];
    size_t n_lines = 0;
    for (size_t i = 0; i < n_max; i++)
    {
        char *str = calloc(n_max, sizeof(char));
        gets(str);
        if (strlen(str) == 0)
        {
            free(str);
            break;
        }
        A[n_lines] = str;
        n_lines++;
    }
    char *B[n_lines];
    size_t n_out = funn(A, n_lines, B);
    for (size_t i = 0; i < n_out; i++){
       printf("%s\n", B[i]);
    }
    for (size_t i = 0; i < n_lines; i++){
        free(A[i]);
    }
    return 0;
}

size_t funn(char **input_lines, size_t n_lines, char **correct_lines)
{
    size_t count = 0;
    for (size_t i = 0; i < n_lines; i++)
    {
        int bkt_count = 0;
        for (size_t j = 0; j < strlen(input_lines[i]); j++)
        {
            if (input_lines[i][j] == '(')
            {
                bkt_count++;
            }
            else if (input_lines[i][j] == ')')
            {
                bkt_count--;
            }
            if (bkt_count < 0)
            {
                break;
            }
        }
        if (bkt_count == 0)
        {
            correct_lines[count] = input_lines[i];
            count++;
        }
    }
    return count;
}