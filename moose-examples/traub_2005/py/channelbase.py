# trbchan.py --- 
# 
# Filename: trbchan.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Fri May  4 14:55:52 2012 (+0530)
# Version: 
# Last-Updated: Fri May  3 11:45:07 2013 (+0530)
#           By: subha
#     Update #: 337
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# Base class for channels in Traub model.
# 
# 

# Change log:
# 
# 2012-05-04 14:55:56 (+0530) subha started porting code from
# channel.py in old moose version to dh_branch.
# 

# Code:

from warnings import warn
import numpy as np
import moose
import config

vmin = -120e-3
vmax = 40e-3
vdivs = 640
v_array = np.linspace(vmin, vmax, vdivs+1)

ca_min = 0.0
ca_max = 1000.0
ca_divs = 1000
ca_conc = np.linspace(ca_min, ca_max, ca_divs + 1)

prototypes = {}

def setup_gate_tables(gate, param_dict, bases):
    suffix = None
    if gate.name == 'gateX':
        suffix = 'x'
    elif gate.name == 'gateY':
        suffix = 'y'
    elif gate.name == 'gateZ':
        suffix = 'z'
    else:
        raise NameError('Gate in a channel must have names in [`gateX`, `gateY`, `gateZ`]')
    if suffix in ['x', 'y']:
        gate.min = vmin
        gate.max = vmax
        gate.divs = vdivs
    else:
        gate.min = ca_min
        gate.max = ca_max
        gate.divs = ca_divs
    gate.useInterpolation = True
    keys = ['%s_%s' % (key, suffix) for key in ['tau', 'inf', 'alpha', 'beta', 'tableA', 'tableB']]
    msg = ''
    if keys[0] in param_dict:
        msg = 'Using tau/inf tables'
        gate.tableA = param_dict[keys[1]] / param_dict[keys[0]]
        gate.tableB = 1 / param_dict[keys[0]]
    elif keys[2] in param_dict:
        msg = 'Using alpha/beta tables'
        gate.tableA = param_dict[keys[2]]
        gate.tableB = param_dict[keys[2]] + param_dict[keys[3]]
    elif keys[4] in param_dict:
        msg = 'Using A/B tables'
        gate.tableA = param_dict[keys[4]]
        gate.tableB = param_dict[keys[5]]
    else:
        for base in bases:
            new_bases = base.mro()
            new_param_dict = base.__dict__
            if new_bases:
                new_bases = new_bases[1:]
            if setup_gate_tables(gate, new_param_dict, new_bases):
                msg = 'Gate setup in baseclass: '+base.__class__.__name__
                break
    if msg:
        config.logger.debug('%s: %s' % (gate.path, msg))
        return True
    else:
        config.logger.debug('%s: nothing was setup for this gate' % (gate.path))
        return False
            
def get_class_field(name, cdict, bases, fieldname, default=None):
    if fieldname in cdict:
        config.logger.debug('%s: %s=%s' % (name, fieldname, str(cdict[fieldname])))
        return cdict[fieldname]
    else:
        for base in bases:
            if hasattr(base, fieldname):
                return getattr(base, fieldname)
    # warn('field %s not in the hierarchy of %s class. Returning default value.' % (fieldname, name))
    return default
            

class ChannelMeta(type):
    def __new__(cls, name, bases, cdict):     
        global prototypes
        # classes that set absract=True will be
        # abstract classes. Others will have the prototype insatntiated.
        if  'abstract' in cdict and cdict['abstract'] == True:
            return type.__new__(cls, name, bases, cdict)
        proto = moose.HHChannel('%s/%s' % (config.library.path, name))
        xpower = get_class_field(name, cdict, bases, 'Xpower', default=0.0)
        if xpower > 0:
            proto.Xpower = xpower
            gate = moose.HHGate('%s/gateX' % (proto.path))
            setup_gate_tables(gate, cdict, bases)
            cdict['xGate'] = gate
        ypower = get_class_field(name, cdict, bases, 'Ypower', default=0.0)
        if ypower > 0:
            proto.Ypower = ypower
            gate = moose.HHGate('%s/gateY' % (proto.path))
            setup_gate_tables(gate, cdict, bases)
            cdict['yGate'] = gate
        zpower = get_class_field(name, cdict, bases, 'Zpower', default=0.0)
        if zpower > 0:
            proto.Zpower = zpower
            gate = moose.HHGate('%s/gateZ' % (proto.path))
            setup_gate_tables(gate, cdict, bases)
            cdict['zGate'] = gate
            ca_msg_field = moose.Mstring('%s/addmsg1' % (proto.path))
            ca_msg_field.value = '../CaPool	concOut	. concen'
            proto.instant = get_class_field(name, cdict, bases, 'instant', default=0)
            proto.useConcentration = True
        proto.Ek = get_class_field(name, cdict, bases, 'Ek', default=0.0)
        X = get_class_field(name, cdict, bases, 'X')
        if X is not None:
            proto.X = X
        Y = get_class_field(name, cdict, bases, 'Y')
        if Y is not None:
            proto.Y = Y
        Z = get_class_field(name, cdict, bases, 'Z')
        if Z is not None:
            proto.Z = Z
        mstring_field = get_class_field(name, cdict, bases, 'mstring')
        if mstring_field is not None:
            # print 'mstring_field:', mstring_field
            mstring = moose.Mstring('%s/%s' % (proto.path, mstring_field[0]))
            mstring.value = mstring_field[1]
        if 'annotation' in cdict:
            info = moose.Annotator('%s/info' % (proto.path))
            info.notes = '\n'.join('%s: %s' % kv for kv in cdict['annotation'].items())
            # print proto.path, info.notes
        cdict['prototype'] = proto
        prototypes[name] = proto
        config.logger.info('Created prototype: %s of class %s' % (proto.path, name))        
        return type.__new__(cls, name, bases, cdict)


class ChannelBase(moose.HHChannel):
    annotation = {'cno': 'cno_0000047'}
    abstract = True
    __metaclass__ = ChannelMeta
    def __init__(self, path, xpower=1, ypower=0, Ek=0.0):
        moose.HHChannel.__init__(self, path)


# 
# trbchan.py ends here
