# singlemsgcross.py ---
#
# Filename: singlemsgcross.py
# Description:
# Author:Subhasis Ray
# Maintainer:
# Created: Wed May  1 11:05:30 2013 (+0530)
# Version:
# Last-Updated: Wed May  1 12:16:14 2013 (+0530)
#           By: subha
#     Update #: 43
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

def test_crossing_single():
    """This function creates an ematrix of two PulseGen elements and
    another ematrix of two Table elements.

    The two pulsegen elements have same amplitude but opposite phase.

    Table[0] is connected to PulseGen[1] and Table[1] to Pulsegen[0].

    In the plot you should see two square pulses of opposite phase.

    """
    size = 2
    pg = moose.PulseGen('pulsegen', size)
    for ix, ii in enumerate(pg.vec):
        pulse = moose.element(ii)
        pulse.delay[0] = 1.0
        pulse.width[0] = 2.0
        pulse.level[0] = (-1)**ix
    tab = moose.Table('table', size)
    moose.connect(tab.vec[0], 'requestOut', pg.vec[1], 'getOutputValue', 'Single')
    moose.connect(tab.vec[1], 'requestOut', pg.vec[0], 'getOutputValue', 'Single')
    print('Neighbors:')
    for t in tab.vec:
        print((t.path))
        for n in moose.element(t).neighbors['requestOut']:
            print(('requestOut <-', n.path))
    moose.setClock(0, 0.1)
    moose.useClock(0, '/##', 'process')
    moose.start(5)
    for ii in tab.vec:
        t = moose.Table(ii).vector
        print((len(t)))
        pylab.plot(t)
    pylab.show()

def main():
    """
    This example shows that you can have two ematrix objects and
    connect individual elements using `Single` message
    """
    test_crossing_single()

if __name__ == '__main__':
    main()

#
# singlemsgcross.py ends here
