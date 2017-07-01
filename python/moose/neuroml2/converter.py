# -*- coding: utf-8 -*-
# converter.py ---
#
# Filename: mtoneuroml.py
# Description:
# Author:
# Maintainer:
# Created: Mon Apr 22 12:15:23 2013 (+0530)
# Version:
# Last-Updated: Wed Jul 10 16:36:14 2013 (+0530)
#           By: subha
#     Update #: 819
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
# Utility for converting a MOOSE model into NeuroML2. This uses Python
# libNeuroML.
#
#

# Change log:
#
# Tue May 21 16:58:03 IST 2013 - Subha moved the code for function
# fitting to hhfit.py.

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

#!!!!! TODO: unit conversion !!!!

try:
    from future_builtins import zip
except ImportError:
    pass
import traceback
import warnings
from collections import deque
import numpy as np
from scipy.optimize import curve_fit
from matplotlib import pyplot as plt

import moose
from moose.utils import autoposition
import neuroml
import hhfit


def convert_morphology(root, positions='auto'):
    """Convert moose neuron morphology contained under `root` into a
    NeuroML object. The id of the return object is
    {root.name}_morphology. Each segment object gets the numeric value
    of the moose id of the object. The name of the segments are same
    as the corresponding moose compartment.

    Parameters
    ----------
    root : a moose element containing a single cell model.

    positions : string
    flag to indicate if the positions of the end points of the
    compartments are explicitly available in the compartments or
    should be automatically generated.  Possible values:

    `auto` - automatically generate z coordinates using length of the
    compartments.

    `explicit` - model has explicit coordinates for all compartments.

    Return
    ------
    a neuroml.Morphology instance.

    """
    if positions == 'auto':
        queue = deque([autoposition(root)])
    elif positions == 'explicit':
        compartments = moose.wildcardFind('%s/##[TYPE=Compartment]' % (root.path))
        queue = deque([compartment for compartment in map(moose.element, compartments)
                  if len(compartment.neighbours['axial']) == 0])
        if len(queue) != 1:
            raise Exception('There must be one and only one top level compartment. Found %d' % (len(topcomp_list)))
    else:
        raise Exception('allowed values for keyword argument positions=`auto` or `explicit`')
    comp_seg = {}
    parent = None
    while len(queue) > 0:
        compartment = queue.popleft()
        proximal = neuroml.Point3DWithDiam(x=compartment.x0,
                                           y=compartment.y0,
                                           z=compartment.z0,
                                           diameter=compartment.diameter)
        distal = neuroml.Point3DWithDiam(x=compartment.x,
                                         y=compartment.y,
                                         z=compartment.z,
                                         diameter=compartment.diameter)
        plist = list(map(moose.element, compartment.neighbours['axial']))
        try:
            parent = neuroml.SegmentParent(segments=comp_seg[moose.element(plist[0])].id)
        except (KeyError, IndexError) as e:
            parent = None
        segment = neuroml.Segment(id=compartment.id_.value,
                                  proximal=proximal,
                                  distal=distal,
                                  parent=parent)
        # TODO: For the time being using numerical value of the moose
        # id for neuroml id.This needs to be updated for handling
        # array elements
        segment.name = compartment.name
        comp_seg[compartment] = segment
        queue.extend([comp for comp in map(moose.element, compartment.neighbours['raxial'])])
    morph = neuroml.Morphology(id='%s_morphology' % (root.name))
    morph.segments.extend(comp_seg.values())
    return morph


def define_vdep_rate(fn, name):
    """Define new component type with generic expressions for voltage
    dependent rate.

    """
    ctype = neuroml.ComponentType(name)
    # This is going to be ugly ...



def convert_hhgate(gate):
    """Convert a MOOSE gate into GateHHRates in NeuroML"""
    hh_rates = neuroml.GateHHRates(id=gate.id_.value, name=gate.name)
    alpha = gate.tableA.copy()
    beta = gate.tableB - alpha
    vrange = np.linspace(gate.min, gate.max, len(alpha))
    afn, ap = hhfit.find_ratefn(vrange, alpha)
    bfn, bp = hhfit.find_ratefn(vrange, beta)
    if afn is None:
        raise Exception('could not find a fitting function for `alpha`')
    if bfn is  None:
        raise Exception('could not find a fitting function for `alpha`')
    afn_type = fn_rate_map[afn]
    afn_component_type = None
    if afn_type is None:
        afn_type, afn_component_type = define_component_type(afn)
    hh_rates.forward_rate = neuroml.HHRate(type=afn_type,
                                           midpoint='%gmV' % (ap[2]),
                                           scale='%gmV' % (ap[1]),
                                           rate='%gper_ms' % (ap[0]))
    bfn_type = fn_rate_map[bfn]
    bfn_component_type = None
    if bfn_type is None:
        bfn_type, bfn_component_type = define_component_type(bfn)
    hh_rates.reverse_rate = neuroml.HHRate(type=bfn_type,
                                           midpoint='%gmV' % (bp[2]),
                                           scale='%gmV' % (bp[1]),
                                           rate='%gper_ms' % (bp[0]))
    return hh_rates, afn_component_type, bfn_component_type


def convert_hhchannel(channel):
    """Convert a moose HHChannel object into a neuroml element.

    TODO: need to check useConcentration option for Ca2+ and V
    dependent gates. How to handle generic expressions???

    """
    nml_channel = neuroml.IonChannel(id=channel.id_.value,
                                     name=channel.name,
                                     type='ionChannelHH',
                                     conductance=channel.Gbar)
    if channel.Xpower > 0:
        hh_rate_x = convert_hhgate(channel.gateX[0])
        hh_rate_x.instances = channel.Xpower
        nml_channel.gate.append(hh_rate_x)
    if channel.Ypower > 0:
        hh_rate_y = convert_hhgate(channel.gateY[0])
        hh_rate_y.instances = channel.Ypower
        nml_channel.gate.append(hh_rate_y)
    if channel.Zpower > 0:
        hh_rate_z = convert_hhgate(channel.gateZ[0])
        hh_rate_y.instances = channel.Zpower
        nml_channel.gate.append(hh_rate_z)
    return nml_channel


#
# converter.py ends here
