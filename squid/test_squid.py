# test_squid.py --- 
# 
# Filename: test_squid.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Wed Feb 22 00:41:36 2012 (+0530)
# Version: 
# Last-Updated: Wed Feb 22 00:41:51 2012 (+0530)
#           By: Subhasis Ray
#     Update #: 2
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

# Code:

import unittest
import pylab
import numpy
from squid import SquidModel

class SquidAxonTest(unittest.TestCase):
    def setUp(self):
        self.vrange = numpy.linspace(SquidModel.VMIN, SquidModel.VMAX, SquidModel.VDIVS+1)        
        self.model = SquidModel('testSquidAxon')
        
    def calc_alpha_beta(self, params, table='A'):        
        denominator = params[table+'_C'] + numpy.exp((self.vrange+params[table+'_D'])/params[table+'_F'])
        numerator = params[table+'_A'] + params[table+'_B'] * self.vrange
        y = numpy.zeros(len(self.vrange))
        singularities = numpy.nonzero(denominator == 0.0)[0]
        self.assertLessEqual(len(singularities), 1)
        if len(singularities) == 1:
            y[:singularities[0]] = numerator[:singularities[0]]/denominator[:singularities[0]]
            y[singularities[0]] = params[table+'_B'] * params[table+'_F']
            y[singularities[0]+1:] = numerator[singularities[0]+1:]/denominator[singularities[0]+1:]
        elif len(singularities) == 0:
            y[:] = numerator[:]/denominator[:]
        return y
        
    def test_Na_alpha_m(self):
        alpha_m = self.calc_alpha_beta(SquidModel.Na_m_params, 'A')
        difference = numpy.sqrt(numpy.mean((alpha_m - self.model.Na_channel.alpha_m)**2))
        pylab.title('Na_alpha_m')        
        pylab.plot(alpha_m, label='python')
        pylab.plot(self.model.Na_channel.alpha_m, label='moose')
        pylab.legend()
        pylab.show()
        self.assertLessEqual(difference, numpy.mean(alpha_m)*1e-6)
        
    def test_Na_beta_m(self):
        beta_m = self.calc_alpha_beta(SquidModel.Na_m_params, 'B')
        difference = numpy.sqrt(numpy.mean((beta_m - self.model.Na_channel.beta_m)**2))
        pylab.title('Na_beta_m')
        pylab.plot(beta_m, label='python')
        pylab.plot(self.model.Na_channel.beta_m, label='moose')
        pylab.legend()
        pylab.show()
        self.assertLessEqual(difference, numpy.mean(beta_m)*1e-6)

    def test_Na_alpha_h(self):
        alpha_h = self.calc_alpha_beta(SquidModel.Na_h_params, 'A')
        difference = numpy.sqrt(numpy.mean((alpha_h - self.model.Na_channel.alpha_h)**2))
        pylab.title('Na_alpha_h')
        pylab.plot(alpha_h, label='python')
        pylab.plot(self.model.Na_channel.alpha_h, label='moose')
        pylab.legend()
        pylab.show()
        self.assertLessEqual(difference, numpy.mean(alpha_h)*1e-6)

    def test_Na_beta_h(self):
        beta_h = self.calc_alpha_beta(SquidModel.Na_h_params, 'B')
        difference = numpy.sqrt(numpy.mean((beta_h - self.model.Na_channel.beta_h)**2))
        pylab.title('Na_beta_h')
        pylab.plot(beta_h, label='python')
        pylab.plot(self.model.Na_channel.beta_h, label='moose')
        pylab.legend()
        pylab.show()
        self.assertLessEqual(difference, numpy.mean(beta_h)*1e-6)

    def test_K_alpha_m(self):
        alpha_m = self.calc_alpha_beta(SquidModel.K_n_params, 'A')
        difference = numpy.sqrt(numpy.mean((alpha_m - self.model.K_channel.alpha_m)**2))
        pylab.title('K_alpha_n')
        pylab.plot(alpha_m, label='python')
        pylab.plot(self.model.K_channel.alpha_m, label='moose')
        pylab.legend()
        pylab.show()
        self.assertLessEqual(difference, numpy.mean(alpha_m)*1e-6)

    def test_K_beta_m(self):
        beta_m = self.calc_alpha_beta(SquidModel.K_n_params, 'B')
        difference = numpy.sqrt(numpy.mean((beta_m - self.model.K_channel.beta_m)**2))
        pylab.title('K_beta_n')
        pylab.plot(beta_m, label='python')
        pylab.plot(self.model.K_channel.beta_m, label='moose')
        pylab.legend()
        pylab.show()
        self.assertLessEqual(difference, numpy.mean(beta_m)*1e-6)


# 
# test_squid.py ends here
