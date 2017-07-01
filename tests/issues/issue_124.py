# -*- coding: utf-8 -*-
import moose
print(( 'Using moose form %s' % moose.__file__ ))


def main():
    solver = "gssa"
    moose.Neutral('/model')
    moose.CubeMesh('/model/kinetics')
    moose.Pool('/model/kinetics/A')

    #delete if exists
    if ( moose.exists( 'model/kinetics/stoich' ) ):
        moose.delete( '/model/kinetics/stoich' )
        moose.delete( '/model/kinetics/ksolve' )

    #create solver
    compt = moose.element( '/model/kinetics' )
    ksolve = moose.Gsolve( '/model/kinetics/ksolve' )
    stoich = moose.Stoich( '/model/kinetics/stoich' )
    stoich.compartment = compt
    stoich.ksolve = ksolve
    stoich.path = "/model/kinetics/##"
    print(" before reinit")
    moose.reinit()
    print(" After reinit")
    moose.start( 10 )
    print( "Done" )

# Run the 'main' if this script is executed standalone.
if __name__ == '__main__':
	main()

