# -*- coding: utf-8 -*-
"""test_socket_streamer.py:

    MOOSE must create a socket server on PORT 31616 (default)
    or setup moose.SocketStreamer port to appropriate port number.

    Client can read data from this socket.
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

import os
import sys
sys.path.append(os.path.dirname(__file__))
import time
import socket
import numpy as np
import multiprocessing as mp
import moose
import moose.utils as mu
import json
import models
from collections import defaultdict

print( '[INFO] Using moose form %s' % moose.__file__ )

sockFile_ = '/tmp/MOOSE'

def socket_client(done, q):
    # This function waits for socket to be available.
    global sockFile_
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    address = sockFile_
    while not os.path.exists(address):
        if done.value == 1:
            return
        continue
    s.connect( address )
    print( 'Py: Connected with socket. %s' % sockFile_ )

    # This is client reponsibility to read the data.
    print( 'Py: Fetching...' )
    data = b''
    s.settimeout(0.01)
    while True:
        try:
            data += s.recv(64)
        except socket.timeout:
            print('x', end = '' )

        if done.value == 1:
            print( 'Simulation is over' )
            break
    s.close()
    if not data:
        print("No data streamed")
        done.value = 1
        q.put({})
        return
    res = mu.decode_data(data)
    q.put(res)

def test():
    q = mp.Queue()
    done = mp.Value( 'd', 0.0)
    client = mp.Process(target=socket_client, args=(done, q))
    client.start()
    print( '[INFO] Socket client is running now' )
    time.sleep(0.1)

    # Now create a streamer and use it to write to a stream
    os.environ['MOOSE_STREAMER_ADDRESS'] = 'file://%s' % sockFile_
    models.simple_model_a()

    # Give some time for socket client to make connection.
    moose.reinit()
    moose.start(50)
    time.sleep(0.1)
    done.value = 1

    res = q.get()
    if not res:
        raise RuntimeWarning( 'Nothing was streamed')
    for k in res:
        aWithTime = res[k]
        a = aWithTime[1::2]
        b = moose.element(k).vector
        print(k, len(a), len(b))
        assert (a == b).all()

    # sleep for some time so data can be read.
    client.join()

def main( ):
    test()
    print( '[INFO] All tests passed' )

if __name__ == '__main__':
    main()
