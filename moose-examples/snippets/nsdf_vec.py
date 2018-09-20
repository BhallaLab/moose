# nsdf_vec.py ---
#
# Filename: nsdf_vec.py
# Description:
# Author: subha
# Maintainer:
# Created: Sat Dec 19 22:27:27 2015 (-0500)
# Version:
# Last-Updated: Thu Aug 11 11:09:33 2016 (-0400)
#           By: Subhasis Ray
#     Update #: 135
# URL:
# Keywords:
# Compatibility:
#
import numpy as np
from datetime import datetime
import getpass
import moose
try:
    a = moose.NSDFWriter( '/a' )
except Exception as e:
    print( '[WARN] This build does not have NSDF support.' )
    print( ' -- To build moose with NSDF support please see: \
            https://github.com/BhallaLab/moose-core/blob/master/INSTALL.md' )
    quit()

try:
    import h5py as h5
except ImportError as e:
    print( 'No module h5py. Please install it using pip' )
    quit()

from matplotlib import pyplot as plt


def write_nsdf():
    """
    Setup a dummy model with a PulseGen vec and dump the outputValue in
    NSDF file

    """
    simtime = 100.0
    dt = 1e-3
    elements = 5
    model = moose.Neutral('/model')
    pulsegen = moose.PulseGen('/model/pulse', elements)
    spikegen = moose.SpikeGen('/model/t_lead', elements)
    nsdf = moose.NSDFWriter('/model/writer')
    nsdf.filename = 'nsdf_vec_demo.h5'
    nsdf.mode = 2 #overwrite existing file
    # nsdf.eventInput.num = elements
    nsdf.flushLimit = 100
    for ii in range(elements):
        pulse = pulsegen.vec[ii]
        t_lead = spikegen.vec[ii]
        # Just to make the values different for different elements in
        # the vec ...
        pulse.level[0] = 1.0*(ii+1)
        pulse.delay[0] = 5 * (ii+1)
        pulse.width[0] = 20
        t_lead.threshold = 0.5
        moose.connect(pulse, 'output', t_lead,'Vm')
        moose.connect(nsdf, 'requestOut', pulse, 'getOutputValue')
        # ei = nsdf.eventInput[ii]
        # moose.connect(t_lead, 'spikeOut', ei, 'input')
        # tab = moose.Table('spiketab_{}'.format(ii))
        # tab.threshold = t_lead.threshold
        # moose.connect(pulse, 'output', tab, 'spike')
    clock = moose.element('/clock')
    for ii in range(32):
        moose.setClock(ii, dt)
    print(('Starting simulation at:', datetime.now().isoformat()))
    moose.reinit()
    moose.start(simtime)
    print(('Finished simulation at:', datetime.now().isoformat()))
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
    ####################################################
    ## !! Work in progress: concurrent write via h5py does not work !!
    ####################################################
    ## Now write some custom stuff via h5py
    print('Closing nsdf handle')
    nsdf.close() #explicitly close the file so we do not interfere with h5py
    print('Closed nsdf handle')
    with h5.File(nsdf.filename, 'a') as fd:
        static = fd.create_group('/data/static')
        static_pg = static.create_group(pulsegen.className)
        pulse_info = static_pg.create_dataset('pulse_0', (elements,), dtype=np.dtype([('delay', 'float64'), ('level', 'float64'), ('width','float64')]))
        map_ = fd.create_group('/map/static')
        map_pg = map_.create_group(pulsegen.className)
        map_pulse = map_pg.create_dataset('pulse_0', (elements,), dtype=h5.special_dtype(vlen=str))
        for ii in range(elements):
            pulse_info['delay', ii] = pulsegen.vec[ii].delay[0]
            pulse_info['width', ii] = pulsegen.vec[ii].width[0]
            pulse_info['level', ii] = pulsegen.vec[ii].level[0]
            map_pulse[ii] = pulsegen.vec[ii].path
        #TODO: connect this as a dimension scale on pulse_info

    return nsdf.filename

def read_nsdf(fname):
    """Read the specific file we created in this example.

    Note that the preferable way of associating source with data is to
    use the DimensionScale. But since there is one-to-one
    correspondence between the data rows and the map rows (source
    path), we are exploiting that here.

    """
    with h5.File(fname, 'r') as fd:
        pulse_data = fd['/data/uniform/PulseGen/outputValue']
        pulse_src = fd['/map/uniform/PulseGen/outputValue']
        for ii in range(len(pulse_src)):
            source = pulse_src[ii]
            data = pulse_data[ii, :]
            dt = pulse_data.attrs['dt']
            plt.figure(source)
            ts = np.arange(len(data)) * dt
            plt.plot(ts, data)
            plt.suptitle(source)
    plt.show()

def main():
    """
Example code to dump data from multiple elements in a vector.

In this demo we create a PulseGen vector where each element has a
different set of pulse parameters. After saving the output vector
directly using MOOSE NSDFWriter we open the NSDF file using h5py and
plot the saved data.

You need h5py module installed to run this simulation.

References:

Ray, Chintaluri, Bhalla and Wojcik. NSDF: Neuroscience Simulation Data
Format, Neuroinformatics, 2015.

http://nsdf.readthedocs.org/en/latest/

    """
    fname = write_nsdf()
    read_nsdf(fname)

if __name__ == '__main__':
    main()

#
# nsdf_vec.py ends here
