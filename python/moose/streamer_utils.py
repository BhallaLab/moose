"""streamer_utils.py: 

Utility for Table streamer.

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2017-, Dilawar Singh"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os
import numpy as np
import math
import struct
from collections import defaultdict

def bytes_to_np_arr(data):
    return np.frombuffer(data, float)

def np_array_to_string(arr):
    return np.uint8(arr).tostring()

def np_array_to_data(arr):
    # Make sure that first char of arr is 'H'
    n = 0
    res = defaultdict(list)
    while n < len(arr):
        assert chr(int(arr[n])) == 'H', 'Expected H, Got %s'%arr[n]
        hSize = int(arr[n+1])
        n += 1
        colName = np_array_to_string(arr[n+1:n+1+hSize])
        n += hSize + 1
        assert chr(int(arr[n])) == 'V', 'Expected V'
        n += 1
        dataSize = int(arr[n])
        n += 1
        res[colName].append(arr[n:n+dataSize])
        n += dataSize 
    return { k : np.concatenate(v) for k, v in res.items() }

def decode_data(data):
    arr = bytes_to_np_arr(data)
    assert int(arr[0]) == ord('H'), "First char must be H"
    return np_array_to_data(arr)

def test():
    with open(sys.argv[1], 'rb') as f:
        data = f.read()
    print( "[INFO ] Total bytes read %d" % len(data))
    s = decode_data(data)
    print(s)

if __name__ == '__main__':
    test()
