# This file is part of MOOSE simulator: http://moose.ncbs.res.in.

# MOOSE is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# MOOSE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.


"""

 Description: class NetworkML for loading NetworkML from file or xml element
 into MOOSE

 Version 1.0 by Aditya Gilra, NCBS, Bangalore, India, 2011 for serial MOOSE

 Version 1.5 by Niraj Dudani, NCBS, Bangalore, India, 2012, ported to parallel
 MOOSE

 Version 1.6 by Aditya Gilra, NCBS, Bangalore, India, 2012, further changes for
 parallel MOOSE

 Version 1.7 by Aditya Gilra, NCBS, Bangalore, India, 2013, further support for
 NeuroML 1.8.1

    NeuroML.py is the preferred interface. Use this only if NeuroML L1,L2,L3
    files are misnamed/scattered.  Instantiate NetworlML class, and thence use
    method: readNetworkMLFromFile(...) to load a standalone NetworkML file, OR
    readNetworkML(...) to load from an xml.etree xml element (could be part of a
    larger NeuroML file).

"""

from xml.etree import cElementTree as ET
import string
import os
import sys
import MorphML
import ChannelML
import moose
import moose.neuroml.utils as nmu
import moose.utils as utils
import inspect
import helper.xml_methods as xml_methods
import helper.moose_methods as moose_methods
import core.stimulus as stimulus
import core.config as config
import debug.bugs as bugs
import matplotlib.pyplot as plt
import numpy as np
import re

from math import cos, sin

class NetworkML(object):

    def __init__(self, nml_params):
        self.populationDict = dict()
        self.libraryPath = config.libraryPath
        moose.Neutral(self.libraryPath)
        self.cellPath = self.libraryPath
        moose.Neutral(self.cellPath)
        moose.Neutral(self.libraryPath)
        self.cellDictBySegmentId={}
        self.cellDictByCableId={}
        self.nml_params = nml_params
        self.modelDir = nml_params['model_dir']
        self.elecPath = config.elecPath
        self.dt = 1e-3 # In seconds.
        self.simTime = 1000e-3
        self.plotPaths = 'figs'

    def connectWrapper(self, src, srcF, dest, destF, messageType='Single'):
        """
        Wrapper around moose.connect 
        """
        utils.dump("INFO"
                , "Connecting ({4})`\n\t{0},{1}`\n\t`{2},{3}".format(
                    src.path
                    , srcF
                    , dest.path
                    , destF
                    , messageType
                    )
                , frame = inspect.currentframe()
                )
        try:
            res = moose.connect(src, srcF, dest, destF, messageType)
            assert res, "Failed to connect"
        except Exception as e:
            utils.dump("ERROR", "Failed to connect.")
            raise e

    def plotVector(self, vector, plotname):
        ''' Saving input vector to a file '''
        name = plotname.replace('/', '_')
        fileName = os.path.join(self.plotPaths, name)+'.eps'
        utils.dump("DEBUG"
                , "Saving vector to a file {}".format(fileName)
                )
        plt.vlines(vector, 0, 1)
        plt.savefig(fileName)


    def readNetworkMLFromFile(self, filename, cellSegmentDict, params={}):
    

        """ readNetworkML

        specify tweak
        params = {
            'excludePopulations':[popname1,...]
            , 'excludeProjections':[projname1,...]
            , 'onlyInclude':{'includePopulation':(popname,[id1,...])
            ,'includeProjections':(projname1,...)
            }
        }

        If excludePopulations is present, then excludeProjections must also be
        present. Thus if you exclude some populations, ensure that you exclude
        projections that refer to those populations also!  Though for
        onlyInclude, you may specify only included cells and this reader will
        also keep cells connected to those in onlyInclude.  This reader first
        prunes the exclude-s, then keeps the onlyInclude-s and those that are
        connected.  Use 'includeProjections' if you want to keep some
        projections not connected to the primary 'includePopulation' cells but
        connected to secondary cells that connected to the primary ones: e.g.
        baseline synapses on granule cells connected to 'includePopulation'
        mitrals; these synapses receive file based pre-synaptic events, not
        presynaptically connected to a cell.

        """

        utils.dump("INFO", "reading file %s ... " % filename)
        tree = ET.parse(filename)
        root_element = tree.getroot()
        utils.dump("INFO", "Tweaking model ... ")
        utils.tweak_model(root_element, params)
        utils.dump("INFO", "Loading model into MOOSE ... ")
        return self.readNetworkML(
                root_element
               , cellSegmentDict
               , params
               , root_element.attrib['lengthUnits']
               )

    def readNetworkML(self, network, cellSegmentDict , params={}
        , lengthUnits="micrometer"):

        """
        This returns
         populationDict = {
           'populationName1':(cellname
           , {int(instanceid1):moosecell, ...F}) , ...
           }

         projectionDict = {
            'projectionName1':(source,target
            ,[(syn_name1,pre_seg_path,post_seg_path),...])
            , ...
            }
        """

        if lengthUnits in ['micrometer','micron']:
            self.length_factor = 1e-6
        else:
            self.length_factor = 1.0
        self.network = network
        self.cellSegmentDict = cellSegmentDict
        self.params = params
        utils.dump("STEP", "Creating populations ... ")
        self.createPopulations() 

        utils.dump("STEP", "Creating connections ... ")
        self.createProjections() 

        # create connections
        utils.dump("STEP", "Creating inputs in %s .. " % self.elecPath)

        # create inputs (only current pulse supported)
        self.createInputs() 
        return (self.populationDict, self.projectionDict)

    def createInputs(self):

        """ createInputs

        Create inputs as given in NML file and attach them to moose.
        """

        for inputs in self.network.findall(".//{"+nmu.nml_ns+"}inputs"):
            units = inputs.attrib['units']

            # This dict is to be passed to function which attach an input
            # element to moose.
            factors = {}

            # see pg 219 (sec 13.2) of Book of Genesis
            if units == 'Physiological Units':
                factors['Vfactor'] = 1e-3 # V from mV
                factors['Tfactor'] = 1e-3 # s from ms
                factors['Ifactor'] = 1e-6 # A from microA
            else:
                utils.dump("NOTE", "We got {0}".format(units))
                factors['Vfactor'] = 1.0
                factors['Tfactor'] = 1.0
                factors['Ifactor'] = 1.0

            for inputElem in inputs.findall(".//{"+nmu.nml_ns+"}input"):
                self.attachInputToMoose(inputElem, factors) 
    def attachInputToMoose(self, inElemXml, factors, savePlot=True):

        """attachInputToMoose
        This function create StimulousTable in moose

        There can be two type of stimulous: random_stim or pulse_input.
        """

        # If /elec doesn't exists it creates /elec and returns a reference to
        # it. If it does, it just returns its reference.
        moose.Neutral(self.elecPath)
        inputName = inElemXml.get('name')

        random_stim = inElemXml.find('.//{'+nmu.nml_ns+'}random_stim')
        pulse_stim = inElemXml.find('.//{'+nmu.nml_ns+'}pulse_input')

        if random_stim is not None:
            utils.dump("INFO", "Generating random stimulous")
            utils.dump("TODO", "Test this Poission spike train table")
    
            # Get the frequency of stimulus
            frequency = moose_methods.toFloat(
                    random_stim.get('frequency', '1.0')
                    ) / factors['Tfactor']
            amplitude = random_stim.get('amplitude', 1.0)
            synpMechanism = random_stim.get('synaptic_mechanism')

            # Create random stimulus
            vec = stimulus.generateSpikeTrainPoission(frequency
                    , dt=self.dt
                    , simTime=self.simTime
                    )
            # Stimulus table
            tablePath = os.path.join(self.elecPath, "Stimulus")
            moose.Neutral(tablePath)
            stimPath = os.path.join(tablePath, inputName)
            stim = moose.TimeTable(stimPath)
            stim.vec = vec

            if savePlot:
                self.plotVector(vec, tablePath)

            target = inElemXml.find(".//{"+nmu.nml_ns+"}target")
            population = target.get('population')
            for site in target.findall(".//{"+nmu.nml_ns+"}site"):
               cell_id = site.attrib['cell_id']
               if 'segment_id' in site.attrib:
                   segment_id = site.attrib['segment_id']
               else:
                   # default segment_id is specified to be 0
                   segment_id = 0 

               # To find the cell name fetch the first element of tuple.
               cell_name = self.populationDict[population][0]
               if cell_name == 'LIF':
                   utils.dump("NOTE",
                           "LIF cell_name. Partial implementation"
                           , frame = inspect.currentframe()
                           )

                   LIF = self.populationDict[population][1][int(cell_id)]
                   m = self.connectSynapse(stim, LIF)
               else:
                    segId = '{0}'.format(segment_id)
                    segment_path = self.populationDict[population][1]\
                           [int(cell_id)].path + '/' + \
                            self.cellSegmentDict[cell_name][segId][0]
                    compartment = moose.Compartment(segment_path)
                    synchan = moose.SynChan(
                        os.path.join(compartment.path , '/synchan')
                        )
                    synchan.Gbar = 1e-6
                    synchan.Ek = 0.0
                    self.connectWrapper(synchan, 'channel', compartment, 'channel')
                    synchan.numSynapses = 1
                    m = self.connectSynapse(stim, moose.element(synchan.path+'/synapse'))

        elif pulse_stim is not None:

            Ifactor = factors['Ifactor']
            Tfactor = factors['Tfactor']
            pulseinput = inElemXml.find(".//{"+nmu.nml_ns+"}pulse_input")
            if pulseinput is None:
                utils.dump("WARN"
                        , "This type of stimulous is not supported."
                        , frame = inspect.currentframe()
                        )
                return 

            self.pulseGenPath = self.elecPath + '/PulseGen'
            moose.Neutral(self.pulseGenPath)
            pulseGenPath = '{}/{}'.format(self.pulseGenPath, inputName)
            pulsegen = moose.PulseGen(pulseGenPath)
            icClampPath = '{}/{}'.format(self.elecPath, 'iClamp')
            moose.Neutral(icClampPath)
            iclamp = moose.DiffAmp('{}/{}'.format(icClampPath, inputName))
            iclamp.saturation = 1e6
            iclamp.gain = 1.0

            # free run
            pulsegen.trigMode = 0 
            pulsegen.baseLevel = 0.0
            pulsegen.firstDelay = float(pulseinput.attrib['delay'])*Tfactor
            pulsegen.firstWidth = float(pulseinput.attrib['duration'])*Tfactor
            pulsegen.firstLevel = float(pulseinput.attrib['amplitude'])*Ifactor

            # to avoid repeat
            pulsegen.secondDelay = 1e6 
            pulsegen.secondLevel = 0.0
            pulsegen.secondWidth = 0.0

            # do not set count to 1, let it be at 2 by default else it will
            # set secondDelay to 0.0 and repeat the first pulse!
            #pulsegen.count = 1
            self.connectWrapper(pulsegen,'output', iclamp, 'plusIn')

            # Attach targets
            target = inElemXml.find(".//{"+nmu.nml_ns+"}target")
            population = target.attrib['population']
            for site in target.findall(".//{"+nmu.nml_ns+"}site"):
                cell_id = site.attrib['cell_id']
                if 'segment_id' in site.attrib: 
                    segment_id = site.attrib['segment_id']
                else: 
                    # default segment_id is specified to be 0
                    segment_id = 0 

                # population is populationName,
                # self.populationDict[population][0] is cellname
                cell_name = self.populationDict[population][0]
                if cell_name == 'LIF':
                    debug.printDebut("TODO"
                            , "Rewrite this section"
                            , frame = inspect.currentframe()
                            )
                    continue
                    LIF = self.populationDict[population][1][int(cell_id)]
                    self.connectWrapper(iclamp,'output',LIF,'injectMsg')
                else:
                    segment_path = self.populationDict[population][1]\
                            [int(cell_id)].path+'/'+\
                             self.cellSegmentDict[cell_name][segment_id][0]
                    compartment = moose.Compartment(segment_path)

                    self.connectWrapper(iclamp
                            ,'output'
                            , compartment
                            ,'injectMsg'
                            )
            
    def createPopulations(self):
        """
        Create population dictionary.
        """
        populations =  self.network.findall(".//{"+nmu.nml_ns+"}population")
        if not populations:
            utils.dump("WARN"
                    , [ 
                        "No population find in model"
                        , "Searching in namespace {}".format(nmu.nml_ns)
                        ]
                    , frame = inspect.currentframe()
                    )

        for population in populations:
            cellname = population.attrib["cell_type"]
            populationName = population.attrib["name"]
            utils.dump("INFO"
                    , "Loading population `{0}`".format(populationName)
                    )
            # if cell does not exist in library load it from xml file
            if not moose.exists(self.libraryPath+'/'+cellname):
                utils.dump("DEBUG"
                        , "Searching in subdirectories for cell types" + 
                        " in `{0}.xml` and `{0}.morph.xml` ".format(cellname)
                        )
                mmlR = MorphML.MorphML(self.nml_params)
                model_filenames = (cellname+'.xml', cellname+'.morph.xml')
                success = False
                for modelFile in model_filenames:
                    model_path = nmu.find_first_file(modelFile
                            , self.modelDir
                            )
                    if model_path is not None:
                        cellDict = mmlR.readMorphMLFromFile(model_path)
                        success = True
                        break
                if not success:
                    raise IOError(
                        'For cell {0}: files {1} not found under {2}.'.format(
                            cellname, model_filenames, self.modelDir
                        )
                    )
                self.cellSegmentDict.update(cellDict)
            if cellname == 'LIF':
                cellid = moose.LeakyIaF(self.libraryPath+'/'+cellname)
            else:
                # added cells as a Neuron class.
                cellid = moose.Neuron(self.libraryPath+'/'+cellname) 

            self.populationDict[populationName] = (cellname,{})

            for instance in population.findall(".//{"+nmu.nml_ns+"}instance"):
                instanceid = instance.attrib['id']
                location = instance.find('./{'+nmu.nml_ns+'}location')
                rotationnote = instance.find('./{'+nmu.meta_ns+'}notes')
                if rotationnote is not None:
                    # the text in rotationnote is zrotation=xxxxxxx
                    zrotation = float(rotationnote.text.split('=')[1])
                else:
                    zrotation = 0
                if cellname == 'LIF':
                    cell = moose.LeakyIaF(cellid)
                    self.populationDict[populationName][1][int(instanceid)] = cell
                else:
                    # No Cell class in MOOSE anymore! :( addded Neuron class -
                    # Chaitanya
                    cell = moose.Neuron(cellid) 
                    self.populationDict[populationName][1][int(instanceid)] = cell
                    x = float(location.attrib['x']) * self.length_factor
                    y = float(location.attrib['y']) * self.length_factor
                    z = float(location.attrib['z']) * self.length_factor
                    self.translate_rotate(cell, x, y, z, zrotation)

    # recursively translate all compartments under obj
    def translate_rotate(self,obj,x,y,z,ztheta): 
        for childId in obj.children:
            childobj = moose.Neutral(childId)
            # if childobj is a compartment or symcompartment translate, else
            # skip it
            if childobj.className in ['Compartment','SymCompartment']:
                # SymCompartment inherits from Compartment, so below wrapping by
                # Compartment() is fine for both Compartment and SymCompartment
                child = moose.Compartment(childId)
                x0 = child.x0
                y0 = child.y0
                x0new = x0 * cos(ztheta) - y0 * sin(ztheta)
                y0new = x0 * sin(ztheta) + y0 * cos(ztheta)
                child.x0 = x0new + x
                child.y0 = y0new + y
                child.z0 += z
                x1 = child.x
                y1 = child.y
                x1new = x1 * cos(ztheta) - y1 * sin(ztheta)
                y1new = x1 * sin(ztheta) + y1 * cos(ztheta)
                child.x = x1new + x
                child.y = y1new + y
                child.z += z
            if len(childobj.children)>0:
                # recursive translation+rotation
                self.translate_rotate(childobj
                        , x
                        , y
                        , z
                        , ztheta
                        )

    def getCellPath(self, populationType, instanceId):
        ''' Given a population type and instanceId, return its path '''
        try:
            path = self.populationDict[populationType][1]
        except KeyError as e:
            utils.dump("ERROR"
                    , [ "Population type `{0}` not found".format(populationType)
                        , "Availale population in network are "
                        , self.populationDict.keys()
                    ]
                    )
            raise KeyError("Missing population type : {}".format(populationType))
        except Exception as e:
            raise e

        try:
            path = path[instanceId]
        except KeyError as e:
            msg = "Population type {} has no instance {}".format(
                    populationType
                    , instanceId
                    )
            utils.dump("ERROR"
                    , [msg , "Available instances are" , path.keys() ]
                    )
            raise KeyError(msg)

        # Now get the path from moose path
        path = path.path
        if not re.match(r'(\/\w+)+', path):
            raise UserWarning("{} is not a valid path".format(path))
        return path 


    def addConnection(self, connection, projection, options):

        """
        This function adds connection
        """
        synName = options['syn_name']
        source = options['source']
        target = options['target']
        weight = options['weight']
        threshold = options['threshold']
        propDelay = options['prop_delay']
        projectionName = projection.attrib['name']

        pre_cell_id = connection.attrib['pre_cell_id']
        post_cell_id = connection.attrib['post_cell_id']

        if 'file' not in pre_cell_id:
            # source could be 'mitrals', self.populationDict[source][0] would be
            # 'mitral'
            pre_cell_id = int(pre_cell_id)
            post_cell_id = int(post_cell_id)
            pre_cell_name = self.populationDict[source][0]
            pre_segment_id = connection.attrib.get('pre_segment_id', 0)
            pre_segment_path = "{0}/{1}".format(
                    self.getCellPath(source, pre_cell_id)
                    , self.cellSegmentDict[pre_cell_name][pre_segment_id][0]
                    )
        else:
            # I've removed extra excitation provided via files, so below comment
            # doesn't apply.  'file[+<glomnum>]_<filenumber>' # glomnum is for
            # mitral_granule extra excitation from unmodelled sisters.
            pre_segment_path = "{}_{}".format(
                    pre_cell_id 
                    , connection.attrib['pre_segment_id']
                    )

        # target could be 'PGs', self.populationDict[target][0] would be 'PG'
        post_cell_name = self.populationDict[target][0]
        post_segment_id = connection.attrib.get('post_segment_id', '0')

        post_segment_path = "{}/{}".format(
                self.getCellPath(target, post_cell_id)
                , self.cellSegmentDict[post_cell_name][post_segment_id][0]
                )

        try:
            self.projectionDict[projectionName][2].append(
                    (synName , pre_segment_path, post_segment_path)
                    )
        except KeyError as e:
            utils.dump("ERR", "Failed find key {0}".format(e)
                    , frame = inspect.currentframe())
            print(self.projectionDict.keys())
            sys.exit(0)

        properties = connection.findall('./{'+nmu.nml_ns+'}properties')
        if len(properties) == 0:
            self.connectUsingSynChan(synName, pre_segment_path, post_segment_path
                    , weight, threshold, propDelay
                    )
        else:
            [self.addProperties(pre_segment_path, post_segment_path, p, options) 
                    for p in properties]

    def addProperties(self, pre_segment_path, post_segment_path, props, options):
        '''Add properties 
        '''
        synName = options['syn_name']
        source = options['source']
        target = options['target']
        weight = options['weight']
        threshold = options['threshold']
        propDelay = options['prop_delay']

        synapse = props.attrib.get('synapse_type', None)
        if not synapse: 
            utils.dump("WARN"
                    , "Synapse type {} not found.".format(synapse)
                    , frame = inspect.currentframe()
                    )
            raise UserWarning("Missing parameter synapse_type")

        synName = synapse
        weight_override = float(props.attrib['weight'])
        if 'internal_delay' in props.attrib:
            delay_override = float(props.attrib['internal_delay'])
        else: delay_override = propDelay
        if weight_override != 0.0:
            self.connectUsingSynChan(synName
                    , pre_segment_path
                    , post_segment_path
                    , weight_override
                    , threshold, delay_override
                    )
        else: pass


    def createProjections(self):
        self.projectionDict={}
        projections = self.network.find(".//{"+nmu.nml_ns+"}projections")
        if projections is not None:
            if projections.attrib["units"] == 'Physiological Units':
                # see pg 219 (sec 13.2) of Book of Genesis
                self.Efactor = 1e-3 # V from mV
                self.Tfactor = 1e-3 # s from ms
            else:
                self.Efactor = 1.0
                self.Tfactor = 1.0
        [self.createProjection(p) for p in
                self.network.findall(".//{"+nmu.nml_ns+"}projection")]


    def createProjection(self, projection):
        projectionName = projection.attrib["name"]
        utils.dump("INFO", "Projection {0}".format(projectionName))
        source = projection.attrib["source"]
        target = projection.attrib["target"]
        self.projectionDict[projectionName] = (source,target,[])

        # TODO: Assuming that only one element <synapse_props> under
        # <projection> element.
        synProps = projection.find(".//{"+nmu.nml_ns+"}synapse_props")
        options = self.addSyapseProperties(projection, synProps, source, target)

        connections = projection.findall(".//{"+nmu.nml_ns+"}connection")
        [self.addConnection(c, projection, options) for c in connections]

    def addSyapseProperties(self, projection,  syn_props, source, target):
        '''Add Synapse properties'''
        synName = syn_props.attrib['synapse_type']
        ## if synapse does not exist in library load it from xml file
        if not moose.exists(os.path.join(self.libraryPath, synName)):
            cmlR = ChannelML.ChannelML(self.nml_params)
            modelFileName = synName+'.xml'
            model_path = nmu.find_first_file(modelFileName
                    , self.modelDir
                    )
            if model_path is not None:
                cmlR.readChannelMLFromFile(model_path)
            else:
                msg = 'For mechanism {0}: files {1} not found under {2}.'.format(
                        synName, modelFileName, self.modelDir
                    )
                raise UserWarning(msg)

        weight = float(syn_props.attrib['weight'])
        threshold = float(syn_props.attrib['threshold']) * self.Efactor
        if 'prop_delay' in syn_props.attrib:
            propDelay = float(syn_props.attrib['prop_delay']) * self.Tfactor
        elif 'internal_delay' in syn_props.attrib:
            propDelay = float(syn_props.attrib['internal_delay']) * self.Tfactor
        else:
            propDelay = 0.0

        options = { 'syn_name' : synName , 'weight' : weight
                   , 'source' : source , 'target' : target
                   , 'threshold' : threshold , 'prop_delay' : propDelay 
                   }
        return options

    def connectSynapse(self, spikegen, synapse):
        ''' Add synapse. '''

        assert isinstance(synapse, moose.SynChan), type(synapse)

        #utils.dump("INFO"
        #        , "Connecting ({})\n\t`{}`\n\t`{}`".format(
        #                "Sparse"
        #                , spikegen.path
        #                , synapse.vec.path
        #                )
        #        , frame = inspect.currentframe()
        #        )

        # Following 6 lines are from snippet Izhikevich_with_synapse.py file. I
        # am not sure whether this is the right way to make a synpase. However,
        # let's try this till we get a non-zero result after simulation.
        spikeStim = moose.PulseGen('%s/spike_stim' % (synapse.parent.path))
        spikeStim.delay[0] = 50.0
        spikeStim.level[0] = 1.0
        spikeStim.width[0] = 100.0
        moose.connect(spikeStim, 'output', spikegen, 'Vm')
        m = moose.connect(spikegen, "spikeOut"
                , synapse.synapse.vec, "addSpike"
                , "Sparse"
                )
        m.setRandomConnectivity(1.0, 1)
        return m

    def connectUsingSynChan(self, synName, prePath, post_path
            , weight, threshold, delay
            ):
        """
        Connect two compartments using SynChan
        """

        postcomp = moose.Compartment(post_path)

        # We usually try to reuse an existing SynChan - event based SynChans
        # have an array of weights and delays and can represent multiple
        # synapses i.e.  a new element of the weights and delays array is
        # created every time a 'synapse' message connects to the SynChan (from
        # 'event' of spikegen) BUT for a graded synapse with a lookup table
        # output connected to 'activation' message, not to 'synapse' message, we
        # make a new synapse everytime ALSO for a saturating synapse i.e.
        # KinSynChan, we always make a new synapse as KinSynChan is not meant to
        # represent multiple synapses
        libsyn = moose.SynChan(self.libraryPath+'/'+synName)
        gradedchild = utils.get_child_Mstring(libsyn, 'graded')

        # create a new synapse
        if libsyn.className == 'KinSynChan' or gradedchild.value == 'True': 
            synNameFull = moose_methods.moosePath(synName
                    , utils.underscorize(prePath)
                    )
            synObj = self.makeNewSynapse(synName, postcomp, synNameFull)
        else:
            # See debug/bugs for more details.
            # NOTE: Change the debug/bugs to enable/disable this bug.
            if bugs.BUG_NetworkML_500:
                utils.dump("INFO"
                        , "See the code. There might be a bug here"
                        , frame = inspect.currentframe()
                        )
                synNameFull = moose_methods.moosePath(synName
                        , utils.underscorize(prePath)
                        )
                synObj = self.makeNewSynapse(synName, postcomp, synNameFull)

            else: # If the above bug is fixed.
                synNameFull = synName
                if not moose.exists(post_path+'/'+synNameFull):
                    synObj = self.makeNewSynapse(synName, postcomp, synNameFull)

        # wrap the synapse in this compartment
        synPath = moose_methods.moosePath(post_path, synNameFull)
        syn = moose.SynChan(synPath)

        gradedchild = utils.get_child_Mstring(syn, 'graded')

        # weights are set at the end according to whether the synapse is graded
        # or event-based


        # connect pre-comp Vm (if graded) OR spikegen/timetable (if event-based)
        # to the synapse

        # graded synapse
        if gradedchild.value=='True': 
            table = moose.Table(syn.path+"/graded_table")
            # always connect source to input - else 'cannot create message'
            # error.
            precomp = moose.Compartment(prePath)
            self.connectWrapper(precomp, "VmOut", table, "msgInput")

            # since there is no weight field for a graded synapse
            # (no 'synapse' message connected),
            # I set the Gbar to weight*Gbar
            syn.Gbar = weight * syn.Gbar

        # Event based synapse
        else: 
            # synapse could be connected to spikegen at pre-compartment OR a
            # file!
            if 'file' not in prePath:
                precomp = moose.Compartment(prePath)
                if not moose.exists(prePath+'/IaF_spikegen'):
                    # if spikegen for this synapse doesn't exist in this
                    # compartment, create it spikegens for different synapse_types
                    # can have different thresholds
                    if not moose.exists(prePath+'/'+synName+'_spikegen'):
                        spikegen = moose.SpikeGen(prePath+'/'+synName+'_spikegen')
                        # spikegens for different synapse_types can have different
                        # thresholds
                        spikegen.threshold = threshold
                        # This ensures that spike is generated only on leading edge.
                        spikegen.edgeTriggered = 1 

                        # usually events are raised at every time step that Vm >
                        # Threshold, can set either edgeTriggered as above or
                        # refractT
                        #spikegen.refractT = 0.25e-3 


                    # wrap the spikegen in this compartment
                    spikegen = moose.SpikeGen(prePath+'/'+synName+'_spikegen') 
                else:
                    spikegen = moose.SpikeGen(prePath+'/IaF_spikegen')

                # connect the spikegen to the synapse note that you need to use
                # Synapse (auto-created) under SynChan to get/set weights ,
                # addSpike-s etc.  can get the Synapse element by
                # moose.Synapse(syn.path+'/synapse') or syn.synapse Synpase is
                # an array element, first add to it, to addSpike-s, get/set
                # weights, etc.


                syn.numSynapses += 1
                m = self.connectSynapse(spikegen, syn)

            else:
                # if connected to a file, create a timetable,
                # put in a field specifying the connected filenumbers to this segment,
                # and leave it for simulation-time connection
                ## prePath is 'file[+<glomnum>]_<filenum1>[_<filenum2>...]' i.e. glomnum could be present
                filesplit = prePath.split('+')
                if len(filesplit) == 2:
                    glomsplit = filesplit[1].split('_', 1)
                    glomstr = '_'+glomsplit[0]
                    filenums = glomsplit[1]
                else:
                    glomstr = ''
                    filenums = prePath.split('_', 1)[1]
                tt_path = postcomp.path+'/'+synNameFull+glomstr+'_tt'
                if not moose.exists(tt_path):
                    # if timetable for this synapse doesn't exist in this
                    # compartment, create it, and add the field 'fileNumbers'
                    tt = moose.TimeTable(tt_path)
                    tt.addField('fileNumbers')
                    tt.setField('fileNumbers',filenums)

                    # Be careful to connect the timetable only once while
                    # creating it as below: note that you need to use Synapse
                    # (auto-created) under SynChan to get/set weights ,
                    # addSpike-s etc.  can get the Synapse element by
                    # moose.Synapse(syn.path+'/synapse') or syn.synapse Synpase
                    # is an array element, first add to it, to addSpike-s,
                    # get/set weights, etc.
                    syn.numSynapses += 1
                    m = self.connectSynapse(spikegen, syn.synapse)
                else:
                    # if it exists, append file number to the field 'fileNumbers'
                    tt = moose.TimeTable(tt_path)
                    # append filenumbers from
                    # 'file[+<glomnum>]_<filenumber1>[_<filenumber2>...]'
                    filenums = moose_methods.moosePath(tt.getField('fileNumbers')
                            , filenums)
                    tt.setField('fileNumbers', filenums)

            # syn.Gbar remains the same, but we play with the weight which is a
            # factor to Gbar The delay and weight can be set only after
            # connecting a spike event generator.  delay and weight are arrays:
            # multiple event messages can be connected to a single synapse first
            # argument below is the array index, we connect to the latest
            # synapse created above But KinSynChan ignores weight of the
            # synapse, so set the Gbar for it
            if libsyn.className == 'KinSynChan':
                syn.Gbar = weight*syn.Gbar
            else:
                # note that you need to use Synapse (auto-created) under SynChan
                # to get/set weights , addSpike-s etc.  can get the Synpase
                # element by moose.Synapse(syn.path+'/synapse') or syn.synapse
                syn.synapse[-1].weight = weight
            syn.synapse[-1].delay = delay # seconds
            #print 'len = ',len(syn.synapse)
            #for i,syn_syn in enumerate(syn.synapse):
            #    print i,'th weight =',syn_syn.weight,'\n'

    def makeNewSynapse(self, synName, postcomp, synNameFull):
        '''This function creates a new synapses onto postcomp.

        SpikeGen is spikeGenerator (presynaptic). SpikeGen connects to SynChan,
        a synaptic channel which connects to post-synaptic compartment.

        SpikeGen models the pre-synaptic events.
        '''
        synPath = "%s/%s" % (self.libraryPath, synName)
        utils.dump("SYNAPSE"
                , "Creating {} with path {} onto compartment {}".format(
                    synName
                    , synPath
                    , postcomp.path
                    )
                )
        # if channel does not exist in library load it from xml file
        if not moose.exists(synPath):
            utils.dump("SYNAPSE"
                    , "Synaptic Channel {} does not exists. {}".format(
                        synPath, "Loading is from XML file"
                        )
                    )
            cmlR = ChannelML.ChannelML(self.nml_params)
            cmlR.readChannelMLFromFile(synName+'.xml')

        # deep copies the library synapse to an instance under postcomp named as
        # <arg3>
        if config.disbleCopyingOfObject:
            utils.dump("WARN"
                    , "Copying existing SynChan ({}) to {}".format(
                        synPath
                        , postcomp
                        )
                    )
            synid = moose.copy(moose.Neutral(synPath), postcomp, synNameFull)
        else:
            synid = synPath
        
        syn = moose.SynChan(synid)
        syn = self.configureSynChan(syn, synParams={})
        childmgblock = utils.get_child_Mstring(syn,'mgblock')

        # connect the post compartment to the synapse
        # If NMDA synapse based on mgblock, connect to mgblock
        if childmgblock.value == 'True': 
            mgblock = moose.Mg_block(syn.path+'/mgblock')
            self.connectWrapper(postcomp, "channel", mgblock, "channel")
        # if SynChan or even NMDAChan, connect normally
        else: 
            self.connectWrapper(postcomp,"channel", syn, "channel")

    def configureSynChan(self, synObj, synParams={}):
        '''Configure synapse. If no parameters are given then use the default
        values. 

        '''
        assert(isinstance(synObj, moose.SynChan))
        utils.dump("SYNAPSE"
                , "Configuring SynChan"
                )
        synObj.tau1 = synParams.get('tau1', 5.0)
        synObj.tau2 = synParams.get('tau2', 1.0)
        synObj.Gk = synParams.get('Gk', 1.0)
        synObj.Ek = synParams.get('Ek', 0.0)
        synObj.synapse.num = synParams.get('synapse_num', 1)
        synObj.synapse.delay = synParams.get('delay', 1.0)
        synObj.synapse.weight = synParams.get('weight', 1.0)
        return synObj
