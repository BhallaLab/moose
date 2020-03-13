# -*- coding: utf-8 -*-
# Test moose.Function with namedvar and without wrapper.

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2019-, Dilawar Singh"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"

import numpy as np
import moose
print( "[INFO ] Using moose %s form %s" % (moose.version(), moose.__file__) )

def create_func( funcname, expr ):
    f = moose.Function( funcname )
    f.expr = expr
    t = moose.Table( funcname + 'tab' )
    t.connect('requestOut', f, 'getValue')
    moose.setClock( f.tick, 0.1)
    moose.setClock( t.tick, 0.1)
    return f, t

def test_var_order():
    nsteps = 5
    simtime = nsteps
    dt = 1.0
    fn1 = moose.Function('/fn1')
    fn1.expr = 'B+A+y0+y1'
    inputs = np.arange(0, nsteps+1, 1.0)
    x0 = moose.StimulusTable('/x0')
    x0.vector = inputs
    x0.startTime = 0.0
    x0.stopTime = simtime
    x0.stepPosition = 0.0
    print('x0', x0.vector)
    inputs /= 10
    x1 = moose.StimulusTable('/x1')
    x1.vector = inputs
    x1.startTime = 0.0
    x1.stopTime = simtime
    x1.stepPosition = 0.0
    print('x1', x1.vector)
    inputs /= 10
    y0 = moose.StimulusTable('/y0')
    y0.vector = inputs
    y0.startTime = 0.0
    y0.stopTime = simtime
    y0.stepPosition = 0.0
    print('y0', y0.vector)
    inputs /= 10
    y1 = moose.StimulusTable('/y1')
    y1.vector = inputs
    print('y1', y1.vector)
    y1.startTime = 0.0
    y1.startTime = 0.0
    y1.stopTime = simtime
    y1.stepPosition = 0.0
    moose.connect(x0, 'output', fn1['A'], 'input')
    moose.connect(x1, 'output', fn1['B'], 'input')
    moose.connect(fn1, 'requestOut', y0, 'getOutputValue')
    moose.connect(fn1, 'requestOut', y1, 'getOutputValue')

    z1 = moose.Table('/z1')
    moose.connect(z1, 'requestOut', fn1, 'getValue')
    for ii in range(32):
        moose.setClock(ii, dt)
    moose.reinit()
    moose.start(simtime)
    expected = [0, 1.1, 2.211, 3.322, 4.433, 5.544]
    assert np.allclose(z1.vector, expected), "Excepted %s, got %s" % (expected, z1.vector )
    print( 'Passed order vars' )

def test_t( ):
    f, t = create_func( 'funct', 't/2.0')
    moose.reinit()
    moose.start(1)
    y = t.vector
    d = np.diff( y[1:] ) 
    assert np.mean(d) == d[0]
    print( 'Passed t/2' )

def test_trig( ):
    f, t = create_func('func2', '(sin(t)^2+cos(t)^2)-1')
    moose.reinit()
    moose.start( 1 )
    y = t.vector
    print(y)
    assert np.isclose(np.mean(y),  0.0), np.mean(y)
    assert np.isclose(np.std(y), 0.0), np.std(y)
    print( 'Passed sin^2 x + cos^x=1' )

def test_rand( ):
    moose.seed( 10 )
    f, t = create_func( 'random', 'rnd()')
    moose.reinit()
    moose.start(1)
    expected = [0.29876116, 0.49458993, 0.83191136, 0.02517173, 0.26556613,
            0.15037787, 0.81660184, 0.89081653, 0.03061665, 0.72743551, 0.13145815]
    assert np.isclose(t.vector, expected ).all(), "Expected %s, got %s" % (
            expected, t.vector)
    print( 'Passed test random' )

def test_fmod( ):
    f, t = create_func( 'fmod', 'fmod(t, 2)' )
    moose.reinit()
    moose.start(20)
    y = t.vector
    #print(y)
    assert (np.fmod(y, 2) == y).all()
    assert(np.isclose(np.max(y), 1.9)), "Expected 1.9 got %s" % np.max(y)
    assert(np.isclose(np.min(y), 0.0)), "Expected 0.0 got %s" % np.min(y)
    print('Passed fmod(t,2)')

def main():
    test_var_order()
    test_t()
    test_trig()
    test_rand()
    test_fmod()

if __name__ == '__main__':
    main()
