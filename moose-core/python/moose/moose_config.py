# This file is part of MOOSE simulator: http://moose.ncbs.res.in.

# MOOSE is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# MOOSE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.


"""multiscale_config.py: 

Last modified: Sat Jan 18, 2014  05:01PM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

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

logDir = os.environ['HOME']
logFile = os.path.join(logDir, ".moose", 'mumble.log')
if not os.path.isdir(logDir):
    os.makedirs(logDir)

if os.path.exists(logFile):
    os.rename(logFile, '{}_{}'.format(logFile, st))

logging.basicConfig(filename=logFile, level=logging.DEBUG)
mooseLogger = logging.getLogger()
disbleCopyingOfObject = True

