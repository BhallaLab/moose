"""

Description: class MorphML for loading MorphML from file or xml element into
MOOSE.

NeuroML.py is the preferred interface. Use this only if NeuroML L1,L2,L3 files
are misnamed/scattered.  Instantiate MorphML class, and thence use methods:
readMorphMLFromFile(...) to load a standalone MorphML from file OR
readMorphML(...) to load from an xml.etree xml element (could be part of a
larger NeuroML file).  

It is assumed that any channels and synapses referred to by above MorphML have
already been loaded under that same name in /library in MOOSE (use ChannelML
loader).  Note: This has been changed. Default path is now /neuroml/library 


"""
# cELementTree is mostly API-compatible but faster than ElementTree
from xml.etree import cElementTree as ET
import math
import sys
import moose
from moose import utils as moose_utils
from moose.neuroml import utils as neuroml_utils
import helper.moose_methods as moose_methods

from ChannelML import ChannelML
import core.config as config
import debug.debug as debug
import inspect
import re

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
        self.libraryPath = config.libraryPath
        self.cellPath = config.cellPath
        moose.Neutral(self.libraryPath)

    def stringToFloat(self, tempString):
        if type(tempString) == str:
            tempString = tempString.strip()
            if len(tempString) == 0 or tempString is None:
                return 0.0
        else:
            return float(tempString)

    def readMorphMLFromFile(self, filename, params={}):
        """
        specify global params as a dict (presently none implemented)
        returns { cellName1 : segDict, ... }
        see readMorphML(...) for segDict
        """
        debug.printDebug("INFO", "{}".format(filename))
        tree = ET.parse(filename)
        neuroml_element = tree.getroot()
        cellsDict = {}
        for cell in neuroml_element.findall('.//{'+self.neuroml+'}cell'):
            cellDict = self.readMorphML(
                cell
                , params
                , neuroml_element.attrib['lengthUnits']
            )
            cellsDict.update(cellDict)
        return cellsDict


    def addSegment(self, cellName, segnum, segment):

        """
        Adding segment to cell.
        """
        run_dia = 0.0
        running_comp = None
        running_dia_nums = 0
        segmentname = segment.attrib['name']

        debug.printDebug("DEBUG"
                , "Adding segment {} in cell {}".format(segmentname, cellName)
                )

        # cable is an optional attribute. WARNING: Here I assume it is always
        # present.
        cableid = segment.attrib['cable']
        segmentid = segment.attrib['id']

        # Old cableid still running, hence don't start a new compartment, skip
        # to next segment.
        if cableid == self.running_cableid:
            self.cellDictBySegmentId[cellName][1][segmentid] = running_comp
            proximal = segment.find('./{'+self.mml+'}proximal')
            if proximal is not None:
                run_dia += float(proximal.attrib["diameter"])*self.length_factor
                running_dia_nums += 1
            distal = segment.find('./{'+self.mml+'}distal')
            if distal is not None:
                run_dia += float(distal.attrib["diameter"])*self.length_factor
                running_dia_nums += 1

        # new cableid starts, hence start a new compartment; also finish
        # previous / last compartment.
        else:

            # Create a new compartment, the moose "hsolve" method assumes
            # compartments to be asymmetric compartments and symmetrizes them
            # but that is not what we want when translating from Neuron
            # which has only symcompartments -- so be careful!

            # just segmentname is NOT unique - eg: mitral bbmit exported from
            # NEURON.
            mooseCompname = moose_methods.moosePath(segmentname, segmentid)
            mooseComppath = self.mooseCell.path + '/' + mooseCompname
            mooseComp = moose.Compartment(mooseComppath)
            self.cellDictBySegmentId[cellName][1][segmentid] = mooseComp

            # Cables are grouped and densities set for cablegroups. Hence I
            # need to refer to segment according to which cable they belong
            # to..
            self.cellDictByCableId[cellName][1][cableid] = mooseComp
            self.running_cableid = cableid
            running_segid = segmentid
            running_comp = mooseComp
            run_dia = 0.0
            running_dia_nums = 0

            if 'parent' in segment.attrib:
                # I assume the parent is created before the child so that I can
                # immediately # connect the child.
                parentid = segment.attrib['parent']
                parent = self.cellDictBySegmentId[cellName][1][parentid]

                # It is always assumed that axial of parent is connected to
                # raxial of moosesegment THIS IS WHAT GENESIS readcell()
                # DOES!!! UNLIKE NEURON!  THIS IS IRRESPECTIVE OF WHETHER
                # PROXIMAL x,y,z OF PARENT = PROXIMAL x,y,z OF CHILD.  THIS IS
                # ALSO IRRESPECTIVE OF fraction_along_parent SPECIFIED IN
                # CABLE!  THUS THERE WILL BE NUMERICAL DIFFERENCES BETWEEN
                # MOOSE/GENESIS and NEURON.  moosesegment sends Ra and Vm to
                # parent, parent sends only Vm actually for symmetric
                # compartment, both parent and moosesegment require each
                # other's Ra/2, but axial and raxial just serve to distinguish
                # ends.

                moose.connect(parent, 'axial', mooseComp, 'raxial')
            else:
                parent = None
            proximal = segment.find('./{'+self.mml+'}proximal')
            if proximal is None:
                # If proximal tag is not present, then parent attribute MUST be
                # present in the segment tag!  if proximal is not present, then
                # by default the distal end of the parent is the proximal end
                # of the child
                mooseComp.x0 = parent.x
                mooseComp.y0 = parent.y
                mooseComp.z0 = parent.z
            else:
                mooseComp.x0 = float(proximal.attrib["x"])*self.length_factor
                mooseComp.y0 = float(proximal.attrib["y"])*self.length_factor
                mooseComp.z0 = float(proximal.attrib["z"])*self.length_factor
                run_dia += float(proximal.attrib["diameter"])*self.length_factor
                running_dia_nums += 1
            distal = segment.find('./{'+self.mml+'}distal')
            if distal is not None:
                run_dia += float(distal.attrib["diameter"]) * self.length_factor
                running_dia_nums += 1

            # finished creating new compartment Update the end position,
            # diameter and length, and segDict of this comp/cable/section with
            # each segment that is part of this cable (assumes contiguous
            # segments in xml).  This ensures that we don't have to do any
            # 'closing ceremonies', if a new cable is encoutered in next
            # iteration.

            if distal is not None:
                running_comp.x = float(distal.attrib["x"])*self.length_factor
                running_comp.y = float(distal.attrib["y"])*self.length_factor
                running_comp.z = float(distal.attrib["z"])*self.length_factor

            # Set the compartment diameter as the average diameter of all the
            # segments in this section
            running_comp.diameter = run_dia / float(running_dia_nums)

            # Set the compartment length
            running_comp.length = math.sqrt(
                (running_comp.x-running_comp.x0)**2+\
                (running_comp.y-running_comp.y0)**2+\
                (running_comp.z-running_comp.z0)**2
            )

            # NeuroML specs say that if (x0,y0,z0)=(x,y,z), then round
            # compartment e.g. soma.  In Moose set length = dia to give same
            # surface area as sphere of dia.

            if running_comp.length == 0.0:
                running_comp.length = running_comp.diameter

            # Set the segDict the empty list at the end below will get
            # populated with the potential synapses on this segment, in
            # function set_compartment_param(..)
            self.segDict[running_segid] = [
                running_comp.name
                , (running_comp.x0 , running_comp.y0 , running_comp.z0)
                , (running_comp.x,running_comp.y,running_comp.z)
                , running_comp.diameter
                , running_comp.length
                , []
            ]

            if neuroml_utils.neuroml_debug:
                debug.printDebug(
                    "STEP"
                    , "Set up compartment/section %s" % running_comp.name
                )



    def readMorphML(self, cell, params={}, lengthUnits="micrometer"):

        """
        returns {cellName:segDict}
        where segDict = { segid1 : [ segname
        , (proximalx,proximaly,proximalz)
        , (distalx,distaly,distalz),diameter,length,[potential_syn1, ... ] ]
        , ... }
        segname is "<name>_<segid>" because 1) guarantees uniqueness,
        2) later scripts obtain segid from the compartment's name!
        """

        debug.printDebug("DEBUG", "Entered function readMorphML")
        if lengthUnits in ['micrometer','micron']:
            self.length_factor = 1e-6
        else:
            self.length_factor = 1.0
        cellName = cell.attrib["name"]

        if cellName == 'LIF':
            self.mooseCell = moose.LeakyIaF(self.cellPath+'/'+cellName)
            self.segDict = {}
        else:
            # using moose Neuron class - in previous version 'Cell' class
            # Chaitanya.
            self.mooseCell = moose.Neuron(self.cellPath+'/'+cellName)
            self.cellDictBySegmentId[cellName] = [self.mooseCell,{}]
            self.cellDictByCableId[cellName] = [self.mooseCell,{}]
            self.segDict = {}

            # load morphology and connections between compartments Many neurons
            # exported from NEURON have multiple segments in a section Combine
            # those segments into one Compartment / section assume segments of
            # a compartment/section are in increasing order and assume all
            # segments of a compartment/section have the same cableid findall()
            # returns elements in document order:

            self.running_cableid = ''
            running_segid = ''
            segments = cell.findall(".//{"+self.mml+"}segment")
            segmentstotal = len(segments)
            for segnum, segment in enumerate(segments):
                self.addSegment(cellName, segnum, segment)

        # load cablegroups into a dictionary
        self.cablegroupsDict = {}

        # Two ways of specifying cablegroups in neuroml 1.x
        # <cablegroup>s with list of <cable>s
        cablegroups = cell.findall(".//{"+self.mml+"}cablegroup")
        for cablegroup in cablegroups:
            cablegroupname = cablegroup.attrib['name']
            self.cablegroupsDict[cablegroupname] = []
            for cable in cablegroup.findall(".//{"+self.mml+"}cable"):
                cableid = cable.attrib['id']
                self.cablegroupsDict[cablegroupname].append(cableid)

        # <cable>s with list of <meta:group>s
        cables = cell.findall(".//{"+self.mml+"}cable")
        for cable in cables:
            cableid = cable.attrib['id']
            cablegroups = cable.findall(".//{"+self.meta+"}group")
            for cablegroup in cablegroups:
                cablegroupname = cablegroup.text
                if cablegroupname in list(self.cablegroupsDict.keys()):
                    self.cablegroupsDict[cablegroupname].append(cableid)
                else:
                    self.cablegroupsDict[cablegroupname] = [cableid]

        # Add bioPhysics to the cell
        self.addBiophysics(cell, cellName)

        # load connectivity / synapses into the compartments
        connectivity = cell.find(".//{"+self.neuroml+"}connectivity")
        if connectivity is not None:
            self.addConnectivity(cell, cellName, connectivity)
        return {cellName:self.segDict}

    def addConnectivity(self, cell, cellName, connectivity):

      """
      Add connectivity to cell.
      """
      synLocs = cell.findall(".//{"+self.nml+"}potential_syn_loc")
      for psl in synLocs:
        if 'synapse_direction' in list(psl.attrib.keys()):
            if psl.attrib['synapse_direction'] in ['post']:
                self.set_group_compartment_param(
                    cell
                    , cellName
                    , psl
                    , 'synapse_type'
                    , psl.attrib['synapse_type']
                    , self.nml
                    , mechName='synapse'
                )
            if psl.attrib['synapse_direction'] in ['pre']:
                self.set_group_compartment_param(
                    cell
                    , cellName
                    , psl
                    , 'spikegen_type'
                    , psl.attrib['synapse_type']
                    , self.nml
                    , mechName='spikegen'
                )


    def addBiophysics(self, cell, cellName):
        """Add Biophysics to cell
        """
        biophysics = cell.find(".//{"+self.neuroml+"}biophysics")
        if biophysics is None: return None

        if biophysics.attrib["units"] == 'Physiological Units':
            # see pg 219 (sec 13.2) of Book of Genesis
            self.CMfactor = 1e-2 # F/m^2 from microF/cm^2
            self.Cfactor = 1e-6  # F from microF
            self.RAfactor = 1e1  # Ohm*m from KOhm*cm
            self.RMfactor = 1e-1 # Ohm*m^2 from KOhm*cm^2
            self.Rfactor = 1e-3  # Ohm from KOhm
            self.Efactor = 1e-3  # V from mV
            self.Gfactor = 1e1   # S/m^2 from mS/cm^2
            self.Ifactor = 1e-6  # A from microA
            self.Tfactor = 1e-3  # s from ms
        else:
            self.CMfactor = 1.0
            self.Cfactor = 1.0
            self.RAfactor = 1.0
            self.RMfactor = 1.0
            self.Rfactor = 1.0
            self.Efactor = 1.0
            self.Gfactor = 1.0
            self.Ifactor = 1.0
            self.Tfactor = 1.0

        IaFpresent = False
        for mechanism in cell.findall(".//{"+self.bio+"}mechanism"):
            mechName = mechanism.attrib["name"]
            if mechName == "integrate_and_fire": IaFpresent = True
            ## integrate-and-fire-meachanism
            if IaFpresent:
                self.integateAndFireMechanism(mechanism)

        # Other mechanism are non-I&F type
        sc = cell.find(".//{"+self.bio+"}spec_capacitance")
        if sc is not None:
            self.addSpecCapacitance(sc, cell, cellName)

        sar = cell.find(".//{"+self.bio+"}spec_axial_resistance")
        if sar is not None:
            self.addSpecAxialResistance(sar, cell, cellName)

        imp = cell.find(".//{"+self.bio+"}init_memb_potential")
        if imp is not None:
            self.addInitMembPotential(imp, cell, cellName)

        # Add mechanisms

        mechanisms = cell.findall(".//{"+self.bio+"}mechanism")
        for mechanism in mechanisms:
            self.addMechanism(mechanism, cell, cellName)


    def addMechanism(self, mechanism, cell, cellName):
        """ Add mechanism to cell.
        """
        mechName = mechanism.attrib["name"]
        debug.printDebug("STEP", "Loading mechanism {0}".format(mechName))
        passive = False
        if "passive_conductance" in mechanism.attrib:
            if mechanism.attrib['passive_conductance'].lower() == "true":
                passive = True

        # ONLY creates channel if at least one parameter (like gmax) is
        # specified in the xml  Neuroml does not allow you to specify all
        # default values.  However, granule cell example in neuroconstruct has
        # Ca ion pool without a parameter, applying default values to all
        # compartments!

        mech_params = mechanism.findall(".//{"+self.bio+"}parameter")

        ## if no params, apply all default values to all compartments
        if len(mech_params) == 0:
            compartments = self.cellDictByCableId[cellName][1].values()
            for c in compartments:
                self.set_compartment_param(c, None, 'default', mechName)

        # if params are present, apply params to specified cable/compartment
        # groups.

        for parameter in mech_params:
            options = { 'cellName' : cellName
                       , 'mechName' : mechName
                       , 'passive' : passive
                       }
            self.addParameterToCompartment(parameter, cell, options)

        # Connect the Ca pools and channels Am connecting these at the very end
        # so that all channels and pools have been created Note: this function
        # is in moose.utils not moose.neuroml.utils !

        # temperature should be in Kelvin for Nernst
        temperature = self.stringToFloat(self.temperature)
        moose_utils.connect_CaConc(
            list(self.cellDictByCableId[cellName][1].values())
            , temperature + neuroml_utils.ZeroCKelvin
        )

    def addParameterToCompartment(self, parameter, cell, options):
        """ Add parameter to compartment
        """
        cellName = options['cellName']
        mechName = options['mechName']
        passive = options['passive']

        paramName = parameter.attrib['name']
        if passive:
            if paramName in ['gmax']:
                self.set_group_compartment_param(
                    cell
                    , cellName
                    , parameter
                    , 'RM'
                    , self.RMfactor*1.0/float(parameter.attrib["value"])
                    , self.bio
                )
            elif paramName in ['e','erev']:
                self.set_group_compartment_param(
                    cell
                    , cellName
                    , parameter
                    , 'Em'
                    , self.Efactor*float(parameter.attrib["value"])
                    , self.bio
                )
            elif paramName in ['inject']:
                self.set_group_compartment_param(
                    cell
                    , cellName
                    , parameter
                    , 'inject'
                    , self.Ifactor*float(parameter.attrib["value"])
                    , self.bio
                )
            else:
                msg = "Yo programmar of MorphML! You did not implemented"
                msg += " parameter {0} in mechanism {1} ".format(
                    paramName
                    , mechName
                    )
                debug.printDebug("WARN", msg, frame=inspect.currentframe())
        else:
            if paramName in ['gmax']:
                gmaxval = float(eval( parameter.attrib["value"]
                                     ,{"__builtins__":None},{}
                                     )
                                )
                self.set_group_compartment_param(
                    cell
                    , cellName
                    , parameter
                    , 'Gbar'
                    , self.Gfactor*gmaxval
                    , self.bio
                    , mechName
                )
            elif paramName in ['e','erev']:
                self.set_group_compartment_param(
                    cell
                    , cellName
                    , parameter
                    , 'Ek'
                    , self.Efactor*float(parameter.attrib["value"])
                    , self.bio
                    , mechName
                )
            elif paramName in ['depth']: # has to be type Ion Concentration!
                self.set_group_compartment_param(
                    cell
                    , cellName
                    , parameter
                    , 'thick'
                    , self.length_factor*float(parameter.attrib["value"])
                    , self.bio
                    , mechName
                )
            else:
                msg = "Yo programmar of MorphML! You did not implemented"
                msg += " parameter {0} in mechanism {1} ".format(
                    paramName
                    , mechName
                    )
                debug.printDebug("WARN", msg, frame=inspect.currentframe())

    def addSpecCapacitance(self, spec_capacitance, cell, cellName):
        """
        Adding specific capacitance to cell
        """
        for parameter in spec_capacitance.findall(".//{"+self.bio+"}parameter"):
            self.set_group_compartment_param(
                cell
                , cellName
                , parameter
                , 'CM'
                , float(parameter.attrib["value"])*self.CMfactor
                , self.bio
            )


    def addSpecAxialResistance(self, spec_axial_resistance, cell, cellName):
        """
        Add specific axial resistance to cell.
        """
        for p in spec_axial_resistance.findall(".//{"+self.bio+"}parameter"):
            self.set_group_compartment_param(
                cell
                , cellName
                , p
                , 'RA'
                , float(p.attrib["value"])*self.RAfactor
                , self.bio
            )

    def integateAndFireMechanism(self, mechanism):
        """  Integrate and fire mechanism
        """
        mech_params = mechanism.findall(".//{"+self.bio+"}parameter")
        for parameter in mech_params:
            paramName = parameter.attrib['name']
            if paramName == 'inject':
                self.mooseCell.inject = float(parameter.attrib["value"])*self.Ifactor
            elif paramName == 'Rm':
                self.mooseCell.Rm = float(parameter.attrib["value"])*self.Rfactor
            elif paramName == 'Cm':
                self.mooseCell.Cm = float(parameter.attrib["value"])*self.Cfactor
            elif paramName == 'Em':
                self.mooseCell.Em = float(parameter.attrib["value"])*self.Efactor
            elif paramName == 'v_reset':
                # voltage after spike, typicaly below resting
                self.mooseCell.Vreset = float(parameter.attrib["value"])*self.Efactor
                self.mooseCell.initVm = self.mooseCell.Vreset
            elif paramName == 'threshold':
                # firing threshold potential
                self.mooseCell.Vthreshold = \
                        float(parameter.attrib["value"])*self.Efactor
            elif paramName == 't_refrac':
                # min refractory time before next spike
                msg = "Use this refractory period in simulation"
                debug.printDebug("TODO" , msg, frame=inspect.currentframe())
                self.mooseCell.refractoryPeriod = \
                        float(parameter.attrib["value"])*self.Tfactor
            elif paramName == 'inject':
                # inject into soma
                self.mooseCell.refractoryPeriod = \
                        float(parameter.attrib["value"])*self.Ifactor


    def addInitMembPotential(self, init_memb_potential, cell, cellName):
        """Add init membrane potential to cell.
        """
        parameters = init_memb_potential.findall(".//{"+self.bio+"}parameter")
        for parameter in parameters:
            self.set_group_compartment_param(
                    cell
                    , cellName
                    , parameter
                    , 'initVm'
                    , float(parameter.attrib["value"]) * self.Efactor
                    , self.bio
                    )


    def set_group_compartment_param(self, cell, cellName, parameter, name
        , value, grouptype, mechName=None):

        """
        Find the compartments that belong to the cablegroups refered to
         for this parameter and set_compartment_param.
        """

        for group in parameter.findall(".//{"+grouptype+"}group"):
            cablegroupname = group.text
            if cablegroupname == 'all':
                for compartment in self.cellDictByCableId[cellName][1].values():
                    self.set_compartment_param(
                        compartment
                        , name
                        , value
                        , mechName
                    )
            else:
                for cableid in self.cablegroupsDict[cablegroupname]:
                    compartment = self.cellDictByCableId[cellName][1][cableid]
                    self.set_compartment_param(
                        compartment
                        , name
                        , value
                        , mechName
                    )

    def set_compartment_param(self, compartment, name, value, mechName):
        """ Set the param for the compartment depending on name and mechName. """
        if name == 'CM':
            compartment.Cm = value *math.pi*compartment.diameter*compartment.length
        elif name == 'RM':
            compartment.Rm = value/(math.pi*compartment.diameter*compartment.length)
        elif name == 'RA':
            compartment.Ra = value * compartment.length / \
                    (math.pi*(compartment.diameter/2.0)**2)
        elif name == 'Em':
            compartment.Em = value
        elif name == 'initVm':
            compartment.initVm = value
        elif name == 'inject':
            msg = " {0} inject {1} A.".format(compartment.name, value)
            debug.printDebug("INFO", msg)
            compartment.inject = value
        elif mechName is 'synapse':

           # synapse being added to the compartment
           # these are potential locations, we do not actually make synapses.
           # I assume below that compartment name has _segid at its end

           # get segment id from compartment name
           segid = moose_methods.getCompartmentId(compartment.name)
           self.segDict[segid][5].append(value)

        # spikegen being added to the compartment
        elif mechName is 'spikegen': 
            # these are potential locations, we do not actually make the
            # spikegens.  spikegens for different synapses can have different
            # thresholds, hence include synapse_type in its name value contains
            # name of synapse i.e. synapse_type
            #spikegen = moose.SpikeGen(compartment.path+'/'+value+'_spikegen')
            #moose.connect(compartment,"VmSrc",spikegen,"Vm")
            pass
        elif mechName is not None:

            # if mechanism is not present in compartment, deep copy from library
            if not moose.exists(compartment.path+'/'+mechName):

                # if channel does not exist in library load it from xml file
                if not moose.exists(self.libraryPath+"/"+mechName):
                    cmlR = ChannelML(self.nml_params)
                    model_filename = mechName+'.xml'
                    model_path = neuroml_utils.find_first_file(
                        model_filename
                        , self.model_dir
                    )
                    if model_path is not None:
                        cmlR.readChannelMLFromFile(model_path)
                    else:
                        msg = 'Mechanism {0}: files {1} not found under {2}'\
                                .format( mechName
                                        , model_filename
                                        , self.model_dir
                                        )
                        debug.printDebug("ERROR"
                                , msg
                                , frame = inspect.currentframe()
                                )
                        sys.exit(0)

                neutralObj = moose.Neutral(self.libraryPath+"/"+mechName)

                # Ion concentration pool
                if 'CaConc' == neutralObj.className:
                    libcaconc = moose.CaConc(self.libraryPath+"/"+mechName)

                    # deep copies the library caconc under the compartment
                    caconc = moose.copy(libcaconc,compartment,mechName)
                    caconc = moose.CaConc(caconc)

                    # CaConc connections are made later using connect_CaConc()
                    # Later, when calling connect_CaConc, B is set for caconc
                    # based on thickness of Ca shell and compartment l and dia
                    # OR based on the Mstring phi under CaConc path.
                    channel = None

                elif 'HHChannel2D' == neutralObj.className : ## HHChannel2D
                    libchannel = moose.HHChannel2D(self.libraryPath+"/"+mechName)
                    ## deep copies the library channel under the compartment
                    channel = moose.copy(libchannel,compartment,mechName)
                    channel = moose.HHChannel2D(channel)
                    moose.connect(channel,'channel',compartment,'channel')
                elif 'HHChannel' == neutralObj.className : ## HHChannel
                    libchannel = moose.HHChannel(self.libraryPath+"/"+mechName)

                    # deep copies the library channel under the compartment
                    channel = moose.copy(libchannel,compartment,mechName)
                    channel = moose.HHChannel(channel)
                    moose.connect(channel,'channel',compartment,'channel')
            # if mechanism is present in compartment, just wrap it
            else:
                neutralObj = moose.Neutral(compartment.path+'/'+mechName)
                # Ion concentration pool
                if 'CaConc' == neutralObj.className:
                    # wraps existing channel
                    caconc = moose.CaConc(compartment.path+'/'+mechName)
                    channel = None
                elif 'HHChannel2D' == neutralObj.className: ## HHChannel2D
                    # wraps existing channel
                    channel = moose.HHChannel2D(compartment.path+'/'+mechName)
                elif 'HHChannel' == neutralObj.className : ## HHChannel
                    # wraps existing channel
                    channel = moose.HHChannel(compartment.path+'/'+mechName)
            if name == 'Gbar':
                # if CaConc, neuroConstruct uses gbar for thickness or phi
                if channel is None:
                    # If child Mstring 'phi' is present, set gbar as phi BUT,
                    # value has been multiplied by Gfactor as a Gbar, SI or
                    # physiological not known here, ignoring Gbar for CaConc,
                    # instead of passing units here
                    child = moose_utils.get_child_Mstring(caconc,'phi')
                    if child is not None:
                        #child.value = value
                        pass
                    else:
                        #caconc.thick = value
                        pass
                else: # if ion channel, usual Gbar
                    channel.Gbar = value * math.pi * compartment.diameter \
                            * compartment.length
            elif name == 'Ek':
                channel.Ek = value

            # thick seems to be NEURON's extension to NeuroML level 2.
            elif name == 'thick':
                # JUST THIS WILL NOT DO - HAVE TO SET B based on this thick!
                caconc.thick = value
                # Later, when calling connect_CaConc, B is set for caconc based
                # on thickness of Ca shell and compartment l and dia.  OR based
                # on the Mstring phi under CaConc path.

        if neuroml_utils.neuroml_debug:
            msg = "Setting {0} for {1} value {2}".format(name, compartment.path
                                                         , value
                                                         )
            debug.printDebug("DEBUG", msg, frame=inspect.currentframe())
