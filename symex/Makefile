all:
	clang -I ~/klee_src/include -c -emit-llvm -g sudoku-klee.c
	klee --libc=uclibc -posix-runtime -emit-all-errors sudoku-klee.bc