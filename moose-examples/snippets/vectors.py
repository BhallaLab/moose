# vectors.py ---
#
# Filename: vectors.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Tue May  6 12:49:53 2014 (+0530)
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

import sys
sys.path.append('../../python')
import moose
import pylab as pl
import numpy as np

def main():
    """Demonstrates how to use vectors of moose elements"""
    comps = moose.vec(path='mycomps', n=2800, dtype='Compartment')
    comps.Em = np.random.sample(len(comps))
    pl.plot(comps.Em)
    pl.show()

if __name__ == '__main__':
    main()
#
# vectors.py ends here
