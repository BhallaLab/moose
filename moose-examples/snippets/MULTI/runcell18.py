# First pass at loading and running a cell model 
import os
os.environ['NUMPTHREADS'] = '1'

import moose
import proto18

def dumpPlots( fname ):
	if ( os.path.exists( fname ) ):
		os.remove( fname )
	tab.xplot( fname, 'soma.Vm' )
	catab.xplot( fname, 'soma.Ca' )

library = moose.Neutral( '/library' )
moose.setCwe( '/library' )
proto18.make_Ca()
proto18.make_Ca_conc()
proto18.make_K_AHP()
proto18.make_K_C()
proto18.make_Na()
proto18.make_K_DR()
proto18.make_K_A()
proto18.make_glu()
proto18.make_NMDA()
proto18.make_Ca_NMDA()
proto18.make_NMDA_Ca_conc()
proto18.make_axon()

cellId = moose.loadModel( 'ca1_asym.p', '/cell', "hsolve" )
moose.le( cellId )
moose.le( '/cell/lat_14_1' )
#le( '/cell' )
graphs = moose.Neutral( '/graphs' )
tab = moose.Table( '/graphs/soma' )
catab = moose.Table( '/graphs/ca' )
soma = moose.element( '/cell/soma' )
soma.inject = 2e-10
moose.connect( tab, 'requestOut', soma, 'getVm' )
capool = moose.element( '/cell/soma/Ca_conc' )
moose.connect( catab, 'requestOut', capool, 'getCa' )
print 1
dt = 50e-6
moose.setClock( 0, dt )
moose.setClock( 1, dt )
moose.setClock( 2, dt )
moose.setClock( 3, 2e-4 )
moose.useClock( 0, '/cell/##[ISA=Compartment]', 'init' )
moose.useClock( 1, '/cell/##[ISA=Compartment]', 'process' )
moose.useClock( 2, '/cell/##[ISA!=Compartment]', 'process' )
moose.useClock( 3, '/graphs/soma,/graphs/ca', 'process' )

print 2
moose.reinit()
print 3
moose.start( 0.1 )
dumpPlots( '50usec.plot' )
print 4
moose.reinit()
hsolve = moose.HSolve( '/cell/hsolve' )
moose.useClock( 1, '/cell/hsolve', 'process' )
hsolve.dt = dt
hsolve.target = '/cell/soma'
moose.reinit()
moose.reinit()
print 5
moose.start( 0.1 )
print 6

dumpPlots( 'h50usec.plot' )

print 7
