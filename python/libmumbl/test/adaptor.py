

"""adaptor.py: 

    This file is a minimal example on how to setup two compartment and pass
    message from one to other.

Last modified: Mon Jan 06, 2014  04:11PM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import moose
import matplotlib.pyplot as plt

def makeModel():
        # create container for model
        model = moose.Neutral( 'model' )
        compartment = moose.CubeMesh( '/model/compartment' )
        compartment.volume = 1e-15
        # the mesh is created automatically by the compartment
        mesh = moose.element( '/model/compartment/mesh' ) 

        # create molecules and reactions
        a = moose.Pool( '/model/compartment/a' )
        b = moose.Pool( '/model/compartment/b' )
        c = moose.Pool( '/model/compartment/c' )
        enz1 = moose.Enz( '/model/compartment/b/enz1' )
        enz2 = moose.Enz( '/model/compartment/c/enz2' )
        cplx1 = moose.Pool( '/model/compartment/b/enz1/cplx' )
        cplx2 = moose.Pool( '/model/compartment/c/enz2/cplx' )
        reac = moose.Reac( '/model/compartment/reac' )

        # Connect them up throught adaptors.
        moose.Neutral('/model/adaptor')
        adapt1 = moose.Adaptor('/model/adaptor/a1')
        adapt1.setField('inputOffset', 0)
        adapt1.setField('scale', 1)
        moose.connect(adapt1, 'requestField', a, 'getConc')
        moose.connect(adapt1, 'output', b, 'setConc')


        # connect them up for reactions
        moose.connect( enz1, 'sub', a, 'reac' )
        moose.connect( enz1, 'prd', b, 'reac' )
        moose.connect( enz1, 'enz', b, 'reac' )
        moose.connect( enz1, 'cplx', cplx1, 'reac' )

        moose.connect( enz2, 'sub', b, 'reac' )
        moose.connect( enz2, 'prd', a, 'reac' )
        moose.connect( enz2, 'enz', c, 'reac' )
        moose.connect( enz2, 'cplx', cplx2, 'reac' )

        moose.connect( reac, 'sub', a, 'reac' )
        moose.connect( reac, 'prd', b, 'reac' )

        # connect them up to the compartment for volumes
        for x in ( a, b, c, cplx1, cplx2 ):
            moose.connect( x, 'mesh', mesh, 'mesh' )

        # Assign parameters
        a.concInit = 1
        b.concInit = 0
        c.concInit = 0.01
        enz1.kcat = 0.4
        enz1.Km = 4
        enz2.kcat = 0.6
        enz2.Km = 0.01
        reac.Kf = 0.001
        reac.Kb = 0.01

        # Create the output tables
        graphs = moose.Neutral( '/model/graphs' )
        outputA = moose.Table ( '/model/graphs/concA' )
        outputB = moose.Table ( '/model/graphs/concB' )

        # connect up the tables
        moose.connect( outputA, 'requestData', a, 'get_conc' );
        moose.connect( outputB, 'requestData', b, 'get_conc' );

        # Schedule the whole lot
        moose.setClock( 4, 0.001 ) # for the computational objects
        moose.setClock( 8, 1.0 ) # for the plots
        moose.setClock( 9, 10 ) # for the plots
        # The wildcard uses # for single level, and ## for recursive.
        moose.useClock( 4, '/model/compartment/##', 'process' )
        moose.useClock( 8, '/model/graphs/#', 'process' )
        moose.useClock( 8, '/model/adaptor/#', 'process' )


def main():
        makeModel()

        moose.reinit()
        moose.start( 100.0 ) # Run the model for 100 seconds.

        a = moose.element( '/model/compartment/a' )
        b = moose.element( '/model/compartment/b' )

        # move most molecules over to b
        b.conc = b.conc + a.conc * 0.9
        a.conc = a.conc * 0.1
        moose.start( 100.0 ) # Run the model for 100 seconds.

        # move most molecules back to a
        a.conc = a.conc + b.conc * 0.99
        b.conc = b.conc * 0.01
        moose.start( 100.0 ) # Run the model for 100 seconds.

        # Iterate through all plots, dump their contents to data.plot.
        for x in moose.wildcardFind( '/model/graphs/conc#' ):
            #moose.element(x[0]).xplot('scriptKineticModel.plot', x[0].name)
            filename = (x.path+'.eps').replace('/', '_')
            plt.plot(x[0].vec)
            plt.savefig(filename)
        quit()

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
    main()
