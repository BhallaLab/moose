# test_hhfit.py --- 
# 
# Filename: test_hhfit.py
# Description: 
# Author: 
# Maintainer: 
# Created: Tue May 21 16:34:45 2013 (+0530)
# Version: 
# Last-Updated: Tue May 21 16:37:28 2013 (+0530)
#           By: subha
#     Update #: 9
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
# Tue May 21 16:34:53 IST 2013 - Subha moved code from
# test_converter.py to test_hhfit.py.

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

from __future__ import print_function
import os
import numpy as np
import uuid
import unittest
import pylab
import hhfit

class TestFindRateFn(unittest.TestCase):
    def setUp(self):
        self.vmin = -120e-3
        self.vmax = 40e-3
        self.vdivs = 640
        self.v_array = np.linspace(self.vmin, self.vmax, self.vdivs+1)
        # Parameters for sigmoid function - from traub2005, NaF->m_inf
        p_sigmoid = (1.0, 1/-10e-3, -38e-3, 0.0)
        self.sigmoid = p_sigmoid[0] / (1.0 + np.exp(p_sigmoid[1] * (self.v_array - p_sigmoid[2]))) + p_sigmoid[3]
        self.p_sigmoid = p_sigmoid
        # Parameters for exponential function - from traub2005, KC->n_inf
        p_exp = (2e3, 1/-27e-3, -53.5e-3, 0.0)
        self.exp = p_exp[0] * np.exp(p_exp[1] * (self.v_array - p_exp[2])) + p_exp[3]
        self.p_exp = p_exp
        # Parameters for linoid function: alpha_n from original Hodgkin-Huxley K channel.
        p_linoid = (-0.01*1e3, -1/10e-3, 10e-3, 0.0)
        self.linoid = p_linoid[3] + p_linoid[0] * (self.v_array - p_linoid[2]) / (np.exp(p_linoid[1] * (self.v_array - p_linoid[2])) - 1)
        self.p_linoid = p_linoid
        # This is tau_m of transient Ca2+ current (eq. 7) from
        # Huguenard and McCormick, J Neurophysiol, 68:1373-1383,
        # 1992.;
        #1e-3 * (0.612 + 1 / (np.exp((self.v_array*1e3 + 132)/-16.7) + np.exp((self.v_array*1e3 + 16.8)/18.2)))
        p_dblexp = (1e-3, -1/16.7e-3, -132e-3, 1/18.2e-3, -16.8e-3, 0.612e-3)
        self.dblexp = p_dblexp[5] + p_dblexp[0] / (np.exp(p_dblexp[1] * (self.v_array - p_dblexp[2])) + 
                                                        np.exp(p_dblexp[3] * (self.v_array - p_dblexp[4])))
        self.p_dblexp = p_dblexp

    def test_sigmoid(self):
        print('Testing sigmoid')
        fn, params = hhfit.find_ratefn(self.v_array, self.sigmoid)
        print('Sigmoid params original:', self.p_sigmoid, 'detected:', params)
        pylab.plot(self.v_array, self.sigmoid, 'y-', 
                   self.v_array, hhfit.sigmoid(self.v_array, *self.p_sigmoid), 'b--', 
                   self.v_array, fn(self.v_array, *params), 'r-.')
        pylab.legend(('original sigmoid', 'computed', 'fitted %s' % (fn)))
        pylab.show()
        self.assertEqual(hhfit.sigmoid, fn)
        rms_error = np.sqrt(np.mean((self.sigmoid - fn(self.v_array, *params))**2))
        self.assertAlmostEqual(rms_error/max(abs(self.sigmoid)), 0.0, places=3)

    def test_exponential(self):
        print('Testing exponential')
        fn, params = hhfit.find_ratefn(self.v_array, self.exp)
        print('Exponential params original:', self.p_exp, 'detected:', params)
        fnval = hhfit.exponential(self.v_array, *params)
        pylab.plot(self.v_array, self.exp, 'y-',
                   self.v_array, hhfit.exponential(self.v_array, *self.p_exp), 'b--',
                   self.v_array, fnval, 'r-.')
        pylab.legend(('original exp', 'computed', 'fitted %s' % (fn)))
        pylab.show()
        self.assertEqual(hhfit.exponential, fn)
        # The same exponential can be satisfied by an infinite number
        # of parameter values. Hence we cannot compare the parameters,
        # but only the fit
        rms_error = np.sqrt(np.sum((self.exp - fnval)**2))
        # pylab.plot(self.v_array, self.exp, 'b-')
        # pylab.plot(self.v_array, fnval, 'r-.') 
        # pylab.show()
        print(rms_error, rms_error/max(self.exp))
        self.assertAlmostEqual(rms_error/max(self.exp), 0.0, places=3)

    def test_linoid(self):
        print('Testing linoid')
        fn, params = hhfit.find_ratefn(self.v_array, self.linoid)
        print('Linoid params original:', self.p_linoid, 'detected:', params)
        pylab.plot(self.v_array, self.linoid, 'y-', 
                   self.v_array, hhfit.linoid(self.v_array, *self.p_linoid), 'b--',
                   self.v_array, fn(self.v_array, *params), 'r-.')
        pylab.legend(('original linoid', 'computed', 'fitted %s' % (fn)))
        pylab.show()
        self.assertEqual(hhfit.linoid, fn)
        fnval = fn(self.v_array, *params)
        rms_error = np.sqrt(np.mean((self.linoid - fnval)**2))
        self.assertAlmostEqual(rms_error/max(self.linoid), 0.0, places=3)
        # errors = params - np.array(self.p_linoid)
        # for orig, err in zip(self.p_linoid, errors):
        #     self.assertAlmostEqual(abs(err/orig), 0.0, places=2)

    def test_dblexponential(self):
        print('Testing double exponential')
        fn, params = hhfit.find_ratefn(self.v_array, self.dblexp)
        fnval = fn(self.v_array, *params)
        pylab.plot(self.v_array, self.dblexp, 'y-', 
                   self.v_array, hhfit.double_exp(self.v_array, *self.p_dblexp), 'b--',
                   self.v_array, fnval, 'r-.')
        pylab.legend(('original dblexp', 'computed', 'fitted %s' % (fn)))
        pylab.show()
        self.assertEqual(hhfit.double_exp, fn)
        rms_error = np.sqrt(np.mean((self.dblexp - fnval)**2))
        print(params, rms_error)
        self.assertAlmostEqual(rms_error/max(self.dblexp), 0.0, places=3)


if __name__ == '__main__':
    unittest.main()
        

# 
# test_hhfit.py ends here
