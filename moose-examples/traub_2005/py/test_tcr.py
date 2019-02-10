# test_tcr.py --- 
# 
# Filename: test_tcr.py
# Description: 
# Author: 
# Maintainer: 
# Created: Mon Jul 16 16:12:55 2012 (+0530)
# Version: 
# Last-Updated: Sun Jun 25 16:30:03 2017 (-0400)
#           By: subha
#     Update #: 531

import unittest
from cell_test_util import setup_current_step_model, SingleCellCurrentStepTest
import testutils
import cells
import moose
from moose import utils
import pylab

simdt = 5e-6
plotdt = 0.25e-3
simtime = 300e-3
    
class TestTCR(SingleCellCurrentStepTest):
    def __init__(self, *args, **kwargs):
        self.celltype = 'TCR'
        SingleCellCurrentStepTest.__init__(self, *args, **kwargs)
        self.pulse_array = [(100e-3, 100e-3, 1e-9),
                            (1e9, 0, 0)]
        # self.solver = 'hsolve'
        self.simdt = simdt
        self.plotdt = plotdt

    def setUp(self):
        SingleCellCurrentStepTest.setUp(self)

    def testVClamp(self):
        clamp = moose.VClamp('%s/vclamp' % (self.model_container.path))
        moose.connect(clamp, 'currentOut', self.cell.soma, 'injectMsg')
        moose.connect(self.cell.soma, 'VmOut', clamp, 'sensedIn')
        self.pulsegen.delay[0] = 1e9 # disable current clamp
        self.pulsegen = moose.PulseGen('%s/vclampCommand' % (self.model_container.path))
        self.pulsegen.delay[0] = 100e-3
        self.pulsegen.width[0] = 100e-3
        self.pulsegen.level[0] = -10e-3
        moose.connect(self.pulsegen, 'output', clamp, 'commandIn')
        tab = moose.Table('%s/command' % (self.data_container.path))
        moose.connect(tab, 'requestOut', clamp, 'getCommand')
        for ii in moose.wildcardFind('/##[TYPE=VClamp]'):
            print((ii.path))
        self.runsim(simtime)
        print((tab, len(tab.vector)))
        pylab.plot(pylab.linspace(0, simtime, len(tab.vector)), tab.vector, 'kx')
        self.plot_vm()

    # def testChannelDensities(self):
    #     pass
        # equal = compare_cell_dump(self.dump_file, '../nrn/'+self.dump_file)
        # self.assertTrue(equal)


if __name__ == '__main__':
    unittest.main()



# 
# test_tcr.py ends here
