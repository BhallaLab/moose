# test_hsolve_tcr.py --- 
# 
# Filename: test_hsolve_tcr.py
# Description: 
# Author: 
# Maintainer: 
# Created: Wed Jun 12 11:10:44 2013 (+0530)
# Version: 
# Last-Updated: Tue Jul  9 18:34:01 2013 (+0530)
#           By: subha
#     Update #: 195
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
import sys
import os
from datetime import datetime
import unittest
import numpy as np
import pylab

import moose
import config

from cell_test_util import setup_current_step_model, SingleCellCurrentStepTest
import testutils
import cells
from moose import utils

simdt = 1e-6
plotdt = 0.25e-3
simtime = 100e-3
    

# pulsearray = [[1.0, 100e-3, 1e-9],
#               [0.5, 100e-3, 0.3e-9],
#               [0.5, 100e-3, 0.1e-9],
#               [0.5, 100e-3, -0.1e-9],
#               [0.5, 100e-3, -0.3e-9]]

pulsearray = [[50e-3, 20e-3, 1e-9],
              [1e9, 0, 0]]

class TestHSolveEETCR(SingleCellCurrentStepTest):
    """Run HSolve and Exponential solver simulation of the same cell."""
    celltype = 'TCR'
    def testHSolve(self):
        self.schedule(simdt, plotdt, 'hsolve')
        self.runsim(simtime, pulsearray=pulsearray)
        self.savedata()

    def testEE(self):
        self.schedule(simdt, plotdt, 'ee')
        self.runsim(simtime, pulsearray=pulsearray)
        self.savedata()

if __name__ == '__main__':
    unittest.main()




# 
# test_hsolve_tcr.py ends here
