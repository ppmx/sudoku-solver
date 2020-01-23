/* This program provides an unsolved sudoku puzzle and waits for the input of
 * a solved version of that puzzle.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include "klee/klee.h"

#define UNASSIGNED 0

void sudoku_pretty_print(int grid[9][9])
{
	// print field as one-liner:
	printf("[+] Sudoku grid: ");

	for (size_t i = 0; i < 9; i++)
		for (size_t j = 0; j < 9; j++)
			grid[i][j] == UNASSIGNED ? printf(".") : printf("%d", grid[i][j]);

	printf("\n");

	// this is the "pretty" printing part:
	for (size_t row = 0; row < 9; row++) {
		if (row % 3 == 0)
			printf("+–––––––––+–––––––––+–––––––––+\n");

		for (size_t col = 0; col < 9; col++) {
			if (col % 3 == 0)
				printf("|");
			grid[row][col] == UNASSIGNED ? printf(" . ") : printf(" %d ", grid[row][col]);
		}

		printf("|\n");
	}

	printf("+–––––––––+–––––––––+–––––––––+\n");
}

/* This function checks if the given sudoku grid fulfills all constraints
 * of a solved sudoku puzzle.
 *
 * Returns 1 i the grid is solved and 0 otherwise.
 */
int sudoku_is_solved(int grid[9][9])
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
			if (!(grid[row][col] >= 1 && grid[row][col] <= 9))
				return 0;

			// If we still counted some value:
			if (numbers[grid[row][col] - 1] != 0)
				return 0;

			numbers[grid[row][col] - 1] += 1;
		}
	}

	// Testing vertical columns:
	for (size_t col = 0; col < 9; col++) {
		memset(numbers, 0, sizeof(int) * 9);

		for (size_t row = 0; row < 9; row++) {
			// If there is no valid number in the field:
			if (!(grid[row][col] >= 1 && grid[row][col] <= 9))
				return 0;

			// If we still counted some value:
			if (numbers[grid[row][col] - 1] != 0)
				return 0;

			numbers[grid[row][col] - 1] += 1;
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
					if (!(grid[r][c] >= 1 && grid[r][c] <= 9))
						return 0;

					// If we still counted some value:
					if (numbers[grid[r][c] - 1] != 0)
						return 0;

					numbers[grid[r][c] - 1] += 1;	
				}
			}
		}
	}

	return 1;
}

/* This function receives a grid representation stored in buffer, where
 * each element is a number for one position in the grid.
 *
 * Returns 0 if the grid representation was valid and -1 otherwise.
 */
int parse_field(int grid[9][9], char buffer[81])
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
			if (grid[row][col] != UNASSIGNED && grid[row][col] != s)
				return -1;

			grid[row][col] = s;
		}
	}

	return 0;
}

/* This function prepares symbolic space for a representation
 * of a sudoku grid.
 *
 * Returns 0.
 */
int read_solution(int grid[9][9])
{
	int buffer[81];
	int pos = 0;

	klee_make_symbolic(buffer, sizeof(buffer), "buffer");

	for (size_t row = 0; row < 9; row++) {
		for (size_t col = 0; col < 9; col++) {
			pos = row * 9 + col;

			if (grid[row][col] == UNASSIGNED) {
				klee_assume(buffer[pos] >= 1);
				klee_assume(buffer[pos] <= 9);
			} else {
				buffer[pos] = grid[row][col];
			}

			grid[row][col] = buffer[pos];
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	int grid[9][9];

	// empty the field:
	for (size_t i = 0; i < 9; i++)
		for (size_t j = 0; j < 9; j++)
			grid[i][j] = 0;

	// char game[] = "004083700085702090700000108050804003000395000600107080406000007090208360007430500";
	char game[] = "100060050003000100060300800000057960070000080039610000001004020002000600080090004";

	// Read initial game with unfilled cells:
	if (parse_field(grid, game) != 0) {
		printf("[!] error reading game\n");
		return -1;
	}

	// Read symbolic input:
	if (read_solution(grid) != 0) {
		printf("[!] error reading solution\n");
		return -1;
	}

	if (sudoku_is_solved(grid))
		klee_assert(0);

	return 0;
}
