# loadKineticModel.py ---
#
# Filename: loadKineticModel.py
# Description:
# Author: Upi Bhalla
# Maintainer:
# Created: Sat Oct 04 12:14:15 2014 (+0530)
# Version:
# Last-Updated: Tue Apr 18 17:40:00 2017(+0530)
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

import moose
import pylab
import numpy
import sys
import os
def main():
    """
    This example illustrates loading, running, and saving a kinetic
    model defined in kkit format. It uses a default kkit model but
    you can specify another using the command line
    ``python loadKineticModel.py filepath runtime solver``.
    We use default solver as gsl.
    The model already defines a couple of plots and sets the runtime 20 secs.
    """
    
    defaultsolver = "gsl"  # Pick any of gsl, gssa, ee..
    defaultfile = '../genesis/kkit_objects_example.g'
    defaultruntime = 20.0
    
    try:
        sys.argv[1]
    except IndexError:
        filepath = defaultfile
    else:
        filepath = sys.argv[1]
    if not os.path.exists(filepath):
        print ("Filename or path does not exist \"%s\" loading default file \"%s\" " %(filepath ,defaultfile))
        filepath = defaultfile
    
    try:
        sys.argv[2]
    except :
        runtime = defaultruntime
    else:
        runtime = float(sys.argv[2])

    try:
        sys.argv[3]
    except :
        solver = defaultsolver
    else:
        solver = sys.argv[3]
    
    modelId = moose.loadModel( filepath, 'model', solver )
    
    # Increase volume so that the stochastic solver gssa
    # gives an interesting output
    #compt = moose.element( '/model/kinetics' )
    #compt.volume = 1e-19

    moose.reinit()
    moose.start( runtime )

    # Report parameters
    '''
    for x in moose.wildcardFind( '/model/kinetics/##[ISA=PoolBase]' ):
            print x.name, x.nInit, x.concInit
    for x in moose.wildcardFind( '/model/kinetics/##[ISA=ReacBase]' ):
            print x.name, 'num: (', x.numKf, ', ',  x.numKb, '), conc: (', x.Kf, ', ', x.Kb, ')'
    for x in moose.wildcardFind('/model/kinetics/##[ISA=EnzBase]'):
            print x.name, '(', x.Km, ', ',  x.numKm, ', ', x.kcat, ')'
            '''

    # Display all plots.
    for x in moose.wildcardFind( '/model/#graphs/conc#/#' ):
        t = numpy.arange( 0, x.vector.size, 1 ) * x.dt
        pylab.plot( t, x.vector, label=x.name )
    pylab.legend()
    pylab.show()

    quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main()
