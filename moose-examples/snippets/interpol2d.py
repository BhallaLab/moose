# interpol2d.py ---
#
# Filename: interpol2d.py
# Description:
# Author:Subhasis Ray
# Maintainer:
# Created: Thu Jun 28 15:19:46 2012 (+0530)
# Version:
# Last-Updated: Thu Jun 28 17:11:42 2012 (+0530)
#           By: subha
#     Update #: 49
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
"""Example of Interpol object in 2 dimensions."""

import numpy as np
import sys
sys.path.append('../../python')
import moose


def interpolation_demo():
    """ Example of Interpol object in 2-dimension."""
    interpol = moose.Interpol2D('/interpol2D')
    interpol.xmin = 0.0
    interpol.xmax = 1.0
    interpol.ymin = 0.0
    interpol.ymax = 1.0
    # Make a 50 element array with entries at equal distance from
    # [0,1) and reshape it into a 10x5 matrix and assign to table.
    matrix = np.linspace(0, 1.0, 50).reshape(10, 5)
    print('Setting table to')
    print(matrix)
    interpol.tableVector2D = matrix
    # interpolating beyond top left corner.
    # value should be
    pos = (0.8, 0.3)

    print(('Interpolated value at', pos))
    print((interpol.z[pos[0], pos[1]]))

    print(('Point going out of bound on both x and y', interpol.z[1.1, 1.1]))
    print(('Point going out of bound on both x and y', interpol.z[0.5, 1.1]))

if __name__ == '__main__':
    interpolation_demo()
#
# interpol2d.py ends here
