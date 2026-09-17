# Dynamic Validation of Sudoku

A multi-threaded C++ project that validates Sudoku grids using different synchronization mechanisms and compares their behavior.

## Overview

This project checks whether a given Sudoku board is valid by validating:

- each row,
- each column, and
- each sub-grid (3x3 for standard Sudoku).

The implementation uses multiple synchronization techniques to coordinate worker threads while they validate independent tasks:

- Test-and-Set (TAS)
- Compare-and-Swap (CAS)
- Bounded CAS
- Sequential validation (baseline)
- TAS with early termination

The program records execution metrics such as entry/exit times and writes the results to output files.

## Project Structure

- `Assgn2Src1_CO23BTECH11003.cpp` — Sudoku validation with TAS synchronization
- `Assgn2Src2_CO23BTECH11003.cpp` — Sudoku validation with CAS synchronization
- `Assgn2Src3_CO23BTECH11003.cpp` — Sudoku validation with bounded CAS synchronization
- `Assgn2Src4_CO23BTECH11003.cpp` — Sequential validation without threading
- `Assgn2Src5_CO23BTECH11003.cpp` — TAS-based validation with early termination
- `Makefile` — builds and runs all implementations
- `Assgn2Report_CO23BTECH11003.pdf` — assignment report

## How it works

The program reads a Sudoku board from `inp.txt` and divides validation into independent tasks:

- validate each row,
- validate each column,
- validate each sub-grid.

Worker threads acquire a synchronization lock before accessing the shared task counter, then process one validation task at a time. Each implementation uses a different synchronization primitive to compare efficiency and correctness.

## Input format

The program expects the following values in `inp.txt`:

```text
k n taskInc
row1col1 row1col2 ... row1coln
row2col1 row2col2 ... row2coln
...
rowncol1 rowncol2 ... rowncoln
```

Where:

- `k` = number of threads
- `n` = size of the Sudoku grid (typically 9 for standard Sudoku)
- `taskInc` = task increment value used by the shared counter
- The remaining `n x n` integers represent the Sudoku board values

Example:

```text
4 9 1
5 3 0 0 7 0 0 0 0
6 0 0 1 9 5 0 0 0
0 9 8 0 0 0 0 6 0
8 0 0 0 6 0 0 0 3
4 0 0 8 0 3 0 0 1
7 0 0 0 2 0 0 0 6
0 6 0 0 0 0 2 8 0
0 0 0 4 1 9 0 0 5
0 0 0 0 8 0 0 7 9
```

## Build and run

From the project root, run:

```bash
make
```

This will compile and execute all variants and generate output files such as:

- `outputtas.txt`
- `outputcas.txt`
- `outputboundcas.txt`
- `outputseq.txt`
- `outputtasearly.txt`

## Output

The program prints whether the Sudoku is valid or invalid and also logs timing metrics including:

- total time taken,
- average lock-entry time,
- average lock-exit time,
- worst-case lock-entry time,
- worst-case lock-exit time.

## Notes

- This is a coursework-style implementation focused on concurrency and synchronization behavior.
- It is intended to compare the practical cost of different synchronization primitives under parallel validation tasks.

## License

This project is provided for academic and learning purposes.
