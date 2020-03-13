# -*- coding: utf-8 -*-
# This file is part of MOOSE simulator: http://moose.ncbs.res.in.

# MOOSE is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# MOOSE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with MOOSE.  If not, see <http://www.gnu.org/licenses/>.


"""moose_constants.py:

Last modified: Sat Jan 18, 2014  05:01PM

"""

__author__           = "Dilawar Singh"
__license__          = "GNU GPL"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"

## for Ca Pool
# FARADAY = 96154.0
# Coulombs
# from cadecay.mod : 1/(2*96154.0) = 5.2e-6 which is the Book of Genesis / readcell value
FARADAY = 96485.3415 # Coulombs/mol # from Wikipedia

## Table step_mode
# table acts as lookup - default mode
TAB_IO = 0

# table outputs value until it reaches the end and then stays at the last value
TAB_ONCE = 2

# table acts as a buffer: succesive entries at each time step
TAB_BUF = 3

# table acts as a buffer for spike times. Threshold stored in the pymoose 'stepSize' field.
TAB_SPIKE = 4

## Table fill modes
BSplineFill = 0 # B-spline fill (default)
CSplineFill = 1 # C_Spline fill (not yet implemented)
LinearFill = 2 # Linear fill

## clock 0 is for init & hsolve
## The ee method uses clocks 1, 2.
## hsolve & ee use clock 3 for Ca/ion pools.
## clocks 4 and 5 are for biochemical simulations.
## clock 6 for lookup tables, clock 7 for stimuli
## clocks 8 and 9 for tables for plots.
INITCLOCK = 0
ELECCLOCK = 1
CHANCLOCK = 2
POOLCLOCK = 3
LOOKUPCLOCK = 6
STIMCLOCK = 7
PLOTCLOCK = 8
