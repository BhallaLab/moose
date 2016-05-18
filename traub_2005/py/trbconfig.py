# config.py --- 
# 
# Filename: config.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Fri May  4 14:46:29 2012 (+0530)
# Version: 
# Last-Updated: Fri May  4 21:05:04 2012 (+0530)
#           By: Subhasis Ray
#     Update #: 140
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# 
# 
# 

# Change log:
# 
# 
# 

# Code:

from datetime import datetime
import ConfigParser as configparser
import logging
import numpy
import os

# runtime info
timestamp = datetime.now()
mypid = os.getpid()
# Unit Conversion Factors
uS = 1e-6 # micro Siemens to Siemens
ms = 1e-3 # milli second to second
mV = 1e-3 # milli Volt to Volt

# limits on HH-gate tables
vmin = -120 * mV
vmax = 40 * mV
ndivs = 640
dv = (vmax - vmin)/ndivs

# element to contain prototypes
libpath = '/library'

# defined channels to be initialized in prototypes
channel_names = ['AR',
                 'CaPool',
                 'CaL',
                 'CaT',
                 'CaT_A',
                 'K2',
                 'KA',
                 'KA_IB',
                 'KAHP',
                 'KAHP_DP',
                 'KAHP_SLOWER',
                 'KC',
                 'KC_FAST',
                 'KDR',
                 'KDR_FS',
                 'KM',
                 'NaF',
                 'NaF2',
                 'NaF_TCR',
                 'NaP',
                 'NaPF',
                 'NaPF_SS',
                 'NaPF_TCR',
                 'NaF2_nRT']

############################################
# Parse configuration file
############################################
_parser = configparser.SafeConfigParser()
_parser.optionxform = str
_parser.read(['defaults.ini', 'custom.ini'])

# seed for random number generator in MOOSE
moose_rngseed = _parser.get('numeric', 'moose_rngseed')

# seed for random number generator in numpy
numpy_rngseed = _parser.get('numeric', 'numpy_rngseed')
# flag if the simulation uses stochastic synchans
stochastic = _parser.get('numeric', 'stochastic') in ['Yes', 'yes', 'True', 'true', '1']
reseed = _parser.get('numeric', 'reseed') in ['Yes', 'yes', 'True', 'true', '1']
solver = _parser.get('numeric', 'solver')
simtime = float(_parser.get('scheduling', 'simtime'))
simdt = float(_parser.get('scheduling', 'simdt'))
plotdt = float(_parser.get('scheduling', 'plotdt'))

######################################################################
# configuration for saving simulation data
######################################################################
datadir = os.path.join(_parser.get('directories', 'data'),
                       timestamp.strftime('%Y_%m_%d'))
if not os.access(datadir, os.F_OK):
    os.mkdirs(datadir)
protodir = _parser.get('directories', 'proto')
datafileprefix = 'data'
netfileprefix = 'network'
filesuffix = '_%s_%d' % (timestamp.strftime('%Y%m%d_%H%M%S'), mypid)
datafilepath = os.path.join(datadir, datafileprefix + filesuffix + '.h5')
netfilepath = os.path.join(datadir, netfileprefix + filesuffix + '.h5')

#####################################################################
#  Logging
#####################################################################
logfileprefix = 'traub2005'
logfilename = os.path.join(datadir, logfileprefix + filesuffix + '.log')
loglevel = int(_parser.get('logging', 'level'))
logger = logging.getLogger(logfileprefix)
logging.basicConfig(filename=logfilename,
                    level=loglevel,
                    format='%(asctime)s \
                    %(levelname)s \
                    %(name)s \
                    %(filename)s \
                    %(funcName)s: \
                    %(message)s',
                    filemode='w')
benchmark = int(_parser.get('logging', 'benchmark'))
benchmarker = logging.getLogger(logfileprefix + '.benchmark')
benchmarker.setLevel(logging.DEBUG)

_inited = False
def init():
    if _inited:
        return
    _inited = True
    if reseed:
        if moose_rngseed:
            moose.seed(int(moose_rngseed))
        else:
            moose.seed(0)
        if numpy_rngseed:
            numpy.random.seed(int(numpy_rngseed))

init()

# 
# config.py ends here
