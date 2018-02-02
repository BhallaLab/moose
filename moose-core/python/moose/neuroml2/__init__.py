# -*- coding: utf-8 -*-
# __init__.py ---
#
# Filename: __init__.py
# Description:
# Author: subha
# Maintainer:
# Created: Sun Apr 17 13:55:30 2016 (-0400)
# Version:
# Last-Updated: Sun Apr 17 14:59:06 2016 (-0400)
#           By: subha
#     Update #: 20
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
"""neuroml2 submodule handles NeuroML2 models in MOOSE.

NML2Reader class allows reading of NeuroML2 cell models with
Hodgkin-Huxley type ion channels.

The [Ca2+] dependent channel reading may be incomplete.

Some functions for converting in-memory MOOSE models into NeuroML2 are
available in converter.py. But the complete cell-model writing is not
fully implemented. Hence this is not included in __all__.

"""
from __future__ import absolute_import, division, print_function
import moose.utils as mu

__author__ = 'Subhasis Ray'

from .reader import NML2Reader

# __init__.py ends here
