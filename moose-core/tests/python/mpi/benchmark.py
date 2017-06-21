# benchmark.py --- 
# 
# Filename: benchmark.py
# Description: Script for performance benchmarking
# Author: 
# Maintainer: 
# Created: Thu Jan 23 16:06:25 2014 (+0530)
# Version: 
# Last-Updated: 
#           By: 
#     Update #: 0
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# 
# 
# 

# Change log:
# 
# 
# 
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
# 
# 

# Code:
"""This script tries to run the test case multiple times with
different number of nodes and dump the performance. It shuffles the
runs with increasing number of processes in an attempt to avoid
systematic error based on process sequence.

"""
from __future__ import print_function

import getopt
import subprocess
import sys
import os
import socket
import multiprocessing
import subprocess
from datetime import datetime, timedelta
from collections import defaultdict
import random

def create_hostfile(slotcount=None, filename='hostfile'):
    """Create a file with name `filename` and write host info for openmpi
    in this."""
    if slotcount is None:
        slotcount = multiprocessing.cpu_count()
    with open(filename, 'w') as hostfile:
        hostfile.write('%s slots=%d\n' % (socket.gethostname(), slotcount))
    

def run(script, scriptargs=[], hostfile='hostfile', np=2, ni=1, oversubscribe=False):
    """Run `script` with arguments in `scriptargs` list. Use `hostfile`
    for host and slot info, use `np` moose processes in addition to
    Python, do `ni` iterations to get average performance."""
    if not oversubscribe:
        np = multiprocessing.cpu_count()
    s0 = datetime.now()
    avg = defaultdict(timedelta)
    procs = range(np)
    for jj in range(ni):
        random.shuffle(procs)
        for ii in procs:
            print('Running with', ii, 'processes')
            if ii > 0:
                args = ['mpirun', '--hostfile', hostfile, '-np', '1', 'python', script] + scriptargs + [':', '-np', str(ii), '../../../moose']
            else:
                args = ['mpirun', '--hostfile', hostfile, '-np',  '1', 'python', script] + scriptargs
            print('Running:', args)
            s1 = datetime.now()
            ret = subprocess.check_output(args)
            print('====================== OUTPUT START ======================')
            print(ret)
            print('====================== OUTPUT   END ======================')
            e1 = datetime.now()
            d1 = e1 - s1
            avg[ii] += d1
            print('Time to run ', args)
            print('            =', d1.days * 86400 + d1.seconds + 1e-6 * d1.microseconds)
    e0 = datetime.now()
    d0 = e0 - s0
    avg['all'] += d0
    for ii in sorted(procs):
        print('Time to run using', ii, 'additional moose processes: ')
        print('            =', (avg[ii].days * 86400 + avg[ii].seconds + 1e-6 * avg[ii].microseconds) / ni)
    print('Total time for all different process counts averaged over all', ni, 'iterations:', (avg['all'].days * 86400 + avg['all'].seconds + 1e-6 * avg['all'].microseconds) / ni)

def print_usage(argv0):
    print('''Usage: %s [-s slotcount]  [-f hostfile] [-n maxprocess] [-i iterations] {script} [script arguments]
    
    Run {script} using up to {slotcount} slots and display
    execution time. If specified, the host information will be
    written in `hostfile`. Default is "hostfile".''' % (argv0))
    sys.exit(1)
    
if __name__ == '__main__':
    if len(sys.argv) < 2:
        print_usage(sys.argv[0])
    hostfile = 'hostfile'
    scriptargs = []
    slots = None
    np = 1
    ni = 1
    options, rest = getopt.getopt(sys.argv[1:], 'hf:s:n:i:', ['hostfile=','slots=', 'help', 'numproc='])
    for opt, arg in options:
        if opt in ('-h', '--help'):
            print_usage(sys.argv[0])
        elif opt in ('-f', '--hostfile'):
            hostfile = arg
        elif opt in ('-s', '--slots'):
            slots = int(arg)
        elif opt in ('-n', '--numproc'):
            np = int(arg)
        elif opt in ('-i', '--iterations'):
            ni = int(arg)
    if len(rest) < 1:
        print_usage(sys.argv[0])
    else:
        script = rest[0]
    if len(rest) > 1:
        scriptargs = rest[1:]
    create_hostfile(slotcount=slots, filename=hostfile)
    run(script, scriptargs, hostfile=hostfile, np=np, ni=ni)
    


# 
# benchmark.py ends here
