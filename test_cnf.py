import pytest

import cnf


def test_reduce_clause():
    assert cnf.reduce_clause([[2, 3], -2]) == []
    assert cnf.reduce_clause([[2, 2], -3]) == [2, -3]
    assert cnf.reduce_clause([[1, 1, -1], -1]) == []
    assert cnf.reduce_clause([[3], -3]) == []


def test_interprete_template():
    template0 = '[[-1, 2, -3], [1, -2, -3], [-2, -3], [1, 2, 3], [-1, -3], [-1, -2], [-1, -2, 3], [-1, -2, -3]]'
    tr1 = {'1': 'a', '2': 'b', '3': 'c'}
    tr2 = {'a': '3', 'b': '1', 'c': '2'}
    template2 = cnf.interprete_template(tr1, tr2, template0)
    assert template2 == '[[-3, 1, -2], [3, -1, -2], [-1, -2], [3, 1, 2], [-3, -2], [-3, -1], [-3, -1, 2], [-3, -1, -2]]'

