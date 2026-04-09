#include <stdio.h>
#include <stdlib.h>
#include "evaluator.h"
#include "sheet.h"
#include <math.h>
#include <unistd.h>

int get_value(Value *val, int *has_error)
{
    if (val->is_cell) 
    {
        if(sheet[val->row][val->col].has_error)
        {
            *has_error = 1;
            return 0;
        }
        return sheet[val->row][val->col].value;
    } else {
        return val->constant;
    }
}

int evaluate(Expression *expr, int *has_error)
{

    *has_error = 0;

    //plain constant case
    if (expr->type == constant) 
    {
        return expr->left.constant;
    }

    //cell reference case
    if (expr->type == cell)
    {
        return get_value(&expr->left, has_error);
    }

    //arithmetic case
    if (expr->type == arithmetic)
    {
        int left_val = get_value(&expr->left, has_error);
        if (*has_error) return 0;

        int right_val = get_value(&expr->right, has_error);
        if (*has_error) return 0;

        if (expr->op == add) return left_val + right_val;
        if (expr->op == subtract) return left_val - right_val;
        if (expr->op == multiply) return left_val * right_val;
        if (expr->op == divide) {
            if (right_val == 0) {
                *has_error = 1; //div by zero error
                return 0;
            }
            return left_val / right_val; // integer division
        }
    }

    //function case
    if (expr->type == func)
    {
        //SLEEP CASE
        if (expr->func == slp)
        {
            int secs = get_value(&expr->left, has_error);
            if (*has_error) return 0;
            if (secs < 0) {
                *has_error = 1;
                return 0;
            }
            sleep(secs);
            return secs;

        }

        //loop through cells
        int start_row = expr->range_start_row;
        int start_col = expr->range_start_col;
        int end_row = expr->range_end_row;
        int end_col = expr->range_end_col;

        if (start_row > end_row || start_col > end_col) 
        {
            *has_error = 1; //invalid range error
            return 0;
        }

        int count = (end_row - start_row + 1) * (end_col - start_col + 1);

        // check for errors in range first
        for (int r = start_row; r <= end_row; r++) {
            for (int c = start_col; c <= end_col; c++) {
                if (sheet[r][c].has_error) {
                    *has_error = 1;
                    return 0;
                }
            }
        }

        int sum_val = 0;
        int min_val = sheet[start_row][start_col].value;
        int max_val = sheet[start_row][start_col].value;
        double mean = 0;

        for (int r = start_row; r <= end_row; r++) {
            for (int c = start_col; c <= end_col; c++) {
                int cell_val = sheet[r][c].value;
                sum_val += cell_val;
                if (cell_val < min_val) min_val = cell_val;
                if (cell_val > max_val) max_val = cell_val;
            }
        }

    mean = (double)sum_val / count;

    if(expr->func == sum) return sum_val;
    if(expr->func == min) return min_val;
    if(expr->func == max) return max_val;
    if(expr->func == avg) return (int)mean;

    if(expr->func == stdev) 
    {
        double variance = 0;
        for (int r = start_row; r <= end_row; r++) {
            for (int c = start_col; c <= end_col; c++) {
                double diff = sheet[r][c].value - mean;
                variance += diff * diff;
            }
        }
        variance /= count;
        return (int)sqrt(variance);
    }

    }

    *has_error = 1; 
    return 0;
}