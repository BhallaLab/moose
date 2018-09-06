import sys
import moose
import rdesigneur as rd

if len( sys.argv ) > 1:
    fname = sys.argv[1]
else:
    fname = './cells/h10.CNG.swc'
rdes = rd.rdesigneur(
    cellProto = [[fname, 'elec']],
    stimList = [['soma', '1', '.', 'inject', 't * 25e-9' ]],
    plotList = [['#', '1', '.', 'Vm', 'Membrane potential'],
            ['#', '1', 'Ca_conc', 'Ca', 'Ca conc (uM)']],
    moogList = [['#', '1', '.', 'Vm', 'Soma potential']]
)

rdes.buildModel()

moose.reinit()
rdes.displayMoogli( 0.001, 0.1, rotation = 0.02 )
