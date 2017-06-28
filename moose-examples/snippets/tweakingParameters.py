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
    This example illustrates parameter tweaking. It uses a kinetic model
    for a relaxation oscillator, defined in kkit format.
    We use the gsl solver here.
    The model looks like this::

                _________
                |        |
                V        |
        M-----Enzyme---->M*           All in compartment A
        |\             /| ^
        | \___basal___/   |
        |                 |
        endo              |
        |                 exo
        |       _______   |
        |       |      \  |
        V       V       \ |
        M-----Enzyme---->M*           All in compartment B
        \             /|
        \___basal___/


    The way it works: We set the run off for a few seconds with the original
    model parameters. This version oscillates. Then we double the endo
    and exo forward rates and run it further to show that the period becomes
    nearly twice as fast. Then we restore endo and exo, and instead double
    the initial amounts of M. We run it further again to see what happens.
    """
    mfile = '../genesis/OSC_Cspace.g'
    runtime = 4000.0
    modelId = moose.loadModel( mfile, 'model', 'gsl' )

    moose.reinit()
    moose.start( runtime )

    # Here begins the parameter tweaking.
    # Now we double the rates of the endo and exo reactions.
    endo = moose.element( '/model/kinetics/endo' )
    endo.Kf *= 2.0
    exo = moose.element( '/model/kinetics/exo' )
    exo.Kf *= 2.0
    moose.start( runtime ) # run it again

    # Now we restore rates, but double the total amount of M.
    endo.Kf /= 2.0
    exo.Kf /= 2.0
    M = moose.element( '/model/kinetics/A/M' )
    M.conc += M.concInit # concInit was the starting concentration.
    moose.start( runtime ) # run it again

    # Now plot the whole lot.
    for x in moose.wildcardFind( '/model/#graphs/conc#/#' ):
        pylab.plot( x.vector, label=x.name )
    pylab.legend()
    pylab.show()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
        main()
