#include <stdio.h>
#include "display.h"

void print_sheet() 
{
    int row_end = view_row + VIEW_ROWS;
    if (row_end > num_rows) row_end = num_rows;

    int col_end = view_col + VIEW_COLS;
    if (col_end > num_cols) col_end = num_cols;

    printf("    "); // leaving a gap for row numbers (matches %3d + space)
    for (int j = view_col; j < col_end; j++) {
        char col_name[10];
        index_to_col_name(j, col_name);
        printf("%6s", col_name); // printing with 6 character width
    }
    printf("\n");

    for (int i = view_row; i < row_end; i++)
    {
        printf("%3d ", i + 1); // printing row number with 3 character width
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