import moose
import rdesigneur as rd
rdes = rd.rdesigneur(
    stimList = [['soma', '1', '.', 'inject', '(t>0.1 && t<0.2) * 2e-8' ]],
    plotList = [['soma', '1', '.', 'Vm', 'Soma membrane potential']]
)
rdes.buildModel()
moose.reinit()
moose.start( 0.3 )

rdes.display()