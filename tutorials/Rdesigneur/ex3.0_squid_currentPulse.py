import moose
import pylab
import rdesigneur as rd
rdes = rd.rdesigneur(
    chanProto = [['make_HH_Na()', 'Na'], ['make_HH_K()', 'K']],
    chanDistrib = [
        ['Na', 'soma', 'Gbar', '1200' ],
        ['K', 'soma', 'Gbar', '360' ]],
    stimList = [['soma', '1', '.', 'inject', '(t>0.1 && t<0.2) * 1e-8' ]],
    plotList = [['soma', '1', '.', 'Vm', 'Membrane potential']]
)

rdes.buildModel()
moose.reinit()
moose.start( 0.3 )

rdes.display()
