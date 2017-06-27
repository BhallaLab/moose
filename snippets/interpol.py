# interpol.py ---
#
# Filename: interpol.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Wed Jun 25 16:13:26 2014 (+0530)
# Version:
# Last-Updated:
#           By:
#     Update #: 0
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
__author__ = 'Subhasis Ray'

import sys
import math
import numpy as np
from matplotlib import pyplot as plt

sys.path.append('../../python')
import moose

def main():
    """
    Example of Interpol object.
    """
    simtime = 1.0
    simdt = 0.001
    model = moose.Neutral('/model')
    data = moose.Neutral('/data')
    interpol = moose.Interpol('/model/sin')
    vec = np.sin(np.linspace(-3.14, 3.14, 100))
    interpol.vector = vec
    interpol.xmax = 3.14
    interpol.xmin = -3.14
    recorded = moose.Table('/data/output')
    moose.connect(recorded, 'requestOut', interpol, 'getY')

    stimtab = moose.StimulusTable('/model/x')
    stimtab.stepSize = 0.0
    # stimtab.startTime = 0.0
    # stimtab.stopTime = simtime
    stimtab.vector = np.linspace(-4, 4, 1000)
    print((stimtab.vector))
    print((interpol.vector))
    moose.connect(stimtab, 'output', interpol, 'input')


    moose.setClock(0, simdt)
    moose.useClock(0, '/data/##,/model/##', 'process')
    moose.reinit()
    moose.start(simtime)
    plt.plot(np.linspace(0, simtime, len(recorded.vector)), recorded.vector, 'b-+', label='interpolated')
    plt.plot(np.linspace(0, simtime, len(vec)), vec, 'r-+', label='original')
    plt.show()

if __name__ == '__main__':
    main()



#
# interpol.py ends here
