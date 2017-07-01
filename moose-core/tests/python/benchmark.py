# -*- coding: utf-8 -*-
from __future__ import print_function

from datetime import datetime
import moose

def time_creation(n=1000):
    elist = []
    start = datetime.now()
    for ii in range(n):
        elist.append(moose.Neutral('a_%d' % (ii)))
    end = datetime.now()
    delta = end - start
    print('total time to create %d Neutral elements: %g' % (n, delta.days * 86400 + delta.seconds + delta.microseconds * 1e-6))
    return delta

if __name__ == '__main__':
    time_creation()
