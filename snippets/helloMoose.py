#########################################################################
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU Lesser General Public License version 2.1
## See the file COPYING.LIB for the full notice.
#########################################################################

import moose
import pylab
import numpy

def main():
    """
    This is the Hello MOOSE program. It shows how to get MOOSE to do
    its most basic operations: to load, run, and graph a model defined
    in an external model definition file.

    The loadModel function is the core of this example. It can accept
    a range of file and model types, including kkit, cspace and GENESIS .p
    files. It autodetects the file type and loads in the simulation.

    The wildcardFind function finds all objects matching the specified path,
    in this case Table objects hoding the simulation results. They were
    all defined in the model file.
    """
    mfile = '../genesis/kkit_objects_example.g'
    modelId = moose.loadModel( mfile, 'model', 'gsl' ) # Load it
    moose.reinit()          # Set initial conditions
    moose.start( 20.0 )     # Run it
    # Graph it
    for x in moose.wildcardFind( '/model/#graphs/conc#/#' ):
        pylab.plot( x.vector, label=x.name )
    pylab.legend()
    pylab.show()

if __name__ == '__main__':
        main()
