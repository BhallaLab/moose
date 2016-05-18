# test_moose_thread.py --- 
# 
# Filename: test_moose_thread.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Thu Mar  8 09:38:02 2012 (+0530)
# Version: 
# Last-Updated: Thu Mar  8 15:16:03 2012 (+0530)
#           By: Subhasis Ray
#     Update #: 162
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# Example of using multithreading to run a MOOSE simulation in
# parallel with querying MOOSE objects involved. See the documentatin
# of the classes to get an idea of this demo's function.
# 

# Change log:
# 
# 2012-03-08 12:31:46 (+0530) Initial version by Subha
# 

# Code:

import sys
from datetime import datetime
import threading
import Queue
import time

import moose

worker_queue = Queue.Queue()
status_queue = Queue.Queue()

class WorkerThread(threading.Thread):
    """This thread initializes the simulation (reinit) and then runs
    the simulation in its run method. It keeps querying moose for
    running status every second and returns when the simulation is
    over. It puts its name in the global worker_queue at the end to
    signal successful completion."""
    def __init__(self, runtime):
        threading.Thread.__init__(self)
        self.runtime = runtime
        print 'Created WorkerThread of name', self.name
        
    def run(self):
        print self.name, 'Starting run for', self.runtime, ' seconds'        
        moose.reinit()
        moose.start(self.runtime)
        while moose.isRunning():
            time.sleep(1.0)
            print self.name, 'Table length', len(moose.Table('/tab').vector)
        print self.name, 'Finishing simulation'
        worker_queue.put(self.name)

class StatusThread(threading.Thread):
    """This thread checks the status of the moose worker thread by
    checking the worker_queue for available entry. If there is
    nothing, it goes to sleep for a second and then prints current
    length of the table. If there is an entry, it puts its name in the
    status queue, which is used by the main thread to recognize
    successful completion."""
    def __init__(self, tab):
        threading.Thread.__init__(self)
        self.table = tab
        print 'Created StatusThread of name', self.name
        
    def run(self):
        while True:
            try:
                value = worker_queue.get(False)
                print self.name, 'Received queue entry: ', value, '. Final table length:', len(self.table.vector), ' ... now Finishing'
                status_queue.put(self.name)
                return
            except Queue.Empty:
                time.sleep(1.0)
                print self.name, 'Queue is empty. Current table length:', len(self.table.vector)
        
if __name__ == '__main__':
    pg = moose.PulseGen('pg')
    pg.firstDelay = 10.0
    pg.firstLevel = 10.0
    pg.firstWidth = 5.0
    tab = moose.Table('tab')
    moose.connect(tab, 'requestOut', pg, 'getOutputValue')
    moose.setClock(0, 1.0)
    moose.useClock(0, 'pg,tab', 'process')
    t1 = WorkerThread(10000)
    t2 = StatusThread(tab)
    t2.start()
    t1.start()
    status_queue.get(True)
    tab.xplot('threading_demo.dat', 'pulsegen_output')
    print 'Ending threading_demo: final length of table', len(tab.vector) 

# 
# threading_demo.py ends here
