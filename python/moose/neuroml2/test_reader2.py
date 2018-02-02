# -*- coding: utf-8 -*-
# test_reader.py ---
#
# Filename: test_reader2.py
# Description:
# Author:
# Maintainer: P Gleeson
# Version:
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

from __future__ import print_function
import unittest
import moose
from reader import NML2Reader
import neuroml as nml

class TestPassiveCell(unittest.TestCase):
    def setUp(self):
        self.reader = NML2Reader(verbose=True)
        
        self.lib = moose.Neutral('/library')
        self.filename = 'test_files/passiveCell.nml'
        print('Loading: %s'%self.filename)
        self.reader.read(self.filename)
        for ncell in self.reader.nml_to_moose:
            if isinstance(ncell, nml.Cell):
                self.ncell = ncell
                break
        self.mcell = moose.element('/library/%s'%self.ncell.id)
        self.soma = moose.element(self.mcell.path + '/soma')
        
                
    def test_basicLoading(self):
        pass
        self.assertEqual(self.reader.filename, self.filename, 'filename was not set')
        self.assertIsNotNone(self.reader.doc, 'doc is None')
    
    def test_createCellPrototype(self):
        self.assertIsInstance(self.mcell, moose.Neuron)
        self.assertEqual(self.mcell.name, self.ncell.id)
        
    def test_createMorphology(self):
        for comp_id in moose.wildcardFind(self.mcell.path + '/##[ISA=Compartment]'):
            comp = moose.element(comp_id)
            p0 = self.reader.moose_to_nml[comp].proximal
            if p0:
                self.assertAlmostEqual(comp.x0, float(p0.x)*1e-6) # Assume micron unit for length
                self.assertAlmostEqual(comp.y0, float(p0.y)*1e-6)
                self.assertAlmostEqual(comp.z0, float(p0.z)*1e-6)
            p1 = self.reader.moose_to_nml[comp].distal
            self.assertAlmostEqual(comp.x, float(p1.x)*1e-6)
            self.assertAlmostEqual(comp.y, float(p1.y)*1e-6)
            self.assertAlmostEqual(comp.z, float(p1.z)*1e-6)

    def test_capacitance(self):
        for comp_id in moose.wildcardFind(self.mcell.path + '/##[ISA=Compartment]'):
            comp = moose.element(comp_id)
            # We know that a few um^2 compartment with uF/cm^2 specific capacitance must be around a pico Farad.
            self.assertTrue((comp.Cm > 0) and (comp.Cm < 1e-6))
            
    def test_protochans(self):
        """TODO: verify the prototype cahnnel."""
        for chan_id in moose.wildcardFind('/library/##[ISA=HHChannel]'):
            print(moose.element(chan_id))

if __name__ == '__main__':
    unittest.main()
    #p = TestPassiveCell()

#
# test_reader.py ends here
