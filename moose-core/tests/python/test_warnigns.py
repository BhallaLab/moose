# -*- coding: utf-8 -*-
import moose
print( 'Using moose from %s' % moose.__file__ )
a = moose.Neutral( '/a' )
b = moose.Ksolve( '/a/k' )

try:
    c = moose.Ksolve( '/a/k' )
    d = moose.Neutral( '/a' )
    d = moose.Neutral( '/a' )
    quit(-1)
except Exception as e:
    c = moose.element( '/a/k' )
    quit( 0 )
