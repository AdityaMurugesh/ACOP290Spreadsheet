CC = gcc
CFLAGS = -Isrc/include -lm

SRCS = src/main.c src/sheet.c src/parser.c src/evaluator.c src/display.c src/graph.c

sheet: $(SRCS)
	$(CC) -o sheet $(SRCS) $(CFLAGS)

test: sheet
	$(CC) -o test_sheet tests/test.c $(CFLAGS)
	./test_sheet

report:
	pdflatex report.tex

clean:
	rm -f sheet test_sheet
