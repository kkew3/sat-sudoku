import math
import argparse
import contextlib
import time

import pycosat
import numpy as np

import enc
import encc


def disp_board(board, outfile):
    for i in range(board.shape[0]):
        for j in range(board.shape[1]):
            if board[i, j] == 0:
                outfile.write('  ')
            else:
                outfile.write(str(board[i, j]).rjust(2))
            outfile.write(' ')
        outfile.write('\n')


def validate_board(board):
    n = board.shape[0]
    nsq = int(math.sqrt(n))
    assert math.isclose(nsq, math.sqrt(n))
    for i in range(n):
        assert np.unique(board[i]).size == n
        assert np.unique(board[:, i]).size == n
    for i in range(0, n, nsq):
        for j in range(0, n, nsq):
            assert np.unique(np.ravel(board[i:i + nsq, j:j + nsq])).size == n


@contextlib.contextmanager
def stdout_timer():
    begin = time.perf_counter()
    yield
    end = time.perf_counter()
    print('Consumed {} sec'.format(end - begin))


def make_parser():
    parser = argparse.ArgumentParser(
            description='Solve sudoku and write result to file.')
    parser.add_argument('infile', metavar='FILE')
    parser.add_argument('outfile', metavar='OUTFILE')
    return parser


def main_enc():
    args = make_parser().parse_args()

    print('Loading board ...')
    with stdout_timer():
        board = np.loadtxt(args.infile, dtype=int, delimiter=',')

    print('Encoding board ...')
    with stdout_timer():
        xencoder = enc.ExtendedEncoder(board)
        clauses = xencoder.encode()
        mvars = enc.VariableManager()
        cclauses = list(mvars.convert(clauses))

    print('Start solving ...')
    with stdout_timer():
        solution = pycosat.solve(cclauses)

    assert not isinstance(solution, str), solution
    for lit in mvars.back(solution):
        x = lit.x - 1
        y = lit.y - 1
        assert board[x, y] == 0 or board[x, y] == lit.v, lit
        board[x, y] = lit.v
    validate_board(board)
    np.savetxt(args.outfile, board, fmt='%d', delimiter=',')


def main_encc():
    args = make_parser().parse_args()

    print('Loading board ...')
    with stdout_timer():
        board = np.loadtxt(args.infile, dtype=int, delimiter=',')

    print('Encoding board ...')
    with stdout_timer():
        cclauses = encc.extended_encode(board)

    print('Start solving ...')
    with stdout_timer():
        solution = pycosat.solve(cclauses)

    assert not isinstance(solution, str), solution
    for lit in encc.interpret_solution(board.shape[0], solution):
        x = lit.x - 1
        y = lit.y - 1
        assert board[x, y] == 0 or board[x, y] == lit.v, lit
        board[x, y] = lit.v
    validate_board(board)
    np.savetxt(args.outfile, board, fmt='%d', delimiter=',')


if __name__ == '__main__':
    main_encc()
