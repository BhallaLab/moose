# tabledemo.py ---
#
# Filename: tabledemo.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Fri Aug 29 18:26:20 2014 (+0530)
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

def example():
    pg = moose.PulseGen('pulse')
    pg.delay[0] = 1.0
    pg.width[0] = 0.2
    pg.level[0] = 0.5
    tab = moose.Table('tab')
    moose.connect(tab, 'requestOut', pg, 'getOutputValue')
    moose.setClock(0, 0.01)
    moose.setClock(1, 0.01)
    moose.useClock(0, pg.path, 'process')
    moose.useClock(1, tab.path, 'process')
    moose.reinit()
    moose.start(5.0)
    tab.plainPlot('output_tabledemo.csv')

def main():
    """:class:`Table` can be used for recording and saving data in ascii text formats.
    In this example we create a square-pulse generator object and
    record the output using a table.

    The steps are:

    1. Create a PulseGen element `pulse`.

    2. Set `delay[0]=1.0`, `width[0]=0.2`, `level[0]=0.5`, so it
       generates 0.2 s wide square pulses with 0.5 amplitude every 1 s.

    3. Create a Table element `tab`.

    4. Connect the `outputValue` field of `pulse` to `tab`.

    5. We set tick-interval of ticks 0 and 1 to 0.01 and schedule
       `pulse` on tick 0 and `tab` on tick 1.

    6. Run the simulation for 5 s and save data to the ascii file
       `output_tabledemo.csv`.
    """
    example()

if __name__ == '__main__':
    example()


#
# tabledemo.py ends here
