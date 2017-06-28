# loadCspaceModel.py ---
#
# Filename: loadCspaceModel.py
# Description:
# Author: Upi Bhalla
# Maintainer:
# Created: Sat Oct 04 10:14:15 2014 (+0530)
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

import math
import pylab
import numpy
import moose
def main():
    """ This example illustrates loading and running, a kinetic model 
     defined in cspace format. We use the gsl solver here. The model already
     defines a couple of plots and sets the runtime to 3000 seconds. 
    """
    # This command loads the file into the path '/model', and tells
    # the system to use the gsl solver.
    modelId = moose.loadModel( 'Osc.cspace', 'model', 'gsl' )
    moose.reinit()
    moose.start( 3000.0 ) # Run the model for 300 seconds.

    # display all plots
    for x in moose.wildcardFind( '/model/graphs/#' ):
        t = numpy.arange( 0, x.vector.size, 1 ) #sec
        pylab.plot( t, x.vector, label=x.name )
    pylab.legend()
    pylab.show()

    # moose.saveModel( modelId, 'saveReaction.g' )
    quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main()
