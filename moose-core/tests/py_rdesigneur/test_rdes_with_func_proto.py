import matplotlib
matplotlib.use('Agg')
import os
import moose
import rdesigneur as rd
import numpy as np

sm_diam = 20e-6
sm_len = 20e-6
dend_diam = 4e-6
dend_len = 500e-6
numDend = 10
sm_area = np.pi*sm_len*sm_diam
elecPlotDt = 50e-6
elecDt = 10e-6
RM = 1
CM = 0.01
RA = 1.5
Vrest = -0.065

sdir_ = os.path.dirname(os.path.realpath(__file__))

def test_proto():
    # Boilerplate
    rdes = rd.rdesigneur(
        elecPlotDt = elecPlotDt,
        elecDt = elecDt,
        cellProto = [['somaProto', 'soma', sm_diam, sm_len]],
        chanProto = [[ os.path.join(sdir_, 'Na_Chan_Migliore2018_.Na_Chan()'), 'Na']],
        chanDistrib = [
            ['Na', 'soma', 'Gbar', '360']
            ],
        passiveDistrib = [
            ['#', 'RM', str(RM), 'CM', str(CM), 'initVm', str(Vrest), 'Em', str(Vrest)],
        ],
        stimList = [['soma', '1', '.', 'vclamp', '-0.065 + (t>0.1 && t<0.2) * 0.02' ]],
        plotList = [
            ['soma', '1', '.', 'Vm', 'Soma membrane potential'],
            ['soma', '1', 'vclamp', 'current', 'Soma holding current'],
        ]
    )
    rdes.buildModel()

    if moose.exists('/model/elec/soma/vclamp'):
        moose.element( '/model/elec/soma/vclamp' ).gain = CM*sm_area/elecDt
        moose.element( '/model/elec/soma/vclamp' ).tau = 5*elecDt
        moose.element( '/model/elec/soma/vclamp' ).ti = elecDt*2
        moose.element( '/model/elec/soma/vclamp' ).td = 0

    moose.reinit()
    moose.start( 0.3 )
    rdes.display()

def main():
    test_proto()

if __name__ == '__main__':
    main()
