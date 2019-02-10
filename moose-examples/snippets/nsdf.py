# nsdf.py ---
#
# Filename: nsdf.py
# Description:
# Author: subha
# Maintainer:
# Created: Fri Jun 26 12:23:07 2015 (-0400)
# Version:
# Last-Updated: Tue Dec 29 12:50:27 2015 (-0500)
#           By: Subhasis Ray
#     Update #: 6
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
# __BROKEN__
#
#
#

# Change log:
#
#
#
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
#
#

# Code:
"""
NSDF : Neuroscience Simulation Data Format
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

NSDF is an HDF5 based format for storing data from neuroscience
simulation.

This script is for demonstrating the use of NSDFWriter class to
dump data in NSDF format.

The present implementation of NSDFWriter puts all value fields
connected to its requestData into
/data/uniform/{className}/{fieldName} 2D dataset - each row
corresponding to one object.

Event data are stored in
/data/event/{className}/{fieldName}/{Id}_{dataIndex}_{fieldIndex}
where the last component is the string representation of the ObjId of
the source.

The model tree (starting below root element) is saved as a tree of
groups under /model/modeltree (one could easily add the fields as
atributes with a little bit of more code).

The mapping between data source and uniformly sampled data is stored
as a dimension scale in /map/uniform/{className}/{fieldName}. That for
event data is stored as a compound dataset in
/map/event/{className}/{fieldName} with a [source, data] columns.

The start and end timestamps of the simulation are saved as file
attributes: C/C++ time functions have this limitation that they give
resolution up to a second, this means for simulation lasting < 1 s the
two timestamps may be identical.

Much of the environment specification is set as HDF5 attributes (which
is a generic feature from HDF5WriterBase).

MOOSE is unit agnostic at present so unit specification is not
implemented in NSDFWriter. But units can be easily added as dataset
attribute if desired as shown in this example.


References:

Ray, Chintaluri, Bhalla and Wojcik. NSDF: Neuroscience Simulation Data
Format, Neuroinformatics, 2015.

http://nsdf.readthedocs.org/en/latest/

"""

import numpy as np
from datetime import datetime
import getpass

import moose
try:
    nsdf = moose.NSDFWriter('/test')
except AttributeError as e:
    print( "[INFO ] This build is not built with NSDFWriter." )
    print( '\tPlease see https://github.com/BhallaLab/moose-core' )
    quit()


def setup_model():
    """
    Setup a dummy model with a PulseGen and a SpikeGen. The SpikeGen
    detects the leading edges of the pulses created by the PulseGen
    and sends out the event times. We record the PulseGen outputValue
    as Uniform data and leading edge time as Event data in the NSDF
    file.

    """
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
    print(('event input', nsdf.eventInput, nsdf.eventInput.num))
    print(nsdf)

    nsdf.eventInput.num = 1
    ei = nsdf.eventInput[0]
    print((ei.path))
    moose.connect(t_lead, 'spikeOut', nsdf.eventInput[0], 'input')
    tab = moose.Table('spiketab')
    tab.threshold = t_lead.threshold
    clock = moose.element('/clock')
    for ii in range(32):
        moose.setClock(ii, dt)
    moose.connect(pulse, 'output', tab, 'spike')
    print(('Starting simulation at:', datetime.now().isoformat()))
    moose.reinit()
    moose.start(simtime)
    print(('Finished simulation at:', datetime.now().isoformat()))
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

def main():
    setup_model()

if __name__ == '__main__':
    main()

#
# nsdf.py ends here
