# -*- coding: utf-8 -*-
import moose
import re

def fixPath(path):
    path = re.sub(r'/+', '/', path)
    return path

paths = [ '/a'
        , '//a'
        , '/a/b'
        , '/a/b/'
        , '//a//b/////'
        , '/a/./b'
        , '///a/././b'
        ]
expectedPath = [fixPath(p) for p in paths]

expectedChanges = zip(paths, expectedPath)

for p in expectedPath:
    print( 'Accessing %s' % p )
    try:
        moose.Neutral(p)
    except Exception as e:
        print( 'Same path access by element' )
        moose.element( p )

foundPath = []
for p in moose.wildcardFind('/##'):
    if "/a" in p.path:
       foundPath.append(p.path)

testFailed = False
for f in foundPath:
    f = re.sub(r'\[\d+\]', '', f)
    if f not in expectedPath:
        testFailed = True

if testFailed:
    print("Test failed on paths")
