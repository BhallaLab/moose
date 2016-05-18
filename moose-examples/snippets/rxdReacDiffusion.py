import numpy
import pylab
import moose
import time

'''
This example implements a reaction-diffusion like system which is
bistable and propagates losslessly. It is based on the NEURON example 
rxdrun.py, but incorporates more compartments and runs for a longer time.
The system is implemented as a hybrid of a reaction and a function which
sets its rates. Please see rxdFuncDiffusion.py for a variant that uses
just a function object to set up the system.
'''

dt = 0.1

# define the geometry
compt = moose.CylMesh( '/cylinder' )
compt.r0 = compt.r1 = 1
compt.x1 = 100
compt.diffLength = 0.2
assert( compt.numDiffCompts == compt.x1/compt.diffLength )

#define the molecule. Its geometry is defined by its parent volume, cylinder
c = moose.Pool( '/cylinder/pool' )
c.diffConst = 1 # define diffusion constant
# There is an implicit reaction substrate/product. MOOSE makes it explicit.
buf = moose.BufPool( '/cylinder/buf' )
buf.nInit = 1

# The reaction is something entirely peculiar, not a chemical thing.
reaction = moose.Reac( '/cylinder/reac' )
reaction.Kb = 0

# so here we set up a function calculation to do the same thing.
func = moose.Function( '/cylinder/reac/func' )
func.expr = "(1 - x0) * (0.3 - x0)"
func.x.num = 1 #specify number of input variables.

#Connect the reaction to the pools
moose.connect( reaction, 'sub', c, 'reac' )
moose.connect( reaction, 'prd', buf, 'reac' )

#Connect the function to the reaction
moose.connect( func, 'valueOut', reaction, 'setNumKf' )

#Connect the molecules to the func
moose.connect( c, 'nOut', func.x[0], 'input' )

#Set up solvers
ksolve = moose.Ksolve( '/cylinder/ksolve' )
dsolve = moose.Dsolve( '/cylinder/dsolve' )
stoich = moose.Stoich( '/cylinder/stoich' )
stoich.compartment = compt
stoich.ksolve = ksolve
stoich.dsolve = dsolve
stoich.path = '/cylinder/##'
for i in range( 10, 18 ):
    moose.setClock( i, dt )

#initialize
x = numpy.arange( 0, compt.x1, compt.diffLength )
c.vec.nInit = [ (q < 0.2 * compt.x1) for q in x ]

# Run and plot it.
moose.reinit()
updateDt = 50
runtime = updateDt * 4
plt = pylab.plot( x, c.vec.n, label='t = 0 ')
t1 = time.time()
for t in range( 0, runtime-1, updateDt ):
    moose.start( updateDt )
    plt = pylab.plot( x, c.vec.n, label='t = '+str(t + updateDt) )
print "Time = ", time.time() - t1

pylab.ylim( 0, 1.05 )
pylab.legend()
pylab.show()

