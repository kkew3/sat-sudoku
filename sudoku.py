import pdb
import json
import sys
import itertools
import argparse
import functools
import logging

import numpy as np

import cnf
import pycosat


B_H = 9    # sudoku board height
B_W = 9    # ...          width
B_SH = 3   # ...          sub-grid height
B_SW = 3   # ...                   width
N = 9      # number of digits to fill


def disp_board(board):
    logger = logging.getLogger(f'{__name__}.{disp_board.__name__}')
    sbuf = [' '.join(map(lambda x: str(x) if x else '_', r)) for r in board]
    logger.info('\n' + '\n'.join(sbuf))


def inv_oh(N_, digits):
    ih = np.ones(N_, dtype=np.int64)
    ih[digits - 1] = 0
    return ih

def make_cnf_clauses_by_group(N_, board_group, varboard_group):
    """
    :param board_group: e.g. a row of sudoku board, of shape (M...)
    :param varboard_group: e.g. a row of sudoku variable id,
           of shape (M..., N_)
    """
    cclauses_local = []
    board_group = board_group.reshape(-1)
    varboard_group = varboard_group.reshape((board_group.shape[0], -1))
    oh = inv_oh(N_, board_group[board_group > 0])
    vidx = varboard_group[np.where(board_group == 0)[0]]
    poh = (oh > 0)
    ohpvidx = vidx[:, np.where(poh)[0]].T
    ohnvidx = vidx[:, np.where(~poh)[0]].reshape(-1)
    cclauses_local.extend(itertools.chain.from_iterable(
        (cnf.load_precomputed_xorcnf(x.tolist()) for x in ohpvidx)))
    cclauses_local.extend((-ohnvidx[:, np.newaxis]).tolist())
    return cclauses_local


def make_cnf_clauses_by_vars(board, varboard):
    board = board.reshape(-1)
    varboard = varboard.reshape((board.shape[0], -1))
    vidx = varboard[np.where(board == 0)[0]]
    cclauses_local = list(itertools.chain.from_iterable(
        (cnf.load_precomputed_xorcnf(x.tolist()) for x in vidx)))
    return cclauses_local


def resolve_solution(N_, board, sol):
    if sol in ('UNSAT', 'UNKNOWN'):
        raise RuntimeError(sol)
    board = np.copy(board)
    sol = np.array(sorted(sol, key=abs)).reshape((-1, N_))
    assert sol.shape[0] == np.sum(board == 0), sol
    nmlsol = np.sign(sol) + 1
    assert np.all(np.sum(nmlsol, axis=1) == 2), sol
    zlocs = np.where(board == 0)
    for r, xy in zip(nmlsol, zip(*zlocs)):
        board[xy] = np.nonzero(r)[0][0] + 1
    return board

def make_parser():
    parser = argparse.ArgumentParser(description='Solve sudoku')
    parser.add_argument('-c', '--write-clauses-to', dest='cltofile',
                        metavar='FILE',
                        help='write the JSON-represented CNF clauses to FILE')
    parser.add_argument('-H', dest='human_readable', action='store_true',
                        help='write human readable representation rather '
                             'rather than JSON; only effective when `-c\' is '
                             'presented')
    parser.add_argument('filename', metavar='BOARD_FILE',
                        help='CSV file that describe the sudoku to solve; '
                             '`0\' signifies blank cell and `1\'..`9\' the '
                             'digits')
    return parser


def main():
    logging.basicConfig(level=logging.INFO,
                        format='%(levelname)s|%(asctime)s|%(message)s')
    args = make_parser().parse_args()
    board = np.loadtxt(args.filename, dtype=np.int64, delimiter=',')
    assert board.shape == (B_H, B_W), f'illegal sudoku shape {board.shape}'
    disp_board(board)

    varboard = np.zeros((B_H, B_W, N), dtype=np.int64)
    b0 = (board == 0)
    n_vars = N * np.sum(b0)
    varboard[b0] = np.arange(1, n_vars + 1).reshape((np.sum(b0), N))
    cclauses = []

    # by digits
    cclauses.extend(make_cnf_clauses_by_vars(board, varboard))
    # by row
    cclauses.extend(itertools.chain.from_iterable(
        (make_cnf_clauses_by_group(N, *x)
         for x in zip(board, varboard))))
    # by column
    cclauses.extend(itertools.chain.from_iterable(
        (make_cnf_clauses_by_group(N, *x)
         for x in zip(board.T, varboard.transpose(1, 0, 2)))))
    # by boxes
    for ix, iy in itertools.product(range(B_H // B_SH), range(B_W // B_SW)):
        box = board[ix*B_SH:(1+ix)*B_SH, iy*B_SW:(1+iy)*B_SW]
        varbox = varboard[ix*B_SH:(1+ix)*B_SH, iy*B_SW:(1+iy)*B_SW]
        cclauses.extend(make_cnf_clauses_by_group(N, box, varbox))

    n_vars_ = abs(max(itertools.chain.from_iterable(cclauses), key=abs))
    logging.info('Converted sudoku into CNF; %d clauses in total; '
                 '%d variables in total',
                 len(cclauses), n_vars_)
    if args.cltofile:
        try:
            with open(args.cltofile, 'w') as outfile:
                if args.human_readable:
                    outfile.write(cnf.cnf2str(cclauses))
                else:
                    json.dump(cclauses, outfile)
                outfile.write('\n')
        except OSError:
            logging.exception('Failed to write CNF clauses to "%s"',
                              args.cltofile)
        else:
            logging.info('Written CNF clauses to "%s"', args.cltofile)

    solution = pycosat.solve(cclauses)
    logging.info('Solved')
    solved_board = resolve_solution(N, board, solution)
    disp_board(solved_board)


if __name__ == '__main__':
    main()
