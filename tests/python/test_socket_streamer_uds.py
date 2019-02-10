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
import threading
import moose
import json
import models
from collections import defaultdict

finish_all_ = False

print( '[INFO] Using moose form %s' % moose.__file__ )

sockFile_ = '/tmp/MOOSE'

def get_msg(s, n=1024):
    d = s.recv(n)
    while(len(d) < n):
        d1 = s.recv(n-len(d))
        d += d1
    return d

def socket_client( ):
    # This function waits for socket to be available.
    global finish_all_
    global sockFile_
    s = socket.socket( socket.AF_UNIX, socket.SOCK_STREAM )
    address = sockFile_
    while 1:
        if finish_all_:
            print( '[INFO] MOOSE is done before I could connect' )
            break
        #  print('Py: Trying to connect to %s, %s' % (host, port))
        #  print( end = '.' )
        try:
            s.connect( address )
            break
        except Exception as e:
            pass

    if not finish_all_:
        print( 'Py: Connected with socket.' )

    # This is client reponsibility to read the data.
    data = b''
    s.settimeout(1)
    while not finish_all_:
        try:
            d = get_msg(s, 1024)
            data += d
        except socket.timeout as e:
            pass
    s.close()

    assert data, "No data streamed"
    res = defaultdict(list)
    for x in data.split(b'\n'):
        if not x.strip():
            continue
        x = x.decode( 'utf8' )
        try:
            d = json.loads(x)
        except Exception as e:
            print( data )
            raise e
        for k, v in d.items():
            res[k] += v

    expected = {u'/compt/tabB/tabC': ([25.,1.07754388], [14.71960144,  0.16830373])
            , u'/compt/a/tab': ([25., 0.42467006], [14.71960144,  0.16766705])
            , u'/compt/tabB': ([25.,  2.57797725], [14.71960144,  0.16842971])
            }
    nd = {}
    for k in res:
        v = res[k]
        nd[k] = (np.mean(v, axis=0), np.std(v, axis=0))
        assert np.isclose(expected[k], nd[k]).all(), \
                "Exptected %s, got %s" % (str(expected[k]), str(nd[k]))

def test():
    global finish_all_
    client = threading.Thread(target=socket_client, args=())
    #client.daemon = True
    client.start()
    print( '[INFO] Socket client is running now' )
    tables = models.simple_model_a()
    # Now create a streamer and use it to write to a stream
    os.environ['MOOSE_STREAMER_ADDRESS'] = 'file:///tmp/moose' 

    # Give some time for socket client to make connection.
    moose.reinit()
    moose.start(50)
    time.sleep(0.1)
    finish_all_ = True
    print( 'MOOSE is done' )
    # sleep for some time so data can be read.
    client.join()
    print( 'Test 2 passed' )

def main( ):
    test()
    print( '[INFO] All tests passed' )

if __name__ == '__main__':
    main()
