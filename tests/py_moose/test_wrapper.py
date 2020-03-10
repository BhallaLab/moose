# -*- coding: utf-8 -*-

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2019-, Dilawar Singh"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"

import moose
import numpy as np
print( "[INFO ] Using moose %s form %s" % (moose.version(), moose.__file__) )

def create_func( funcname, expr ):
    f = moose.Function( funcname )
    f.expr = expr
    t = moose.Table( funcname + 'tab' )
    t.connect('requestOut', f, 'getValue')
    moose.setClock( f.tick, 0.1)
    moose.setClock( t.tick, 0.1)
    return f, t

def test_function():
    nsteps = 5
    simtime = nsteps
    dt = 1.0
    fn1 = moose.Function('/fn1', expr = 'B+A+y0+y1')
    inputs = np.arange(0, nsteps+1, 1.0)
    x0 = moose.StimulusTable('/x0', vector=inputs, startTime = 0.0
            , stopTime = simtime, stepPosition = 0.0)
    print('x0', x0.vector)
    inputs /= 10
    x1 = moose.StimulusTable('/x1', vector=inputs, startTime=0.0
            , stopTime=simtime, stepPosition=0.0, k = 11)
    print('x1', x1.vector)
    inputs /= 10
    y0 = moose.StimulusTable('/y0', vector=inputs, startTime=0.0
            , stopTime=simtime, stepPosition=0.0)
    print('y0', y0.vector)
    inputs /= 10
    y1 = moose.StimulusTable('/y1', vector=inputs, startTime=0.0
            , stopTime=simtime, stepPosition=0.0)
    print('y1', y1.vector)
    x0.connect('output', fn1['A'], 'input')
    x1.connect('output', fn1['B'], 'input')
    fn1.connect('requestOut', y0, 'getOutputValue')
    fn1.connect('requestOut', y1, 'getOutputValue')

    z1 = moose.Table('/z1')
    z1.connect('requestOut', fn1, 'getValue')
    for ii in range(32):
        moose.setClock(ii, dt)
    moose.reinit()
    moose.start(simtime)
    expected = [0, 1.1, 2.211, 3.322, 4.433, 5.544]
    assert np.allclose(z1.vector, expected), "Excepted %s, got %s" % (expected, z1.vector )
    print( 'Passed order vars' )


def test_creation():
    a1 = moose.Neutral('a')
    a2 = moose.Neutral('//a')
    a3 = moose.Neutral('/.//a/')
    assert a1 == a2 == a3
    assert a1.path == a2.path == a3.path == '/a[0]', a1.path
    a3 = moose.Neutral('a2')
    assert a3 != a1

def test_print():
    a = moose.Function('f', expr='sqrt(sin(x0))+cos(x1)^2+ln(20+sin(t))')
    # Get equivalent sympy expression. And do whatever you like with it.
    try:
        sympyExpr = a.sympyExpr()
        print('Sympy expression is:', sympyExpr)

        # Or pretty-print using sympy
        a.printUsingSympy()
    except ImportError:
        pass

def test_basic():
    f = moose.Function('f1')
    f.expr = 'A+B+sqrt(B)+zhadu(q)'
    print(f)

def main():
    test_function()
    test_basic()
    test_creation()
    test_print()

if __name__ == '__main__':
    main()
