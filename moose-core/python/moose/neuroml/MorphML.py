# -*- coding: utf-8 -*-
## Description: class MorphML for loading MorphML from file or xml element into MOOSE
## Version 1.0 by Aditya Gilra, NCBS, Bangalore, India, 2011 for serial MOOSE
## Version 1.5 by Niraj Dudani, NCBS, Bangalore, India, 2012, ported to parallel MOOSE
## Version 1.6 by Aditya Gilra, NCBS, Bangalore, India, 2012, further changes for parallel MOOSE
## Version 1.7 by Aditya Gilra, NCBS, Bangalore, India, 2013, further support for NeuroML 1.8.1
## Version 1.8 by Aditya Gilra, NCBS, Bangalore, India, 2013, changes for new IntFire and SynHandler classes

"""
NeuroML.py is the preferred interface. Use this only if NeuroML L1,L2,L3 files are misnamed/scattered.
Instantiate MorphML class, and thence use methods:
readMorphMLFromFile(...) to load a standalone MorphML from file OR
readMorphML(...) to load from an xml.etree xml element (could be part of a larger NeuroML file).
It is assumed that any channels and synapses referred to by above MorphML
have already been loaded under that same name in /library in MOOSE (use ChannelML loader).
"""

from __future__ import print_function
from xml.etree import cElementTree as ET # cELementTree is mostly API-compatible but faster than ElementTree
import string
import sys
import math
from os import path
import moose
from moose import utils as moose_utils
from moose.neuroml import utils as neuroml_utils
from moose.neuroml.ChannelML import ChannelML, make_new_synapse

import logging
# define a Handler which writes INFO messages or higher to the sys.stderr
console = logging.StreamHandler()
console.setLevel(logging.INFO)
# set a format which is simpler for console use
formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
# tell the handler to use this format
console.setFormatter(formatter)
# add the handler to the root logger
logging.getLogger('moose.nml.morphml').addHandler(console)
_logger = logging.getLogger('')


class MorphML():

    def __init__(self,nml_params):
        self.neuroml='http://morphml.org/neuroml/schema'
        self.bio='http://morphml.org/biophysics/schema'
        self.mml='http://morphml.org/morphml/schema'
        self.nml='http://morphml.org/networkml/schema'
        self.meta='http://morphml.org/metadata/schema'
        self.cellDictBySegmentId={}
        self.cellDictByCableId={}
        self.nml_params = nml_params
        self.model_dir = nml_params['model_dir']
        self.temperature = nml_params['temperature']

    def readMorphMLFromFile(self,filename,params={}):
        """
        specify params for this MorphML file as a dict:
         presently combineSegments and createPotentialSynapses are implemented.
         See readMorphML().
        See also nml_params in __init__().
        returns { cellname1 : (segDict,cableDict), ... }
        see readMorphML(...) for segDict and cableDict
        """

        _logger.info("Reading morphology from %s" % filename)
        try:
            tree = ET.parse(filename)
        except Exception as e:
            _logger.error("Failed to load morphology from file %s" % filename)

        neuroml_element = tree.getroot()
        cellsDict = {}
        for cell in neuroml_element.findall('.//{'+self.neuroml+'}cell'):
            if 'lengthUnits' in neuroml_element.attrib:
                lengthUnits = neuroml_element.attrib['lengthUnits']
            else:
                lengthUnits = 'micrometer'
            cellDict = self.readMorphML(cell,params,lengthUnits)
            cellsDict.update(cellDict)
        return cellsDict

    def readMorphML(self,cell,params={},lengthUnits="micrometer"):
        """
        returns cellDict = { cellname: (segDict, cableDict) } # note: single cell only
        where segDict = { segid1 : [ segname,(proximalx,proximaly,proximalz),
            (distalx,distaly,distalz),diameter,length,[potential_syn1, ... ] ] , ... }
        segname is "<name>_<segid>" because 1) guarantees uniqueness,
            & 2) later scripts obtain segid from the compartment's name!
        and cableDict = { cablegroupname : [campartment1name, compartment2name, ... ], ... }.
        params is dict which can contain, combineSegments and/or createPotentialSynapses,
         both boolean.
        """
        if lengthUnits in ['micrometer','micron']:
            self.length_factor = 1e-6
        else:
            self.length_factor = 1.0
        cellname = cell.attrib["name"]
        moose.Neutral('/library') # creates /library in MOOSE tree; elif present, wraps
        _logger.info("Loading cell %s into /library ." % cellname)

        #~ moosecell = moose.Cell('/library/'+cellname)
        #using moose Neuron class - in previous version 'Cell' class Chaitanya
        moosecell = moose.Neuron('/library/'+cellname)
        self.cellDictBySegmentId[cellname] = [moosecell,{}]
        self.cellDictByCableId[cellname] = [moosecell,{}]
        self.segDict = {}
        if 'combineSegments' in params:
            self.combineSegments = params['combineSegments']
        else:
            self.combineSegments = False
        if 'createPotentialSynapses' in params:
            self.createPotentialSynapses = params['createPotentialSynapses']
        else:
            self.createPotentialSynapses = False
        _logger.info("readMorphML using combineSegments = %s" % self.combineSegments)

        ###############################################
        #### load cablegroups into a dictionary
        self.cablegroupsDict = {}
        self.cablegroupsInhomoparamsDict = {}
        ## Two ways of specifying cablegroups in neuroml 1.x
        ## <cablegroup>s with list of <cable>s
        cablegroups = cell.findall(".//{"+self.mml+"}cablegroup")
        for cablegroup in cablegroups:
            cablegroupname = cablegroup.attrib['name']
            self.cablegroupsDict[cablegroupname] = []
            self.cablegroupsInhomoparamsDict[cablegroupname] = []
            for cable in cablegroup.findall(".//{"+self.mml+"}cable"):
                cableid = cable.attrib['id']
                self.cablegroupsDict[cablegroupname].append(cableid)
            # parse inhomogenous_params
            for inhomogeneous_param in cablegroup.findall(".//{"+self.mml+"}inhomogeneous_param"):
                metric = inhomogeneous_param.find(".//{"+self.mml+"}metric")
                if metric.text == 'Path Length from root':
                    inhomoparamname = inhomogeneous_param.attrib['name']
                    inhomoparamvar = inhomogeneous_param.attrib['variable']
                    self.cablegroupsInhomoparamsDict[cablegroupname].append(\
                                (inhomoparamname,inhomoparamvar))
                else:
                    _logger.warning('Only "Path Length from root" metric is '
                            ' supported currently, ignoring %s ' % metric.text
                            )

        ## <cable>s with list of <meta:group>s
        cables = cell.findall(".//{"+self.mml+"}cable")
        for cable in cables:
            cableid = cable.attrib['id']
            cablegroups = cable.findall(".//{"+self.meta+"}group")
            for cablegroup in cablegroups:
                cablegroupname = cablegroup.text
                if cablegroupname in self.cablegroupsDict:
                    self.cablegroupsDict[cablegroupname].append(cableid)
                else:
                    self.cablegroupsDict[cablegroupname] = [cableid]

        ###################################################
        ## load all mechanisms in this cell into /library for later copying
        ## set which compartments have integrate_and_fire mechanism
        self.intFireCableIds = {}   # dict with keys as Compartments/cableIds which are IntFire
                                    # with mechanismnames as values
        for mechanism in cell.findall(".//{"+self.bio+"}mechanism"):
            mechanismname = mechanism.attrib["name"]
            passive = False
            if "passive_conductance" in mechanism.attrib:
                if mechanism.attrib['passive_conductance'] in ["true",'True','TRUE']:
                    passive = True
            if not passive:
                ## if channel does not exist in library load it from xml file
                if not moose.exists("/library/"+mechanismname):
                    _logger.info("Loading mechanism %s into library." % mechanismname)
                    cmlR = ChannelML(self.nml_params)
                    model_filename = mechanismname+'.xml'
                    model_path = neuroml_utils.find_first_file(model_filename,self.model_dir)
                    if model_path is not None:
                        cmlR.readChannelMLFromFile(model_path)
                    else:
                        raise IOError(
                            'For mechanism {0}: files {1} not found under {2}.'.format(
                                mechanismname, model_filename, self.model_dir)
                        )

                    ## set those compartments to be LIF for which
                    ## any integrate_and_fire parameter is set
                    if not moose.exists( "/library/"+mechanismname):
                        _logger.warn("Mechanism doesn't exist: %s " % mechanismname)
                        moose.le( '/library' )
                    moosemech = moose.element("/library/"+mechanismname)
                    if moose.exists(moosemech.path+"/integrate_and_fire"):
                        mooseIaF = moose.element(moosemech.path+"/integrate_and_fire") # Mstring
                        if mooseIaF.value in ['true','True','TRUE']:
                            mech_params = mechanism.findall(".//{"+self.bio+"}parameter")
                            for parameter in mech_params:
                                parametername = parameter.attrib['name']
                                ## check for the integrate_and_fire parameters
                                if parametername in ['threshold', 't_refrac', 'v_reset','g_refrac']:
                                    for group in parameter.findall(".//{"+self.bio+"}group"):
                                        cablegroupname = group.text
                                        if cablegroupname == 'all':
                                            self.intFireCableIds = {'all':mechanismname}
                                            break
                                        else:
                                            for cableid in self.cablegroupsDict[cablegroupname]:
                                                ## only one intfire mechanism is allowed in a cable
                                                ## the last one parsed will override others
                                                self.intFireCableIds[cableid] = mechanismname
                                if 'all' in self.intFireCableIds:
                                    break

        ############################################################
        #### load morphology and connections between compartments
        ## Many neurons exported from NEURON have multiple segments in a section
        ## If self.combineSegments = True,
        ##  then combine those segments into one Compartment / section
        ##  for combining, assume segments of a compartment/section are in increasing order
        ##  and assume all segments of a compartment/section have the same cableid
        ## findall() returns elements in document order:
        running_cableid = ''
        running_segid = ''
        running_comp = None
        running_diameter = 0.0
        running_dia_nums = 0
        segments = cell.findall(".//{"+self.mml+"}segment")
        segmentstotal = len(segments)
        for segnum,segment in enumerate(segments):
            segmentname = segment.attrib['name']
            ## cable is an optional attribute. WARNING: Here I assume it is always present.
            cableid = segment.attrib['cable']
            segmentid = segment.attrib['id']
            ## if old cableid still running AND self.combineSegments == True,
            ## then don't start a new compartment, skip to next segment
            if cableid == running_cableid and self.combineSegments:
                self.cellDictBySegmentId[cellname][1][segmentid] = running_comp
                proximal = segment.find('./{'+self.mml+'}proximal')
                if proximal is not None:
                    running_diameter += float(proximal.attrib["diameter"]) * self.length_factor
                    running_dia_nums += 1
                distal = segment.find('./{'+self.mml+'}distal')
                if distal is not None:
                    running_diameter += float(distal.attrib["diameter"]) * self.length_factor
                    running_dia_nums += 1
            ## if (self.combineSegments and new cableid starts) or if not self.combineSegments,
            ##  then start a new compartment
            else:
                ## Create a new compartment
                ## the moose "hsolve" method assumes compartments to be
                ## asymmetric compartments and symmetrizes them
                ## but that is not what we want when translating
                ## from Neuron which has only symcompartments -- so be careful!

                ## Check if integrate_and_fire mechanism is present,
                ## if so use LIF instead of Compartment
                moosecompname = segmentname+'_'+segmentid   # just segmentname is NOT unique
                                                            # eg: mitral bbmit exported from NEURON
                moosecomppath = moosecell.path+'/'+moosecompname
                mechanismname = None
                if 'all' in self.intFireCableIds:
                    mechanismname = self.intFireCableIds['all']
                if cableid in self.intFireCableIds:
                    mechanismname = self.intFireCableIds[cableid]
                if mechanismname is not None: # this cableid is an intfire
                    ## create LIF (subclass of Compartment) and set to default values
                    moosecomp = moose.LIF(moosecomppath)
                    moosechannel = moose.Neutral('/library/'+mechanismname)
                    moosechannelval = moose.Mstring(moosechannel.path+'/vReset')
                    moosecomp.vReset = moosechannelval.value
                    moosechannelval = moose.Mstring(moosechannel.path+'/thresh')
                    moosecomp.thresh = moosechannelval.value
                    moosechannelval = moose.Mstring(moosechannel.path+'/refracT')
                    moosecomp.refractoryPeriod = moosechannelval.value
                    ## refracG is currently not supported by moose.LIF
                    ## when you implement it, check if refracG or g_refrac
                    ## is a conductance density or a conductance, I think the former
                    #moosechannelval = moose.Mstring(moosechannel.path+'/refracG')
                else:
                    moosecomp = moose.Compartment(moosecomppath)
                self.cellDictBySegmentId[cellname][1][segmentid] = moosecomp
                ## cables are grouped and mechanism densities are set for cablegroups later.
                ## hence I will need to refer to segment according to which cable it belongs to.
                ## if combineSegments is False, there can be multiple segments per cable,
                ##  so make array of compartments for cellDictByCableId[cellname][1][cableid]
                if cableid in self.cellDictByCableId[cellname][1]:
                    self.cellDictByCableId[cellname][1][cableid].append(moosecomp)
                else:
                    self.cellDictByCableId[cellname][1][cableid] = [moosecomp]
                running_cableid = cableid
                running_segid = segmentid
                running_comp = moosecomp
                running_diameter = 0.0
                running_dia_nums = 0
                if 'parent' in segment.attrib:
                    parentid = segment.attrib['parent'] # I assume the parent is created before the child
                                                        # so that I can immediately connect the child.
                    parent = self.cellDictBySegmentId[cellname][1][parentid]
                    ## It is always assumed that axial of parent is connected to raxial of moosesegment
                    ## THIS IS WHAT GENESIS readcell() DOES!!! UNLIKE NEURON!
                    ## THIS IS IRRESPECTIVE OF WHETHER PROXIMAL x,y,z OF PARENT = PROXIMAL x,y,z OF CHILD.
                    ## THIS IS ALSO IRRESPECTIVE OF fraction_along_parent SPECIFIED IN CABLE!
                    ## THUS THERE WILL BE NUMERICAL DIFFERENCES BETWEEN MOOSE/GENESIS and NEURON.
                    ## moosesegment sends Ra and Vm to parent, parent sends only Vm
                    ## actually for symmetric compartment, both parent and moosesegment require each other's Ra/2,
                    ## but axial and raxial just serve to distinguish ends.
                    moose.connect(parent,'axial',moosecomp,'raxial')
                else:
                    parent = None
                proximal = segment.find('./{'+self.mml+'}proximal')
                if proximal is None:         # If proximal tag is not present,
                                              # then parent attribute MUST be present in the segment tag!
                    ## if proximal is not present, then
                    ## by default the distal end of the parent is the proximal end of the child
                    moosecomp.x0 = parent.x
                    moosecomp.y0 = parent.y
                    moosecomp.z0 = parent.z
                else:
                    moosecomp.x0 = float(proximal.attrib["x"])*self.length_factor
                    moosecomp.y0 = float(proximal.attrib["y"])*self.length_factor
                    moosecomp.z0 = float(proximal.attrib["z"])*self.length_factor
                    running_diameter += float(proximal.attrib["diameter"]) * self.length_factor
                    running_dia_nums += 1
                distal = segment.find('./{'+self.mml+'}distal')
                if distal is not None:
                    running_diameter += float(distal.attrib["diameter"]) * self.length_factor
                    running_dia_nums += 1
                ## finished creating new compartment

            ## Update the end position, diameter and length, and segDict of this comp/cable/section
            ## with each segment that is part of this cable (assumes contiguous segments in xml).
            ## This ensures that we don't have to do any 'closing ceremonies',
            ## if a new cable is encoutered in next iteration.
            if distal is not None:
                running_comp.x = float(distal.attrib["x"])*self.length_factor
                running_comp.y = float(distal.attrib["y"])*self.length_factor
                running_comp.z = float(distal.attrib["z"])*self.length_factor
            ## Set the compartment diameter as the average diameter of all the segments in this section
            running_comp.diameter = running_diameter / float(running_dia_nums)
            ## Set the compartment length
            running_comp.length = math.sqrt((running_comp.x-running_comp.x0)**2+\
                (running_comp.y-running_comp.y0)**2+(running_comp.z-running_comp.z0)**2)
            ## NeuroML specs say that if (x0,y0,z0)=(x,y,z), then round compartment e.g. soma.
            ## In Moose set length = dia to give same surface area as sphere of dia.
            if running_comp.length == 0.0:
                running_comp.length = running_comp.diameter
            ## Set the segDict
            ## the empty list at the end below will get populated
            ## with the potential synapses on this segment, in function set_compartment_param(..)
            self.segDict[running_segid] = [running_comp.name,\
                (running_comp.x0,running_comp.y0,running_comp.z0),\
                (running_comp.x,running_comp.y,running_comp.z),\
                running_comp.diameter,running_comp.length,[]]
            if neuroml_utils.neuroml_debug:
                _logger.info('Set up compartment/section %s' % running_comp.name)

        ###############################################
        #### load biophysics into the compartments
        biophysics = cell.find(".//{"+self.neuroml+"}biophysics")
        if biophysics is not None:
            ## see pg 219 (sec 13.2) of Book of Genesis for Physiological Units
            if biophysics.attrib["units"] == 'Physiological Units':
                CMfactor = 1e-2 # F/m^2 from microF/cm^2
                Cfactor = 1e-6 # F from microF
                RAfactor = 1e1 # Ohm*m from KOhm*cm
                RMfactor = 1e-1 # Ohm*m^2 from KOhm*cm^2
                Rfactor = 1e-3 # Ohm from KOhm
                Efactor = 1e-3 # V from mV
                Gfactor = 1e1 # S/m^2 from mS/cm^2
                Ifactor = 1e-6 # A from microA
                Tfactor = 1e-3 # s from ms
            else:
                CMfactor = 1.0
                Cfactor = 1.0
                RAfactor = 1.0
                RMfactor = 1.0
                Rfactor = 1.0
                Efactor = 1.0
                Gfactor = 1.0
                Ifactor = 1.0
                Tfactor = 1.0

            spec_capacitance = cell.find(".//{"+self.bio+"}spec_capacitance")
            for parameter in spec_capacitance.findall(".//{"+self.bio+"}parameter"):
                self.set_group_compartment_param(cell, cellname, parameter,\
                 'CM', float(parameter.attrib["value"])*CMfactor, self.bio)
            spec_axial_resitance = cell.find(".//{"+self.bio+"}spec_axial_resistance")
            for parameter in spec_axial_resitance.findall(".//{"+self.bio+"}parameter"):
                self.set_group_compartment_param(cell, cellname, parameter,\
                 'RA', float(parameter.attrib["value"])*RAfactor, self.bio)
            init_memb_potential = cell.find(".//{"+self.bio+"}init_memb_potential")
            for parameter in init_memb_potential.findall(".//{"+self.bio+"}parameter"):
                self.set_group_compartment_param(cell, cellname, parameter,\
                 'initVm', float(parameter.attrib["value"])*Efactor, self.bio)
            chan_distrib = [] # the list for moose to parse inhomogeneous params (filled below)
            for mechanism in cell.findall(".//{"+self.bio+"}mechanism"):
                mechanismname = mechanism.attrib["name"]
                passive = False
                if "passive_conductance" in mechanism.attrib:
                    if mechanism.attrib['passive_conductance'] in ["true",'True','TRUE']:
                        passive = True
                _logger.info("Loading mechanism %s " % mechanismname)
                ## ONLY creates channel if at least one parameter (like gmax) is specified in the xml
                ## Neuroml does not allow you to specify all default values.
                ## However, granule cell example in neuroconstruct has Ca ion pool without
                ## a parameter, applying default values to all compartments!
                mech_params = mechanism.findall(".//{"+self.bio+"}parameter")
                ## if no params, apply all default values to all compartments
                if len(mech_params) == 0:
                    for compartment_list in self.cellDictByCableId[cellname][1].values():
                        for compartment in compartment_list:
                            self.set_compartment_param(compartment,None,'default',mechanismname)
                ## if params are present, apply params to specified cable/compartment groups
                for parameter in mech_params:
                    parametername = parameter.attrib['name']
                    if passive:
                        if parametername in ['gmax']:
                            self.set_group_compartment_param(cell, cellname, parameter,\
                             'RM', RMfactor*1.0/float(parameter.attrib["value"]), self.bio)
                        elif parametername in ['e','erev']:
                            self.set_group_compartment_param(cell, cellname, parameter,\
                             'Em', Efactor*float(parameter.attrib["value"]), self.bio)
                        elif parametername in ['inject']:
                            self.set_group_compartment_param(cell, cellname, parameter,\
                             'inject', Ifactor*float(parameter.attrib["value"]), self.bio)
                        else:
                            _logger.warning(["Yo programmer of MorphML! You didn't"
                                , " implement parameter %s " % parametername
                                , " in mechanism %s " % mechanismname
                                ]
                                )
                    else:
                        if parametername in ['gmax']:
                            gmaxval = float(eval(parameter.attrib["value"],{"__builtins__":None},{}))
                            self.set_group_compartment_param(cell, cellname, parameter,\
                             'Gbar', Gfactor*gmaxval, self.bio, mechanismname)
                        elif parametername in ['e','erev']:
                            self.set_group_compartment_param(cell, cellname, parameter,\
                             'Ek', Efactor*float(parameter.attrib["value"]), self.bio, mechanismname)
                        elif parametername in ['depth']: # has to be type Ion Concentration!
                            self.set_group_compartment_param(cell, cellname, parameter,\
                             'thick', self.length_factor*float(parameter.attrib["value"]),\
                             self.bio, mechanismname)
                        elif parametername in ['v_reset']:
                            self.set_group_compartment_param(cell, cellname, parameter,\
                             'v_reset', Efactor*float(parameter.attrib["value"]),\
                             self.bio, mechanismname)
                        elif parametername in ['threshold']:
                            self.set_group_compartment_param(cell, cellname, parameter,\
                             'threshold', Efactor*float(parameter.attrib["value"]),\
                             self.bio, mechanismname)
                        elif parametername in ['t_refrac']:
                            self.set_group_compartment_param(cell, cellname, parameter,\
                             't_refrac', Tfactor*float(parameter.attrib["value"]),\
                             self.bio, mechanismname)
                        else:
                            _logger.warning(["Yo programmer of MorphML import! You didn't"
                                    , " implement parameter %s " % parametername
                                    , " in mechanism %s " % mechanismname ]
                                    )

                ## variable parameters:
                ##  varying with:
                ##  p, g, L, len, dia
                ##	p: path distance from soma, measured along dendrite, in metres.
                ##	g: geometrical distance from soma, in metres.
                ##	L: electrotonic distance (# of lambdas) from soma, along dend. No units.
                ##	len: length of compartment, in metres.
                ##	dia: for diameter of compartment, in metres.
                var_params = mechanism.findall(".//{"+self.bio+"}variable_parameter")
                if len(var_params) > 0:
                    ## if variable params are present
                    ##  and use MOOSE to apply the variable formula
                    for parameter in var_params:
                        parametername = parameter.attrib['name']
                        cablegroupstr4moose = ""
                        ## the neuroml spec says there should be a single group in a variable_parameter
                        ##  of course user can always have multiple variable_parameter tags,
                        ##  if user wants multiple groups conforming to neuroml specs.
                        group = parameter.find(".//{"+self.bio+"}group")
                        cablegroupname = group.text
                        if cablegroupname == 'all':
                            cablegroupstr4moose = "#"
                        else:
                            for cableid in self.cablegroupsDict[cablegroupname]:
                                for compartment in self.cellDictByCableId[cellname][1][cableid]:
                                    cablegroupstr4moose += "#"+compartment.name+"#,"
                            if cablegroupstr4moose[-1] == ',':
                                cablegroupstr4moose = cablegroupstr4moose[:-1] # remove last comma
                        inhomo_value = parameter.find(".//{"+self.bio+"}inhomogeneous_value")
                        inhomo_value_name = inhomo_value.attrib['param_name']
                        inhomo_value_value = inhomo_value.attrib['value']
                        if parametername == 'gmax':
                            inhomo_eqn = '('+inhomo_value_value+')*'+str(Gfactor)
                                        # careful about physiol vs SI units
                        else:
                            inhomo_eqn = inhomo_value_value
                            _logger.warning('Physiol. vs SI units translation not'
                            ' implemented for parameter '+parametername+
                            'in channel '+mechanismname)+'. Use SI units'
                            'or ask for implementation.'
                        chan_distrib.extend((mechanismname,cablegroupstr4moose,parametername,inhomo_eqn,""))
                                    # use extend, not append, moose wants it this way
            ## get mooose to parse the variable parameter gmax channel distributions
            #pu.info("Some channel parameters distributed as per "+str(chan_distrib))
            moosecell.channelDistribution = chan_distrib
            #### Connect the Ca pools and channels
            #### Am connecting these at the very end so that all channels and pools have been created
            #### Note: this function is in moose.utils not moose.neuroml.utils !
            for compartment_list in self.cellDictByCableId[cellname][1].values():
                moose_utils.connect_CaConc(compartment_list,\
                    self.temperature+neuroml_utils.ZeroCKelvin) # temperature should be in Kelvin for Nernst

        ##########################################################
        #### load connectivity / synapses into the compartments
        connectivity = cell.find(".//{"+self.neuroml+"}connectivity")
        if connectivity is not None:
            for potential_syn_loc in cell.findall(".//{"+self.nml+"}potential_syn_loc"):
                if 'synapse_direction' in potential_syn_loc.attrib:
                    if potential_syn_loc.attrib['synapse_direction'] in ['post','preAndOrPost']:
                        self.set_group_compartment_param(cell, cellname, potential_syn_loc,\
                            'synapse_type', potential_syn_loc.attrib['synapse_type'],\
                            self.nml, mechanismname='synapse')
                    if potential_syn_loc.attrib['synapse_direction'] in ['pre','preAndOrPost']:
                        self.set_group_compartment_param(cell, cellname, potential_syn_loc,\
                            'spikegen_type', potential_syn_loc.attrib['synapse_type'],\
                            self.nml, mechanismname='spikegen')

        ##########################################################
        #### annotate each compartment with the cablegroups it belongs to
        self.cableDict = {}
        for cablegroupname in self.cablegroupsDict:
            comp_list = []
            for cableid in self.cablegroupsDict[cablegroupname]:
                for compartment in self.cellDictByCableId[cellname][1][cableid]:
                    cableStringPath = compartment.path+'/cable_groups'
                    cableString = moose.Mstring(cableStringPath)
                    if cableString.value == '':
                        cableString.value += cablegroupname
                    else:
                        cableString.value += ',' + cablegroupname
                    comp_list.append(compartment.name)
            self.cableDict[cablegroupname] = comp_list

        _logger.info("Finished loading into library, cell: %s " % cellname)
        return {cellname:(self.segDict,self.cableDict)}

    def set_group_compartment_param(self, cell, cellname, parameter,\
                name, value, grouptype, mechanismname=None):
        """
        Find the compartments that belong to the cablegroups refered to
         for this parameter and set_compartment_param.
        """
        for group in parameter.findall(".//{"+grouptype+"}group"):
            cablegroupname = group.text
            if cablegroupname == 'all':
                for compartment_list in self.cellDictByCableId[cellname][1].values():
                    for compartment in compartment_list:
                        self.set_compartment_param(compartment,name,value,mechanismname)
            else:
                for cableid in self.cablegroupsDict[cablegroupname]:
                    for compartment in self.cellDictByCableId[cellname][1][cableid]:
                        self.set_compartment_param(compartment,name,value,mechanismname)

    def set_compartment_param(self, compartment, name, value, mechanismname):
        """ Set the param for the compartment depending on name and mechanismname. """
        if name == 'CM':
            compartment.Cm = value*math.pi*compartment.diameter*compartment.length
        elif name == 'RM':
            compartment.Rm = value/(math.pi*compartment.diameter*compartment.length)
        elif name == 'RA':
            compartment.Ra = value*compartment.length/(math.pi*(compartment.diameter/2.0)**2)
        elif name == 'Em':
            compartment.Em = value
        elif name == 'initVm':
            compartment.initVm = value
        elif name == 'inject':
            # this reader converts to SI
            _logger.info("Comparment %s inject %s A." % (compartment.name, value))
            compartment.inject = value
        elif name == 'v_reset':
            compartment.vReset = value # compartment is a moose.LIF instance (intfire)
        elif name == 'threshold':
            compartment.thresh = value # compartment is a moose.LIF instance (intfire)
        elif name == 't_refrac':
            compartment.refractoryPeriod = value # compartment is a moose.LIF instance (intfire)
        elif name == 'g_refrac':
            _logger.info("SORRY, current moose.LIF doesn't support g_refrac.")
        elif mechanismname is 'synapse': # synapse being added to the compartment
            ## these are potential locations, we do not actually make synapses,
            ## unless the user has explicitly asked for it
            if self.createPotentialSynapses:
                syn_name = value
                if not moose.exists(compartment.path+'/'+syn_name):
                    make_new_synapse(syn_name, compartment, syn_name, self.nml_params)
            ## I assume below that compartment name has _segid at its end
            segid = compartment.name.split('_')[-1] # get segment id from compartment name
            self.segDict[segid][5].append(value)
        elif mechanismname is 'spikegen': # spikegen being added to the compartment
            ## these are potential locations, we do not actually make the spikegens.
            ## spikegens for different synapses can have different thresholds,
            ## hence include synapse_type in its name
            ## value contains name of synapse i.e. synapse_type
            #spikegen = moose.SpikeGen(compartment.path+'/'+value+'_spikegen')
            #moose.connect(compartment,"VmSrc",spikegen,"Vm")
            pass
        ## previous were mechanism that don't need a ChannelML definition
        ## including integrate_and_fire (I ignore the ChannelML definition)
        ## thus integrate_and_fire mechanism default values cannot be used
        ## i.e. nothing needed in /library, but below mechanisms need.
        elif mechanismname is not None:
            ## if mechanism is not present in compartment, deep copy from library
            ## all mechanisms have been loaded into the library earlier
            if not moose.exists(compartment.path+'/'+mechanismname):
                neutralObj = moose.element("/library/"+mechanismname) # gives error if not present
                if 'CaConc' == neutralObj.className: # Ion concentration pool
                    libcaconc = moose.CaConc("/library/"+mechanismname)
                    ## deep copies the library caconc under the compartment
                    caconc = moose.copy(libcaconc,compartment,mechanismname)
                    caconc = moose.CaConc(caconc)
                    ## CaConc connections are made later using connect_CaConc()
                    ## Later, when calling connect_CaConc,
                    ## B is set for caconc based on thickness of Ca shell and compartment l and dia
                    ## OR based on the Mstring phi under CaConc path.
                    channel = None
                elif 'HHChannel2D' == neutralObj.className : ## HHChannel2D
                    libchannel = moose.HHChannel2D("/library/"+mechanismname)
                    ## deep copies the library channel under the compartment
                    channel = moose.copy(libchannel,compartment,mechanismname)
                    channel = moose.HHChannel2D(channel)
                    moose.connect(channel,'channel',compartment,'channel')
                elif 'HHChannel' == neutralObj.className : ## HHChannel
                    libchannel = moose.HHChannel("/library/"+mechanismname)
                    ## deep copies the library channel under the compartment
                    channel = moose.copy(libchannel,compartment,mechanismname)
                    channel = moose.HHChannel(channel)
                    moose.connect(channel,'channel',compartment,'channel')
            ## if mechanism is present in compartment, just wrap it
            else:
                neutralObj = moose.element(compartment.path+'/'+mechanismname)
                if 'CaConc' == neutralObj.className: # Ion concentration pool
                    caconc = moose.CaConc(compartment.path+'/'+mechanismname) # wraps existing channel
                    channel = None
                elif 'HHChannel2D' == neutralObj.className : ## HHChannel2D
                    channel = moose.HHChannel2D(compartment.path+'/'+mechanismname) # wraps existing channel
                elif 'HHChannel' == neutralObj.className : ## HHChannel
                    channel = moose.HHChannel(compartment.path+'/'+mechanismname) # wraps existing channel
            if name == 'Gbar':
                if channel is None: # if CaConc, neuroConstruct uses gbar for thickness or phi
                    ## If child Mstring 'phi' is present, set gbar as phi
                    ## BUT, value has been multiplied by Gfactor as a Gbar,
                    ## SI or physiological not known here,
                    ## ignoring Gbar for CaConc, instead of passing units here
                    child = moose_utils.get_child_Mstring(caconc,'phi')
                    if child is not None:
                        #child.value = value
                        pass
                    else:
                        #caconc.thick = value
                        pass
                else: # if ion channel, usual Gbar
                    channel.Gbar = value*math.pi*compartment.diameter*compartment.length
            elif name == 'Ek':
                channel.Ek = value
            elif name == 'thick': # thick seems to be NEURON's extension to NeuroML level 2.
                caconc.thick = value ## JUST THIS WILL NOT DO - HAVE TO SET B based on this thick!
                ## Later, when calling connect_CaConc,
                ## B is set for caconc based on thickness of Ca shell and compartment l and dia.
                ## OR based on the Mstring phi under CaConc path.
        if neuroml_utils.neuroml_debug:
            _logger.info("Setting %s  for comparment %s to %s" % (name, compartment.path, value))
