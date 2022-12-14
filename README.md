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
cd pycosat-0.6.3 && python setup.py install
pip install -r requirements.txt
```

# How to use

```bash
python sudoku.py SUDOKU_BOARD_FILE OUTPUT_FILE
```

An example of `SUDOKU_BOARD_FILE` is `example-boards/board.txt`.

## Related project

Another SAT-based sudoku solver can be found in [here](https://github.com/ContinuumIO/pycosat/blob/master/examples/sudoku.py), which may differ from the method described in this repository (I didn't read into it).

## References

- [*A Sudoku-Solver for Large Puzzles using SAT*](https://easychair.org/publications/open/VF3m)
- [*Sudoku as a SAT Problem*](http://anytime.cs.umass.edu/aimath06/proceedings/P34.pdf)
