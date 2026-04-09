#ifndef GRAPH_H
#define GRAPH_H
#include"parser.h"

typedef struct DependencyNode {
    int row,col;
    struct DependencyNode* next;
} DependencyNode;

typedef struct {
    DependencyNode *dependents;
    Expression *expr;
    int has_formula;
} CellGraphNode;

extern CellGraphNode **graph;
void init_graph(int rows, int cols);
void remove_from_dependents(int from_row, int from_col, int to_row, int to_col);
void clear_dependency_graph(int row, int col);
void add_dependency(int from_row, int from_col, int to_row, int to_col);
int has_circular_dependency(int row, int col);
void recalculate_dependents(int row, int col);

#endif