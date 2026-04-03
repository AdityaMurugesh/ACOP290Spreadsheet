#ifndef PARSER_H
#define PARSER_H

#include "sheet.h"

typedef enum{
    constant,
    cell,
    arithmetic,
    func
}ExprType;

typedef enum{
    add,
    subtract,
    multiply,
    divide
}OperatorType;

typedef enum{
    min,
    max,
    avg,
    sum,
    stdev,
    slp,
}FunctionType;

typedef struct{
    int is_cell;
    int constant;
    int row,col;
} Value;

typedef struct {
    ExprType type;

    Value left;
    OperatorType op;
    Value right;

    FunctionType func;
    int range_start_row, range_start_col;
    int range_end_row, range_end_col;
} Expression;

int parse_input(char *input);

#endif
