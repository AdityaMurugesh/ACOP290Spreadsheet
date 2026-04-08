#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "sheet.h"
#include "display.h"
#include "evaluator.h"
#include "graph.h"
#include <sys/time.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: ./sheet R C\n");
        return 1;
    }

    // validate arguments are purely numeric
    for (int k = 0; argv[1][k] != '\0'; k++) {
        if (!isdigit(argv[1][k])) {
            printf("Error: invalid arguments. Usage: ./sheet R C\n");
            return 1;
        }
    }
    for (int k = 0; argv[2][k] != '\0'; k++) {
        if (!isdigit(argv[2][k])) {
            printf("Error: invalid arguments. Usage: ./sheet R C\n");
            return 1;
        }
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    if(rows < 1 || rows > 999 || cols < 1 || cols > 18278)
    {
        printf("Error: rows must be between 1 and 999, cols must be between 1 and 18278.\n");
        return 1;

    }

    init_sheet(rows, cols);
    init_graph(rows, cols);

    print_sheet();

    char input[256]; //keeps running till user types q
    char *status = "ok";
    double elapsed=0.0; //time taken to execute the command
    int output_enabled = 1; //output is enabled by default

    while (1)
    {
        printf("[%.1f] (%s) > ", elapsed, status);
        fflush(stdout);
        status = "ok";

        if(fgets(input, sizeof(input), stdin) == NULL)
        {
            break; //stops at more than one line of input
        }

        input[strcspn(input, "\n")] = 0; //removes /n character
        struct timeval tv_start, tv_end; //start time after input is read
        gettimeofday(&tv_start, NULL);

        if (strcmp(input, "q") == 0)
        {
            break; //stops at q
        }

        else if (strcmp(input, "w") == 0)
        {
            view_row -= 10;
            if (view_row < 0) view_row = 0; //prevents going above top row
            if (output_enabled) print_sheet();

        }

        else if (strcmp(input, "s") == 0)
        {
            view_row += 10;
            if (view_row > num_rows) view_row = num_rows - 1; //prevents from scrolling too far down
            if (output_enabled) print_sheet();

        }

        else if (strcmp(input, "a") == 0)
        {
            view_col -= 10;
            if (view_col < 0) view_col = 0; //prevents going left of column A
            if (output_enabled) print_sheet();

        }

        else if (strcmp(input, "d") == 0)
        {
            view_col += 10;
            if (view_col > num_cols) view_col = num_cols - 1;
            if (output_enabled) print_sheet();
        }

        else if (strcmp(input, "disable_output") == 0)
        {
            output_enabled = 0;
        }

        else if (strcmp(input, "enable_output") == 0)
        {
            output_enabled = 1;
        }

        else if (strncmp(input, "scroll_to ", 10) == 0)
        {
            char *cell = input + 10; // skips past the first 10 characters
            int ci = 0; //to extract column and row from the input
            char col_str[4];
            int col_len = 0;
            while (isalpha(cell[ci])) {
                col_str[col_len++] = cell[ci++];
            }
            col_str[col_len] = '\0';
            char row_str[5];
            int row_len = 0;
            while (isdigit(cell[ci])) {
                row_str[row_len++] = cell[ci++];
            }
            row_str[row_len] = '\0';

            if (col_len > 0 && row_len > 0) { //valid input
                view_col = col_name_to_index(col_str);
                view_row = row_name_to_index(row_str);
            }
            if (output_enabled) print_sheet();
        }

        else
        {
            int result = parse_input(input);
            if (result == 0)
            {
                if (output_enabled) print_sheet();
            }
            else if (result == 1)
            {
                status = "unrecognized cmd";
                if (output_enabled) print_sheet();
            }
            else if (result == 2)
            {
                status = "circular dependency";
                if (output_enabled) print_sheet();
            }
        }

        gettimeofday(&tv_end, NULL); //end time after command finished, includes recalculation time for dependent cells
        elapsed = (tv_end.tv_sec - tv_start.tv_sec) + (tv_end.tv_usec - tv_start.tv_usec) / 1000000.0;

    }


    return 0;
}