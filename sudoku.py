#!/usr/bin/env python3

def cross(A, B):
    return [(a + b) for a in A for b in B]

class Sudoku:
    @staticmethod
    def parse_grid(puzzle):
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

        puzzle = 'A1A2A3A4...' and every element holds a value of '123456789.'
        where the dot represents an empty cell.
        """

        s = Sudoku()

        if any(c not in "123456789." for c in puzzle) or len(puzzle) != 81:
            raise Exception("got invalid puzzle format")

        elements = cross("ABCDEFGHI", "123456789")
        s.values = {e: v for e,v in zip(elements, puzzle)}
        return s

    def __init__(self, values=dict()):
        # mapping cells -> "123456789." where the dot represents an empty cell
        # cells = cross product of "ABCDEFGHI" and "123456789"
        self.values = values

        # we define some additional informations that may be used by a solving function:
        rows, cols = "ABCDEFGHI", "123456789"
        self.elements = cross(rows, cols)

        self.unitlist = []
        self.unitlist += [cross(rows, c) for c in cols]
        self.unitlist += [cross(r, cols) for r in rows]
        self.unitlist += [cross(rs, cs) for rs in ["ABC", "DEF", "GHI"] for cs in ["123", "456", "789"]]

        self.units = {e: [u for u in self.unitlist if e in u] for e in self.elements}

    def is_solved(self):
        # assure that every cell holds a single value between 1 and 9:
        if not all(k in "123456789" for k in self.values.values()):
            return False

        # assure that every cell of every unit is unique in the proper unit:
        unitsolved = lambda u: set([self.values[e] for e in u]) == set("123456789")
        return all(unitsolved(u) for u in self.unitlist)

    def __str__(self):
        lines, elements = [], cross("ABCDEFGHI", "123456789")

        for index_row, row in enumerate("ABCDEFGHI", 1):
            line = ''

            for index_col, col in enumerate("123456789", 1):
                line += ' ' + self.values[row + col]

                if index_col == 3 or index_col == 6:
                    line += ' |'

            lines.append(line)

            if index_row == 3 or index_row == 6:
                lines.append('–––––––+–––––––+–––––––')

        return '\n'.join(lines + [''])

def Z3Solving(sudoku):
    from z3 import Solver, Int, Or, Distinct, sat

    elements = cross("ABCDEFGHI", "123456789")
    symbols = {e: Int(e) for e in elements}

    # first we build a solver with the general constraints for sudoku puzzles:
    s = Solver()

    # assure that every cell holds a value of [1,9]
    for symbol in symbols.values():
        s.add(Or([symbol == i for i in range(1, 10)]))

    # assure that every row covers every value:
    for row in "ABCDEFGHI":
        s.add(Distinct([symbols[row + col] for col in "123456789"]))

    # assure that every column covers every value:
    for col in "123456789":
        s.add(Distinct([symbols[row + col] for row in "ABCDEFGHI"]))

    # assure that every block covers every value:
    for i in range(3):
        for j in range(3):
            s.add(Distinct([symbols["ABCDEFGHI"[m + i * 3] + "123456789"[n + j * 3]] for m in range(3) for n in range(3)]))

    # now we put the assumptions of the given puzzle into the solver:
    for elem, value in sudoku.values.items():
        if value in "123456789":
            s.add(symbols[elem] == value)

    if not s.check() == sat:
        raise Exception("unsolvable")

    model = s.model()
    values = {e: model.evaluate(s).as_string() for e, s in symbols.items()}
    return Sudoku(values)

def main(puzzle):
    print("[+] processing puzzle:", puzzle)

    s = Sudoku.parse_grid(puzzle)

    print("[+] pretty print of given puzzle")
    print(s)

    print("[+] trying to solve it with z3")
    s_solved = Z3Solving(s)
    print("[+] it is solved:", s_solved.is_solved())
    print(s_solved)

if __name__ == "__main__":
    from sys import argv

    if len(argv) != 2:
        print("[!] we are using some test puzzle due to missing argument")
        puzzle = "4.....8.5.3..........7......2.....6.....8.4......1.......6.3.7.5..2.....1.4......"
        main(puzzle)
    else:
        main(argv[1])

