#include <stdio.h>
#include <stdlib.h>
#include "sheet.h"

Cell **sheet;
int num_rows, num_cols;
int view_row = 0, view_col = 0;

void init_sheet(int rows, int cols) {
    num_rows = rows;
    num_cols = cols;

    sheet = malloc(rows * sizeof(Cell *));  // allocate array of row pointers
    for (int i = 0; i < rows; i++) {
        sheet[i] = malloc(cols * sizeof(Cell));  // allocate each row
        for (int j = 0; j < cols; j++) {
            sheet[i][j].value = 0;
            sheet[i][j].has_error = 0;
            sheet[i][j].formula[0] = '\0';
        }
    }
}

int col_name_to_index(char *col) {
    int result = 0;
    for (int i = 0; col[i] != '\0'; i++) {
        result = result * 26 + (col[i] - 'A' + 1);
    }
    return result - 1; // converting to 0 based index
}

int row_name_to_index(char *row) {
    return atoi(row) - 1; // converting to 0 based index, converting ascii to integer
}

void index_to_col_name(int index, char *buffer) {
    index++; // converting to 1-based index
    int len = 0;
    char tmp[10];

    while(index > 0)
    {   
        tmp[len++] = 'A' + (index - 1) % 26; //adding the character corresponding to the current index
        index = (index - 1) / 26; //moving to next letter
    }

    for (int i = 0; i < len; i++) {
        buffer[i] = tmp[len - i - 1]; //reversing the string to get the correct column name
    }

    buffer[len] = '\0'; 
}