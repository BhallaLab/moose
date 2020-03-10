# -*- coding: utf-8 -*-

# Author: Subha
# Maintainer: Dilawar Singh
# Created: Tue May 21 16:34:45 2013 (+0530)
# This test is fragile.

from __future__ import print_function, division, absolute_import
import numpy as np
import unittest
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import moose.neuroml2.hhfit as hhfit

np.random.seed(10)


class TestFindRateFn(unittest.TestCase):
    def setUp(self):
        self.vmin = -120e-3
        self.vmax = 40e-3
        self.vdivs = 640
        self.v_array = np.linspace(self.vmin, self.vmax, self.vdivs + 1)
        # Parameters for sigmoid function - from traub2005, NaF->m_inf
        p_sigmoid = (1.0, 1 / -10e-3, -38e-3, 0.0)
        self.sigmoid = p_sigmoid[0] / (
            1.0 + np.exp(p_sigmoid[1] *
                         (self.v_array - p_sigmoid[2]))) + p_sigmoid[3]
        self.p_sigmoid = p_sigmoid
        # Parameters for exponential function - from traub2005, KC->n_inf
        p_exp = (2e3, 1 / -27e-3, -53.5e-3, 0.0)
        self.exp = p_exp[0] * np.exp(p_exp[1] *
                                     (self.v_array - p_exp[2])) + p_exp[3]
        self.p_exp = p_exp
        # Parameters for linoid function: alpha_n from original Hodgkin-Huxley K channel.
        p_linoid = (-0.01 * 1e3, -1 / 10e-3, 10e-3, 0.0)
        self.linoid = p_linoid[3] + p_linoid[0] * \
                (self.v_array - p_linoid[2]) / (np.exp(p_linoid[1] * (self.v_array - p_linoid[2])) - 1)
        self.p_linoid = p_linoid
        # This is tau_m of transient Ca2+ current (eq. 7) from
        # Huguenard and McCormick, J Neurophysiol, 68:1373-1383,
        # 1992.;
        #1e-3 * (0.612 + 1 / (np.exp((self.v_array*1e3 + 132)/-16.7) + np.exp((self.v_array*1e3 + 16.8)/18.2)))
        p_dblexp = (1e-3, -1 / 16.7e-3, -132e-3, 1 / 18.2e-3, -16.8e-3,
                    0.612e-3)
        self.dblexp = p_dblexp[5] + p_dblexp[0] / (
            np.exp(p_dblexp[1] * (self.v_array - p_dblexp[2])) +
            np.exp(p_dblexp[3] * (self.v_array - p_dblexp[4])))
        self.p_dblexp = p_dblexp

    def test_sigmoid(self):
        print('Testing sigmoid')
        fn, params = hhfit.find_ratefn(self.v_array, self.sigmoid)
        print('Sigmoid params original:', self.p_sigmoid, 'detected:', params)
        self.assertEqual(hhfit.sigmoid, fn)
        rms_error = np.sqrt(
            np.mean((self.sigmoid - fn(self.v_array, *params))**2))
        self.assertAlmostEqual(rms_error / max(abs(self.sigmoid)),
                               0.0,
                               places=3)
        plt.plot(self.v_array, self.sigmoid, 'y-', self.v_array,
                 hhfit.sigmoid(self.v_array, *self.p_sigmoid), 'b--',
                 self.v_array, fn(self.v_array, *params), 'r-.')
        plt.legend('original sigmoid %s fitted %s' % (self.p_sigmoid, fn))
        plt.savefig("__test_sigmoid.png")

    def test_exponential(self):
        print('Testing exponential')
        fn, params = hhfit.find_ratefn(self.v_array, self.exp)
        print('Exponential params original:', self.p_exp, 'detected:', params)
        if params is not None:
            # The `find_ratefn` might return a parameter array for different
            # function sometimes. exponential takes only upto 5 parameters. 
            fnval = hhfit.exponential(self.v_array, *params[:4])
            self.assertEqual(hhfit.exponential, fn)
            # The same exponential can be satisfied by an infinite number
            # of parameter values. Hence we cannot compare the parameters,
            # but only the fit
            rms_error = np.sqrt(np.sum((self.exp - fnval)**2))
            print(rms_error, rms_error / max(self.exp))
            self.assertAlmostEqual(rms_error / max(self.exp), 0.0, places=3)
            plt.plot(self.v_array, self.exp, 'y-', self.v_array,
                     hhfit.exponential(self.v_array, *self.p_exp), 'b--',
                     self.v_array, fnval, 'r-.')
            plt.legend('original exp %s fitted %s' % (self.p_exp, fn))
            out = "__test_exponential.png"
            plt.savefig(out)
            print('Plot is saved saved to %s' % out)
        else:
            print("[INFO ] Failed find a suitable approximation...")
            

    def test_linoid(self):
        print('Testing linoid')
        fn, params = hhfit.find_ratefn(self.v_array, self.linoid)
        if params is not None:
            print('Linoid params original:', self.p_linoid, 'detected:', params)
            self.assertEqual(hhfit.linoid, fn)
            fnval = fn(self.v_array, *params)
            rms_error = np.sqrt(np.mean((self.linoid - fnval)**2))
            self.assertAlmostEqual(rms_error / max(self.linoid), 0.0, places=3)
            plt.plot(self.v_array, self.linoid, 'y-', self.v_array,
                     hhfit.linoid(self.v_array, *self.p_linoid), 'b--',
                     self.v_array, fn(self.v_array, *params), 'r-.')
            plt.legend('Original linoid %s fitted %s' % (self.p_linoid, fn))
            out = "__test_linoid.png"
            plt.savefig(out)
            print('Plot is saved saved to %s' % out)
        else:
            print('Failed to find a  suitable fit.')

    def test_dblexponential(self):
        print('Testing double exponential')
        fn, params = hhfit.find_ratefn(self.v_array, self.dblexp)
        fnval = fn(self.v_array, *params)
        plt.plot(self.v_array, self.dblexp, 'y-', self.v_array,
                 hhfit.double_exp(self.v_array, *self.p_dblexp), 'b--',
                 self.v_array, fnval, 'r-.')
        self.assertEqual(hhfit.double_exp, fn)
        rms_error = np.sqrt(np.mean((self.dblexp - fnval)**2))
        print(params, rms_error)
        self.assertAlmostEqual(rms_error / max(self.dblexp), 0.0, places=3)
        plt.legend('Original dblexp %s, fitted %s' %(self.dblexp, fn))
        out = "__test_dblexponential.png"
        plt.savefig(out)
        print('Plot is saved saved to %s' % out)


if __name__ == '__main__':
    unittest.main()
