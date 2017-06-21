import moose
import rdesigneur as rd
rdes = rd.rdesigneur(
    cellProto = [['./cells/h10.CNG.swc', 'elec']],
    stimList = [['soma', '1', '.', 'inject', 't * 25e-9' ]],
    plotList = [['#', '1', '.', 'Vm', 'Membrane potential'],
            ['#', '1', 'Ca_conc', 'Ca', 'Ca conc (uM)']],
    moogList = [['#', '1', '.', 'Vm', 'Soma potential']]
)

rdes.buildModel()

moose.reinit()
rdes.displayMoogli( 0.0002, 0.1 )
