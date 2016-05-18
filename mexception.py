# mexception.py --- 
# 
# Filename: mexception.py
# Description: Implements some custom exceptions
# Author: Subhasis Ray
# Maintainer: 
# Created: Fri Apr 19 14:34:51 2013 (+0530)
# Version: 
# Last-Updated: Wed May 22 14:25:18 2013 (+0530)
#           By: subha
#     Update #: 22
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
"""Exception for MOOSE. All error-level exceptions must be derived
from MooseError. Otherwise they will not be handled by system error
handler which displays a message box.

"""
class MooseInfo(Exception):
    def __init__(self, *args, **kwargs):
        Exception.__init__(self, *args, **kwargs)

class MooseWarning(Warning):
    def __init__(self, *args, **kwargs):
        Warning.__init__(self, *args, **kwargs)

class MooseError(StandardError):
    def __init__(self, *args, **kwargs):
        StandardError.__init__(self, *args, **kwargs)
    
class FileLoadError(MooseError):
    def __init__(self, *args, **kwargs):
        StandardError.__init__(self, *args, **kwargs)
    
class ElementNameError(MooseError):
    def __init__(self, *args, **kwargs):
        StandardError.__init__(self, *args, **kwargs)

# 
# mexception.py ends here
