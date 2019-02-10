#!/usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import division, print_function

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2017-, Dilawar Singh"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os
import socket
import time
import tarfile
import tempfile

def gen_prefix( msg, maxlength = 10 ):
    msg = '>%s' % msg
    if len(msg) < maxlength:
        msg += ' ' * (maxlength - len(msg))
    return msg[:maxlength].encode( 'utf-8' )

def write_data_to_socket(conn, msg):
    msg = b'%010d%s' % (len(msg), msg)
    conn.sendall(msg)

def relativePath(path, base):
    return os.path.relpath(path, base)

def gen_payload( args ):
    path = args.path
    archive = os.path.join(tempfile.mkdtemp(), 'data.tar.bz2')

    # This mode (w|bz2) is suitable for streaming. The blocksize is default to
    # 20*512 bytes. We change this to 2048
    with tarfile.open(archive, 'w|bz2', bufsize=2048 ) as h:
        if len(args.main) > 0:
            for i, f in enumerate(args.main):
                h.add(f, arcname='__main__%d.py'%i)
        if os.path.isfile(path):
            h.add(path, os.path.basename(path))
        elif os.path.isdir(path):
            for d, ds, fs in os.walk(path):
                for f in fs:
                    p = os.path.join(d, f)
                    h.add(os.path.realpath(p), arcname=relativePath(p, path))
        else:
            print( "[ERROR] Neither file nor directory %s" % path )
            

    with open(archive, 'rb') as f:
        data = f.read()
    return data

def offload( args ):
    zfile = create_zipfile( args.path )
    send_zip( zfile )

def loop( sock ):
    sock.settimeout(1e-2)
    while True:
        try:
            d = sock.recv(10).strip()
            if len(d) > 0:
                print(d)
        except socket.timeout as e:
            print( '.', end='' )
            sys.stdout.flush()

def read_msg(conn):
    d = conn.recv(1024)
    try: 
        d = d.decode('utf8').strip()
    except Exception as e:
        pass
    return d

def save_bz2(conn, outfile):
    # first 6 bytes always tell how much to read next. Make sure the submit job
    # script has it
    d = conn.recv(10)
    while len(d) < 10:
        try:
            d = conn.recv(10)
        except Exception as e:
            print( "[ERROR] Error in format. First 6 bytes are size of msg." )
            continue

    print( "Needs to get %s bytes" % d )
    data = conn.recv(int(d))
    with open(outfile, 'wb') as f:
        f.write(data)
    return data

def main( args ):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        host, port = args.server.split(':')
        sock.connect( (host, int(port)) )
        sock.settimeout(1)
    except Exception as e:
        print( "[ERROR] Failed to connect to %s... " % args.server )
        print( e )
        quit()

    data = gen_payload( args )
    print( "[INFO ] Total bytes to send: %d" % len(data), end = '')
    write_data_to_socket(sock, data)
    print( ' ... [SENT]' )
    while True:
        try:
            d = read_msg( sock )
            print( d )
            if '>DONE SIMULATION' in d:
                break
        except socket.timeout as e:
            time.sleep(0.5)

    data = save_bz2(sock, 'results.tar.bz2' )
    print( "[INFO ] All done" )


if __name__ == '__main__':
    import argparse
    # Argument parser.
    description = '''Submit a job to moose server.'''
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('path', metavar='path'
        , help = 'File or directory to execute on server.'
        )
    parser.add_argument('--main', '-m', action = 'append'
        , required = False, default = []
        , help = 'In case of multiple files, scripts to execute'
                ' on the server, e.g. -m file1.py -m file2.py.'
                ' If not given, server will try to guess the best option.'
        )
    parser.add_argument('--server', '-s'
        , required = False, type=str, default='localhost:31417'
        , help = 'IP address and PORT number of moose server e.g.'
                 ' 172.16.1.2:31416'
        )
    class Args: pass
    args = Args()
    parser.parse_args(namespace=args)
    main(args)
