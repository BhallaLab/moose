import moose
import pylab
import rdesigneur as rd
rdes = rd.rdesigneur(
    turnOffElec = True,
    diffusionLength = 1e-3, # Default diffusion length is 2 microns
    chemProto = [['makeChemOscillator()', 'osc']],
    chemDistrib = [['osc', 'soma', 'install', '1' ]],
    plotList = [['soma', '1', 'dend/a', 'conc', 'a Conc'],
        ['soma', '1', 'dend/b', 'conc', 'b Conc']]
)

rdes.buildModel()
b = moose.element( '/model/chem/dend/b' )
b.concInit *= 5
moose.reinit()
moose.start( 200 )

rdes.display()
