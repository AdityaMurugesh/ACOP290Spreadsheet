#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int passed = 0;
int failed = 0;

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
assert_contains("invalid range", output, "invalid range");

output = run_sheet(3, 3, "A1=5\\nB1=2\\nC1=8\\nA2=MIN(A1:C1)\\nq");
assert_contains("MIN function", output, "2");

output = run_sheet(3, 3, "A1=5\\nB1=2\\nC1=8\\nA2=MAX(A1:C1)\\nq");
assert_contains("MAX function", output, "8");

output = run_sheet(2, 2, "A1=2\\nB1=4\\nA2=AVG(A1:B1)\\nq");
assert_contains("AVG function", output, "3");

output = run_sheet(2, 2, "A1=2\\nB1=4\\nA2=STDEV(A1:B1)\\nq");
assert_contains("STDEV function", output, "1");

output = run_sheet(2, 2, "A1=SLEEP(1)\\nq");
assert_contains("SLEEP returns value", output, "1");

output = run_sheet(2, 2, "A1=A1+1\\nq");
assert_contains("circular dependency", output, "circular dependency");

output = run_sheet(3, 3, "A1=1\\nB1=1/A1\\nC1=B1+1\\nA1=0\\nq");
assert_contains("ERR propagation", output, "ERR");

output = run_sheet(2, 2, "A3=5\\nq");
assert_contains("out of bounds", output, "unrecognized cmd");

output = run_sheet(2, 2, "disable_output\\nA1=5\\nq");
assert_not_contains("disable output hides grid", output, "5");

output = run_sheet(3, 3, "A1=1\\nB1=A1+1\\nC1=B1+1\\nA1=5\\nq");
assert_contains("chained recalculation", output, "7");

output = run_sheet(2, 2, "A1=5/2\\nq");
assert_contains("integer division truncation", output, "2");

output = run_sheet(2, 2, "scroll_to Z999\\nq");
assert_contains("scroll_to out of bounds", output, "unrecognized cmd");

output = run_sheet(3, 3, "A1=1\\nB1=2\\nA2=3\\nB2=4\\nC1=SUM(A1:B2)\\nq");
assert_contains("2D SUM range", output, "10");

output = run_sheet(2, 2, "A1=B1\\nB1=A1\\nq");
assert_contains("two-cell circular dep", output, "circular dependency");

output = run_sheet(3, 3, "A1=B1\\nB1=C1\\nC1=A1\\nq");
assert_contains("three-cell circular dep", output, "circular dependency");

output = run_sheet(2, 2, "A1=2\\nB1=A1+1\\nB1=B1+1\\nA1=10\\nq");
assert_contains("old formula preserved after circular", output, "10    11");

output = run_sheet(2, 2, "A1=A1+1\\nA1=A1+1\\nA1=5\\nq");
assert_contains("recovery after repeated circular", output, "5");

output = run_sheet(2, 2, "A1=SUM(A1:A1)\\nq");
assert_contains("circular dep via range", output, "circular dependency");

output = run_sheet(2, 2, "A1=-5\\nq");
assert_contains("negative constant", output, "-5");

output = run_sheet(2, 2, "A1=3\\nB1=A1-10\\nq");
assert_contains("negative arithmetic result", output, "-7");

output = run_sheet(2, 2, "A1=SLEEP(-1)\\nq");
assert_contains("negative SLEEP", output, "ERR");

output = run_sheet(3, 3, "A1=10\\nB1=1/A1\\nC1=B1+5\\nA1=0\\nq");
assert_contains("ERR cascade through chain", output, "ERR");

output = run_sheet(3, 3, "A1=10\\nB1=1/A1\\nC1=B1+5\\nA1=0\\nA1=2\\nq");
assert_contains("ERR recovery after fix", output, "5");

output = run_sheet(3, 3, "A1=1\\nB1=1/0\\nC1=SUM(A1:B1)\\nq");
assert_contains("ERR in range propagates", output, "ERR");

output = run_sheet(2, 2, "A1=Z99+1\\nq");
assert_contains("out of bounds cell ref in formula", output, "unrecognized cmd");

output = run_sheet(2, 2, "A1=SUM(A1:Z99)\\nq");
assert_contains("out of bounds range", output, "unrecognized cmd");

output = run_sheet(3, 3, "disable_output\\nA1=99\\nenable_output\\nscroll_to A1\\nq");
assert_contains("enable output shows sheet", output, "99");

output = run_sheet(3, 3, "A1=1\\nB1=2\\nscroll_to B1\\nq");
assert_contains("scroll_to valid cell", output, "(ok)");

output = run_sheet(2, 2, "A1=\\nq");
assert_contains("empty formula", output, "unrecognized cmd");

output = run_sheet(2, 2, "A1=2\\nB1=A1+1\\nB1=99\\nA1=10\\nq");
assert_contains("overwrite formula with constant", output, "99");

output = run_sheet(2, 2, "A1=7/2\\nq");
assert_contains("division truncation 7/2", output, "3");

output = run_sheet(2, 2, "A1=3\\nB1=A1*4\\nq");
assert_contains("multiplication", output, "12");

output = run_sheet(0, 0, "q");
assert_contains("invalid args zero", output, "Error");

output = run_sheet(2, 2, "A1=0\\nq");
assert_contains("assign zero", output, "0");

output = run_sheet(2, 2, "B1=5\\nA1=B1\\nq");
assert_contains("cell ref to another cell", output, "5     5");

output = run_sheet(2, 2, "A1=2\\nB1=A1+1\\nB1=A1*3\\nq");
assert_contains("overwrite formula with formula", output, "6");

output = run_sheet(2, 2, "A1=999999\\nq");
assert_contains("large constant", output, "999999");

output = run_sheet(2, 2, "A1=5\\nB1=A1-5\\nq");
assert_contains("subtraction to zero", output, "0");

output = run_sheet(2, 2, "A1=7\\nB1=SUM(A1:A1)\\nq");
assert_contains("SUM single cell", output, "7");

output = run_sheet(2, 2, "A1=3\\nB1=3\\nA2=MAX(A1:B1)\\nq");
assert_contains("MAX all same values", output, "3");

output = run_sheet(2, 2, "A1=3\\nB1=3\\nA2=MIN(A1:B1)\\nq");
assert_contains("MIN all same values", output, "3");

output = run_sheet(2, 2, "A1=1\\nB1=2\\nA2=AVG(A1:B1)\\nq");
assert_contains("AVG truncates to int", output, "1");

output = run_sheet(2, 2, "A1=0\\nB1=5/A1\\nq");
assert_contains("div by zero via cell ref", output, "ERR");

output = run_sheet(3, 3, "A1=1\\nB1=A1+1\\nC1=A1+2\\nA1=10\\nq");
assert_contains("multiple deps on same source B1", output, "11");

output = run_sheet(3, 3, "A1=1\\nB1=A1+1\\nC1=A1+2\\nA1=10\\nq");
assert_contains("multiple deps on same source C1", output, "12");

output = run_sheet(2, 2, "w\\nq");
assert_contains("scroll up at top", output, "(ok)");

output = run_sheet(2, 2, "a\\nq");
assert_contains("scroll left at left", output, "(ok)");

output = run_sheet(2, 2, "   \\nq");
assert_contains("spaces only input", output, "unrecognized cmd");

output = run_sheet(2, 2, "A1 = 2 + 3\\nq");
assert_contains("formula with spaces", output, "5");

output = run_sheet(2, 2, "A1=SLEEP(0)\\nq");
assert_contains("SLEEP zero", output, "0");

output = run_sheet(2, 2, "A1=1/0\\nA1=5\\nq");
assert_contains("ERR overwritten with constant", output, "5");

    printf("\n%d passed, %d failed\n", passed, failed);
    return failed > 0 ? 1 : 0;
}
