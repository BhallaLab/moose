import moose
import pylab
import rdesigneur as rd
rdes = rd.rdesigneur(
    chanProto = [
        ['./chans/hd.xml'],
        ['./chans/kap.xml'],
        ['./chans/kad.xml'],
        ['./chans/kdr.xml'],
        ['./chans/na3.xml'],
        ['./chans/nax.xml'],
        ['./chans/CaConc.xml'],
        ['./chans/Ca.xml']
    ],
    cellProto = [['./cells/h10.CNG.swc', 'elec']],
    spineProto = [['makeActiveSpine()', 'spine']],
    chanDistrib = [
        ["hd", "#dend#,#apical#", "Gbar", "50e-2*(1+(p*3e4))" ],
        ["kdr", "#", "Gbar", "p < 50e-6 ? 500 : 100" ],
        ["na3", "#soma#,#dend#,#apical#", "Gbar", "850" ],
        ["nax", "#soma#,#axon#", "Gbar", "1250" ],
        ["kap", "#axon#,#soma#", "Gbar", "300" ],
        ["kap", "#dend#,#apical#", "Gbar",
            "300*(H(100-p*1e6)) * (1+(p*1e4))" ],
        ["Ca_conc", "#", "tau", "0.0133" ],
        ["kad", "#soma#,#dend#,#apical#", "Gbar", "50" ],
        ["Ca", "#", "Gbar", "50" ]
    ],
    spineDistrib = [['spine', '#dend#,#apical#', '20e-6', '1e-6']],
    stimList = [['soma', '1', '.', 'inject', '(t>0.02) * 1e-9' ]],
    plotList = [['#', '1', '.', 'Vm', 'Membrane potential'],
            ['#', '1', 'Ca_conc', 'Ca', 'Ca conc (uM)']],
    moogList = [['#', '1', 'Ca_conc', 'Ca', 'Calcium conc (uM)', 0, 120],
        ['#', '1', '.', 'Vm', 'Soma potential']]
)

rdes.buildModel()

moose.reinit()
rdes.displayMoogli( 0.0002, 0.0255 )
