import moose
import rdesigneur as rd

library = moose.Neutral( '/library' )
compt = rd.makeChemOscillator( 'osc' )
compt.volume = 1e-18
moose.copy( compt, '/library/osc', 'spine' )
moose.element( '/library/osc/spine' ).volume = 1e-19
moose.copy( compt, '/library/osc', 'psd' )
moose.element( '/library/osc/psd' ).volume = 1e-20

rdes = rd.rdesigneur(
    turnOffElec = True,
    useGssa = False,
    cellProto = [[ './cells/h10.CNG.swc', 'elec']],
    spineProto = [[ 'makePassiveSpine()', 'spine' ] ],
    spineDistrib = [ ["spine", '#apical#,#dend#', '10e-6', '1e-6' ]],
    chemProto = [['/library/osc', 'osc']],
    chemDistrib =[[ 'osc', '#apical#,#dend#', 'install', 'H(p - 5e-4)' ]],
    plotList = [
        ['#', '1', 'psd/a', 'conc', 'conc of a in PSD'],
        ['#', '1', 'spine/a', 'conc', 'conc of a in spine'],
        ['#', '1', 'dend/a', 'conc', 'conc of a in Dend']
    ]
)

rdes.buildModel()

av = moose.vec( '/model/chem/psd/a' )
av[0].concInit *= 10

'''
dv = moose.vec( '/model/chem/dend/a' )
print len( dv )
dv[0].concInit *= 2
'''

moose.reinit()
moose.start( 100 )

rdes.display()
