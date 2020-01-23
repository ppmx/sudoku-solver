#!/usr/bin/env python3

"""
A1 A2 A3 | A4 A5 A6 | A7 A8 A9
B1 B2 B3 | B4 B5 B6 | B7 B8 B9
C1 C2 C3 | C4 C5 C6 | C7 C8 C9
–––––––––+––––––––––+–––––––––
D1 D2 D3 | D4 D5 D6 | D7 D8 D9
E1 E2 E3 | E4 E5 E6 | E7 E8 E9
F1 F2 F3 | F4 F5 F6 | F7 F8 F9
–––––––––+––––––––––+–––––––––
G1 G2 G3 | G4 G5 G6 | G7 G8 G9
H1 H2 H3 | H4 H5 H6 | H7 H8 H9
I1 I2 I3 | I4 I5 I6 | I7 I8 I9
"""

def cross(iter_a, iter_b):
    """ Returns a list of all elements of the cross product of iter_a and iter_b """

    return [(x + y) for x in iter_a for y in iter_b]

class Sudoku:
    """ Sudoku grid representation.

    Provides a routine to check if current grid is solved and to print it more
    pretty.
    """

    @staticmethod
    def parse(grid):
        """ ... """

        if any(c not in "123456789." for c in grid) or len(grid) != 81:
            raise Exception("invalid grid")

        positions = cross("ABCDEFGHI", "123456789")
        mapping = dict(zip(positions, grid))
        return Sudoku(mapping)

    def __init__(self, mapping=None):
        rows, cols = "ABCDEFGHI", "123456789"

        self.positions = cross(rows, cols)

        # self.grid maps a cell (e.g. 'A1') to its value (e.g. int('9')).
        # A dot ('.') represents a cell that is not yet filled.
        self.grid = {pos: '.' for pos in self.positions}

        if mapping:
            print(mapping)
            self.grid.update(mapping)

        # self.units stores a list of lists with cells belonging together:
        self.units = []
        self.units += [cross(rows, col) for col in cols]
        self.units += [cross(row, cols) for row in rows]
        self.units += [cross(r, c) for r in ["ABC", "DEF", "GHI"] for c in ["123", "456", "789"]]

    def position_occupied(self, position):
        """ Returns True if and only if the cell is filled. """

        return self.grid[position] != '.'

    def get_value(self, position):
        """ Returns the value in a position. Returns '.' if unfilled. """

        return self.grid[position]

    def set_value(self, position, value):
        """ Sets a value in the grid to a specific position """

        self.grid[position] = value

    def __str__(self):
        """ Returns simple string representation of the current grid. Can be used
        as input for parse().
        """

        return ''.join([self.grid[pos] for pos in self.positions])

    def pretty_print(self):
        """ Returns a pretty representation of the grid as string. """

        lines = []

        for index_row, row in enumerate("ABCDEFGHI"):
            if index_row % 3 == 0:
                lines.append("+–––––––––+–––––––––+–––––––––+")

            line = ''

            for index_col, col in enumerate("123456789"):
                line += "{1} {0} ".format(self.grid[row + col], '|' if index_col % 3 == 0 else '')

            lines.append(line + '|')

        lines.append("+–––––––––+–––––––––+–––––––––+")
        return '\n'.join(lines)

    def is_solved(self):
        """ Returns true if the current grid is solved and false otherwise. """

        # assert that every cell holds a value the range of 1 to 9:
        if not all([cell in "123456789" for cell in self.grid.values()]):
            return False

        # assert that each unit is solved:
        check_unit = lambda unit: set(self.grid[cell] for cell in unit) == set("123456789")
        return all(check_unit(unit) for unit in self.units)

def main():
    """ Reads a grid from command line and prints it on stdout """

    from sys import argv

    try:
        grid = argv[1]
    except IndexError:
        print("Usage:", argv[0], "<grid>")
        return

    try:
        sudoku = Sudoku.parse(grid)
    except Exception as error:
        print("[!] Error:", error)
        return

    print(sudoku.pretty_print())

if __name__ == "__main__":
    main()
