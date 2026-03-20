#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"
#include "display.h"


void parse_input(char *input)
{
char col_name[4];
int col_len=0;
int i=0;

while (isalpha(input[i])){
    col_name[col_len]=input[i];
    col_len++;
    i++;
}
col_name[col_len]='\0';


if (col_len==0){
    printf("invalid input\n");
    return;
}
char row_name[5];
int row_len = 0;

while (isdigit(input[i]))
{
    row_name[row_len] = input[i];
    row_len++;
    i++;
}
row_name[row_len] = '\0';

if (row_len == 0)
{
    printf("invalid input\n");
    return;
}
while (input[i] == ' ')
{
    i++;
}

if (input[i] != '=')
{
    printf("invalid input\n");
    return;
}
i++;

while (input[i] == ' ')
{
    i++;
}

if (input[i] == '\0')
{
    printf("invalid input\n");
    return;
}

int value = atoi(&input[i]);

int col = col_name_to_index(col_name);
int row = row_name_to_index(row_name);

if (row < 0 || row >= num_rows || col < 0 || col >= num_cols)
{
    printf("invalid cell\n");
    return;
}

sheet[row][col].value = value;
sheet[row][col].has_error = 0;

print_sheet();

}
