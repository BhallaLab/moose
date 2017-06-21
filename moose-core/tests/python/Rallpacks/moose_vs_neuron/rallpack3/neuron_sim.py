#!/usr/bin/env python

"""rallpack3_moose_vs_neuron.py: 

    This file compares moose and neuron for rallpack3.

Last modified: Sun May 25, 2014  07:05AM

"""
from __future__ import print_function
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import os
import re
import sys
import subprocess
import time
import datetime

templateFile = './active_cable_template.nrn'


def getNeuronScript(args, template):
    # set recording positions
    compSize = args['length'] / args['ncomp']
    args['comp_length'] = compSize * 1e6
    args['0'] = (compSize / 2) * 1e3
    args['x'] = (compSize * args['ncomp'] * 1e3) - (compSize * 1e3 / 2)
    args['diameter'] = args['diameter'] / 1e-6
    args['RA'] = args['RA'] * 100
    args['Em'] = args['Em'] * 1e3
    args['dt'] = args['dt'] * 1e3
    args['run_time'] = args['run_time'] * 1e3
    for k in args.keys():
        template = template.replace('||{}||'.format(k), '%s'%args[k])
    return template

def main(args):
    with open(templateFile, "r") as f:
        templateTxt = f.read()
    nrnText = getNeuronScript(args, templateTxt)
    outFile = 'cable_in_neuron.hoc'
    with open(outFile, 'w') as outF:
        outF.write(nrnText)

    if os.path.isfile(outFile):
        cmd = [ 'nrniv', '-nobanner', '-nogui', outFile ]
        print("[STEP] Executing: {}".format(" ".join(cmd)))
        t = time.time()
        subprocess.check_call( cmd, shell=False)
        simTime = time.time() - t
        st = time.time()
        stamp = datetime.datetime.fromtimestamp(st).strftime('%Y-%m-%d-%H%M%S')
        with open('neuron.log', 'a') as logF:
            logF.write('<simulation time_stamp="{}">\n'.format(stamp))
            logF.write("\t<elements>\n")
            logF.write("\t\t<Compartment>{}</Compartment>\n".format(args['ncomp']))
            logF.write("\t</elements>\n")
            logF.write("\t<times>\n")
            logF.write("\t\t<Simulation>{}</Simulation>\n".format(simTime))
            logF.write("\t</times>\n")
            logF.write("</simulation>\n")
    else:
        print("Can't write to {}".format(outFile))

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(
            description = 'Rallpacks3: A cable with n compartment with HHChannel'
            )
    parser.add_argument( '--tau'
            , default = 0.04
            , type = float
            , help = 'Time constant of membrane'
            )
    parser.add_argument( '--run_time'
            , default = 0.25
            , type = float
            , help = 'Simulation run time'
            )
    parser.add_argument( '--dt'
            , default = 5e-5
            , type = float
            , help = 'Step time during simulation'
            )
    parser.add_argument( '--Em'
            , default = -65e-3
            , type = float
            , help = 'Resting potential of membrane'
            )
    parser.add_argument( '--RA'
            , default = 1.0
            , type = float
            , help = 'Axial resistivity'
            )
    parser.add_argument( '--lambda'
            , default = 1e-3
            , type = float
            , help = 'Lambda, what else?'
            )
    parser.add_argument( '--x'
            , default = 999
            , type = int
            , help = 'Index of compartment at which membrane potential is recorded'
            ) 
    parser.add_argument( '--length'
            , default = 1e-3
            , type = float
            , help = 'Length of the cable'
            )
    parser.add_argument( '--diameter'
            , default = 1e-6
            , type = float
            , help = 'Diameter of cable'
            )
    parser.add_argument( '--inj'
            , default = 1e-10
            , type = float
            , help = 'Current injected at one end of the cable'
            )
    parser.add_argument( '--ncomp'
            , default = 1000
            , type = int
            , help = 'No of compartment in cable'
            )
    parser.add_argument('--data'
            , default = 'cable.out'
            , type = str
            , help = 'Data in text file'
            )
    parser.add_argument( '--output'
            , default = None
            , type = str
            , help = 'Store simulation results to this file'
            )
    args = parser.parse_args()
    main( vars(args) )

