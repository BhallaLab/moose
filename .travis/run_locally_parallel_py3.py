#!/usr/bin/env python3

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2017-, Dilawar Singh"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import execute_travis_serial as ets
import signal
import multiprocessing
import os

sdir_ = os.path.dirname( __file__ )

def init_worker( ):
    signal.signal( signal.SIGINT, signal.SIG_IGN )

def run_all( scripts, workers = 2 ):
    print( "[INFO ] Using %s workers" % workers )
    pool = multiprocessing.Pool( workers, init_worker )
    try:
        pool.map( ets.run_script, scripts )
    except KeyboardInterrupt as e:
        print( "[INFO ] Keyboard interrupt. Shutting down" )
        pool.terminate()
        pool.join()
    print( '... DONE' )

def main():
    scriptWithTimeout = ets.find_scripts_to_run(os.path.join(sdir_, '..'))
    scripts, timeouts = zip(*scriptWithTimeout)
    print( "[INFO ] Total %s scripts found" % len(scripts) )
    ets.print_ignored( )
    print( '== Now running files' )
    workers = max( 2, multiprocessing.cpu_count() // 2 )
    run_all( scripts, workers )

if __name__ == '__main__':
    main()

