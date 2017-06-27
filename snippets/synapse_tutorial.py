# synapse_tutorial.py ---
#
# Filename: synapse_tutorial.py
# Description:
# Author: Subhasis Ray
# Maintainer:
# Created: Fri Jan 17 09:43:51 2014 (+0530)
# Version:
# Last-Updated: Thu Oct  2 11:27:05 IST 2014
#           By: Upi
#     Update #: 0
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
# This is a tutorial based on an example Upi suggested. The code is
# exported from an ipython notebook and the comments present the
# markdown version of the tutorial text.
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
# Floor, Boston, MA 02110-1301, USA.# -*- coding: utf-8 -*-

#
#

# Code:
import moose
import random # We need this for random number generation
from numpy import random as nprand

import moose
import random # We need this for random number generation
from numpy import random as nprand

def main():
    """
    In this example we walk through creation of a vector of IntFire
    elements and setting up synaptic connection between them. Synapse on
    IntFire elements is an example of ElementField - elements that do not
    exist on their own, but only as part of another element. This example
    also illustrates various operations on `vec` objects and
    ElementFields.
    """

    size = 1024     # number of IntFire objects in a vec
    delayMin = 0
    delayMax = 4
    Vmax = 1.0
    thresh = 0.8
    refractoryPeriod = 0.4
    connectionProbability = 0.1
    weightMax = 0.5

    # The above sets the constants we shall use in this example. Now we create a vector of IntFire elements of size `size`.

    net = moose.IntFire('/network', size)

    # This creates a `vec` of `IntFire`  elements of size 1024 and returns the first `element`, i.e. "/network[0]".

    net = moose.element('/network[0]')

    # You need now to provide synaptic input to the network

    synh = moose.SimpleSynHandler( '/network/synh', size )

    # These need to be connected to the nodes in the network

    moose.connect( synh, 'activationOut', net, 'activation', 'OneToOne' )

    # You can access the underlying vector of elements using the `vec` field on any element. This is very useful for vectorized field access:

    net.vec.Vm = [thresh / 2.0] * size

    # The right part of the assigment creates a Python list of length `size` with each element set to `thresh/2.0`, which is 0.4. You can index into the `vec` to access individual elements' field:

    print((net.vec[1].Vm))

    # `SimpleSynHandler` class has an `ElementField` called `synapse`. It is just like a `vec` above in terms of field access, but by default its size is 0.

    print((len(synh.synapse)))

    # To actually create synapses, you can explicitly assign the `num` field of this, or set the `numSynapses` field of the `IntFire` element. There are some functions which can implicitly set the size of the `ElementField`.

    synh.numSynapses = 3
    print((len(synh.synapse)))

    synh.synapse.num = 4
    print((len(synh.synapse)))

    # Now you can index into `net.synapse` as if it was an array.

    print(('Before:', synh.synapse[0].delay))
    synh.synapse[0].delay = 1.0
    print(('After:', synh.synapse[0].delay))

    # You could do the same vectorized assignment as with `vec` directly:

    synh.synapse.weight = [0.2] * len(synh.synapse)
    print((synh.synapse.weight))

    # You can create the synapses and assign the weights and delays using loops:

    for syn in synh.vec:
        syn.synapse.num = random.randint(1,10) # create synapse fields with random size between 1 and 10, end points included
        # Below is one (inefficient) way of setting the individual weights of the elements in 'synapse'
        for ii in range(len(syn.synapse)):
            syn.synapse[ii].weight = random.random() * weightMax
        # This is a more efficient way - rhs of `=` is list comprehension in Python and rather fast
        syn.synapse.delay = [delayMin + random.random() * delayMax for ii in range(len(syn.synapse))]
        # An even faster way will be to use numpy.random.rand(size) which produces array of random numbers uniformly distributed between 0 and 1
        syn.synapse.delay = delayMin + nprand.rand(len(syn.synapse)) * delayMax

    # Now display the results, we use slice notation on `vec` to show the values of delay and weight for the first 5 elements in `/network`

    for syn in synh.vec[:5]:
        print(('Delays for synapses on ', syn.path, ':', syn.synapse.delay))
        print(('Weights for synapses on ', syn.path, ':', syn.synapse.weight))

if __name__ == '__main__':
    main()
#
# synapse_tutorial.py ends here
