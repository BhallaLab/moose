#!/usr/bin/env python
from __future__ import print_function, division

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2017-, Dilawar Singh"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os
try:
    import subprocess32 as subprocess
except ImportError as e:
    import subprocess
import multiprocessing
import re
import glob
import datetime
import signal
from collections import defaultdict
import shutil
import random

sdir_       = os.path.dirname( os.path.realpath( __file__) )
willNotRun_ = defaultdict(set)
result_     = defaultdict(list)
pypath_     = sys.executable

def renormalize_path( path ):
    return os.path.normpath(path)


def filter_scripts( x ):
    filename, timeout = x 
    # filter scripts by criteria.
    global willNotRun_
    with open( filename, 'r' ) as f:
        txt = f.read()

    if not re.search( r'if\s+__name__\s+==\s+(\'|\")__main__(\'|\")\s*\:', txt):
        willNotRun_['NO_MAIN'].add(filename)
        return False

    if re.search( r'import\s+PyQt(4|5)|from\s+PyQt(4|5)\s+import', txt):
        willNotRun_['PYQT_REQUIRED'].add(filename)
        return False

    m = re.search( r'input\(\s*\)|raw_input\(\s*\)', txt)
    if m:
        #  print( "[INFO ] User input required in %s. Ignoring" % filename )
        #  print( "\t%s" % m.group(0) )
        willNotRun_['USER_INTERACTION'].add(filename)
        return False

    return True

def print_ignored( ):
    global willNotRun_
    for k in willNotRun_:
        fs = willNotRun_[k]
        fs = [ '- [ ] %3d: %s' % (i,x) for i, x in enumerate(fs) ]
        print( k )
        print( '\n'.join(fs) )
        print( '\n\n' )

def print_results( ):
    print( "[INFO ] Printing results ..." )
    global result_
    for k in result_:
        with open( "%s.txt" % k, 'w' ) as f:
            for fl, r in result_[k]:
                f.write( '- %s\n' % fl )
                f.write( '```' )
                try:
                    r = r.decode( 'utf8' )
                except Exception as e:
                    pass
                f.write( str(r) )
                f.write( '```\n')
        print("[INFO ] Wrote files with status %s to %s.txt " % (k,k))

    for k in result_:
        print( k )
        for f, r in result_[k]:
            print( f )
        print( '' )

    if len(result_['FAILED']) > 0:
        for f, r in result_['FAILED']:
            print( f )
            print( '```\n%s\n```\n' % r )
        quit(1)

def find_scripts_to_run( d ):
    print( "[INFO ] Searching for files in %s"  % d )
    files = []
    for d, sd, fs in os.walk( d ):
        d = renormalize_path(d)
        if d == sdir_:
            continue
        for f in fs:
            fname = renormalize_path(os.path.join(d,f))
            if fname.split( '.' )[-1] == 'py':
                timeout = 20
                if 'traub_2015' in fname:
                    timeout = 5
                files.append( (fname,timeout) )

    if os.environ.get('TRAVIS', ''):
        print( "[INFO ] Ignoring some scripts on Travis." )
        files = list(filter(filter_scripts, files))
    return files


def run_script( filename, timeout = 30 ):
    global sdir_
    global result_
    global pypath_
    # Run the script in the directory of file.
    tgtdir = os.path.dirname( os.path.realpath( filename ) )
    # copy matplotlibrc file to this directory.
    try:
        shutil.copy( os.path.join( sdir_, 'matplotlibrc' ), tgtdir )
    except Exception as  e:
        pass

    status = 'FAILED'
    res = None
    try:
        res = subprocess.run( [ pypath_, filename ], cwd = tgtdir
                , timeout = timeout
                , stdout = subprocess.PIPE
                , stderr = subprocess.PIPE
                )
        if res.returncode == 0:
            status = 'PASSED'
        else:
            status = 'FAILED'
    except subprocess.TimeoutExpired as e:
        status = 'TIMEOUT'

    stamp = datetime.datetime.now().isoformat()
    print( '[%s] %10s %s' % (stamp, status,filename) )

    if res is not None:
        result_[status].append( (filename,res.stdout+res.stderr) )
    else:
        result_[status].append( (filename,'UNKNOWN') )

def main( ):
    scripts = find_scripts_to_run(os.path.join(sdir_, '..'))
    print( "[INFO ] Total %s scripts found" % len(scripts) )
    print_ignored( )

    scripts = random.sample( scripts, 50 )
    print( '= Now running randomly selected %d files' % len(scripts) )
    for i, (x,t) in enumerate(scripts):
        print( '%3d/%d-' % (i,len(scripts)), end = '' )
        run_script( x, t )
    print_results()

if __name__ == '__main__':
    if len(sys.argv) > 1:
        pypath_ = sys.argv[1] 
    main(  )
