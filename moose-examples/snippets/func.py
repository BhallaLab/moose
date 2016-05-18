# func.py --- 
# 
# Filename: func.py
# Description: 
# Author: Subhasis Ray
# Maintainer: Subhasis Ray
# Created: Mon May 27 17:45:05 2013 (+0530)
# Version: 
# Last-Updated: Sat Jun  1 19:07:20 2013 (+0530)
#           By: subha
#     Update #: 253
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# 
# 
# 

# Change log:
# 
# 
# 
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
# 
# 

# Code:
"""Demonstrate the use of Func class"""
import numpy as np
import pylab 

import moose

def test_func_nosim():
    """Create a Func object for computing function values without running
    a simulations."""
    # func_0 demonstrates multivariable function
    lib = moose.Neutral('/library')
    func_0 = moose.Func('%s/func_0' % (lib.path))
    func_0.mode = 1
    num = 5
    expr = 'avg('
    for ii in range(num-1):
        expr += 'x_%d, ' % (ii)
    expr += 'x_%d)' % (num-1)
    print 'Expression:', expr    
    func_0.expr = expr
    for ii in range(num):
        var = 'x_%d' % (ii)
        print 'Setting:', var, '=', func_0.var[var]
        func_0.var[var] = float(ii)
    print 'Expression:', func_0.expr
    print 'Variables after assignment:'
    for v in func_0.vars:
        print '  %s = %g' % (v, func_0.var[v])
    print 'value %g\n' % (func_0.value)

def test_func():
    """This function creates a Func object evaluating a function of a
    single variable. It both shows direct evaluation without running a
    simulation and a case where the x variable comes from another
    source.

    """
    model = moose.Neutral('/model')
    data = moose.Neutral('/data')

    func_1 = moose.Func('%s/func_1' % (model.path))
    func_1.mode = 3 # mode = 1 : value, mode = 2 : derivative
    # Expression is that for tau_m in Traub's NaF channel model
    func_1.expr = 'x < -30e-3? 1.0e-3 * (0.025 + 0.14 * exp((x + 30.0e-3) / 10.0e-3)): 1.0e-3 * (0.02 + 0.145 * exp(( - x - 30.0e-3) / 10.0e-3))'
    # First we display the use of Func as a standalone funculator
    xarr = np.linspace(-120e-3, 40e-3, 1000)
    values = []
    deriv = []
    for x in xarr:
        func_1.var['x'] = x
        values.append(func_1.value)
        deriv.append(func_1.derivative)
    pylab.plot(xarr, values, 'g-', label='f(no-sim)')
    pylab.plot(xarr, np.array(deriv)/1000, 'k-.', label="1e-3 * f'(no-sim)")

    
    simdt = xarr[1] - xarr[0]
    input = moose.StimulusTable('%s/xtab' % (model.path))
    input.vector = xarr
    input.startTime = 0.0
    input.stepPosition = xarr[0]
    input.stopTime = xarr[-1] - xarr[0]
    print input.startTime, input.stopTime
    
    moose.connect(input, 'output', func_1, 'xIn')

    x_tab = moose.Table('/data/xtab')
    moose.connect(x_tab, 'requestOut', input, 'getOutputValue')

    y_tab = moose.Table('%s/y' % (data.path))
    moose.connect(y_tab, 'requestOut', func_1, 'getValue')
    yprime_tab = moose.Table('%s/yprime' % (data.path))
    moose.connect(yprime_tab, 'requestOut',
                  func_1, 'getDerivative')
    func_1.mode = 3 # This forces both f ad f' to be computed and sent out
    moose.setClock(0, simdt)
    moose.setClock(1, simdt)
    moose.setClock(2, simdt)
    moose.setClock(3, simdt)
    moose.useClock(0, '%s/##[TYPE=StimulusTable]' % (model.path), 'process')
    moose.useClock(1, '%s/##[TYPE=Func]' % (model.path), 'process')
    moose.useClock(2, '%s/##[TYPE=DiffAmp]' % (model.path), 'process')    
    moose.useClock(3, '%s/##' % (data.path), 'process')
    moose.reinit()
    t = xarr[-1] - xarr[0]
    print 'Run for', t
    moose.start(t)
    y = np.asarray(y_tab.vector)
    yp = np.asarray(yprime_tab.vector)
    pylab.plot(x_tab.vector, y, 'r-.', label='f(x)')
    pylab.plot(x_tab.vector, yp/1000, 'b--', label="1e-3 * f'(x)")
    pylab.legend()
    pylab.show()

if __name__ == '__main__':
    test_func_nosim()
    test_func()


# 
# func.py ends here
