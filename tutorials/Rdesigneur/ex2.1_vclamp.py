import moose
import rdesigneur as rd
rdes = rd.rdesigneur(
    stimList = [['soma', '1', '.', 'vclamp', '-0.065 + (t>0.1 && t<0.2) * 0.02' ]],
    plotList = [
        ['soma', '1', '.', 'Vm', 'Soma membrane potential'],
        ['soma', '1', 'vclamp', 'current', 'Soma holding current'],
    ]
)
rdes.buildModel()
moose.reinit()
moose.start( 0.3 )

rdes.display()
