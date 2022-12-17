# SAT Sudoku solver

This is a Sudoku solver based on [PicoSAT](http://fmv.jku.at/picosat/), using the python wrapper [pycosat](https://github.com/ContinuumIO/pycosat.git).
See more about SAT [here](https://en.wikipedia.org/wiki/Boolean_satisfiability_problem).

Four constraints are applied:

- Each cell is filled with one and only one digit from 1 to 9.
- Each row contains all nine different digits.
- Each column contains all nine different digits.
- Each sub-box contains all nine different digits.


## Install

```bash
python3 -m virtualenv rt
. rt/bin/activate
pip install -r requirements.txt
```

### To use Python implementation

Change the `main_encc()` at the last line of `sudoku.py` to `main_enc()`.

### To use C++ implementation

Comment out zero or more macro definitions in `define_macros` keyword argument in `setup.py` to redefine macros in C++ implementation.
Then

```
python setup.py build_ext -if
```

# How to use

```bash
python sudoku.py SUDOKU_BOARD_FILE OUTPUT_FILE
```

An example of `SUDOKU_BOARD_FILE` is `example-boards/board.txt`.

## Related project

Another SAT-based sudoku solver can be found in [here](https://github.com/ContinuumIO/pycosat/blob/master/examples/sudoku.py), which seems to use the *minimal encoding* as found in this repo.

## References

- [*Sudoku as a SAT Problem*](http://anytime.cs.umass.edu/aimath06/proceedings/P34.pdf)
- [*A Sudoku-Solver for Large Puzzles using SAT*](https://easychair.org/publications/open/VF3m): note on using the strategy of this paper can be found in source code `c_enc.cpp`.
