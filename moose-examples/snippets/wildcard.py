# wildcard.py ---
#
# Filename: wildcard.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Tue Jun  4 11:20:57 2013 (+0530)
# Version:
# Last-Updated: Tue Jun  4 12:27:42 2013 (+0530)
#           By: subha
#     Update #: 143
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

import moose

def wildcard_setup():
    a = moose.Neutral('/alfa')
    b = moose.Compartment('/alfa/bravo')
    c = moose.HHChannel('/alfa/bravo/charlie')
    f = moose.HHChannel2D('/alfa/bravo/foxtrot')
    e = moose.Neutral('/alfa/echo')
    d = moose.DiffAmp('/alfa/echo/delta')
    p = moose.PulseGen('/alfa/echo/papa')
    e1 = moose.Pool('/alfa/bravo/charlie/echo')
    g = moose.HHChannel('%s/golf' % (e1.path))
    f1 = moose.Neutral('/alfa/bravo/foxtail')
    c1 = moose.Neutral('/alfa/bravo/charlee')
    b.Rm = 2.0e6
    c.Gbar = 1e-9
    f.Gbar = 0.5e-6
    p.delay[0] = 10e-3
    d.gain = 3.0
    g.Gbar = 1e-6

def wildcard_test():
    # TYPE matches the type of the object
    wildcard = '/alfa/bravo/#[TYPE=HHChannel2D]'
    print(('\nElements Matching:', wildcard))
    for element in moose.wildcardFind(wildcard):
        print(('\t', element.path))

    # ISA matches elements of specified type or its subclasses
    wildcard = '/alfa/bravo/#[ISA=HHChannel]'
    print(('\nElements Matching:', wildcard))
    for element in moose.wildcardFind(wildcard):
        print(('\t', element.path))

    # HHChannel and HHChannel2D are subclasses of ChanBase
    wildcard = '/alfa/bravo/#[ISA=ChanBase]'
    print(('\nElements Matching:', wildcard))
    for element in moose.wildcardFind(wildcard):
        print(('\t', element.path))

    # HHChannel and HHChannel2D are subclasses of ChanBase
    wildcard = '/alfa/bravo/#[TYPE=ChanBase]'
    print(('\nElements Matching:', wildcard))
    for element in moose.wildcardFind(wildcard):
        print(('\t', element.path))

    # `#` can be used only once and matches all subsequent characters in name
    wildcard = '/alfa/bravo/charl?e'
    print(('\nElements Matching:', wildcard))
    for element in moose.wildcardFind(wildcard):
        print(('\t', element.path))

    # `?` can be used any number of times but substitutes a single character
    wildcard = '/alfa/bravo/fox#'
    print(('\nElements Matching:', wildcard))
    for element in moose.wildcardFind(wildcard):
        print(('\t', element.path))

    # Specify `##` to search through all levels below the path prefixing it
    wildcard = '/##[ISA=ChanBase]'
    print(('\nElements Matching:', wildcard))
    for element in moose.wildcardFind(wildcard):
        print(('\t', element.path))

    # You can even select by field - but if the field does not exist,
    # this will produce a lot of harmless error messages in debug
    # builds.
    wildcard = '/##[FIELD(name)=echo]'
    print(('\nElements Matching:', wildcard))
    for element in moose.wildcardFind(wildcard):
        print(('\t', element.path))

    # == and = are same
    wildcard = '/##[FIELD(name)==echo]'
    print(('\nElements Matching:', wildcard))
    for element in moose.wildcardFind(wildcard):
        print(('\t', element.path))

    # Comparison operators other than == are allowed for floating point number fields
    wildcard = '/alfa/##[FIELD(Gbar)<1e-6]'
    print(('\nElements Matching:', wildcard))
    for element in moose.wildcardFind(wildcard):
        print(('\t', element.path, moose.element(element).Gbar))

    # Equality is not defined for floating point fields
    wildcard = '/alfa/##[FIELD(Gbar)=1e-6]'
    print(('\nElements Matching:', wildcard))
    for element in moose.wildcardFind(wildcard):
        print(('\t', element.path, moose.element(element).Gbar))

    # Another operator is !=
    wildcard = '/alfa/bravo/#[TYPE!=HHChannel]'
    print(('\nElements Matching:', wildcard))
    for element in moose.wildcardFind(wildcard):
        print(('\t', element.path, ', class:', element.className))

    # With `##` you can get a listing of all elements under a path if
    # you don't use a condition
    wildcard = '/alfa/##'
    print(('\nElements Matching:', wildcard))
    for element in moose.wildcardFind(wildcard):
        print(('\t', element.path, ', class:', element.className))

def main():
    """Explore the wildcard search in moose models"""
    wildcard_setup()
    wildcard_test()

if __name__ == '__main__':
    main()



#
# wildcard.py ends here
