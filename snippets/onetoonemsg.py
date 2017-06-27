# onetoonemsg.py ---
#
# Filename: onetoonemsg.py
# Description:
# Author:Subhasis Ray
# Maintainer:
# Created: Wed May  1 11:05:30 2013 (+0530)
# Version:
# Last-Updated: Wed May  1 11:12:07 2013 (+0530)
#           By: subha
#     Update #: 24
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

import pylab
import sys
sys.path.append('../../python')
import moose

def test_one_to_one(size=2):
    """
    Demonstrates one-to-one connection using the 'connect' function of MOOSE.
    """
    pg = moose.PulseGen('pulsegen', size)
    for ix, ii in enumerate(pg.vec):
        pulse = moose.element(ii)
        pulse.delay[0] = 1.0
        pulse.width[0] = 2.0
        pulse.level[0] = (-1)**ix
    tab = moose.Table('pulseamp', size)
    moose.connect(tab, 'requestOut', pg, 'getOutputValue', 'OneToOne')
    moose.setClock(0, 0.1)
    moose.useClock(0, '/##', 'process')
    moose.start(5)
    for ii in tab.vec:
        t = moose.Table(ii).vector
        pylab.plot(t)
    pylab.show()

def main():
    """
Demonstrates one-to-one connection between objects through ''moose.connect''.
    """
    test_one_to_one()

#
# onetoonemsg.py ends here
