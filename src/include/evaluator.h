#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "parser.h"

int evaluate(Expression *expr, int *has_error); //sets has_error to 1 in case of any error like division by zero

#endif 