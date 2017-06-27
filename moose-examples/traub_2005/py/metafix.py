# metafix.py --- 
# 
# Filename: metafix.py
# Description: 
# Author: subha
# Maintainer: 
# Created: Sun Jun 25 09:56:51 2017 (-0400)
# Version: 
# Last-Updated: Sun Jun 25 10:00:23 2017 (-0400)
#           By: subha
#     Update #: 3
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
"""Fix to avoid metaclass incompatibility between Python 2 and 3.

Borrowed from: https://stackoverflow.com/a/22409540/508222
"""
def with_metaclass(mcls):
    def decorator(cls):
        body = vars(cls).copy()
        # clean out class body
        body.pop('__dict__', None)
        body.pop('__weakref__', None)
        return mcls(cls.__name__, cls.__bases__, body)
    return decorator


# 
# metafix.py ends here
