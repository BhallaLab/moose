# -*- coding: utf-8 -*-
# test_nsdf.py ---
# Changed from nsdf.py from
# https://github.com/BhallaLab/moose-examples/snippets/nsdf.py

from __future__ import print_function
import numpy as np
from datetime import datetime
import getpass
import os

# Use in-repo moose to install.
import moose
print('using moose from: %s' % moose.__file__)

global nsdf

def setup_model():
    """Setup a dummy model with a PulseGen and a SpikeGen. The SpikeGen
    detects the leading edges of the pulses created by the PulseGen
    and sends out the event times. We record the PulseGen outputValue
    as Uniform data and leading edge time as Event data in the NSDF
    file.

    """
    global nsdf
    simtime = 100.0
    dt = 1e-3
    model = moose.Neutral('/model')
    pulse = moose.PulseGen('/model/pulse')
    pulse.level[0] = 1.0
    pulse.delay[0] = 10
    pulse.width[0] = 20
    t_lead = moose.SpikeGen('/model/t_lead')
    t_lead.threshold = 0.5
    moose.connect(pulse, 'output', t_lead,'Vm');
    nsdf = moose.NSDFWriter('/model/writer')
    nsdf.filename = 'nsdf_demo.h5'
    nsdf.mode = 2 #overwrite existing file
    nsdf.flushLimit = 100
    moose.connect(nsdf, 'requestOut', pulse, 'getOutputValue')
    print('event input', nsdf.eventInput, nsdf.eventInput.num)
    print(nsdf)

    nsdf.eventInput.num = 1
    ei = nsdf.eventInput[0]
    print(ei.path)
    moose.connect(t_lead, 'spikeOut', nsdf.eventInput[0], 'input')
    tab = moose.Table('spiketab')
    tab.threshold = t_lead.threshold
    clock = moose.element('/clock')
    for ii in range(32):
        moose.setClock(ii, dt)
    moose.connect(pulse, 'output', tab, 'spike')
    print(datetime.now().isoformat())
    moose.reinit()
    moose.start(simtime)
    print(datetime.now().isoformat())
    np.savetxt('nsdf.txt', tab.vector)
    ###################################
    # Set the environment attributes
    ###################################
    nsdf.stringAttr['title'] = 'NSDF writing demo for moose'
    nsdf.stringAttr['description'] = '''An example of writing data to NSDF file from MOOSE simulation. In
this simulation we generate square pules from a PulseGen object and
use a SpikeGen to detect the threshold crossing events of rising
edges. We store the pulsegen output as Uniform data and the threshold
crossing times as Event data. '''
    nsdf.stringAttr['creator'] = getpass.getuser()
    nsdf.stringVecAttr['software'] = ['python2.7', 'moose3' ]
    nsdf.stringVecAttr['method'] = ['']
    nsdf.stringAttr['rights'] = ''
    nsdf.stringAttr['license'] = 'CC-BY-NC'
    # Specify units. MOOSE is unit agnostic, so we explicitly set the
    # unit attibutes on individual datasets
    nsdf.stringAttr['/data/uniform/PulseGen/outputValue/tunit'] = 's'
    nsdf.stringAttr['/data/uniform/PulseGen/outputValue/unit'] = 'A'
    eventDataPath = '/data/event/SpikeGen/spikeOut/{}_{}_{}/unit'.format(t_lead.vec.value,
                                                                         t_lead.getDataIndex(),
                                                                         t_lead.fieldIndex)
    nsdf.stringAttr[eventDataPath] = 's'

def test_nsdf():
    try:
        setup_model()
    except AttributeError as e:
        print( 'This MOOSE is not compiled with NSDF support' )
        return 0
    except Exception as e:
        raise e
    # Very basic tests
    nsdfFile = 'nsdf.txt'
    if not os.path.exists( nsdf.filename ):
        raise Exception("Test failed. No files : %s" % nsdfFile)
    if not os.path.exists( nsdfFile ):
        raise Exception("Test failed. No file : %s" % nsdfFile)

    data = np.loadtxt( nsdfFile )
    assert len(data) == 4, "Expected 4 entries"

def main():
    test_nsdf()

if __name__ == '__main__':
    main()
