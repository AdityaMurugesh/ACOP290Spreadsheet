#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "sheet.h"
#include "evaluator.h"

CellGraphNode **graph;

void init_graph(int rows, int cols) {
    graph = malloc(rows * sizeof(CellGraphNode *));  
    for (int i = 0; i < rows; i++) {
        graph[i] = malloc(cols * sizeof(CellGraphNode)); 
        for (int j = 0; j < cols; j++) {
            graph[i][j].dependents = NULL;
            graph[i][j].expr = NULL;
            graph[i][j].has_formula = 0;
        }
    }
}

void remove_from_dependents(int from_row, int from_col, int to_row, int to_col) {
    DependencyNode **current = &graph[from_row][from_col].dependents;
    while (*current != NULL) {
        if ((*current)->row == to_row && (*current)->col == to_col) {
            DependencyNode *temp = *current;
            *current = (*current)->next;
            free(temp);
            return;
        }
        current = &(*current)->next;
    }
}

void clear_dependency_graph(int row, int col) {
    if (!graph[row][col].has_formula || graph[row][col].expr == NULL)
        return;

    Expression *expr = graph[row][col].expr;

    if (expr->type == cell) {
        // was depending on one cell
        remove_from_dependents(expr->left.row, expr->left.col, row, col);
    }
    else if (expr->type == arithmetic) {
        // check left operand
        if (expr->left.is_cell)
            remove_from_dependents(expr->left.row, expr->left.col, row, col);
        // check right operand
        if (expr->right.is_cell)
            remove_from_dependents(expr->right.row, expr->right.col, row, col);
    }
    else if (expr->type == func && expr->func != slp) {
        for (int r = expr->range_start_row; r <= expr->range_end_row; r++) {
            for (int c = expr->range_start_col; c <= expr->range_end_col; c++) {
                remove_from_dependents(r, c, row, col);
            }
        }
    }
    else if (expr->type == func && expr->func == slp) {
        if (expr->left.is_cell)
            remove_from_dependents(expr->left.row, expr->left.col, row, col);
    }

    free(graph[row][col].expr);
    graph[row][col].expr = NULL;
    graph[row][col].has_formula = 0;
}

void add_dependency(int from_row, int from_col, int to_row, int to_col) {
    DependencyNode *node = malloc(sizeof(DependencyNode));
    node->row = to_row;
    node->col = to_col;
    node->next = graph[from_row][from_col].dependents;
    graph[from_row][from_col].dependents = node;
}

int dfs_cycle(int start_row, int start_col, int row, int col, int **visited) {
    DependencyNode *dep = graph[row][col].dependents;
    while (dep != NULL) {
        if (dep->row == start_row && dep->col == start_col)
            return 1;
        if (!visited[dep->row][dep->col]) {
            visited[dep->row][dep->col] = 1;
            if (dfs_cycle(start_row, start_col, dep->row, dep->col, visited))
                return 1;
        }
        dep = dep->next;
    }
    return 0;
}

int has_circular_dependency(int row, int col) {
    int **visited = malloc(num_rows * sizeof(int *));
    for (int i = 0; i < num_rows; i++) {
        visited[i] = calloc(num_cols, sizeof(int));
    }

    int result = dfs_cycle(row, col, row, col, visited);

    for (int i = 0; i < num_rows; i++) {
        free(visited[i]);
    }
    free(visited);

    return result;
}

void recalculate_dependents(int row, int col) {
    DependencyNode *dep = graph[row][col].dependents;
    while (dep != NULL) {
        int r = dep->row;
        int c = dep->col;

        if (graph[r][c].has_formula && graph[r][c].expr != NULL) {
            int has_error = 0;
            int result = evaluate(graph[r][c].expr, &has_error);

            sheet[r][c].has_error = has_error;
            if (!has_error)
                sheet[r][c].value = result;

            recalculate_dependents(r, c);
        }

        dep = dep->next;
    }
}
