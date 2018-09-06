# -*- coding: utf-8 -*-
from __future__ import division, print_function
import random
import sys
import math
import moose
from moose.neuroml import *

# This program plots a channel's state variables / hinf, htau etc. as a function
# of voltage.

mechanisms = {
        'H_STG': ['minf','mtau'],
        'CaS_STG': ['minf','mtau','hinf','htau'],
        'CaT_STG': ['minf','mtau','hinf','htau'],
        'KA_STG': ['minf','mtau','hinf','htau'],
        'Kd_STG': ['ninf','ntau'],
        'Na_STG': ['minf','mtau','hinf','htau']
        }

import sys
if len(sys.argv)<2:
    print(("Selecting a channel randomly form %s" % list(mechanisms.keys())))
    channel_name = random.choice( list(mechanisms.keys()) )
    print(("Selected %s" % channel_name ))
else:
    channel_name = sys.argv[1]

if channel_name in mechanisms:
    mechanism_vars = mechanisms[channel_name]
else:
    print(("Undefined channel, please use one of", list(mechanisms.keys())))
    sys.exit(1)

CELSIUS = 35 # degrees Centigrade
CML = ChannelML({'temperature':CELSIUS})
CML.readChannelMLFromFile('../channels/'+channel_name+'.xml')

from pylab import *
                        
if __name__ == "__main__":

    for varidx in range(len(mechanism_vars)//2): # loop over each inf and tau
        print(( "Running for %s" % varidx ))
        var = ['X','Y','Z'][varidx]
        gate = moose.element('/library/'+channel_name+'/gate'+var)
        VMIN = gate.min
        VMAX = gate.max
        NDIVS = gate.divs
        dv = (VMAX-VMIN)/NDIVS
        # will use same VMIN, VMAX and dv for A and B tables.
        vrange = array([VMIN+i*dv for i in range(NDIVS+1)])
        figure()
        plot(vrange*1000,gate.tableA/gate.tableB,'b-,') # Assume A and B have corresponding number of entries
        xlabel('Voltage (mV)')
        ylabel('steady state value')
        title('state variable '+mechanism_vars[2*varidx]+' of '+channel_name+' vs Voltage (mV)')
        figure()
        plot(vrange*1000,1./gate.tableB*1000.,'b-,')
        xlabel('Voltage (mV)')
        ylabel('tau (ms)')
        title('state variable '+mechanism_vars[2*varidx+1]+' of '+channel_name+' vs Voltage (mV)')
    show()

