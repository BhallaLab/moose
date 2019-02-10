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
sys.path.append( os.path.dirname(__file__))
import time
import socket
import numpy as np
import threading
import moose
import models
import json
from collections import defaultdict

finish_all_ = False

print( '[INFO] Using moose form %s' % moose.__file__ )

def get_msg(s, n=1024):
    d = s.recv( n, socket.MSG_WAITALL)
    while len(d) < n:
        d += s.recv(n-len(d), socket.MSG_WAITALL)
    return d

def socket_client(host='127.0.0.1', port = 31416):
    # This function waits for socket to be available.
    global finish_all_
    s = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
    while 1:
        if finish_all_:
            print( '[INFO] MOOSE is done before I could connect' )
            break
        #  print('Py: Trying to connect to %s, %s' % (host, port))
        #  print( end = '.' )
        try:
            s.connect( ('', port) )
            break
        except Exception as e:
            continue
            print(e, end = 'x')
            sys.stdout.flush()
            pass

    print( 'Connected' )
    if not finish_all_:
        print( 'Py: Connected with socket.' )

    # This is client reponsibility to read the data.
    s.settimeout(0.1)
    data = b''
    while not finish_all_:
        try:
            data += get_msg(s)
        except socket.timeout as e:
            print( e, end = ' ' )

    s.close()
    assert data, "No data streamed"
    print( 'recieved data:\n', data )
    res = defaultdict(list)
    for x in data.split(b'\n'):
        if not x.strip():
            continue
        d = json.loads(x)
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

def sanity_test( ):
    a = moose.Table( '/t1' )
    b = moose.Table( '/t1/t1' )
    c = moose.Table( '/t1/t1/t1' )
    st = moose.SocketStreamer( '/s' )
    st.addTable( a )
    assert( st.numTables == 1 )
    st.addTable( b )
    assert( st.numTables == 2 )
    st.addTable( c )
    assert( st.numTables == 3 )
    st.addTable( c )
    assert( st.numTables == 3 )
    st.addTable( c )
    assert( st.numTables == 3 )

    st.removeTable( c )
    assert( st.numTables == 2 )
    st.removeTable( c )
    assert( st.numTables == 2 )
    st.removeTable( a )
    assert( st.numTables == 1 )
    st.removeTable( b )
    assert( st.numTables == 0 )
    st.removeTable( b )
    assert( st.numTables == 0 )
    print( 'Sanity test passed' )

    st.addTables( [a, b, c ])
    assert st.numTables == 3
    st.removeTables( [a, a, c] )
    assert st.numTables == 1
    moose.delete( '/t1' )
    moose.delete( '/s' )

def test():
    global finish_all_
    os.environ['MOOSE_STREAMER_ADDRESS'] = 'http://127.0.0.1:31416'
    client = threading.Thread(target=socket_client, args=())
    #  client.daemon = True
    client.start()
    print( '[INFO] Socket client is running now' )
    tables = models.simple_model_a()
    time.sleep(0.1)
    moose.reinit()
    moose.start(50)
    print( 'MOOSE is done' )

    # sleep for some time so data can be read.
    time.sleep(1)
    finish_all_ = True
    client.join()
    print( 'Test 1 passed' )

def test_without_env():
    global finish_all_
    os.environ['MOOSE_STREAMER_ADDRESS'] = ''
    client = threading.Thread(target=socket_client, args=())
    #client.daemon = True
    client.start()
    print( '[INFO] Socket client is running now' )
    tables = create_model()
    # Now create a streamer and use it to write to a stream
    st = moose.SocketStreamer( '/compt/streamer' )
    st.address = 'http://127.0.0.1:31416'
    assert st.port == 31416, "Got %s expected %s" % (st.port, expected)
    st.addTable(tables[0])
    st.addTables(tables[1:])
    assert st.numTables == 3
    # Give some time for socket client to make connection.
    moose.reinit()
    moose.start(50)
    time.sleep(1)
    print( 'MOOSE is done' )
    # sleep for some time so data can be read.
    finish_all_ = True
    client.join()
    print( 'Test 2 passed' )

def main( ):
    test( )
    #  test_without_env()
    print( '[INFO] All tests passed' )

if __name__ == '__main__':
    main()
