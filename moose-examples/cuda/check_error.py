import numpy as np
import sys

f1 = sys.argv[1]
f2 = sys.argv[2]

val1 = np.loadtxt(f1)
val2 = np.loadtxt(f2)

e = abs(val1-val2)
Totalerror = sum(e.flatten())
avgError = Totalerror/len(val1)

filename = f1.split('/')[-1]

print(filename, Totalerror, avgError)
