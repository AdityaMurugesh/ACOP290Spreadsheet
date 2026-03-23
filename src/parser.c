#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"
#include "display.h"

int parse_value(char *input, int i, Value *val)
{
    if (isdigit(input[i]))
    {
        val->is_cell = 0;
        val->constant = atoi(&input[i]);
        while (isdigit(input[i]))
        {
            i++;
        }
        return i;
    }
    else if (isalpha(input[i]))
    {
        val->is_cell = 1;
        char col_str[4];
        int len = 0;
        while (isalpha(input[i]))
        {
            col_str[len++] = input[i++];
        }
        col_str[len] = '\0';

        char row_str[5];
        len = 0;
        while (isdigit(input[i]))
        {
            row_str[len++] = input[i++];
        }
        row_str[len] = '\0';

        val->col = col_name_to_index(col_str);
        val->row = row_name_to_index(row_str);
        return i;
    }
    return -1;
}

void parse_input(char *input)
{
char col_name[4];
int col_len=0;
int i=0;

while (isalpha(input[i])){
    col_name[col_len]=input[i];
    col_len++;
    i++;
}
col_name[col_len]='\0';


if (col_len==0){
    printf("invalid input\n");
    return;
}
char row_name[5];
int row_len = 0;

while (isdigit(input[i]))
{
    row_name[row_len] = input[i];
    row_len++;
    i++;
}
row_name[row_len] = '\0';

if (row_len == 0)
{
    printf("invalid input\n");
    return;
}
while (input[i] == ' ')
{
    i++;
}

if (input[i] != '=')
{
    printf("invalid input\n");
    return;
}
i++;

while (input[i] == ' ')
{
    i++;
}

if (input[i] == '\0')
{
    printf("invalid input\n");
    return;
}
Expression expr;
int save_i = i;
char function_name[10];
int func_len = 0;
while (isalpha(input[i]))
{
    function_name[func_len] = input[i];
    func_len++;
    i++;

}
function_name[func_len] = '\0';

if (input[i]=='('){
    expr.type=func;

    if (strcmp(function_name, "MIN") == 0)
    {
        expr.func = min;
    }
    else if (strcmp(function_name, "MAX") == 0)
    {
        expr.func = max;
    }
    else if (strcmp(function_name, "AVG") == 0)
    {
        expr.func = avg;
    }
    else if (strcmp(function_name, "SUM") == 0)
    {
        expr.func = sum;
    }
    else if (strcmp(function_name, "STDEV") == 0)
    {
        expr.func = stdev;
    }
    else if (strcmp(function_name, "SLEEP") == 0)
    {
        expr.func = slp;
    }
    else
    {
        printf("invalid function\n");
        return;
    }

    i++;
    if (expr.func ==slp){
        Value sleep_val;
        i = parse_value(input, i, &sleep_val);
        expr.left = sleep_val;
    }
    else{
        char start_col_name[4];
        int start_col_len = 0;
        while (isalpha(input[i]))
        {
            start_col_name[start_col_len] = input[i];
            start_col_len++;
            i++;
        }
        start_col_name[start_col_len] = '\0';
        char start_row_name[5];
        int start_row_len = 0;
        while (isdigit(input[i]))
        {
            start_row_name[start_row_len] = input[i];
            start_row_len++;
            i++;
        }
        start_row_name[start_row_len] = '\0';

        expr.range_start_col = col_name_to_index(start_col_name);
        expr.range_start_row = row_name_to_index(start_row_name);
        
        if (input[i] != ':')
        {
            printf("invalid range\n");
            return;
        }
        i++;
        char end_col_name[4];
        int end_col_len = 0;
        while (isalpha(input[i]))
        {
            end_col_name[end_col_len] = input[i];
            end_col_len++;
            i++;
        }
        end_col_name[end_col_len] = '\0';
        char end_row_name[5];
        int end_row_len = 0;
        while (isdigit(input[i]))
        {
            end_row_name[end_row_len] = input[i];
            end_row_len++;
            i++;
        }
        end_row_name[end_row_len] = '\0';
        expr.range_end_col = col_name_to_index(end_col_name);
        expr.range_end_row = row_name_to_index(end_row_name);

    }
    if (input[i] != ')')
    {
        printf("invalid function format\n");
        return;
    }
    i++;


}
else
{
    i = save_i;

    Value left;
    i = parse_value(input, i, &left);
    if (i == -1)
    {
        printf("invalid input\n");
        return;
    }
    expr.left = left;

    while (input[i] == ' ')
    {
        i++;
    }

    if (input[i] == '+' || input[i] == '-' || input[i] == '*' || input[i] == '/')
    {
        expr.type = arithmetic;

        if (input[i] == '+') expr.op = add;
        else if (input[i] == '-') expr.op = subtract;
        else if (input[i] == '*') expr.op = multiply;
        else if (input[i] == '/') expr.op = divide;
        i++;

        while (input[i] == ' ')
        {
            i++;
        }

        Value right;
        i = parse_value(input, i, &right);
        if (i == -1)
        {
            printf("invalid input\n");
            return;
        }
        expr.right = right;
    }
    else if (input[i] == '\0')
    {
        if (left.is_cell)
            expr.type = cell;
        else
            expr.type = constant;
    }
    else
    {
        printf("invalid input\n");
        return;
    }
}

int col = col_name_to_index(col_name);
int row = row_name_to_index(row_name);

if (row < 0 || row >= num_rows || col < 0 || col >= num_cols)
{
    printf("invalid cell\n");
    return;
}

if (expr.type == constant)
{
    sheet[row][col].value = expr.left.constant;
    sheet[row][col].has_error = 0;
}

strcpy(sheet[row][col].formula, input);

print_sheet();
}


    
