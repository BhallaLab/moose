import moose
import logging
import datetime
import time
import os

# Universal paths 
nmlPath = '/neuroml'
nmlCellPath = os.path.join(nmlPath, 'cells')
libraryPath = os.path.join(nmlPath, 'cells')
cellPath = nmlCellPath
elecPath = os.path.join(nmlPath, 'electrical')

mumblePath = '/mumbl'


st = time.time()
st = datetime.datetime.fromtimestamp(st).strftime('%Y-%m-%d-%H%M')

logDir = 'logs'
logFile = os.path.join(logDir, 'moose.log')
if not os.path.isdir(logDir):
    os.makedirs(logDir)

if os.path.exists(logFile):
    os.rename(logFile, 'logs/{0}'.format(st))

logging.basicConfig(filename=logFile, level=logging.DEBUG)
mooseLogger = logging.getLogger()
disbleCopyingOfObject = True

