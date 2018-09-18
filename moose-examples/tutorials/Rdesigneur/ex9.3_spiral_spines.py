##########################################################################
# This illustrates some of the capabilities for spine placement.
# It has spines whose size increase with distance from the soma.
# Further, the angular direction of the spines spirals around the dendrite.
##########################################################################
import moose
import rdesigneur as rd
rdes = rd.rdesigneur(
    cellProto = [['ballAndStick', 'elec', 10e-6, 10e-6, 2e-6, 300e-6, 50]],
    spineProto = [['makePassiveSpine()', 'spine']],
    spineDistrib = [['spine', '#dend#', '3e-6', '-1e-6', '1+p*2e4', '0', 'p*6.28e7', '0']],
    stimList = [['soma', '1', '.', 'inject', '(t>0.02) * 1e-9' ]],
    moogList = [['#', '1', '.', 'Vm', 'Soma potential']]
)

rdes.buildModel()

moose.reinit()
rdes.displayMoogli( 0.0002, 0.025, 0.02 )
