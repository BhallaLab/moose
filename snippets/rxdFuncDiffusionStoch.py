import numpy
import pylab
import moose
import time

'''
This example implements a reaction-diffusion like system which is
bistable and propagates losslessly. It is based on the NEURON example 
rxdrun.py, but incorporates more compartments and runs for a longer time.
The system is implemented in a function rather than as a proper system
of chemical reactions. Please see rxdReacDiffusion.py for a variant that 
uses a reaction plus a function object to control its rates.
'''

dt = 0.1

# define the geometry
compt = moose.CylMesh( '/cylinder' )
compt.r0 = compt.r1 = 100e-9
compt.x1 = 100e-9
compt.diffLength = 0.2e-9
assert( compt.numDiffCompts == compt.x1/compt.diffLength )

#define the molecule. Its geometry is defined by its parent volume, cylinder
c = moose.Pool( '/cylinder/pool' )
c.diffConst = 1e-13 # define diffusion constant


# Here we set up a function calculation
func = moose.Function( '/cylinder/pool/func' )
func.expr = "(-x0 * (30e-9 - x0) * (100e-9 - x0))*0.0001"
func.x.num = 1 #specify number of input variables.

#Connect the molecules to the func
moose.connect( c, 'nOut', func.x[0], 'input' )
#Connect the function to the pool
moose.connect( func, 'valueOut', c, 'increment' )

#Set up solvers
ksolve = moose.Gsolve( '/cylinder/Gsolve' )
dsolve = moose.Dsolve( '/cylinder/dsolve' )
stoich = moose.Stoich( '/cylinder/stoich' )
stoich.compartment = compt
stoich.ksolve = ksolve
stoich.dsolve = dsolve
stoich.path = '/cylinder/##'

#for i in range( 10, 18 ):
#    moose.setClock( i, dt )

#initialize
x = numpy.arange( 0, compt.x1, compt.diffLength )
# c.vec.nInit = [ 100.0 * (q < 0.2 * compt.x1) for q in x ]
c.vec.nInit = [ 100 for q in x ]

# Run and plot it.
moose.reinit()
print dir(compt)
updateDt = 50
runtime = updateDt * 4
plt = pylab.plot( x, c.vec.n, label='t = 0 ')
t1 = time.time()
for t in range( 0, runtime-1, updateDt ):
    moose.start( updateDt )
    plt = pylab.plot( x, c.vec.n, label='t = '+str(t + updateDt) )
print "Time = ", time.time() - t1

pylab.ylim( 0, 105 )
pylab.legend()
pylab.show()

