# -*- coding: utf-8 -*-
## Description: class NetworkML for loading NetworkML from file or xml element into MOOSE
## Version 1.0 by Aditya Gilra, NCBS, Bangalore, India, 2011 for serial MOOSE
## Version 1.5 by Niraj Dudani, NCBS, Bangalore, India, 2012, ported to parallel MOOSE
## Version 1.6 by Aditya Gilra, NCBS, Bangalore, India, 2012, further changes for parallel MOOSE
## Version 1.7 by Aditya Gilra, NCBS, Bangalore, India, 2013, further support for NeuroML 1.8.1
## Version 1.8 by Aditya Gilra, NCBS, Bangalore, India, 2013, changes for new IntFire and SynHandler classes

"""
NeuroML.py is the preferred interface. Use this only if NeuroML L1,L2,L3 files are misnamed/scattered.
Instantiate NetworkML class, and thence use method:
readNetworkMLFromFile(...) to load a standalone NetworkML file, OR
readNetworkML(...) to load from an xml.etree xml element (could be part of a larger NeuroML file).
"""

from __future__ import print_function
from xml.etree import cElementTree as ET
import string
import os
from math import cos, sin
from moose.neuroml.MorphML import MorphML
from moose.neuroml.ChannelML import ChannelML, make_new_synapse
import moose
from moose.neuroml.utils import meta_ns, nml_ns, find_first_file, tweak_model
from moose import utils

import logging
# define a Handler which writes INFO messages or higher to the sys.stderr
console = logging.StreamHandler()
console.setLevel(logging.INFO)
# set a format which is simpler for console use
formatter = logging.Formatter('%(name)-12s: %(levelname)-8s %(message)s')
# tell the handler to use this format
console.setFormatter(formatter)
# add the handler to the root logger
logging.getLogger('moose.nml.networkml').addHandler(console)
_logger = logging.getLogger('moose.nml.networkml')

class NetworkML():

    def __init__(self, nml_params):
        self.nml_params = nml_params
        self.model_dir = nml_params['model_dir']

    def readNetworkMLFromFile(self,filename,cellSegmentDict,params={}):
        """
        specify tweak params = {'excludePopulations':[popname1,...], 'excludeProjections':[projname1,...], \
            'onlyInclude':{'includePopulation':(popname,[id1,...]),'includeProjections':(projname1,...)} }
        If excludePopulations is present, then excludeProjections must also be present:
        Thus if you exclude some populations,
            ensure that you exclude projections that refer to those populations also!
        Though for onlyInclude, you may specify only included cells and this reader will
            also keep cells connected to those in onlyInclude.
        This reader first prunes the exclude-s,
            then keeps the onlyInclude-s and those that are connected.
        Use 'includeProjections' if you want to keep some projections not connected to
            the primary 'includePopulation' cells
        but connected to secondary cells that connected to the primary ones:
        e.g. baseline synapses on granule cells connected to 'includePopulation' mitrals;
            these synapses receive file based pre-synaptic events,
            not presynaptically connected to a cell.
        In params, you further specify:
            'createPotentialSynapses' : True (False by default)
            to create synapses at all potential locations/compartments specified in the MorphML cell file
            even before Projections tag is parsed.
            'combineSegments' : True (False by default)
            to ask neuroml to combine segments belonging to a cable
            (Neuron generates multiple segments per section).
        """
        _logger.info("Reading file %s " % filename)
        tree = ET.parse(filename)
        root_element = tree.getroot()
        _logger.info("Tweaking model ... ")
        tweak_model(root_element, params)
        _logger.info("Loading model into MOOSE ... ")
        return self.readNetworkML(root_element,cellSegmentDict,params,root_element.attrib['lengthUnits'])

    def readNetworkML(self,network,cellSegmentDict,params={},lengthUnits="micrometer"):
        """
        This returns populationDict = { 'populationname1':(cellname,{int(instanceid1):moosecell, ... }) , ... }
        and projectionDict = { 'projectionname1':(source,target,[(syn_name1,pre_seg_path,post_seg_path),...]) , ... }
        """
        if lengthUnits in ['micrometer','micron']:
            self.length_factor = 1e-6
        else:
            self.length_factor = 1.0
        self.network = network
        self.cellSegmentDict = cellSegmentDict
        self.params = params

        self.populationDict = {}
        [ self.createPopulation(pop) for pop in
                self.network.findall(".//{"+nml_ns+"}population")
                ]

        self.projectionDict={}
        projections = self.network.find(".//{"+nml_ns+"}projections")
        if projections:
            # see pg 219 (sec 13.2) of Book of Genesis
            if projections.attrib["units"] == 'Physiological Units':
                Efactor = 1e-3 # V from mV
                Tfactor = 1e-3 # s from ms
            else:
                Efactor = 1.0
                Tfactor = 1.0
            [ self.createProjection(proj, Efactor, Tfactor) for proj in projections ]

        allinputs = self.network.findall(".//{"+nml_ns+"}inputs")
        for inputs in allinputs:
            _logger.info("Creating input under /elec ")
            units = inputs.attrib['units']
            # see pg 219 (sec 13.2) of Book of Genesis
            if units == 'Physiological Units':
                Vfactor, Tfactor, Ifactor = 1e-3, 1e-3, 1e-6
            else:
                Vfactor, Tfactor, Ifactor = 1.0, 1.0, 1.0
            [ self.createInput(inputelem, Vfactor, Tfactor, Ifactor) for
                    inputelem in self.network.findall(".//{"+nml_ns+"}input")
                    ]

        return (self.populationDict,self.projectionDict)

    def createInput(self, inputelem, Vfactor, Tfactor, Ifactor):
        """Create input """
        inputname = inputelem.attrib['name']
        pulseinput = inputelem.find(".//{"+nml_ns+"}pulse_input")
        if pulseinput is not None:
            ## If /elec doesn't exists it creates /elec
            ## and returns a reference to it. If it does,
            ## it just returns its reference.
            moose.Neutral('/elec')
            pulsegen = moose.PulseGen('/elec/pulsegen_'+inputname)
            iclamp = moose.DiffAmp('/elec/iclamp_'+inputname)
            iclamp.saturation = 1e6
            iclamp.gain = 1.0
            pulsegen.trigMode = 0 # free run
            pulsegen.baseLevel = 0.0
            _logger.debug("Tfactor, Ifactor: %s, %s" % (Tfactor, Ifactor))
            _logger.debug("Pulsegen attributes: %s" % str(pulseinput.attrib))
            pulsegen.firstDelay = float(pulseinput.attrib['delay'])*Tfactor
            pulsegen.firstWidth = float(pulseinput.attrib['duration'])*Tfactor
            pulsegen.firstLevel = float(pulseinput.attrib['amplitude'])*Ifactor
            pulsegen.secondDelay = 1e6 # to avoid repeat
            pulsegen.secondLevel = 0.0
            pulsegen.secondWidth = 0.0
            ## do not set count to 1, let it be at 2 by default
            ## else it will set secondDelay to 0.0 and repeat the first pulse!
            #pulsegen.count = 1
            moose.connect(pulsegen,'output',iclamp,'plusIn')
            target = inputelem.find(".//{"+nml_ns+"}target")
            population = target.attrib['population']
            for site in target.findall(".//{"+nml_ns+"}site"):
                cell_id = site.attrib['cell_id']
                if 'segment_id' in site.attrib: segment_id = site.attrib['segment_id']
                else: segment_id = 0 # default segment_id is specified to be 0
                ## population is populationname, self.populationDict[population][0] is cellname
                cell_name = self.populationDict[population][0]
                segment_path = self.populationDict[population][1][int(cell_id)].path+'/'+\
                    self.cellSegmentDict[cell_name][0][segment_id][0]
                compartment = moose.element(segment_path)
                _logger.debug("Adding pulse at {0}: {1}".format(
                    segment_path, pulsegen.firstLevel )
                    )

                _logger.debug("Connecting {0}:output to {1}:injectMst".format(
                    iclamp, compartment)
                    )

                moose.connect(iclamp, 'output', compartment, 'injectMsg')

    def createPopulation(self, population):
        """Create a population with given cell type """
        cellname = population.attrib["cell_type"]
        populationname = population.attrib["name"]

        if not moose.exists('/library/'+cellname):
            ## if cell does not exist in library load it from xml file
            mmlR = MorphML(self.nml_params)
            model_filenames = (cellname+'.xml', cellname+'.morph.xml')
            success = False
            for model_filename in model_filenames:
                model_path = find_first_file(model_filename,self.model_dir)
                if model_path is not None:
                    cellDict = mmlR.readMorphMLFromFile(model_path, self.params)
                    success = True
                    break
            if not success:
                raise IOError(
                    'For cell {0}: files {1} not found under {2}.'.format(
                        cellname, model_filenames, self.model_dir
                    )
                )
            self.cellSegmentDict.update(cellDict)

        libcell = moose.Neuron('/library/'+cellname) #added cells as a Neuron class.
        self.populationDict[populationname] = (cellname,{})
        moose.Neutral('/cells')
        _logger.info(
                "Creating population {0} of cell type {1}".format(
                    populationname, cellname
                    )
                )

        for instance in population.findall(".//{"+nml_ns+"}instance"):
            instanceid = instance.attrib['id']
            location = instance.find('./{'+nml_ns+'}location')
            rotationnote = instance.find('./{'+meta_ns+'}notes')
            if rotationnote is not None:
                ## the text in rotationnote is zrotation=xxxxxxx
                zrotation = float(rotationnote.text.split('=')[1])
            else:
                zrotation = 0
            ## deep copies the library cell to an instance under '/cells' named as <arg3>
            ## /cells is useful for scheduling clocks as all sim elements are in /cells
            cellid = moose.copy(libcell,moose.Neutral('/cells'),populationname+"_"+instanceid)
            cell = moose.Neuron(cellid)
            self.populationDict[populationname][1][int(instanceid)]=cell
            x = float(location.attrib['x'])*self.length_factor
            y = float(location.attrib['y'])*self.length_factor
            z = float(location.attrib['z'])*self.length_factor
            self.translate_rotate(cell,x,y,z,zrotation)

    def translate_rotate(self,obj,x,y,z,ztheta): # recursively translate all compartments under obj
        for childId in obj.children:
            try:
                childobj = moose.element(childId)
                if childobj.className in ['Compartment','SymCompartment']:
                    ## SymCompartment inherits from Compartment,
                    ## so below wrapping by Compartment() is fine for both Compartment and SymCompartment
                    child = moose.element(childId)
                    x0 = child.x0
                    y0 = child.y0
                    x0new = x0*cos(ztheta)-y0*sin(ztheta)
                    y0new = x0*sin(ztheta)+y0*cos(ztheta)
                    child.x0 = x0new + x
                    child.y0 = y0new + y
                    child.z0 += z
                    x1 = child.x
                    y1 = child.y
                    x1new = x1*cos(ztheta)-y1*sin(ztheta)
                    y1new = x1*sin(ztheta)+y1*cos(ztheta)
                    child.x = x1new + x
                    child.y = y1new + y
                    child.z += z
                if len(childobj.children)>0:
                    self.translate_rotate(childobj,x,y,z,ztheta) # recursive translation+rotation
            except TypeError:  # in async13, gates which have not been created still 'exist'
                                # i.e. show up as a child, but cannot be wrapped.
                pass
            ## if childobj is a compartment or symcompartment translate, else skip it

    def createProjection(self, projection, Efactor, Tfactor):
        projectionname = projection.attrib["name"]
        _logger.info("Setting %s" % projectionname)
        source = projection.attrib["source"]
        target = projection.attrib["target"]
        self.projectionDict[projectionname] = (source,target,[])
        for syn_props in projection.findall(".//{"+nml_ns+"}synapse_props"):
            syn_name = syn_props.attrib['synapse_type']
            ## if synapse does not exist in library load it from xml file
            if not moose.exists("/library/"+syn_name):
                cmlR = ChannelML(self.nml_params)
                model_filename = syn_name+'.xml'
                model_path = find_first_file(model_filename,self.model_dir)
                if model_path is not None:
                    cmlR.readChannelMLFromFile(model_path)
                else:
                    raise IOError(
                        'For mechanism {0}: files {1} not found under {2}.'.format(
                            mechanismname, model_filename, self.model_dir
                        )
                    )
            weight = float(syn_props.attrib['weight'])
            threshold = float(syn_props.attrib['threshold'])*Efactor
            if 'prop_delay' in syn_props.attrib:
                prop_delay = float(syn_props.attrib['prop_delay'])*Tfactor
            elif 'internal_delay' in syn_props.attrib:
                prop_delay = float(syn_props.attrib['internal_delay'])*Tfactor
            else: prop_delay = 0.0
            for connection in projection.findall(".//{"+nml_ns+"}connection"):
                pre_cell_id = connection.attrib['pre_cell_id']
                post_cell_id = connection.attrib['post_cell_id']
                if 'file' not in pre_cell_id:
                    # source could be 'mitrals', self.populationDict[source][0] would be 'mitral'
                    pre_cell_name = self.populationDict[source][0]
                    if 'pre_segment_id' in connection.attrib:
                        pre_segment_id = connection.attrib['pre_segment_id']
                    else: pre_segment_id = "0" # assume default segment 0, usually soma
                    pre_segment_path = self.populationDict[source][1][int(pre_cell_id)].path+'/'+\
                        self.cellSegmentDict[pre_cell_name][0][pre_segment_id][0]
                else:
                    # I've removed extra excitation provided via files, so below comment doesn't apply.
                    # 'file[+<glomnum>]_<filenumber>' # glomnum is
                    # for mitral_granule extra excitation from unmodelled sisters.
                    pre_segment_path = pre_cell_id+'_'+connection.attrib['pre_segment_id']
                # target could be 'PGs', self.populationDict[target][0] would be 'PG'
                post_cell_name = self.populationDict[target][0]
                if 'post_segment_id' in connection.attrib:
                    post_segment_id = connection.attrib['post_segment_id']
                else: post_segment_id = "0" # assume default segment 0, usually soma
                post_segment_path = self.populationDict[target][1][int(post_cell_id)].path+'/'+\
                    self.cellSegmentDict[post_cell_name][0][post_segment_id][0]
                self.projectionDict[projectionname][2].append((syn_name, pre_segment_path, post_segment_path))
                properties = connection.findall('./{'+nml_ns+'}properties')
                if len(properties)==0:
                    self.connect(syn_name, pre_segment_path, post_segment_path, weight, threshold, prop_delay)
                else:
                    for props in properties:
                        synapse_type = props.attrib['synapse_type']
                        if syn_name in synapse_type:
                            weight_override = float(props.attrib['weight'])
                            if 'internal_delay' in props.attrib:
                                delay_override = float(props.attrib['internal_delay'])
                            else: delay_override = prop_delay
                            if weight_override != 0.0:
                                self.connect(syn_name, pre_segment_path, post_segment_path,\
                                    weight_override, threshold, delay_override)

    def connect(self, syn_name, pre_path, post_path, weight, threshold, delay):
        postcomp = moose.element(post_path)
        ## We usually try to reuse an existing SynChan & SynHandler -
        ## event based SynHandlers have an array of weights and delays and can represent multiple synapses,
        ## so a new element of the weights and delays array is created
        ## every time a 'synapse' message connects to the SynHandler (from 'event' of spikegen)
        ## BUT for a graded synapse with a lookup table output connected to 'activation' message,
        ## not to 'synapse' message, we make a new synapse everytime
        ## ALSO for a saturating synapse i.e. KinSynChan, we always make a new synapse
        ## as KinSynChan is not meant to represent multiple synapses
        libsyn = moose.SynChan('/library/'+syn_name)
        gradedchild = utils.get_child_Mstring(libsyn,'graded')
        if libsyn.className == 'KinSynChan' or gradedchild.value == 'True': # create a new synapse
            syn_name_full = syn_name+'_'+utils.underscorize(pre_path)
            make_new_synapse(syn_name, postcomp, syn_name_full, self.nml_params)
        else:
            ## if syn doesn't exist in this compartment, create it
            syn_name_full = syn_name
            if not moose.exists(post_path+'/'+syn_name_full):
                make_new_synapse(syn_name, postcomp, syn_name_full, self.nml_params)
        ## moose.element is a function that checks if path exists,
        ## and returns the correct object, here SynChan
        syn = moose.element(post_path+'/'+syn_name_full) # wrap the SynChan in this compartment
        synhandler = moose.element(post_path+'/'+syn_name_full+'/handler') # wrap the SynHandler
        gradedchild = utils.get_child_Mstring(syn,'graded')
        #### weights are set at the end according to whether the synapse is graded or event-based

        #### If graded, connect pre-comp Vm to the table which is connected to SynChan's activation
        #### If event-based, connect spikegen/timetable's spikeOut to Simple/STDP SynHandler's addSpike
        ## I rely on second term below not being evaluated if first term is None;
        ## otherwise None.value gives error.
        if gradedchild is not None and gradedchild.value=='True': # graded synapse
            interpol = moose.element(syn.path+"/graded_table")
            #### always connect source to input - else 'cannot create message' error.
            precomp = moose.element(pre_path)
            moose.connect(precomp,"VmOut",interpol,"input")
            try:
                tau_table = moose.element(syn.path+'/tau_table')
                tau_table_present = True
            except ValueError:
                tau_table_present = False
            # if tau_table is not found, don't connect it
            if tau_table_present:
                moose.connect(precomp,'VmOut',tau_table,'input')
            ## since there is no weight field for a graded synapse
            ## (no 'synapse' message connected),
            ## I set the Gbar to weight*Gbar
            syn.Gbar = weight*syn.Gbar
        else: # Event based synapse
            ## synapse could be connected to either spikegen at pre-compartment OR to a file!
            if 'file' not in pre_path:
                ## element() can return either Compartment() or IzhikevichNrn(),
                ## since it queries and wraps the actual object
                precomp = moose.element(pre_path)
                ## if spikegen for this synapse doesn't exist in this compartment, create it
                ## spikegens for different synapse_types can have different thresholds
                ## but an integrate and fire spikegen supercedes all other spikegens
                if 'IF' in precomp.className: # intfire LIF
                    spikegen = precomp # LIF has a spikeOut message
                else:
                    if not moose.exists(pre_path+'/'+syn_name+'_spikegen'):
                        ## create new spikegen
                        spikegen = moose.element(pre_path+'/'+syn_name+'_spikegen')
                        ## connect the compartment Vm to the spikegen
                        moose.connect(precomp,"VmOut",spikegen,"Vm")
                        ## spikegens for different synapse_types can have different thresholds
                        spikegen.threshold = threshold
                        spikegen.edgeTriggered = 1 # This ensures that spike is generated only on leading edge.
                        ## usually events are raised at every time step that Vm > Threshold,
                        ## can set either edgeTriggered as above or refractT
                        #spikegen.refractT = 0.25e-3
                    ## wrap the existing or newly created spikegen in this compartment
                    spikegen = moose.SpikeGen(pre_path+'/'+syn_name+'_spikegen')
                ## connect the spikegen to the SynHandler
                ## note that you need to use Synapse (auto-created) under SynHandler
                ## to get/set weights , addSpike-s etc.
                ## wrap Synapse element by moose.Synapse(synhandler.path+'/synapse') or synhandler.synapse
                ## Synpase is an array element, first add to it, to addSpike-s, get/set weights, etc.
                synhandler.numSynapses += 1
                ## see Demos/snippets/synapse.py for an example of
                ## how to connect multiple SpikeGens to the same SynChan
                m = moose.connect(spikegen, 'spikeOut',
                                    synhandler.synapse[-1], 'addSpike', 'Single')
            else:
                ## if connected to a file, create a timetable,
                ## put in a field specifying the connected filenumbers to this segment,
                ## and leave it for simulation-time connection
                ## pre_path is 'file[+<glomnum>]_<filenum1>[_<filenum2>...]' i.e. glomnum could be present
                ## hack for my (Aditya's) OB model to use files in NeuroML, should not affect others
                filesplit = pre_path.split('+')
                if len(filesplit) == 2:
                    glomsplit = filesplit[1].split('_',1)
                    glomstr = '_'+glomsplit[0]
                    filenums = glomsplit[1]
                else:
                    glomstr = ''
                    filenums = pre_path.split('_',1)[1]
                tt_path = postcomp.path+'/'+syn_name_full+glomstr+'_tt'
                if not moose.exists(tt_path):
                    ## if timetable for this synapse doesn't exist in this compartment, create it,
                    ## and add the field 'fileNumbers'
                    tt = moose.TimeTable(tt_path)
                    tt_filenums = moose.Mstring(tt_path+'/fileNumbers')
                    tt_filenums.value = filenums
                    ## Be careful to connect the timetable only once while creating it as below:
                    ## note that you need to use Synapse (auto-created) under SynChan
                    ## to get/set weights , addSpike-s etc.
                    ## wrap Synapse element by moose.Synapse(synhandler.path+'/synapse') or synhandler.synapse
                    ## Synpase is an array element, first add to it, to addSpike-s, get/set weights, etc.
                    synhandler.numSynapses += 1
                    m = moose.connect(tt,"eventOut",synhandler.synapse[-1],"addSpike","Single")
                else:
                    ## if it exists, append file number to the field 'fileNumbers'
                    ## append filenumbers from 'file[+<glomnum>]_<filenumber1>[_<filenumber2>...]'
                    tt_filenums = moose.Mstring(tt_path+'/fileNumbers')
                    tt_filenums.value += '_' + filenums
            #### syn.Gbar remains the same, but we play with the weight which is a factor to Gbar
            #### The delay and weight can be set only after connecting a spike event generator.
            #### delay and weight are arrays: multiple event messages can be connected to a single synapse
            ## first argument below is the array index, we connect to the latest synapse created above
            ## But KinSynChan ignores weight of the synapse, so set the Gbar for it
            if libsyn.className == 'KinSynChan':
                syn.Gbar = weight*syn.Gbar
            else:
                ## note that you need to use Synapse (auto-created) under SynHandler
                ## to get/set weights , addSpike-s etc.
                ## wrap Synpase element by moose.Synapse(synhandler.path+'/synapse') or synhandler.synapse
                synhandler.synapse[-1].weight = weight
            synhandler.synapse[-1].delay = delay # seconds
            #print 'len = ',len(synhandler.synapse)
            #for i,syn_syn in enumerate(synhandler.synapse):
            #    print i,'th weight =',syn_syn.weight,'\n'
