/* 
 * This program serves an unsolved sudoku puzzle and waits for the input of
 * a solved version of that puzzle. This example is built for using KLEE as
 * solving backend. KLEE is a symbolic execution engine that traverses
 * through a program while discovering every possible execution path.
 *
 * author: https://github.com/ppmx
 *
 * See also: http://klee.github.io/
 *
 * Compile and run:
 * ================
 * $ clang -I ~/klee/include -c -emit-llvm -g sudoku-klee.c
 * $ klee --libc=uclibc -posix-runtime sudoku-klee.bc -sym-stdin 82
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNASSIGNED 0

void sudoku_pretty_print(int f[9][9])
{
	// print field as one-liner:
	printf("[+] Puzzle: ");
	for (size_t i = 0; i < 9; i++)
		for (size_t j = 0; j < 9; j++)
			f[i][j] == UNASSIGNED ? printf(".") : printf("%d", f[i][j]);
	printf("\n");

	// this is the "pretty" printing part:
	for (size_t row = 0; row < 9; row++) {
		if (row % 3 == 0)
			printf("+–––––––––+–––––––––+–––––––––+\n");

		for (size_t col = 0; col < 9; col++) {
			if (col % 3 == 0)
				printf("|");
			f[row][col] == UNASSIGNED ? printf(" . ") : printf(" %d ", f[row][col]);
		}

		printf("|\n");
	}
	printf("+–––––––––+–––––––––+–––––––––+\n");
}

/* This function checks if a given field fulfills all constraints of a solved
 * sudoku puzzle. It returns 1 if so and 0 otherwise.
 */
int sudoku_is_solved(int f[9][9])
{
	/*
	 * A sudoku puzzle is solved if and only if the following conditions are
	 * fulfilled:
	 *
	 * - Every 3x3 Block contains every number of [1, 9]
	 * - Every row contains every number of [1,9]
	 * - Every column contains every number of [1,9]
	 */

	int numbers[9];

	// Testing horizontal lines:
	for (size_t row = 0; row < 9; row++) {
		memset(numbers, 0, sizeof(int) * 9);

		for (size_t col = 0; col < 9; col++) {
			// If there is no valid number in the field:
			if (!(f[row][col] >= 1 && f[row][col] <= 9))
				return 0;

			// If we still counted some value:
			if (numbers[f[row][col] - 1] != 0)
				return 0;

			numbers[f[row][col] - 1] += 1;
		}
	}

	// Testing vertical columns:
	for (size_t col = 0; col < 9; col++) {
		memset(numbers, 0, sizeof(int) * 9);

		for (size_t row = 0; row < 9; row++) {
			// If there is no valid number in the field:
			if (!(f[row][col] >= 1 && f[row][col] <= 9))
				return 0;

			// If we still counted some value:
			if (numbers[f[row][col] - 1] != 0)
				return 0;

			numbers[f[row][col] - 1] += 1;
		}
	}

	// Testing every block:
	for (size_t block_row = 0; block_row < 3; block_row++) {
		for (size_t block_col = 0; block_col < 3; block_col++) {
			memset(numbers, 0, sizeof(int) * 9);

			for (size_t row = 0; row < 3; row++) {
				for (size_t col = 0; col < 3; col++) {
					size_t r = block_row * 3 + row;
					size_t c = block_col * 3 + col;

					// If there is no valid number in the field:
					if (!(f[r][c] >= 1 && f[r][c] <= 9))
						return 0;

					// If we still counted some value:
					if (numbers[f[r][c] - 1] != 0)
						return 0;

					numbers[f[r][c] - 1] += 1;	
				}
			}
		}
	}

	return 1;
}

/* This function parses a given field and writes it to the field
 * 'structure'.
 */
int parse_field(int f[9][9], char buffer[81])
{
	if (strlen(buffer) != 81)
		return -1;

	// apply submission to the field:
	for (size_t row = 0; row < 9; row++) {
		for (size_t col = 0; col < 9; col++) {
			int s = *buffer++ - '0';

			// only accept values between 1 and 9 and the empty cell
			if (!(s >= 1 && s <= 9) && s != UNASSIGNED)
				return -1;

			// The current value of the submission is not okay if the proper
			// position in the field is not empty and the submitted value
			// differs from the given value
			if (f[row][col] != UNASSIGNED && f[row][col] != s)
				return -1;

			f[row][col] = s;
		}
	}

	return 0;
}

/* This function reads a field from stdin by reading 81 characters and
 * interprets every character as number.
 *
 * returns -1 on error and 0 on success.
 */
int read_solution(int f[9][9])
{
	char buffer[82];

	printf("Please submit your solution: ");
	fgets(buffer, 82, stdin);
	return parse_field(f, buffer);
}

int main(int argc, char **argv)
{
	int field[9][9];

	// empty the field:
	for (size_t i = 0; i < 9; i++)
		for (size_t j = 0; j < 9; j++)
			field[i][j] = 0;

	// char puzzle[] = "004083700085702090700000108050804003000395000600107080406000007090208360007430500";
	char puzzle[] = "100060050003000100060300800000057960070000080039610000001004020002000600080090004";

	if (parse_field(field, puzzle) != 0) {
		fprintf(stderr, "[!] error reading puzzle\n");
		return -1;
	}

	sudoku_pretty_print(field);

	// note: here could be your autosolver code instead of reading from stdin... :-)
	if (read_solution(field) != 0) {
		fprintf(stderr, "[!] error reading your solution\n");
		return -1;
	}

	sudoku_pretty_print(field);

	if (sudoku_is_solved(field))
		printf("[+] it's solved. congratulations.\n");
	else
		printf("[!] no, it's not solved.\n");

	return 0;
}

