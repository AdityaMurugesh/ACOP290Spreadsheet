#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"
#include "display.h"
#include "evaluator.h"
#include "graph.h"

int parse_value(char *input, int i, Value *val)
{
    if (isdigit(input[i]) || (input[i] == '-' && isdigit(input[i + 1])))
    {
        val->is_cell = 0;
        val->constant = atoi(&input[i]);
        if (input[i] == '-') i++; // skip the minus sign
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
        if (val->row < 0 || val->row >= num_rows || val->col < 0 || val->col >= num_cols)
            return -1;
        return i;
    }
    return -1;
}

int parse_input(char *input)
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

    return 1;
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
    return 1;
}
while (input[i] == ' ')
{
    i++;
}

if (input[i] != '=')
{
    return 1;
}
i++;

while (input[i] == ' ')
{
    i++;
}

if (input[i] == '\0')
{

    return 1;
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
        
        return 1;
    }

    i++;
    if (expr.func ==slp){
        Value sleep_val;
        i = parse_value(input, i, &sleep_val);
        if (i == -1) return 1;
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

        if (expr.range_start_row < 0 || expr.range_start_row >= num_rows ||
            expr.range_start_col < 0 || expr.range_start_col >= num_cols)
            return 1;

        if (input[i] != ':')
        {
            return 1;
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

        if (expr.range_end_row < 0 || expr.range_end_row >= num_rows ||
            expr.range_end_col < 0 || expr.range_end_col >= num_cols)
            return 1;

        if (expr.range_start_row > expr.range_end_row || expr.range_start_col > expr.range_end_col)
        {
            return 3; // invalid range error
        }

    }
    if (input[i] != ')')
    {
        return 1;
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
        return 1;
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
            return 1;
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
        return 1;
    }
}

int col = col_name_to_index(col_name);
int row = row_name_to_index(row_name);

if (row < 0 || row >= num_rows || col < 0 || col >= num_cols)
{
    return 1;
}

// add new dependencies (temporarily, before clearing old ones)
if (expr.type == cell) {
    add_dependency(expr.left.row, expr.left.col, row, col);
} else if (expr.type == arithmetic) {
    if (expr.left.is_cell)
        add_dependency(expr.left.row, expr.left.col, row, col);
    if (expr.right.is_cell)
        add_dependency(expr.right.row, expr.right.col, row, col);
} else if (expr.type == func && expr.func != slp) {
    for (int r = expr.range_start_row; r <= expr.range_end_row; r++) {
        for (int c = expr.range_start_col; c <= expr.range_end_col; c++) {
            add_dependency(r, c, row, col);
        }
    }
} else if (expr.type == func && expr.func == slp) {
    if (expr.left.is_cell)
        add_dependency(expr.left.row, expr.left.col, row, col);
}

// check for circular dependency before committing
if (has_circular_dependency(row, col)) {
    // undo the new dependencies we just added
    if (expr.type == cell) {
        remove_from_dependents(expr.left.row, expr.left.col, row, col);
    } else if (expr.type == arithmetic) {
        if (expr.left.is_cell)
            remove_from_dependents(expr.left.row, expr.left.col, row, col);
        if (expr.right.is_cell)
            remove_from_dependents(expr.right.row, expr.right.col, row, col);
    } else if (expr.type == func && expr.func != slp) {
        for (int r = expr.range_start_row; r <= expr.range_end_row; r++) {
            for (int c = expr.range_start_col; c <= expr.range_end_col; c++) {
                remove_from_dependents(r, c, row, col);
            }
        }
    } else if (expr.type == func && expr.func == slp) {
        if (expr.left.is_cell)
            remove_from_dependents(expr.left.row, expr.left.col, row, col);
    }
    return 2;  // circular dependency error
}

// new formula passed all checks — now safe to clear old dependencies
clear_dependency_graph(row, col);

// store expression in graph
Expression *stored_expr = malloc(sizeof(Expression));
*stored_expr = expr;
graph[row][col].expr = stored_expr;
graph[row][col].has_formula = (expr.type != constant) ? 1 : 0;

// evaluate expression
int has_error = 0;
int result = evaluate(&expr, &has_error);

sheet[row][col].has_error = has_error;
if (!has_error)
    sheet[row][col].value = result;
else
    sheet[row][col].value = 0;

strcpy(sheet[row][col].formula, input);

// recalculate dependents
recalculate_dependents(row, col);

return 0;

}
    
