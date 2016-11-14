# cell.py --- 
# 
# Filename: cell.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Fri Mar  9 23:17:17 2012 (+0530)
# Version: 
# Last-Updated: Sat Aug  6 15:25:57 2016 (-0400)
#           By: subha
#     Update #: 694
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
# 2012-03-09 23:17:24 (+0530) Subha started porting the cell.py file
# from old moose to dh_branch.
# 

# Code:
from __future__ import print_function
import csv
import numpy as np
from collections import defaultdict
import moose
import config
from config import logger
import nachans
import kchans
import archan
import cachans
import capool
from channelinit import init_chanlib

channel_types = ['ar',
                 'cad',
                 'cal',
                 'cat',
                 'k2',
                 'ka',
                 'kahp',
                 'kc',
                 'kdr',
                 'km',
                 'naf',
                 'naf2',
                 'nap',
                 'napf']

channel_type_dict = {
    'cad': ['CaPool'],
    'km': ['KM'],
    'ar': ['AR'],
    'cal': ['CaL'],    
    'cat':['CaT', 'CaT_A'],
    'k2': ['K2'],    
    'ka': ['KA', 'KA_IB'],
    'kahp': ['KAHP', 'KAHP_DP','KAHP_SLOWER'],
    'kc': ['KC', 'KC_FAST'],
    'kdr': ['KDR', 'KDR_FS'],
    'nap':['NaP'],
    'naf': ['NaF', 'NaF_TCR'],
    'napf': ['NaPF', 'NaPF_SS','NaPF_TCR'],
    'naf2': ['NaF2', 'NaF2_nRT']}


def read_keyvals(filename):
    """Read the mapping between key value pairs from file.
    
    The file filename should have two columns:

    key value

    """
    ret = defaultdict(set)
    try:
        with(open(filename, 'r')) as level_file:
            for line in level_file:
                tokens = line.split()
                if not tokens:
                    continue
                if len(tokens) != 2:
                    print(filename, ' - Tokens: ', tokens, len(tokens))
                    return None
                ret[tokens[1]].add(tokens[0])
    except IOError:
        config.logger.info('No such file %s' % (filename))
    return ret

def adjust_chanlib(cdict):
    """Update the revarsal potentials for channels. Set the initial X
    value for AR channel. Set the tau for Ca pool."""
    channel_dict = init_chanlib()
    for ch in channel_dict.values():
        config.logger.info('adjusting properties of %s' % (ch.path))
        if isinstance(ch, kchans.KChannel):
            ch.Ek = cdict['EK']
        elif isinstance(ch, nachans.NaChannel):
            ch.Ek = cdict['ENa']
        elif isinstance(ch, cachans.CaChannel):
            ch.Ek = cdict['ECa']
        elif isinstance(ch, archan.AR):
            ch.Ek = cdict['EAR']
            if 'X_AR' in cdict:
                ch.X = cdict['X_AR']        
        elif isinstance(ch, moose.CaConc):
            ch.tau = cdict['TauCa']            
        if isinstance(ch, moose.HHChannel):
            config.logger.debug('%s.Ek = %g' % (ch.path, ch.Ek))

def read_prototype(celltype, cdict):
    """Read the cell prototype file for the specified class. The
    channel properties are updated using values in cdict."""
    filename = '%s/%s.p' % (config.modelSettings.protodir, celltype)
    logger.debug('Reading prototype file %s' % (filename))
    adjust_chanlib(cdict)
    cellpath = '%s/%s' % (config.modelSettings.libpath, celltype)
    if moose.exists(cellpath):
        return moose.element(cellpath)
    for handler in logger.handlers:
        handler.flush()
    proto = moose.loadModel(filename, cellpath)
    # If prototype files do not have absolute compartment positions,
    # set the compartment postions to origin. This will avoid
    # incorrect assignemnt of position when the x/y/z values in
    # prototype file is just to for setting the compartment length.
    if not config.modelSettings.morph_has_postion:
        for comp in moose.wildcardFind('%s/#[TYPE=Compartment]' % (proto.path)):
            comp.x = 0.0
            comp.y = 0.0
            comp.z = 0.0
    leveldict = read_keyvals('%s/%s.levels' % (config.modelSettings.protodir, celltype))
    depths = read_keyvals('%s/%s.depths' % (config.modelSettings.protodir, celltype))
    depthdict = {}
    for level, depthset in depths.items():
        if len(depthset) != 1:
            raise Exception('Depth set must have only one entry.')
        depthdict[level] = depthset.pop()
    assign_depths(proto, depthdict, leveldict)
    config.logger.debug('Read %s with %d compartments' % (celltype, len(moose.wildcardFind('%s/#[TYPE=Compartment]' % (proto.path)))))
    return proto

def assign_depths(cell, depthdict, leveldict):
    """Assign depths to the compartments in the cell. The original
    model assigns sets of compartments to particular levels and a
    depth is specified for each level. This should not be required if
    we have the z value in prototype file.

    cell : (prototype) cell instance

    depth : dict mapping level no. to physical depth

    level : dict mapping level no. to compartment nos. belonging to
    that level.
    """
    if not depthdict:
        return
    for level, depth in depthdict.items():
        z = float(depth)
        complist = leveldict[level]
        for comp_number in complist:
            comp = moose.element('%s/comp_%s' % (cell.path, comp_number))
            comp.z = z

            
class CellMeta(type):
    def __new__(cls, name, bases, cdict):
        if name != 'CellBase':
            proto = read_prototype(name, cdict)            
            annotation = None
            if 'annotation' in cdict:
                annotation = cdict['annotation']
            else:
                for base in bases:
                    if hasattr(base, 'annotation'):
                        annotation = base.annotation
                        break
            if annotation is not None:
                info = moose.Annotator('%s/info' % (proto.path))
                info.notes = '\n'.join('"%s": "%s"' % kv for kv in annotation.items())
            if 'soma_tauCa' in cdict:
                moose.element(proto.path + '/comp_1/CaPool').tau = cdict['soma_tauCa']
            cdict['prototype'] = proto
        return type.__new__(cls, name, bases, cdict)

    
class CellBase(moose.Neuron):
    __metaclass__ = CellMeta
    annotation = {'cno': 'cno_0000020'}
    def __init__(self, path):
        if not moose.exists(path):
            path_tokens = path.rpartition('/')
            moose.copy(self.prototype, path_tokens[0], path_tokens[-1])
        moose.Neutral.__init__(self, path)
        self.solver = moose.HSolve('{}/solver'.format(path, 'solver'))
        self.solver.target = path
        self.solver.dt = config.simulationSettings.simulationDt
        
    def comp(self, number):
        path = '%s/comp_%d' % (self.path, number)
        return moose.element(path)

    @property
    def soma(self):
        return self.comp(1)

    @property
    def presynaptic(self):
        """Presynaptic compartment. Each subclass should define
        _presynaptic as the index of this compartment."""
        return self.comp(self.__class__._presynaptic)

    def dump_cell(self, file_path):
        """Dump the cell information compartment by compartment for
        comparison with NEURON in csv format. All parameters are
        converted to SI units."""
        with open(file_path, 'w') as dump_file:
            fieldnames = ["comp", "len", "dia", "sarea", "xarea", "Em", "Cm","Rm","Ra"]
            for chtype in channel_types:
                if chtype != 'cad':
                    fieldnames += ['e_' + chtype, 'gbar_' + chtype]
                else:
                    fieldnames += ['tau_' + chtype, 'beta_' + chtype]
            # print fieldnames
            writer = csv.DictWriter(dump_file, fieldnames=fieldnames, delimiter=',')
            writer.writeheader()
            comps = moose.wildcardFind('%s/##[TYPE=Compartment]' % (self.path))
            comps = sorted(comps, key=lambda x: int(x.name[0].rpartition('_')[-1]))            
            for comp_e in comps:
                comp = moose.element(comp_e)
                row = {}
                row['comp'] = comp.name
                row['len'] = comp.length
                row['dia'] = comp.diameter
                row['sarea'] = comp.length * comp.diameter * np.pi
                row['xarea'] = comp.diameter * comp.diameter * np.pi/4
                row['Em'] = comp.Em
                row['Cm'] = comp.Cm
                row['Rm'] = comp.Rm
                row['Ra'] = comp.Ra
                if moose.exists(comp.path + '/CaPool'):
                    ca_pool = moose.CaConc(comp.path + '/CaPool')
                for chtype in channel_types:
                    found = False
                    for chname in channel_type_dict[chtype]:
                        chpath = comp.path + '/' + chname
                        if moose.exists(chpath):
                            found = True
                            channel = moose.element(chpath)
                            if channel.className == 'HHChannel':
                                row['e_'+chtype] = channel.Ek
                                row['gbar_'+chtype] = channel.Gbar                        
                            elif channel.className == 'CaConc':
                                row['tau_cad'] = channel.tau
                                row['beta_cad'] = channel.B
                            break
                    if not found:
                        if chtype != 'cad':
                            row['e_'+chtype] = 0.0
                            row['gbar_'+chtype] = 0.0
                        else:
                            row['tau_cad'] = 0.0
                            row['beta_cad'] = 0.0
                writer.writerow(row)


class SupPyrRS(CellBase):
    _presynaptic = 72
    ENa = 50e-3
    EK = -95e-3
    ECa = 125e-3
    EAR = -35e-3
    EGABA = -81e-3
    TauCa = 20e-3
    soma_tauCa  = 100e-3

    def __init__(self, path):
        CellBase.__init__(self, path)

    
class SupPyrFRB(CellBase):
    _presynaptic = 72
    ENa = 50e-3
    EK = -95e-3
    EAR = -35e-3
    ECa = 125e-3
    EGABA = -81e-3
    TauCa = 20e-3    
    soma_tauCa = 100e-3

    def __init__(self, path):
        CellBase.__init__(self, path)

    
class SupLTS(CellBase):
    _presynaptic = 59
    ENa = 50e-3
    EK = -100e-3
    ECa = 125e-3
    EAR = -40e-3 # dummy to set things back to original
    EGABA = -75e-3 # Sanchez-Vives et al. 1997 
    TauCa = 20e-3
    X_AR = 0.25
    soma_tauCa = 50e-3

    def __init__(self, path):
        CellBase.__init__(self, path)
    

class SupAxoaxonic(CellBase):
    _presynaptic = 59
    ENa = 50e-3
    EK = -100e-3
    ECa = 125e-3
    EAR = -40e-3
    EGABA = -75e-3
    X_AR = 0.0
    TauCa = 20e-3
    soma_tauCa = 50e-3

    def __init__(self, path):
        CellBase.__init__(self, path)


class SupBasket(CellBase):
    _presynaptic = 59
    ENa = 50e-3
    EK = -100e-3
    EAR = -40e-3
    ECa = 125e-3
    EGABA = -75e-3 # Sanchez-Vives et al. 1997 
    TauCa = 20e-3
    X_AR = 0.0
    soma_tauCa = 50e-3

    def __init__(self, path):
        CellBase.__init__(self, path)

    
class SpinyStellate(CellBase):
    """Spiny-stellate cells of layer 4."""
    _presynaptic = 57
    ENa = 50e-3
    EK = -100e-3
    EAR = -40e-3
    ECa = 125e-3
    EGABA = -75e-3
    TauCa = 20e-3
    X_AR = 0.0
    soma_tauCa = 50e-3

    def __init__(self, path):
        CellBase.__init__(self, path)

    
class NontuftedRS(CellBase):
    _presynaptic = 48
    ENa = 50e-3
    EK = -95e-3
    EAR = -35e-3
    ECa = 125e-3
    EGABA = -75e-3 # Sanchez-Vives et al. 1997 
    TauCa = 20e-3
    X_AR = 0.25
    soma_tauCa = 100e-3

    def __init__(self, path):
        CellBase.__init__(self, path)


class TuftedIB(CellBase):    
    _presynaptic = 60
    ENa = 50e-3
    EK = -95e-3
    EAR = -35e-3
    ECa = 125e-3
    EGABA = -75e-3 # Sanchez-Vives et al. 1997 
    TauCa = 1e-3/0.075
    X_AR = 0.25
    soma_tauCa = 100e-3

    def __init__(self, path):
        CellBase.__init__(self, path)
    
    # for compartments in level 2, i.e. comp_2, 5, 6 have tauCa = 1e-3/0.02        
    @classmethod
    def post_init(cls):
        moose.element(cls.prototype.path + '/comp_2/CaPool').tau = 1e-3/0.02
        moose.element(cls.prototype.path + '/comp_5/CaPool').tau = 1e-3/0.02
        moose.element(cls.prototype.path + '/comp_6/CaPool').tau = 1e-3/0.02


TuftedIB.post_init()


class TuftedRS(CellBase):
    _presynaptic = 60
    ENa = 50e-3
    EK = -95e-3
    EAR = -35e-3
    ECa = 125e-3
    EGABA = -75e-3 # Sanchez-Vives et al. 1997 
    TauCa = 1e-3/0.075
    X_AR = 0.25
    soma_tauCa = 100e-3

    def __init__(self, path):
        CellBase.__init__(self, path)
    
    @classmethod
    def post_init(cls):
        moose.element(cls.prototype.path + '/comp_2/CaPool').tau = 1e-3/0.02
        moose.element(cls.prototype.path + '/comp_5/CaPool').tau = 1e-3/0.02
        moose.element(cls.prototype.path + '/comp_6/CaPool').tau = 1e-3/0.02

TuftedRS.post_init()


class DeepLTS(CellBase):
    _presynaptic = 59
    ENa = 50e-3
    EK = -100e-3
    EAR = -40e-3
    ECa = 125e-3
    EGABA = -75e-3 # Sanchez-Vives et al. 1997 
    TauCa = 20e-3
    X_AR = 0.25
    soma_tauCa = 50e-3

    def __init__(self, path):
        CellBase.__init__(self, path)


class DeepAxoaxonic(CellBase):
    _presynaptic = 59
    ENa = 50e-3
    EK = -100e-3
    ECa = 125e-3
    EAR = -40e-3
    EGABA = -75e-3
    X_AR = 0.0
    TauCa = 20e-3
    soma_tauCa = 50e-3

    def __init__(self, path):
        CellBase.__init__(self, path)

    
class DeepBasket(CellBase):
    _presynaptic = 59
    ENa = 50e-3
    EK = -100e-3
    EAR = -40e-3
    ECa = 125e-3
    EGABA = -75e-3 # Sanchez-Vives et al. 1997 
    TauCa = 20e-3
    X_AR = 0.25
    soma_tauCa = 50e-3

    def __init__(self, path):
        CellBase.__init__(self, path)

    
class TCR(CellBase):
    _presynaptic = 135
    ENa = 50e-3
    EK = -95e-3
    EAR = -35e-3
    ECa = 125e-3
    EGABA = -81e-3
    TauCa = 20e-3
    X_AR = 0.25
    soma_tauCa = 50e-3

    def __init__(self, path):
        CellBase.__init__(self, path)


class nRT(CellBase):
    _presynaptic = 59
    ENa = 50e-3
    EK = -100e-3
    EAR = -40e-3
    ECa = 125e-3
    EGABA = -75e-3 # Sanchez-Vives et al. 1997 
    TauCa = 20e-3
    X_AR = 0.0
    soma_tauCa = 50e-3

    def __init__(self, path):
        CellBase.__init__(self, path)


_cellprototypes = {}

def init_prototypes():
    global _cellprototypes
    if _cellprototypes:
        return _cellprototypes
    _cellprototypes = {
        'SupPyrRS': SupPyrRS(SupPyrRS.prototype.path),
        'SupPyrFRB': SupPyrFRB(SupPyrFRB.prototype.path),
        'SupLTS': SupLTS(SupLTS.prototype.path),
        'SupAxoaxonic': SupAxoaxonic(SupAxoaxonic.prototype.path),
        'SupBasket': SupBasket(SupBasket.prototype.path),
        'SpinyStellate': SpinyStellate(SpinyStellate.prototype.path),
        'NontuftedRS': NontuftedRS(NontuftedRS.prototype.path),
        'TuftedIB': TuftedIB(TuftedIB.prototype.path),
        'TuftedRS': TuftedRS(TuftedRS.prototype.path),
        'DeepLTS':  DeepLTS(DeepLTS.prototype.path),
        'DeepAxoaxonic': DeepAxoaxonic(DeepAxoaxonic.prototype.path),
        'DeepBasket': DeepBasket(DeepBasket.prototype.path),
        'TCR': TCR(TCR.prototype.path),
        'nRT': nRT(nRT.prototype.path),
    }
    return _cellprototypes


# 
# cells.py ends here
