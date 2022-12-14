import math
import os
import argparse

import pycosat
import numpy as np

import enc


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


def make_parser():
    parser = argparse.ArgumentParser(
            description='Solve sudoku and write result to file.')
    parser.add_argument('infile', metavar='FILE')
    parser.add_argument('outfile', metavar='OUTFILE')
    return parser


def main():
    args = make_parser().parse_args()
    print('Loading board ...')
    board = np.loadtxt(args.infile, dtype=int, delimiter=',')
    print('Encoding board ...')
    xencoder = enc.ExtendedEncoder(board)
    clauses = xencoder.encode()
    mvars = enc.VariableManager()
    cclauses = list(mvars.convert(clauses))
    print('Start solving ...')
    solution = pycosat.solve(cclauses)
    assert not isinstance(solution, str), solution
    for lit in mvars.back(solution):
        x = lit.x - 1
        y = lit.y - 1
        assert lit.v > 0
        assert board[x, y] == 0 or board[x, y] == lit.v, lit
        board[x, y] = lit.v
    validate_board(board)
    np.savetxt(args.outfile, board, fmt='%d', delimiter=',')


if __name__ == '__main__':
    main()
