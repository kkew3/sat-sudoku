from libc.stdint cimport int64_t
from libcpp.vector cimport vector

cimport numpy as np

from VariableManager cimport VariableManager as c_VariableManager

import numpy as np


cdef extern from "c_enc.hpp":
    cdef void c_disp_board(int nsq, int64_t *board)
    cdef vector[vector[int]] c_minimal_encode(int nsq, int64_t *board)
    cdef vector[vector[int]] c_extended_encode(int nsq, int64_t *board)
    cdef int c_lit2var(int n, int n2, int x, int y, int v)
    cdef void c_var2lit(int n, int var, int *xp, int *yp, int *vp)


cdef class VariableManager:
    cdef c_VariableManager mvars

    def convert(self, vector[vector[int]] cclauses):
        return self.mvars.convert(cclauses)

    def back(self, vector[int] solution):
        return self.mvars.back(solution)


cdef int find_nsq(int n):
    cdef int nsq = 1
    while nsq <= n:
        if nsq * nsq == n:
            return nsq
        nsq += 1
    return 0


def disp_board(np.ndarray[np.int64_t, ndim=2] board):
    board = np.ascontiguousarray(board)
    if board.size == 0:
        raise ValueError('invalid board shape')
    if board.shape[0] != board.shape[1]:
        raise ValueError('invalid board shape')
    cdef int nsq = find_nsq(board.shape[0])
    if nsq == 0:
        raise ValueError('invalid board shape')

    c_disp_board(nsq, &board[0, 0])


def minimal_encode(np.ndarray[np.int64_t, ndim=2] board):
    board = np.ascontiguousarray(board)
    if board.size == 0:
        raise ValueError('invalid board shape')
    if board.shape[0] != board.shape[1]:
        raise ValueError('invalid board shape')
    cdef int nsq = find_nsq(board.shape[0])
    if nsq == 0:
        raise ValueError('invalid board shape')

    return c_minimal_encode(nsq, &board[0, 0])


def extended_encode(np.ndarray[np.int64_t, ndim=2] board):
    board = np.ascontiguousarray(board)
    if board.size == 0:
        raise ValueError('invalid board shape')
    if board.shape[0] != board.shape[1]:
        raise ValueError('invalid board shape')
    cdef int nsq = find_nsq(board.shape[0])
    if nsq == 0:
        raise ValueError('invalid board shape')

    return c_extended_encode(nsq, &board[0, 0])


class Literal:
    __slots__ = ('x', 'y', 'v')

    def __init__(self, x, y, v):
        assert v != 0
        self.x = x
        self.y = y
        self.v = v

    def __repr__(self):
        return '({}, {}, {})'.format(self.x, self.y, self.v)


def interpret_solution(int n, vector[int] solution):
    cdef:
        int var
        int x
        int y
        int v
    lsolution = []
    for var in solution:
        c_var2lit(n, var, &x, &y, &v)
        if v > 0:
            lsolution.append(Literal(x, y, v))
    return lsolution
