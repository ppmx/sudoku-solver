# Solving Sudoku Puzzles By Algorithms

## Description
This is a sudoku solving engine written in Python and using an SMT Solver (Z3)
rather than the approach of Peter Norvig. I would like to mention that his
blog is really worth reading and I snitched some good ideas from him.

Therefore take a look at [his blog post of solving sudoku puzzles](https://norvig.com/sudoku.html).

## Example

```
$ time ./sudoku.py 
[!] we are using some test puzzle due to missing argument
[+] processing puzzle: 4.....8.5.3..........7......2.....6.....8.4......1.......6.3.7.5..2.....1.4......
[+] pretty print of given puzzle
 4 . . | . . . | 8 . 5
 . 3 . | . . . | . . .
 . . . | 7 . . | . . .
–––––––+–––––––+–––––––
 . 2 . | . . . | . 6 .
 . . . | . 8 . | 4 . .
 . . . | . 1 . | . . .
–––––––+–––––––+–––––––
 . . . | 6 . 3 | . 7 .
 5 . . | 2 . . | . . .
 1 . 4 | . . . | . . .

[+] trying to solve it with z3
[+] it is solved: True
 4 1 7 | 3 6 9 | 8 2 5
 6 3 2 | 1 5 8 | 9 4 7
 9 5 8 | 7 2 4 | 3 1 6
–––––––+–––––––+–––––––
 8 2 5 | 4 3 7 | 1 6 9
 7 9 1 | 5 8 6 | 4 3 2
 3 4 6 | 9 1 2 | 7 5 8
–––––––+–––––––+–––––––
 2 8 9 | 6 4 3 | 5 7 1
 5 7 3 | 2 9 1 | 6 8 4
 1 6 4 | 8 7 5 | 2 9 3

./sudoku.py  0.56s user 0.01s system 99% cpu 0.577 total
```
