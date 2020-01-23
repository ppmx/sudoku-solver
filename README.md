# Solving Sudoku Puzzles By Algorithms

Solving a puzzle is pure fun. So let's do it with different approaches from computer science.

```
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
```


## Native SMT solving

A solved sudoku puzzle can be expressed mathematically, very close to the three rules of sudoku:

1. Each element of a row must be unique in that row
2. Each element of a column must be unique in that column
3. Each element of a subgrid must be unique in that subgrid

So let's use a SMT solver (Z3 for example) to express a solved sudoku puzzle and to solve it actually:

```
$ time python3 sudoku-z3.py 
[!] we are using some test puzzle due to missing argument
[+] parsing puzzle: 4.....8.5.3..........7......2.....6.....8.4......1.......6.3.7.5..2.....1.4......
[+] start solving using Z3
[+] solved: True
417369825632158947958724316825437169791586432346912758289643571573291684164875293
python3 sudoku-z3.py  0.59s user 0.02s system 99% cpu 0.612 total
```


## Fuzzing

Today, everything gets fuzzed. Everything!

Fuzzing a sudoku puzzle is probably not the best idea, because it is a very special corner case to reach exactly that one puzzle (probably there is only one solution) that is the solved version of the input. So this is more... academic! But let's do it: fuzz the sudoku solver using [AFL](http://lcamtuf.coredump.cx/afl/).

AFL needs an initial input set, so we generate an input string that has the correct length but that is probably not a valid sudoku string and especially not the solved version.

```python3
>>> from random import choice
>>> grid = "1...6..5...3...1...6.3..8......5796..7.....8..3961......1..4.2...2...6...8..9...4"
>>> ''.join(choice("123456789") if c == '.' else c for c in grid)
'166162453263324123567375843599857964179194883139613972671534325992492661389198924'
```

After modifying the sudoku solver (raise `SIGSEGV` if solved) AFL is invoked:

```
$ mkdir -p fuzzing/{input,output}
$ echo "teststring" > fuzzing/input/random_grid.txt
$ afl-gcc -o fuzzing/target sudoku-afl.c
$ cd fuzzing; afl-fuzz -i input -o output target
```

After 36 minutes i stopped the execution:

```
                       american fuzzy lop 2.56b (target)

┌─ process timing ─────────────────────────────────────┬─ overall results ─────┐
│        run time : 0 days, 0 hrs, 36 min, 32 sec      │  cycles done : 147    │
│   last new path : 0 days, 0 hrs, 27 min, 32 sec      │  total paths : 102    │
│ last uniq crash : none seen yet                      │ uniq crashes : 0      │
│  last uniq hang : none seen yet                      │   uniq hangs : 0      │
├─ cycle progress ────────────────────┬─ map coverage ─┴───────────────────────┤
│  now processing : 86 (84.31%)       │    map density : 0.23% / 0.29%         │
│ paths timed out : 0 (0.00%)         │ count coverage : 2.15 bits/tuple       │
├─ stage progress ────────────────────┼─ findings in depth ────────────────────┤
│  now trying : splice 2              │ favored paths : 36 (35.29%)            │
│ stage execs : 78/96 (81.25%)        │  new edges on : 37 (36.27%)            │
│ total execs : 8.06M                 │ total crashes : 0 (0 unique)           │
│  exec speed : 3902/sec              │  total tmouts : 0 (0 unique)           │
├─ fuzzing strategy yields ───────────┴───────────────┬─ path geometry ────────┤
│   bit flips : 85/66.3k, 9/66.2k, 2/66.0k            │    levels : 18         │
│  byte flips : 0/8286, 0/8184, 0/7980                │   pending : 0          │
│ arithmetics : 5/464k, 0/2575, 0/0                   │  pend fav : 0          │
│  known ints : 0/53.7k, 0/229k, 0/351k               │ own finds : 101        │
│  dictionary : 0/0, 0/0, 0/0                         │  imported : n/a        │
│       havoc : 0/2.60M, 0/4.14M                      │ stability : 100.00%    │
│        trim : 0.93%/3041, 0.00%                     ├────────────────────────┘
└─────────────────────────────────────────────────────┘          [cpu000: 71%]
```

Statistics can be found in `fuzzing/output/fuzzer_stats`.


## Symbolic Execution

A symbolic execution engine evaluates a program in order to find every possible execution path including the condition to reach that path.

Let's solve it using [KLEE](http://klee.github.io).
