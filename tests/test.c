#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int passed = 0;
int failed = 0;

// Runs ./sheet with the given input and returns the output
char *run_sheet(int rows, int cols, char *input) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "echo '%s' | ./sheet %d %d 2>&1", input, rows, cols);

    FILE *fp = popen(cmd, "r");
    if (!fp) return NULL;

    static char output[8192];
    output[0] = '\0';
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        strcat(output, line);
    }
    pclose(fp);
    return output;
}

// Checks if the output contains the expected string
void assert_contains(char *test_name, char *output, char *expected) {
    if (strstr(output, expected)) {
        printf("PASS: %s\n", test_name);
        passed++;
    } else {
        printf("FAIL: %s\n", test_name);
        printf("  Expected to find: %s\n", expected);
        failed++;
    }
}

// Checks if the output does NOT contain a string
void assert_not_contains(char *test_name, char *output, char *not_expected) {
    if (!strstr(output, not_expected)) {
        printf("PASS: %s\n", test_name);
        passed++;
    } else {
        printf("FAIL: %s\n", test_name);
        printf("  Did not expect: %s\n", not_expected);
        failed++;
    }
}

int main() {
    char *output;

output = run_sheet(2, 2, "A1=2\\nq");
assert_contains("set constant A1=2", output, "2     0");


output = run_sheet(2, 2, "A1=5\\nB1=A1\\nq");
assert_contains("cell reference B1=A1", output, "5     5");

output = run_sheet(2, 2, "A1=10\\nB1=A1+3\\nq");
assert_contains("arithmetic A1+3", output, "10    13");

output = run_sheet(2, 2, "A1=1/0\\nq");
assert_contains("div by zero", output, "ERR");

output = run_sheet(2, 2, "hello\\nq");
assert_contains("unrecognized cmd", output, "unrecognized cmd");

output = run_sheet(2, 2, "A1=2\\nB1=A1+1\\nA1=5\\nq");
assert_contains("recalculation B1 updates", output, "5     6");

output = run_sheet(2, 2, "A1=3\\nB1=7\\nA2=SUM(A1:B1)\\nq");
assert_contains("SUM function", output, "10");

output = run_sheet(2, 2, "A1=MAX(B1:A1)\\nq");
assert_contains("invalid range", output, "Invalid");


    printf("\n%d passed, %d failed\n", passed, failed);
    return failed > 0 ? 1 : 0;
}
