import pickle
import cnf

exprs = []
for i in range(2, 11):
    exprs.append(cnf.xor2cnf(range(1, i)))
    print('.', end='', flush=True)
print()
with open('precompute-xor.pkl', 'wb') as outfile:
    pickle.dump(exprs, outfile)
