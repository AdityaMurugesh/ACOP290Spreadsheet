#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "sheet.h"
#include "display.h"

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: ./sheet R C\n");
        return 1;
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);

    if(rows < 1 || rows > 999 || cols < 1 || cols > 18278)
    {
        printf("Error: rows must be between 1 and 999, cols must be between 1 and 18278.\n");
        return 1;

    }

    init_sheet(rows, cols);

    print_sheet();

    char input[256]; //keeps running till user types q

    while (1)
    {
        printf("[0.0] (ok) >");
        fflush(stdout);

        if(fgets(input, sizeof(input), stdin) == NULL)
        {
            break; //stops at more than one line of input
        }

        input[strcspn(input, "\n")] = 0; //removes /n character

        if (strcmp(input, "q") == 0)
        {
            break; //stops at q
        }

        else if (strcmp(input, "w") == 0)
        {
            view_row -= 10;
            if (view_row < 0) view_row = 0; //prevents going above top row
            print_sheet();

        }

        else if (strcmp(input, "s") == 0)
        {
            view_row += 10;
            if (view_row > num_rows) view_row = num_rows - 1; //prevents from scrolling too far down
            print_sheet();

        }

        else if (strcmp(input, "a") == 0)
        {
            view_col -= 10;
            if (view_col < 0) view_col = 0; //prevents going left of column A
            print_sheet();

        }

        else if (strcmp(input, "d") == 0)
        {
            view_col += 10;
            if (view_col > num_cols) view_col = num_cols - 1; //prevents going right of last column
            print_sheet();

        }

        else
        {
            int result = parse_input(input);
            if (result == 1)
            {
                print_sheet();
                printf("[0.0] (unrecognized cmd) >");
                fflush(stdout);
                fgets(input, sizeof(input), stdin);
            }
        }


    }


    return 0;
}