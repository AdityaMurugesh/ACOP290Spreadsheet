#ifndef SHEET_H
#define SHEET_H

#define MAX_ROWS 999
#define MAX_COLS 18278

#define VIEW_ROWS 10
#define VIEW_COLS 10

typedef struct {
    int value;
    int has_error;
    char formula[256];
} Cell;

extern Cell sheet[MAX_ROWS][MAX_COLS];
extern int num_rows, num_cols;

extern int view_row, view_col;

void init_sheet(int rows, int cols);
void col_name_to_index(char *col);
int row_name_to_index(char *row);
void index_to_col_name(int index, char *buffer);

#endif

