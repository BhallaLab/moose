# -*- coding: utf-8 -*-
# compare_moose_nrn.py ---
#
# Filename: compare_moose_nrn.py
# Description:
# Author: subha
# Maintainer:
# Created: Sat Apr 30 02:43:29 2016 (-0400)
# Version:
# Last-Updated: Sat Apr 30 03:01:04 2016 (-0400)
#           By: subha
#     Update #: 19
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

import pylab as pl

nrn = pl.loadtxt('h10.CNG.nrn.txt')
print(nrn.shape)
ee = pl.loadtxt('h10.CNG.moose.ee.txt')
hs = pl.loadtxt('h10.CNG.moose.hsolve.txt')
pl.plot(nrn[:, 0], nrn[:, 1], label='NEURON soma')
pl.plot(nrn[:, 0], nrn[:, 2], label='NEURON apical[90]')
pl.plot(ee[:, 0]*1e3, ee[:, 1]*1e3, label='MOOSE EE soma')
pl.plot(ee[:, 0]*1e3, ee[:, 2]*1e3, label='MOOSE EE apical_e_177_0')
pl.plot(hs[:, 0]*1e3, hs[:, 1]*1e3, label='MOOSE HSolve soma')
pl.plot(hs[:, 0]*1e3, hs[:, 2]*1e3, label='MOOSE HSolve apical_e_177_0')
pl.legend()
pl.show()


#
# compare_moose_nrn.py ends here
