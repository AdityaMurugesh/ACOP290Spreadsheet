#include <stdio.h>
#include "display.h"

void print_sheet()
{
    int row_end = view_row + VIEW_ROWS;
    if (row_end > num_rows) row_end = num_rows;

    int col_end = view_col + VIEW_COLS;
    if (col_end > num_cols) col_end = num_cols;

    // figure out how many digits the largest displayed row number has
    int max_row_num = row_end;
    int row_width = 0;
    int tmp = max_row_num;
    while (tmp > 0) { row_width++; tmp /= 10; }
    if (row_width == 0) row_width = 1;

    // print column header with gap matching row number width
    for (int s = 0; s < row_width; s++) printf(" ");
    for (int j = view_col; j < col_end; j++) {
        char col_name[10];
        index_to_col_name(j, col_name);
        printf("%6s", col_name);
    }
    printf("\n");

    // print rows
    for (int i = view_row; i < row_end; i++)
    {
        printf("%*d", row_width, i + 1);
        for (int j = view_col; j < col_end; j++)
        {
            if (sheet[i][j].has_error)
            {
                printf("%6s", "ERR");
            }
            else
            {
                printf("%6d", sheet[i][j].value);
            }
        }
        printf("\n");
    }
}
