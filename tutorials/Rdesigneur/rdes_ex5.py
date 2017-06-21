import moose
import pylab
import rdesigneur as rd
rdes = rd.rdesigneur(
    turnOffElec = True,
    chemProto = [['makeChemOscillator()', 'osc']],
    chemDistrib = [['osc', 'soma', 'install', '1' ]],
    plotList = [['soma', '1', 'dend/a', 'conc', 'Concentration of a'],
        ['soma', '1', 'dend/b', 'conc', 'Concentration of b']],
    moogList = [['soma', '1', 'dend/a', 'conc', 'a Conc', 0, 360 ]]
)

rdes.buildModel()
bv = moose.vec( '/model/chem/dend/b' )
bv[0].concInit *= 2
bv[-1].concInit *= 2
moose.reinit()

rdes.displayMoogli( 1, 400, 0.001 )
