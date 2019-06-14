import itertools
import string
import json
import re
import logging

import numpy as np
import pytest


def reduce_clause(clause):
    """
    :param clause: of shape [[x, ...], y]
    """
    cl = clause[0] + [clause[1]]
    cl.sort(key=abs)
    if not cl:
        return []
    rcl = []
    for top in cl:
        if not rcl:
            rcl.append(top)
        elif rcl[-1] == top:
            pass
        elif rcl[-1] + top == 0:
            return []
        else:
            rcl.append(top)
    return rcl


def xor2cnf(vars_):
    dnf = list((np.eye(len(vars_), dtype=int) * 2 - 1) * vars_)
    dnf[0] = dnf[0][:, np.newaxis].tolist()
    for i in range(1, len(dnf)):
        dnf[i] = dnf[i].tolist()
    cnf = dnf.pop(0)
    while dnf:
        top = dnf.pop(0)
        cnf = itertools.product(cnf, top)
        cnf = map(reduce_clause, cnf)
        cnf = filter(None, cnf)
    cnf = list(set(map(tuple, cnf)))
    return cnf


def cnf2str(cclauses):
    assert max(map(abs, itertools.chain.from_iterable(cclauses))) <= 26, cclauses
    letters = string.ascii_lowercase
    digits = range(1, 27)
    tr = dict(zip(digits, letters))
    tr.update(dict(zip([(-x) for x in range(1, 27)],
                       [f'(not {x})' for x in letters])))
    return ' and '.join([('(' + ' or '.join([tr[x] for x in cl]) + ')')
                         for cl in cclauses])


def _make_sed_script(tr_mapping):
    script = [f's/\\<{k}\\>/{v}/g;' for k, v in tr_mapping.items()]
    return ''.join(script)


def interprete_template(tr1, tr2, template0):
    sbuf = json.dumps(template0)
    for k, v in tr1.items():
        sbuf = re.sub(''.join(('\\b', str(k), '\\b')), str(v), sbuf)
    for k, v in tr2.items():
        sbuf = re.sub(''.join(('\\b', str(k), '\\b')), str(v), sbuf)
    template2 = json.loads(sbuf)
    return template2


def load_precomputed_xorcnf(vars_, filename='precompute-xor.json'):
    logger = logging.getLogger(f'{__name__}.{load_precomputed_xorcnf.__name__}')
    with open(filename, 'rb') as infile:
        exprs = json.load(infile)
    pclens = list(map(len, exprs))
    template = None
    if len(vars_) <= len(pclens):
        template = exprs[len(vars_) - 1]
    if not template:
        logger.warning(f'Computing CNF for {vars_}; expect long running time')
        template = xor2cnf(vars_)
        if len(vars_) <= len(pclens):
            exprs[len(vars_) - 1] = template
        else:
            exprs.extend([None for _ in range(len(pclens), len(vars_) - 1)])
            exprs.append(template)
        with open(filename, 'wb') as outfile:
            json.dump(exprs, outfile)
    tmpset = map('.'.join, itertools.product(
        *[string.ascii_lowercase for _ in range(5)]))
    tmpset = list(itertools.islice(tmpset, len(vars_)))
    tr1 = dict(zip(range(1, len(vars_) + 1), tmpset))
    tr2 = dict(zip(tmpset, vars_))
    cnf = interprete_template(tr1, tr2, template)
    return cnf
