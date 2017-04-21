
# pulsegen.py ---
#
# Filename: pulsegen.py
# Description:
# Author:Subhasis Ray
# Maintainer:
# Created: Sat Jun 23 10:26:25 2012 (+0530)
# Version:
# Last-Updated: Sat Jun 23 13:51:28 2012 (+0530)
#           By: subha
#     Update #: 34
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
sys.path.append('../../python')
import moose
from matplotlib import pyplot as plt
import numpy as np

def multilevel_pulsegen():
    pg = moose.PulseGen('pulsegen')
    pg.count = 5
    for ii in range(pg.count):
        pg.level[ii] = ii+1
        pg.width[ii] = 0.1
        pg.delay[ii] = 0.5 * (ii+1)
    tab = moose.Table('tab')
    moose.connect(tab, 'requestOut', pg, 'getOutputValue')
    moose.setClock(0, 0.01)
    moose.useClock(0, '%s,%s' % (pg.path, tab.path), 'process')
    moose.reinit()
    moose.start(20.0)
    plt.plot(tab.vector)
    plt.show()

def main():
    """
    Demonstrates a pulsegen with multiple levels, delays and
    widths.
    """
    multilevel_pulsegen()

if __name__ == '__main__':
    main()


#
# pulsegen.py ends here
