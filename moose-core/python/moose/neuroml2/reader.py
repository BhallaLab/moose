# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import

# reader.py ---
# 
# Filename: reader.py
# Description:
# Author: Subhasis Ray, Padraig Gleeson
# Maintainer: 
# Created: Wed Jul 24 15:55:54 2013 (+0530)
# Version: 
# Last-Updated: 15 Jan 2018, pgleeson
#               16 Jan 2018, dilawar, python3 compatible imports.
#             

"""Implementation of reader for NeuroML 2 models.
TODO: handle morphologies of more than one segment...
"""

try:
    from future_builtins import zip, map
except ImportError:
    pass

import sys
import os
import math
import numpy as np
import neuroml as nml
from pyneuroml import pynml
import moose
import moose.utils as mu
from .units import SI


def _unique( ls ):
    res = [ ]
    for l in ls:
        if l not in res:
            res.append( l )
    return res

def sarea(comp):
    """
    Return the surface area of compartment from length and
    diameter.

    Parameters
    ----------
    comp : Compartment instance.

    Returns
    -------
    s : float
        surface area of `comp`.

    """
    if comp.length > 0:
        return comp.length * comp.diameter * np.pi
    else:
        return comp.diameter * comp.diameter * np.pi

def xarea(comp):
    """
    Return the cross sectional area from diameter of the
    compartment. How to do it for spherical compartment?"""
    return comp.diameter * comp.diameter * np.pi / 4.0

def setRa(comp, resistivity):
    """Calculate total raxial from specific value `resistivity`"""
    if comp.length > 0:
        comp.Ra = resistivity * comp.length / xarea(comp)
    else:
        comp.Ra = resistivity * 8.0 / (comp.diameter * np.pi)

def setRm(comp, condDensity):
    """Set membrane resistance"""
    comp.Rm = 1/(condDensity * sarea(comp))

def setEk(comp, erev):
    """Set reversal potential"""
    comp.setEm(erev)


def getSegments(nmlcell, component, sg_to_segments):
    """Get the list of segments the `component` is applied to"""
    sg = component.segment_groups
    if sg is None:
        segments = nmlcell.morphology.segments
    elif sg == 'all': 
        segments = [seg for seglist in sg_to_segments.values() for seg in seglist]
    else:
        segments = sg_to_segments[sg]
    return _unique(segments)


class NML2Reader(object):
    """Reads NeuroML2 and creates MOOSE model. 

    NML2Reader.read(filename) reads an NML2 model under `/library`
    with the toplevel name defined in the NML2 file.

    Example:

    >>> import moose
    >>> reader = moose.NML2Reader()
    >>> reader.read('moose/neuroml2/test_files/Purk2M9s.nml')

    creates a passive neuronal morphology `/library/Purk2M9s`.
    """
    def __init__(self, verbose=False):
        self.lunit = 1e-6 # micron is the default length unit
        self.verbose = verbose
        self.doc = None
        self.filename = None        
        self.nml_to_moose = {} # NeuroML object to MOOSE object
        self.moose_to_nml = {} # Moose object to NeuroML object
        self.proto_cells = {}  # map id to prototype cell in moose
        self.proto_chans = {}  # map id to prototype channels in moose
        self.proto_pools = {}  # map id to prototype pools (Ca2+, Mg2+)
        self.includes = {}     # Included files mapped to other readers
        self.lib = moose.Neutral('/library')
        self.id_to_ionChannel = {}
        self._cell_to_sg = {} # nml cell to dict - the dict maps segment groups to segments
        
        self.cells_in_populations = {}
        self.pop_to_cell_type = {}
        self.seg_id_to_comp_name = {}
        self.paths_to_chan_elements = {}
        self.network = None

    def read(self, filename, symmetric=True):
        filename = os.path.realpath( filename )
        self.doc = nml.loaders.read_neuroml2_file( 
                filename, include_includes=True, verbose=self.verbose)
        
        if self.verbose:
            mu.info('Parsed NeuroML2 file: %s'% filename)
        self.filename = filename
        
        if len(self.doc.networks)>=1:
            self.network = self.doc.networks[0]
            moose.celsius = self._getTemperature()
            
            
        self.importConcentrationModels(self.doc)
        self.importIonChannels(self.doc)
        self.importInputs(self.doc)
        
        
        for cell in self.doc.cells:
            self.createCellPrototype(cell, symmetric=symmetric)
            
        if len(self.doc.networks)>=1:
            self.createPopulations()
            self.createInputs()
        mu.info("Read all from %s"%filename)
        
    def _getTemperature(self):
        if self.network is not None:
            if self.network.type=="networkWithTemperature":
                return SI(self.network.temperature)
            else:
                # Why not, if there's no temp dependence in nml..?
                return 0
        return SI('25')
        
    def getCellInPopulation(self, pop_id, index):
        return self.cells_in_populations[pop_id][index]
    
    def getComp(self, pop_id, cellIndex, segId):
        return moose.element('%s/%s/%s/%s' % ( self.lib.path, pop_id, cellIndex
            , self.seg_id_to_comp_name[self.pop_to_cell_type[pop_id]][segId])
            )
            
    def createPopulations(self):
        for pop in self.network.populations:
            ep = '%s/%s' % (self.lib.path, pop.id)
            mpop = moose.element(ep) if moose.exists(ep) else moose.Neutral(ep)
            self.cells_in_populations[pop.id] ={}
            for i in range(pop.size):
                mu.info("Creating %s/%s instances of %s under %s"%(i,pop.size,pop.component, mpop))
                self.pop_to_cell_type[pop.id]=pop.component
                chid = moose.copy(self.proto_cells[pop.component], mpop, '%s'%(i))
                self.cells_in_populations[pop.id][i]=chid
                
                
    def getInput(self, input_id):
        return moose.element('%s/inputs/%s'%(self.lib.path,input_id))
               
                
    def createInputs(self):
        for el in self.network.explicit_inputs:
            pop_id = el.target.split('[')[0]
            i = el.target.split('[')[1].split(']')[0]
            seg_id = 0
            if '/' in el.target:
                seg_id = el.target.split('/')[1]
            input = self.getInput(el.input)
            moose.connect(input, 'output', self.getComp(pop_id,i,seg_id), 'injectMsg')
            
        for il in self.network.input_lists:
            for ii in il.input:
                input = self.getInput(il.component)
                moose.connect(input, 'output'
                        , self.getComp(il.populations,ii.get_target_cell__hash(),ii.get_segment__hash())
                        , 'injectMsg')
            

    def createCellPrototype(self, cell, symmetric=True):
        """To be completed - create the morphology, channels in prototype"""
        ep = '%s/%s' % (self.lib.path, cell.id)
        nrn = moose.element(ep) if moose.exists(ep) else moose.Neuron(ep)
        self.proto_cells[cell.id] = nrn
        self.nml_to_moose[cell] = nrn
        self.moose_to_nml[nrn] = cell
        self.createMorphology(cell, nrn, symmetric=symmetric)
        self.importBiophysics(cell, nrn)
        return cell, nrn


    def createMorphology(self, nmlcell, moosecell, symmetric=True):
        """Create the MOOSE compartmental morphology in `moosecell` using the
        segments in NeuroML2 cell `nmlcell`. Create symmetric
        compartments if `symmetric` is True.

        """
        morphology = nmlcell.morphology
        segments = morphology.segments
        id_to_segment = dict([(seg.id, seg) for seg in segments])    
        if symmetric:
            compclass = moose.SymCompartment
        else:
            compclass = moose.Compartment
        # segment names are used as compartment names - assuming
        # naming convention does not clash with that in MOOSE
        cellpath = moosecell.path
        id_to_comp = {}
        self.seg_id_to_comp_name[nmlcell.id]={}
        for seg in segments:
            if seg.name is not None:
                ep = '%s/%s' % (cellpath, seg.name)
                id_to_comp[seg.id] = moose.element(ep) if moose.exists(ep) else compclass(ep)
                self.seg_id_to_comp_name[nmlcell.id][seg.id] = seg.name
            else:
                name = 'comp_%s'%seg.id
                ep = '%s/%s' % (cellpath, name)
                id_to_comp[seg.id] = moose.element(ep) if moose.exists(ep) else compclass(ep)
                self.seg_id_to_comp_name[nmlcell.id][seg.id] = name
        # Now assign the positions and connect up axial resistance
        if not symmetric:
            src, dst = 'axial', 'raxial'
        else:
            src, dst = 'proximal', 'distal'
        for segid, comp in id_to_comp.items():
            segment = id_to_segment[segid]
            try:
                parent = id_to_segment[segment.parent.segments]
            except AttributeError:
                parent = None
            self.moose_to_nml[comp] = segment
            self.nml_to_moose[segment] = comp            
            p0 = segment.proximal            
            if p0 is None:
                if parent:
                    p0 = parent.distal
                else:
                    raise Exception(
                    'No proximal point and no parent segment for segment:'+\
                            'name=%s, id=%s' % (segment.name, segment.id)
                        )
            comp.x0, comp.y0, comp.z0 = (x*self.lunit for x in map(float, (p0.x, p0.y, p0.z)))
            p1 = segment.distal
            comp.x, comp.y, comp.z = (x*self.lunit for x in map(float, (p1.x, p1.y, p1.z)))
            comp.length = np.sqrt((comp.x-comp.x0)**2+(comp.y-comp.y0)**2+(comp.z-comp.z0)**2)

            # This can pose problem with moose where both ends of
            # compartment have same diameter. We are averaging the two
            # - may be splitting the compartment into two is better?
            comp.diameter = (float(p0.diameter)+float(p1.diameter)) * self.lunit / 2
            if parent:
                pcomp = id_to_comp[parent.id]
                moose.connect(comp, src, pcomp, dst)
        sg_to_segments = {}        
        for sg in morphology.segment_groups:
            sg_to_segments[sg.id] = [id_to_segment[m.segments] for m in sg.members]
        for sg in morphology.segment_groups:
            if not sg.id in sg_to_segments:
                sg_to_segments[sg.id] = []
            for inc in sg.includes:
                for cseg in sg_to_segments[inc.segment_groups]:
                    sg_to_segments[sg.id].append(cseg)
            
        if not 'all' in sg_to_segments:
            sg_to_segments['all'] = [ s for s in segments ]
            
        self._cell_to_sg[nmlcell] = sg_to_segments
        return id_to_comp, id_to_segment, sg_to_segments

    def importBiophysics(self, nmlcell, moosecell):
        """Create the biophysical components in moose Neuron `moosecell`
        according to NeuroML2 cell `nmlcell`."""
        bp = nmlcell.biophysical_properties
        if bp is None:
            mu.info('Warning: %s in %s has no biophysical properties' % (nmlcell.id, self.filename))
            return
        self.importMembraneProperties(nmlcell, moosecell, bp.membrane_properties)
        self.importIntracellularProperties(nmlcell, moosecell, bp.intracellular_properties)

    def importMembraneProperties(self, nmlcell, moosecell, mp):
        """Create the membrane properties from nmlcell in moosecell"""
        if self.verbose:
            mu.info('Importing membrane properties')
        self.importCapacitances(nmlcell, moosecell, mp.specific_capacitances)
        self.importChannelsToCell(nmlcell, moosecell, mp)
        self.importInitMembPotential(nmlcell, moosecell, mp)

    def importCapacitances(self, nmlcell, moosecell, specificCapacitances):
        sg_to_segments = self._cell_to_sg[nmlcell]
        for specific_cm in specificCapacitances:
            cm = SI(specific_cm.value)
            for seg in sg_to_segments[specific_cm.segment_groups]:
                comp = self.nml_to_moose[seg]
                comp.Cm = sarea(comp) * cm
                
    def importInitMembPotential(self, nmlcell, moosecell, membraneProperties):
        sg_to_segments = self._cell_to_sg[nmlcell]
        for imp in membraneProperties.init_memb_potentials:
            initv = SI(imp.value)
            for seg in sg_to_segments[imp.segment_groups]:
                comp = self.nml_to_moose[seg]
                comp.initVm = initv 

    def importIntracellularProperties(self, nmlcell, moosecell, properties):
        self.importAxialResistance(nmlcell, properties)
        self.importSpecies(nmlcell, properties)

    def importSpecies(self, nmlcell, properties):
        sg_to_segments = self._cell_to_sg[nmlcell]
        for species in properties.species:
            if (species.concentration_model is not None) and \
               (species.concentration_model.id  not in self.proto_pools):
                continue
            segments = getSegments(nmlcell, species, sg_to_segments)
            for seg in segments:
                comp = self.nml_to_moose[seg]    
                self.copySpecies(species, comp)

    def copySpecies(self, species, compartment):
        """Copy the prototype pool `species` to compartment. Currently only
        decaying pool of Ca2+ supported"""
        proto_pool = None
        if species.concentrationModel in self.proto_pools:
            proto_pool = self.proto_pools[species.concentration_model]
        else:
            for innerReader in self.includes.values():
                if species.concentrationModel in innerReader.proto_pools:
                    proto_pool = innerReader.proto_pools[species.concentrationModel]
                    break
        if not proto_pool:
            raise Exception('No prototype pool for %s referred to by %s' % ( 
                    species.concentration_model, species.id)
                )
        pool_id = moose.copy(proto_pool, compartment, species.id)
        pool = moose.element(pool_id)
        pool.B = pool.B / (np.pi * compartment.length * ( 
            0.5 * compartment.diameter + pool.thickness) * 
            (0.5 * compartment.diameter - pool.thickness)
            )
        return pool

    def importAxialResistance(self, nmlcell, intracellularProperties):
        sg_to_segments = self._cell_to_sg[nmlcell]
        for r in intracellularProperties.resistivities:
            segments = getSegments(nmlcell, r, sg_to_segments)
            for seg in segments:
                comp = self.nml_to_moose[seg]
                setRa(comp, SI(r.value))     
                
    def isPassiveChan(self,chan):
        if chan.type == 'ionChannelPassive':
            return True
        if hasattr(chan,'gates'):
            return len(chan.gate_hh_rates)+len(chan.gates)==0
        return False
    

    def calculateRateFn(self, ratefn, vmin, vmax, tablen=3000, vShift='0mV'):
        """Returns A / B table from ngate."""
        from . import hhfit
        rate_fn_map = {
            'HHExpRate': hhfit.exponential2,
            'HHSigmoidRate': hhfit.sigmoid2,
            'HHSigmoidVariable': hhfit.sigmoid2,
            'HHExpLinearRate': hhfit.linoid2 }

        tab = np.linspace(vmin, vmax, tablen)
        if self._is_standard_nml_rate(ratefn):
            midpoint, rate, scale = map(SI, (ratefn.midpoint, ratefn.rate, ratefn.scale))
            return rate_fn_map[ratefn.type](tab, rate, scale, midpoint)
        else:
            for ct in self.doc.ComponentType:
                if ratefn.type == ct.name:
                    mu.info("Using %s to evaluate rate"%ct.name)
                    rate = []
                    for v in tab:
                        req_vars  = {'v':'%sV'%v,'vShift':vShift,'temperature':self._getTemperature()}
                        vals = pynml.evaluate_component(ct, req_variables =  req_vars)
                        if 'x' in vals:
                            rate.append(vals['x'])
                        if 't' in vals:
                            rate.append(vals['t'])
                        if 'r' in vals:
                            rate.append(vals['r'])
                    return np.array(rate)

        print( "[WARN ] Could not determine rate: %s %s %s" %(ratefn.type,vmin,vmax))
        return np.array([])

    def importChannelsToCell(self, nmlcell, moosecell, membrane_properties):
        sg_to_segments = self._cell_to_sg[nmlcell]
        for chdens in membrane_properties.channel_densities + membrane_properties.channel_density_v_shifts:
            segments = getSegments(nmlcell, chdens, sg_to_segments)
            condDensity = SI(chdens.cond_density)
            erev = SI(chdens.erev)
            try:
                ionChannel = self.id_to_ionChannel[chdens.ion_channel]
            except KeyError:
                mu.info('No channel with id: %s' % chdens.ion_channel)                
                continue
                
            if self.verbose:
                mu.info('Setting density of channel %s in %s to %s; erev=%s (passive: %s)'%(
                    chdens.id, segments, condDensity,erev,self.isPassiveChan(ionChannel))
                    )
            
            if self.isPassiveChan(ionChannel):
                for seg in segments:
                    comp = self.nml_to_moose[seg]
                    setRm(self.nml_to_moose[seg], condDensity)
                    setEk(self.nml_to_moose[seg], erev)
            else:
                for seg in segments:
                    self.copyChannel(chdens, self.nml_to_moose[seg], condDensity, erev)
            '''moose.le(self.nml_to_moose[seg])
            moose.showfield(self.nml_to_moose[seg], field="*", showtype=True)'''

    def copyChannel(self, chdens, comp, condDensity, erev):
        """Copy moose prototype for `chdens` condutcance density to `comp`
        compartment.

        """
        proto_chan = None
        if chdens.ion_channel in self.proto_chans:
            proto_chan = self.proto_chans[chdens.ion_channel]
        else:
            for innerReader in self.includes.values():
                if chdens.ionChannel in innerReader.proto_chans:
                    proto_chan = innerReader.proto_chans[chdens.ion_channel]
                    break
        if not proto_chan:
            raise Exception('No prototype channel for %s referred to by %s' % (chdens.ion_channel, chdens.id))

        if self.verbose:
            mu.info('Copying %s to %s, %s; erev=%s'%(chdens.id, comp, condDensity, erev))
        orig = chdens.id
        chid = moose.copy(proto_chan, comp, chdens.id)
        chan = moose.element(chid)
        for p in self.paths_to_chan_elements.keys():
            pp = p.replace('%s/'%chdens.ion_channel,'%s/'%orig)
            self.paths_to_chan_elements[pp] = self.paths_to_chan_elements[p].replace('%s/'%chdens.ion_channel,'%s/'%orig)
        #mu.info(self.paths_to_chan_elements)
        chan.Gbar = sarea(comp) * condDensity
        chan.Ek = erev
        moose.connect(chan, 'channel', comp, 'channel')
        return chan    

    def _is_standard_nml_rate(self, rate):
        return rate.type=='HHExpLinearRate' \
               or rate.type=='HHExpRate' or \
               rate.type=='HHSigmoidRate' or \
               rate.type=='HHSigmoidVariable'

    def createHHChannel(self, chan, vmin=-150e-3, vmax=100e-3, vdivs=5000):
        mchan = moose.HHChannel('%s/%s' % (self.lib.path, chan.id))
        mgates = [moose.element(x) for x in [mchan.gateX, mchan.gateY, mchan.gateZ]]
        assert len(chan.gate_hh_rates)<=3, "We handle only up to 3 gates in HHCHannel"
        
        if self.verbose:
            mu.info('== Creating channel: %s (%s) -> %s (%s)'%(chan.id, chan.gate_hh_rates, mchan, mgates))

        all_gates = chan.gates + chan.gate_hh_rates
        for ngate, mgate in zip(all_gates,mgates):
            if mgate.name.endswith('X'):
                mchan.Xpower = ngate.instances
            elif mgate.name.endswith('Y'):
                mchan.Ypower = ngate.instances
            elif mgate.name.endswith('Z'):
                mchan.Zpower = ngate.instance
            mgate.min = vmin
            mgate.max = vmax
            mgate.divs = vdivs

            # I saw only examples of GateHHRates in
            # HH-channels, the meaning of forwardRate and
            # reverseRate and steadyState are not clear in the
            # classes GateHHRatesInf, GateHHRatesTau and in
            # FateHHTauInf the meaning of timeCourse and
            # steady state is not obvious. Is the last one
            # refering to tau_inf and m_inf??
            fwd = ngate.forward_rate
            rev = ngate.reverse_rate
            self.paths_to_chan_elements['%s/%s'%(chan.id,ngate.id)] = '%s/%s'%(chan.id,mgate.name)
            q10_scale = 1
            if ngate.q10_settings:
                if ngate.q10_settings.type == 'q10Fixed':
                    q10_scale= float(ngate.q10_settings.fixed_q10)
                elif ngate.q10_settings.type == 'q10ExpTemp':
                    q10_scale = math.pow( float(ngate.q10_settings.q10_factor)
                            , (self._getTemperature()- SI(ngate.q10_settings.experimental_temp))/10
                            )
                else:
                    raise Exception('Unknown Q10 scaling type %s: %s'%( 
                        ngate.q10_settings.type,ngate.q10_settings)
                        )
                    
            if self.verbose:
                mu.info(' === Gate: %s; %s; %s; %s; %s; scale=%s'% ( 
                    ngate.id, mgate.path, mchan.Xpower, fwd, rev, q10_scale)
                    )
                
            if (fwd is not None) and (rev is not None):
                alpha = self.calculateRateFn(fwd, vmin, vmax, vdivs)
                beta = self.calculateRateFn(rev, vmin, vmax, vdivs)

                mgate.tableA = q10_scale * (alpha)
                mgate.tableB = q10_scale * (alpha + beta)

            # Assuming the meaning of the elements in GateHHTauInf ...
            if hasattr(ngate,'time_course') and hasattr(ngate,'steady_state') \
               and (ngate.time_course is not None) and (ngate.steady_state is not None):
                tau = ngate.time_course
                inf = ngate.steady_state
                tau = self.calculateRateFn(tau, vmin, vmax, vdivs)
                inf = self.calculateRateFn(inf, vmin, vmax, vdivs)
                mgate.tableA = q10_scale * (inf / tau)
                mgate.tableB = q10_scale * (1 / tau)
                
            if hasattr(ngate,'steady_state') and (ngate.time_course is None) and (ngate.steady_state is not None):
                inf = ngate.steady_state
                tau = 1 / (alpha + beta)
                if inf is not None:
                    inf = self.calculateRateFn(inf, vmin, vmax, vdivs)
                    if len(inf) > 0:
                        mgate.tableA = q10_scale * (inf / tau)
                        mgate.tableB = q10_scale * (1 / tau)
                
        if self.verbose:
            mu.info('%s: Created %s for %s'%(self.filename,mchan.path,chan.id))
        return mchan

    def createPassiveChannel(self, chan):
        epath = '%s/%s' % (self.lib.path, chan.id)
        if moose.exists( epath ):
            mchan = moose.element(epath)
        else:
            mchan = moose.Leakage(epath)
        if self.verbose:
            mu.info('%s: Created %s for %s'%(self.filename,mchan.path,chan.id))
        return mchan

    def importInputs(self, doc):
        epath = '%s/inputs' % (self.lib.path)
        if moose.exists( epath ):
            minputs = moose.element( epath )
        else:
            minputs = moose.Neutral( epath )

        for pg_nml in doc.pulse_generators:
            epath = '%s/%s' % (minputs.path, pg_nml.id)
            pg = moose.element(epath) if moose.exists(epath) else moose.PulseGen(epath)
            pg.firstDelay = SI(pg_nml.delay)
            pg.firstWidth = SI(pg_nml.duration)
            pg.firstLevel = SI(pg_nml.amplitude)
            pg.secondDelay = 1e9
        

    def importIonChannels(self, doc, vmin=-150e-3, vmax=100e-3, vdivs=5000):
        if self.verbose:
            mu.info('%s: Importing the ion channels' % self.filename )
            
        for chan in doc.ion_channel+doc.ion_channel_hhs:
            if chan.type == 'ionChannelHH':
                mchan = self.createHHChannel(chan)
            elif self.isPassiveChan(chan):
                mchan = self.createPassiveChannel(chan)
            else:
                mchan = self.createHHChannel(chan)
                
            self.id_to_ionChannel[chan.id] = chan
            self.nml_to_moose[chan] = mchan
            self.proto_chans[chan.id] = mchan
            if self.verbose:
                mu.info( self.filename + ': Created ion channel %s for %s %s'%( 
                    mchan.path, chan.type, chan.id))

    def importConcentrationModels(self, doc):
        for concModel in doc.decaying_pool_concentration_models:
            self.createDecayingPoolConcentrationModel(concModel)

    def createDecayingPoolConcentrationModel(self, concModel):
        """Create prototype for concentration model"""        
        if concModel.name is not None:
            name = concModel.name
        else:
            name = concModel.id
        ca = moose.CaConc('%s/%s' % (self.lib.path, id))
        ca.CaBasal = SI(concModel.restingConc)
        ca.tau = SI(concModel.decayConstant)
        ca.thick = SI(concModel.shellThickness)
        ca.B = 5.2e-6 # B = 5.2e-6/(Ad) where A is the area of the 
                      # shell and d is thickness - must divide by 
                      # shell volume when copying
        self.proto_pools[concModel.id] = ca
        self.nml_to_moose[concModel.id] = ca
        self.moose_to_nml[ca] = concModel
        mu.debug('Created moose element: %s for nml conc %s' % (ca.path, concModel.id))
