# savemodel.py --- 
# 
# Filename: savemodel.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Wed Oct 29 00:08:50 2014 (+0530)
# Version: 
# Last-Updated:Thr Dec 23 16:31:00 2015 (+0530) 
#           By: 
#     Update #: 0
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
"""This example illustrates loading a kinetic model defined in Genesis format 
into Moose using "loadModel" function and using "saveModel" function one can 
save the model back to Genesis format
"""

import sys
#sys.path.append('../../python')
import moose
from moose.genesis import *

if __name__ == '__main__':
    """ The script demonstates to convert Chemical (Genesis) file back to Genesis file using moose """
    model = moose.loadModel('../genesis/reaction.g', '/model')
    written = write('/model', 'testsave.g')
    print written
