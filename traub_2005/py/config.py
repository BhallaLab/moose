# config.py --- 
# 
# Filename: config.py
# Description: 
# Author: 
# Maintainer: 
# Created: Wed May 23 11:31:40 2012 (+0530)
# Version: 
# Last-Updated: Sat Aug  6 15:25:16 2016 (-0400)
#           By: subha
#     Update #: 130
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
# 

# Code:

from __future__ import print_function
import settings
import os

# These settings are to imitate sedml entities for configuring simulation
simulationSettings = settings.SimulationSettings()
modelSettings = settings.ModelSettings()
analogSignalRecordingSettings = settings.DataSettings()
spikeRecordingSettings = settings.DataSettings()
changeSettings = settings.ChangeSettings()

simulationSettings.endTime = 10.0

modelSettings.container = '/network'
modelSettings.libpath = '/library'
modelSettings.protodir = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'proto')

modelSettings.morph_has_postion = False
modelSettings.populationSize['SupPyrRS'] = 1000
modelSettings.populationSize['SupPyrFRB'] = 50
modelSettings.populationSize['SupBasket'] = 90       
modelSettings.populationSize['SupAxoaxonic'] = 90
modelSettings.populationSize['SupLTS'] = 90
modelSettings.populationSize['SpinyStellate'] =	240
modelSettings.populationSize['TuftedIB'] = 800
modelSettings.populationSize['TuftedRS'] = 200
modelSettings.populationSize['DeepBasket'] = 100
modelSettings.populationSize['DeepAxoaxonic'] = 100
modelSettings.populationSize['DeepLTS'] = 100
modelSettings.populationSize['NontuftedRS'] = 500
modelSettings.populationSize['TCR'] = 100
modelSettings.populationSize['nRT'] = 100       

analogSignalRecordingSettings.targets = [
    '/network/SupPyrRS/#[NAME=comp_0]',
    '/network/SupPyrFRB/#[NAME=comp_0]',
    '/network/SupBasket/#[NAME=comp_0]',
    '/network/SupAxoaxonic/#[NAME=comp_0]',
    '/network/SupLTS/#[NAME=comp_0]',
    '/network/SpinyStellate/#[NAME=comp_0]',
    '/network/TuftedIB/#[NAME=comp_0]',
    '/network/TuftedRS/#[NAME=comp_0]',
    '/network/DeepBasket/#[NAME=comp_0]',
    '/network/DeepAxoaxonic/#[NAME=comp_0]',
    '/network/DeepLTS/#[NAME=comp_0]',
    '/network/NontuftedRS/#[NAME=comp_0]',
    '/network/TCR/#[NAME=comp_0]',
    '/network/nRT/#[NAME=comp_0]',
    ]

analogSignalRecordingSettings.fractions = 0.1
analogSignalRecordingSettings.fields = {
    'Vm': 'AnalogSignal',
    'CaPool/conc': 'AnalogSignal',
    }

spikeRecordingSettings.targets = [
    '/network/SupPyrRS/#[NAME=comp_0]',
    '/network/SupPyrFRB/#[NAME=comp_0]',
    '/network/SupBasket/#[NAME=comp_0]',
    '/network/SupAxoaxonic/#[NAME=comp_0]',
    '/network/SupLTS/#[NAME=comp_0]',
    '/network/SpinyStellate/#[NAME=comp_0]',
    '/network/TuftedIB/#[NAME=comp_0]',
    '/network/TuftedRS/#[NAME=comp_0]',
    '/network/DeepBasket/#[NAME=comp_0]',
    '/network/DeepAxoaxonic/#[NAME=comp_0]',
    '/network/DeepLTS/#[NAME=comp_0]',
    '/network/NontuftedRS/#[NAME=comp_0]',
    '/network/TCR/#[NAME=comp_0]',
    '/network/nRT/#[NAME=comp_0]',
    ]

spikeRecordingSettings.fractions = 1.0
spikeRecordingSettings.fields = {
    'Vm': 'Event',
    }

#---------------------------------------------------------------------
# Logging
#---------------------------------------------------------------------
from datetime import datetime
import logging

timestamp = datetime.now()
mypid = os.getpid()
data_dir_prefix = 'data'
if not os.access(data_dir_prefix, os.W_OK):
    data_dir_prefix = '/tmp/traub2005_data'
if not os.access(data_dir_prefix, os.F_OK):
    os.mkdir(data_dir_prefix)
data_dir = os.path.join(data_dir_prefix, timestamp.strftime('%Y_%m_%d'))
if not os.access(data_dir, os.F_OK):
    os.mkdir(data_dir)
print('data_dir =', data_dir)
filename_suffix = '_%s_%d' % (timestamp.strftime('%Y%m%d_%H%M%S'), mypid)

def handleError(self, record):
    raise

LOG_FILENAME = os.path.join(data_dir, 'traub2005%s.log' % (filename_suffix))
LOG_LEVEL = logging.DEBUG
logging.Handler.handleError = handleError
logging.basicConfig(filename=LOG_FILENAME, level=LOG_LEVEL, format='%(asctime)s %(levelname)s %(name)s %(filename)s %(funcName)s: %(message)s', filemode='w')
# logging.basicConfig(level=LOG_LEVEL, format='%(asctime)s %(levelname)s %(name)s %(filename)s %(funcName)s: %(message)s', filemode='w')

logger = logging.getLogger('traub2005')

# Add a logging handler to print messages to stderr
ch = logging.StreamHandler()
ch.setLevel(logging.ERROR)
# create formatter and add it to the handlers
formatter = logging.Formatter('%(asctime)s %(name)s %(levelname)s %(filename)s %(funcName)s: %(message)s')
ch.setFormatter(formatter)
# add the handlers to logger
logger.addHandler(ch)

import os
os.environ['NUMPTHREADS'] = '1'
import sys
sys.path.append('../../../python')
import moose
############################################################
# Initialize library and other containers.
############################################################
library = moose.Neutral(modelSettings.libpath)


# 
# config.py ends here
