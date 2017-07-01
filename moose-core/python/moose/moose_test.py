# -*- coding: utf-8 -*-
"""
Test MOOSE installation with moose-examples.

"""

from __future__ import print_function

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2016, Dilawar Singh"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os
import tempfile
import re
import subprocess
import threading
import signal
import logging
from collections import defaultdict
import time

logging.basicConfig(
        level=logging.DEBUG,
        format='%(asctime)s %(name)-12s %(levelname)-8s %(message)s',
        datefmt='%m-%d %H:%M',
        filename='tests.log',
        filemode='w'
        )
console = logging.StreamHandler()
console.setLevel(logging.WARNING)
formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
console.setFormatter(formatter)
_logger = logging.getLogger('moose.test')
_logger.addHandler(console)

test_data_url_ = 'https://github.com/BhallaLab/moose-examples/archive/master.zip'
test_repo_url_ = 'https://github.com/BhallaLab/moose-examples'
test_dir_ = os.path.join( tempfile.gettempdir( ), 'moose-examples' )

ignored_dict_ = defaultdict( list )
test_status_ = defaultdict( list )
total_ = 0

matplotlibrc_ = '''
backend : agg
interactive : True
'''

# Handle CTRL+C
def signal_handler(signal, frame):
    print( 'You pressed Ctrl+C!' )
    print_test_stat( )
    quit(-1)

signal.signal(signal.SIGINT, signal_handler)

# Credit: https://stackoverflow.com/a/4825933/1805129
class Command(object):
    def __init__(self, cmd):
        self.cmd = cmd
        self.process = None
        self.fnull = open( os.devnull, 'w' )

    def __repr__( self ):
        return ' '.join( self.cmd )

    def run(self, timeout):
        def target():
            _logger.info( "Running %s" % self )
            self.process = subprocess.Popen(
                    self.cmd, shell=False
                    , stdout = self.fnull, stderr = subprocess.STDOUT
                    )
            self.process.communicate()

        thread = threading.Thread( target = target )
        thread.start()
        thread.join(timeout)
        if thread.is_alive():
            self.process.terminate()
            thread.join()

        return self.process.returncode

def init_test_dir( ):
    global test_dir_
    global test_url_
    if( not os.path.exists( test_dir_ ) ):
        os.makedirs( test_dir_ )
        _logger.info( "Donwloading test repository" )
        subprocess.call(
                [ 'git', 'clone', '--depth=10', test_repo_url_, test_dir_ ]
                )
    os.chdir( test_dir_ )

def suitable_for_testing( script ):
    with open( script ) as f:
        txt = f.read( )
        if not re.search( r'main\(\s*\)', txt ):
            _logger.debug( 'Script %s does not contain main( )' % script )
            return False, 'main( ) not found'
        if re.search( r'raw_input\(\s*\)', txt ):
            _logger.debug( 'Script %s requires user input' % script )
            return False, 'waits for user input'
    return True, 'OK'

def run_test( index, testfile, timeout,  **kwargs):
    """Run a given test
    """
    global test_status_
    global total_
    pyExec = os.environ.get( 'PYTHON_EXECUTABLE', '/usr/bin/python' )
    cmd = Command( [ pyExec, testfile ] )

    ti = time.time( )
    name = os.path.basename( testfile )
    out = (name + '.' * 50)[:40]
    print( '[TEST %3d/%d] %41s ' % (index, total_, out), end='' )
    sys.stdout.flush( )

    # Run the test.
    status = cmd.run( timeout = timeout )
    t = time.time( ) - ti
    print( '% 7.2f ' % t, end='' )
    sys.stdout.flush( )

    # Change to directory and copy before running then test.
    cwd = os.path.dirname( testfile )
    os.chdir( cwd )
    with open( os.path.join( cwd, 'matplotlibrc' ), 'w' ) as f:
        _logger.debug( 'Writing matplotlibrc to %s' % cwd )
        f.write( matplotlibrc_ )

    if status != 0:
        if status == -15:
            msg = '% 4d TIMEOUT' % status
            test_status_[ 'TIMED-OUT' ].append( testfile )
        else:
            msg = '% 4d FAILED' % status
            test_status_[ 'FAILED' ].append( testfile )
        print( msg )
    else:
        print( '% 4d PASSED' % status )
        test_status_[ 'PASSED' ].append( testfile )

    sys.stdout.flush( )

def print_test_stat( ):
    global test_status_
    for status in test_status_:
        print( 'Total %d tests %s' % (len( test_status_[status] ), status ) )

def test_all( timeout, **kwargs ):
    global test_dir_
    global total_
    scripts = [ ]
    for d, ds, fs in os.walk( test_dir_ ):
        for f in fs:
            if not re.match( r'.+\.py$', f):
                continue
            filepath = os.path.join( d, f  )
            isOK, msg = suitable_for_testing( filepath )
            if isOK:
                scripts.append( filepath )
            else:
                ignored_dict_[ msg ].append( filepath )

    for k in ignored_dict_:
        _logger.debug( '[INFO] Ignored due to %s' % k )
        _logger.debug( '\n\t'.join( ignored_dict_[ k ] ) )

    _logger.info( 'Total %d valid tests found' % len( scripts ) )
    total_ = len( scripts )
    for i, s in enumerate( scripts ):
        _logger.info( 'Running test : %s' % s )
        run_test(i, s, timeout, **kwargs )


def test( timeout = 60, **kwargs ):
    """Download and run tests.

    """
    print( '[INFO] Running test with timeout %d sec' % timeout )
    try:
        init_test_dir( )
    except Exception as e:
        print( '[INFO] Failed to clone moose-examples. Error was %s' % e )
        quit( )

    test_all( timeout = timeout, **kwargs  )
    print_test_stat( )

if __name__ == '__main__':
    test( )
