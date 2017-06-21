#########################################################################
## rdesigneur0_5.py ---
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU General Public License version 2 or later.
## See the file COPYING.LIB for the full notice.
#########################################################################

##########################################################################
## This class builds models of
## Reaction-Diffusion and Electrical SIGnaling in NEURons.
## It loads in neuronal and chemical signaling models and embeds the
## latter in the former, including mapping entities like calcium and
## channel conductances, between them.
##########################################################################
from __future__ import print_function
import imp
import os
import moose
import numpy as np
import pylab
import math
import rmoogli
#import rdesigneurProtos
from rdesigneurProtos import *
from moose.neuroml.NeuroML import NeuroML
from moose.neuroml.ChannelML import ChannelML

try:
  from lxml import etree
except ImportError:
  try:
    # Python 2.5
    import xml.etree.cElementTree as etree
  except ImportError:
    try:
      # Python 2.5
      import xml.etree.ElementTree as etree
    except ImportError:
      try:
        # normal cElementTree install
        import cElementTree as etree
      except ImportError:
        try:
          # normal ElementTree install
          import elementtree.ElementTree as etree
        except ImportError:
          print("Failed to import ElementTree from any known place")

import csv

#EREST_ACT = -70e-3

class BuildError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

#######################################################################

class rdesigneur:
    """The rdesigneur class is used to build models incorporating
    reaction-diffusion and electrical signaling in neurons.
    Params:
        useGssa: True/False               for GSSA in spine and PSD
        combineSegments: True/False       for NeuroML models
        diffusionLength: default 2e-6
        adaptCa: [( Ca_wildcard_string, chem_wildcard_string, offset, scale ),...]
        adaptChem: [( Chem_wildcard_string, elec_wildcard_string, offset, scale ),...]

    I need to put the extra channels now into the NeuroML definition.
    """
    ################################################################
    def __init__(self,
            modelPath = '/model',
            turnOffElec = False,
            useGssa = True,
            combineSegments = True,
            stealCellFromLibrary = False,
            diffusionLength= 2e-6,
            meshLambda = -1.0,    #This is a backward compatibility hack
            temperature = 32,
            chemDt= 0.001,          # Much finer than MOOSE, for multiscale
            diffDt= 0.001,          # 10x finer than MOOSE, for multiscale
            elecDt= 50e-6,          # Same default as from MOOSE
            chemPlotDt = 1.0,       # Same default as from MOOSE
            elecPlotDt = 0.1e-3,    # Same default as from MOOSE
            cellProto = [],
            spineProto = [],
            chanProto = [],
            chemProto = [],
            passiveDistrib= [],
            spineDistrib= [],
            chanDistrib = [],
            chemDistrib = [],
            adaptorList= [],
            stimList = [],
            plotList = [],
            moogList = [],
            params = None
        ):
        """ Constructor of the rdesigner. This just sets up internal fields
            for the model building, it doesn't actually create any objects.
        """

        self.modelPath = modelPath
        self.turnOffElec = turnOffElec
        self.useGssa = useGssa
        self.combineSegments = combineSegments
        self.stealCellFromLibrary = stealCellFromLibrary
        self.diffusionLength= diffusionLength
        if meshLambda > 0.0:
            print("Warning: meshLambda argument is deprecated. Please use 'diffusionLength' instead.\nFor now rdesigneur will accept this argument.")
            self.diffusionLength = meshLambda
        self.temperature = temperature
        self.chemDt= chemDt
        self.diffDt= diffDt
        self.elecDt= elecDt
        self.elecPlotDt= elecPlotDt
        self.chemPlotDt= chemPlotDt

        self.cellProtoList = cellProto
        self.spineProtoList = spineProto
        self.chanProtoList = chanProto
        self.chemProtoList = chemProto

        self.passiveDistrib = passiveDistrib
        self.spineDistrib = spineDistrib
        self.chanDistrib = chanDistrib
        self.chemDistrib = chemDistrib

        self.params = params

        self.adaptorList = adaptorList
        self.stimList = stimList
        self.plotList = plotList
        self.saveList = plotList                    #ADDED BY Sarthak 
        self.saveAs = []
        self.moogList = moogList
        self.plotNames = []
        self.saveNames = []
        self.moogNames = []
        self.cellPortionElist = []
        self.spineComptElist = []
        self.tabForXML = []

        if not moose.exists( '/library' ):
            library = moose.Neutral( '/library' )
        try:
            self.buildCellProto()
            self.buildChanProto()
            self.buildSpineProto()
            self.buildChemProto()
        except BuildError as msg:
            print("Error: rdesigneur: Prototype build failed:", msg)
            quit()
            


    ################################################################
    def _printModelStats( self ):
        print("Rdesigneur: Elec model has",
            self.elecid.numCompartments, "compartments and",
            self.elecid.numSpines, "spines on",
            len( self.cellPortionElist ), "compartments.")
        if hasattr( self , 'chemid' ):
            dmstoich = moose.element( self.dendCompt.path + '/stoich' )
            print("Chem part of model has ",
                self.dendCompt.mesh.num, "dendrite voxels X",
                dmstoich.numAllPools, "pools,\n    ")
            if hasattr( self , 'spineCompt' ):
                smstoich = moose.element( self.spineCompt.path + '/stoich')
                pmstoich = moose.element( self.psdCompt.path + '/stoich' )
                print(self.spineCompt.mesh.num, "spine voxels X",
                    smstoich.numAllPools, "pools,",
                    self.psdCompt.mesh.num, "psd voxels X",
                    pmstoich.numAllPools, "pools.")

    def buildModel( self, modelPath = '/model' ):
        if moose.exists( modelPath ):
            print("rdesigneur::buildModel: Build failed. Model '",
                modelPath, "' already exists.")
            return
        self.model = moose.Neutral( modelPath )
        self.modelPath = modelPath
        try:
            # Protos made in the init phase. Now install the elec and 
            # chem protos on model.
            self.installCellFromProtos()
            # Now assign all the distributions
            self.buildPassiveDistrib()
            self.buildChanDistrib()
            self.buildSpineDistrib()
            self.buildChemDistrib()
            self._configureSolvers()
            self.buildAdaptors()
            self._buildPlots()
            self._buildMoogli()
            self._buildStims()
            self._configureClocks()
            self._printModelStats()
            self._savePlots()

        except BuildError as msg:
            print("Error: rdesigneur: model build failed:", msg)
            moose.delete( self.model )

    def installCellFromProtos( self ):
        if self.stealCellFromLibrary:
            moose.move( self.elecid, self.model )
            if self.elecid.name != 'elec':
                self.elecid.name = 'elec'
        else:
            moose.copy( self.elecid, self.model, 'elec' )
            self.elecid = moose.element( self.model.path + '/elec' )
            self.elecid.buildSegmentTree() # rebuild: copy has happened.
        if hasattr( self, 'chemid' ):
            self.validateChem()
            if self.stealCellFromLibrary:
                moose.move( self.chemid, self.model )
                if self.chemid.name != 'chem':
                    self.chemid.name = 'chem'
            else:
                moose.copy( self.chemid, self.model, 'chem' )
                self.chemid = moose.element( self.model.path + '/chem' )

        ep = self.elecid.path
        somaList = moose.wildcardFind( ep + '/#oma#[ISA=CompartmentBase]' )
        if len( somaList ) == 0:
            somaList = moose.wildcardFind( ep + '/#[ISA=CompartmentBase]' )
        if len( somaList ) == 0:
            raise BuildError( "installCellFromProto: No soma found" )
        maxdia = 0.0
        for i in somaList:
            if ( i.diameter > maxdia ):
                self.soma = i

    ################################################################
    # Some utility functions for building prototypes.
    ################################################################
    # Return true if it is a function.
    def buildProtoFromFunction( self, func, protoName ):
        bracePos = func.find( '()' )
        if bracePos == -1:
            return False

        modPos = func.find( "." )
        if ( modPos != -1 ): # Function is in a file, load and check
            pathTokens = func[0:modPos].split('/')
            pathTokens = ['/'] + pathTokens
            modulePath = os.path.join(*pathTokens[:-1])
            moduleName = pathTokens[-1]
            funcName = func[modPos+1:bracePos]
            moduleFile, pathName, description = imp.find_module(moduleName, [modulePath])
            try:
                module = imp.load_module(moduleName, moduleFile, pathName, description)
                funcObj = getattr(module, funcName)
                funcObj(protoName)
                return True
            finally:
                moduleFile.close()
            return False
        if not func[0:bracePos] in globals():
            raise BuildError( \
                protoName + " Proto: global function '" +func+"' not known.")
        globals().get( func[0:bracePos] )( protoName )
        return True

    # Class or file options. True if extension is found in
    def isKnownClassOrFile( self, name, suffices ):
        for i in suffices:
            if name.rfind( '.'+i ) >= 0 :
                return True
        return False



    # Checks all protos, builds them and return true. If it was a file
    # then it has to return false and invite the calling function to build
    # If it fails then the exception takes over.
    def checkAndBuildProto( self, protoType, protoVec, knownClasses, knownFileTypes ):
        if len(protoVec) != 2:
            raise BuildError( \
                protoType + "Proto: nargs should be 2, is " + \
                    str( len(protoVec)  ))
        if moose.exists( '/library/' + protoVec[1] ):
            # Assume the job is already done, just skip it.
            return True
            '''
            raise BuildError( \
                protoType + "Proto: object /library/" + \
                    protoVec[1] + " already exists." )
            '''
        # Check and build the proto from a class name
        if protoVec[0][:5] == 'moose':
            protoName = protoVec[0][6:]
            if self.isKnownClassOrFile( protoName, knownClasses ):
                try:
                    getAttr( moose, protoName )( '/library/' + protoVec[1] )
                except AttributeError:
                    raise BuildError( protoType + "Proto: Moose class '" \
                            + protoVec[0] + "' not found." )
                return True

        if self.buildProtoFromFunction( protoVec[0], protoVec[1] ):
            return True
        # Maybe the proto is already in memory
        # Avoid relative file paths going toward root
        if protoVec[0][:3] != "../":
            if moose.exists( protoVec[0] ):
                moose.copy( protoVec[0], '/library/' + protoVec[1] )
                return True
            if moose.exists( '/library/' + protoVec[0] ):
                #moose.copy('/library/' + protoVec[0], '/library/', protoVec[1])
                print('renaming /library/' + protoVec[0] + ' to ' + protoVec[1])
                moose.element( '/library/' + protoVec[0]).name = protoVec[1]
                #moose.le( '/library' )
                return True
        # Check if there is a matching suffix for file type.
        if self.isKnownClassOrFile( protoVec[0], knownFileTypes ):
            return False
        else:
            raise BuildError( \
                protoType + "Proto: File type '" + protoVec[0] + \
                "' not known." )
        return True

    ################################################################
    # Here are the functions to build the type-specific prototypes.
    ################################################################
    def buildCellProto( self ):
        if len( self.cellProtoList ) == 0:
            ''' Make HH squid model sized compartment:
            len and dia 500 microns. CM = 0.01 F/m^2, RA =
            '''
            self.elecid = makePassiveHHsoma( name = 'cell' )
            assert( moose.exists( '/library/cell/soma' ) )
            self.soma = moose.element( '/library/cell/soma' )

            '''
            self.elecid = moose.Neuron( '/library/cell' )
            dia = 500e-6
            self.soma = buildCompt( self.elecid, 'soma', dia, dia, 0.0,
                0.33333333, 3000, 0.01 )
            self.soma.initVm = -65e-3 # Resting of -65, from HH
            self.soma.Em = -54.4e-3 # 10.6 mV above resting of -65, from HH
            '''

        for i in self.cellProtoList:
            if self.checkAndBuildProto( "cell", i, \
                ["Compartment", "SymCompartment"], ["swc", "p", "nml", "xml"] ):
                self.elecid = moose.element( '/library/' + i[1] )
            else:
                self._loadElec( i[0], i[1] )
            self.elecid.buildSegmentTree()

    def buildSpineProto( self ):
        for i in self.spineProtoList:
            if not self.checkAndBuildProto( "spine", i, \
                ["Compartment", "SymCompartment"], ["swc", "p", "nml", "xml"] ):
                self._loadElec( i[0], i[1] )

    def parseChanName( self, name ):
        if name[-4:] == ".xml":
            period = name.rfind( '.' )
            slash = name.rfind( '/' )
            if ( slash >= period ):
                raise BuildError( "chanProto: bad filename:" + i[0] )
            if ( slash < 0 ):
                return name[:period]
            else:
                return name[slash+1:period]

    def buildChanProto( self ):
        for i in self.chanProtoList:
            if len(i) == 1:
                chanName = self.parseChanName( i[0] )
            else:
                chanName = i[1]
            j = [i[0], chanName]
            if not self.checkAndBuildProto( "chan", j, [], ["xml"] ):
                cm = ChannelML( {'temperature': self.temperature} )
                cm.readChannelMLFromFile( i[0] )
                if ( len( i ) == 2 ):
                    chan = moose.element( '/library/' + chanName )
                    chan.name = i[1]

    def buildChemProto( self ):
        for i in self.chemProtoList:
            if not self.checkAndBuildProto( "chem", i, \
                ["Pool"], ["g", "sbml", "xml" ] ):
                self._loadChem( i[0], i[1] )
            self.chemid = moose.element( '/library/' + i[1] )

    ################################################################
    # Here we set up the distributions
    ################################################################
    def buildPassiveDistrib( self ):
        temp = []
        for i in self.passiveDistrib:
            temp.extend( i )
            temp.extend( [""] )
        self.elecid.passiveDistribution = temp

    def buildChanDistrib( self ):
        temp = []
        for i in self.chanDistrib:
            temp.extend( i )
            temp.extend( [""] )
        self.elecid.channelDistribution = temp

    def buildSpineDistrib( self ):
        # For uniformity and conciseness, we don't use a dictionary.
        # ordering of spine distrib is
        # name, path, spacing, spacingDistrib, size, sizeDistrib, angle, angleDistrib
        # [i for i in L1 if i in L2]
        # The first two args are compulsory, and don't need arg keys.
        usageStr = 'Usage: name, path, [spacing, spacingDistrib, size, sizeDistrib, angle, angleDistrib]'
        temp = []
        defaults = ['spine', '#dend#,#apical#', '10e-6', '1e-6', '1', '0.5', '0', '6.2831853' ]
        argKeys = ['spacing', 'spacingDistrib', 'size', 'sizeDistrib', 'angle', 'angleDistrib' ]
        for i in self.spineDistrib:
            if len(i) >= 2 :
                arg = i[:2]
                # Backward compat hack here
                bcKeys = [ j for j in i[2:] if j in argKeys ]
                if len( bcKeys ) > 0: # Looks like we have an old arg str
                    print('Rdesigneur::buildSpineDistrib: Warning: Deprecated argument format.\nWill accept for now.')
                    print(usageStr)
                    temp.extend( i + [''] )
                elif len( i ) > len( defaults ):
                    print('Rdesigneur::buildSpineDistrib: Warning: too many arguments in spine definition')
                    print(usageStr)
                else:
                    optArg = i[2:] + defaults[ len(i):]
                    assert( len( optArg ) == len( argKeys ) )
                    for j in zip( argKeys, optArg ):
                        arg.extend( [j[0], j[1]] )
                    temp.extend( arg + [''] )

        self.elecid.spineDistribution = temp

    def buildChemDistrib( self ):
        for i in self.chemDistrib:
            pair = i[1] + " " + i[3]
            # Assign any other params. Possibly the first param should
            # be a global scaling factor.
            self.cellPortionElist = self.elecid.compartmentsFromExpression[ pair ]
            if len( self.cellPortionElist ) == 0:
                raise BuildError( \
                    "buildChemDistrib: No elec compartments found in path: '" \
                        + pair + "'" )
            self.spineComptElist = self.elecid.spinesFromExpression[ pair ]
            '''
            if len( self.spineComptElist ) == 0:
                raise BuildError( \
                    "buildChemDistrib: No spine compartments found in path: '" \
                        + pair + "'" )
            '''
            # Build the neuroMesh
            # Check if it is good. Need to catch the ValueError here.
            self._buildNeuroMesh()
            # Assign the solvers

    ################################################################
    # Here we set up the adaptors
    ################################################################
    def findMeshOnName( self, name ):
        pos = name.find( '/' )
        if ( pos != -1 ):
            temp = name[:pos]
            if temp == 'psd' or temp == 'spine' or temp == 'dend':
                return ( temp, name[pos+1:] )
        return ("","")


    def buildAdaptors( self ):
        for i in self.adaptorList:
            mesh, name = self.findMeshOnName( i[0] )
            if mesh == "":
                mesh, name = self.findMeshOnName( i[2] )
                if  mesh == "":
                    raise BuildError( "buildAdaptors: Failed for " + i[2] )
                self._buildAdaptor( mesh, i[0], i[1], name, i[3], True, i[4], i[5] )
            else:
                self._buildAdaptor( mesh, i[2], i[3], name, i[1], False, i[4], i[5] )

    ################################################################
    # Here we set up the plots. Dummy for cases that don't match conditions
    ################################################################
    def _collapseElistToPathAndClass( self, comptList, path, className ):
        dummy = moose.element( '/' )
        ret = [ dummy ] * len( comptList )
        j = 0
        for i in comptList:
            if moose.exists( i.path + '/' + path ):
                obj = moose.element( i.path + '/' + path )
                if obj.isA[ className ]:
                    ret[j] = obj
            j += 1
        return ret

    # Returns vector of source objects, and the field to use.
    # plotSpec is of the form
    #   [ region_wildcard, region_expr, path, field, title]
    def _parseComptField( self, comptList, plotSpec, knownFields ):
        # Put in stuff to go through fields if the target is a chem object
        field = plotSpec[3]
        if not field in knownFields:
            print("Warning: Rdesigneur::_parseComptField: Unknown field '", field, "'")
            return (), ""

        kf = knownFields[field] # Find the field to decide type.
        if ( kf[0] == 'CaConcBase' or kf[0] == 'ChanBase' or kf[0] == 'NMDAChan' ):
            objList = self._collapseElistToPathAndClass( comptList, plotSpec[2], kf[0] )
            # print ("objList: ", len(objList), kf[1])
            return objList, kf[1]
        elif (field == 'n' or field == 'conc'  ):
            path = plotSpec[2]
            pos = path.find( '/' )
            if pos == -1:   # Assume it is in the dend compartment.
                path  = 'dend/' + path
            pos = path.find( '/' )
            chemCompt = path[:pos]
            cc = moose.element( self.modelPath + '/chem/' + chemCompt)
            voxelVec = []
            if ( chemCompt == 'dend' ):
                for i in comptList:
                    voxelVec.extend( cc.dendVoxelsOnCompartment[i] )
            else:
                em = cc.elecComptMap
                elecComptMap = { moose.element(em[i]):i for i in range(len(em)) }
                for i in comptList:
                    if i in elecComptMap:
                        voxelVec.extend( [ elecComptMap[i] ] )
            # Here we collapse the voxelVec into objects to plot.
            allObj = moose.vec( self.modelPath + '/chem/' + plotSpec[2] )
            #print "####### allObj=", self.modelPath + '/chem/' + plotSpec[2]
            if len( allObj ) >= len( voxelVec ):
                objList = [ allObj[int(j)] for j in voxelVec]
            else:
                objList = []
                print( "Warning: Rdesigneur::_parseComptField: unknown Object: '", plotSpec[2], "'" )
            #print "############", chemCompt, len(objList), kf[1]
            return objList, kf[1]

        else:
            return comptList, kf[1]


    def _buildPlots( self ):
        knownFields = {
            'Vm':('CompartmentBase', 'getVm', 1000, 'Memb. Potential (mV)' ),
            'Im':('CompartmentBase', 'getIm', 1e9, 'Memb. current (nA)' ),
            'inject':('CompartmentBase', 'getInject', 1e9, 'inject current (nA)' ),
            'Gbar':('ChanBase', 'getGbar', 1e9, 'chan max conductance (nS)' ),
            'Gk':('ChanBase', 'getGk', 1e9, 'chan conductance (nS)' ),
            'Ik':('ChanBase', 'getIk', 1e9, 'chan current (nA)' ),
            'ICa':('NMDAChan', 'getICa', 1e9, 'Ca current (nA)' ),
            'Ca':('CaConcBase', 'getCa', 1e3, 'Ca conc (uM)' ),
            'n':('PoolBase', 'getN', 1, '# of molecules'),
            'conc':('PoolBase', 'getConc', 1000, 'Concentration (uM)' )
        }
        graphs = moose.Neutral( self.modelPath + '/graphs' )
        dummy = moose.element( '/' )
        k = 0
        for i in self.plotList:
            pair = i[0] + " " + i[1]
            dendCompts = self.elecid.compartmentsFromExpression[ pair ]
            spineCompts = self.elecid.spinesFromExpression[ pair ]
            plotObj, plotField = self._parseComptField( dendCompts, i, knownFields )
            plotObj2, plotField2 = self._parseComptField( spineCompts, i, knownFields )
            assert( plotField == plotField2 )
            plotObj3 = plotObj + plotObj2
            numPlots = sum( i != dummy for i in plotObj3 )
            if numPlots > 0:
                tabname = graphs.path + '/plot' + str(k)
                scale = knownFields[i[3]][2]
                units = knownFields[i[3]][3]
                self.plotNames.append( ( tabname, i[4], k, scale, units ) )
                k += 1
                if i[3] == 'n' or i[3] == 'conc':
                    tabs = moose.Table2( tabname, numPlots )
                else:
                    tabs = moose.Table( tabname, numPlots )
                vtabs = moose.vec( tabs )
                q = 0
                for p in [ x for x in plotObj3 if x != dummy ]:
                    moose.connect( vtabs[q], 'requestOut', p, plotField )
                    q += 1

    def _buildMoogli( self ):
        knownFields = {
            'Vm':('CompartmentBase', 'getVm', 1000, 'Memb. Potential (mV)', -80.0, 40.0 ),
            'Im':('CompartmentBase', 'getIm', 1e9, 'Memb. current (nA)', -10.0, 10.0 ),
            'inject':('CompartmentBase', 'getInject', 1e9, 'inject current (nA)', -10.0, 10.0 ),
            'Gbar':('ChanBase', 'getGbar', 1e9, 'chan max conductance (nS)', 0.0, 1.0 ),
            'Gk':('ChanBase', 'getGk', 1e9, 'chan conductance (nS)', 0.0, 1.0 ),
            'Ik':('ChanBase', 'getIk', 1e9, 'chan current (nA)', -10.0, 10.0 ),
            'ICa':('NMDAChan', 'getICa', 1e9, 'Ca current (nA)', -10.0, 10.0 ),
            'Ca':('CaConcBase', 'getCa', 1e3, 'Ca conc (uM)', 0.0, 10.0 ),
            'n':('PoolBase', 'getN', 1, '# of molecules', 0.0, 200.0 ),
            'conc':('PoolBase', 'getConc', 1000, 'Concentration (uM)', 0.0, 2.0 )
        }
        moogliBase = moose.Neutral( self.modelPath + '/moogli' )
        k = 0
        for i in self.moogList:
            kf = knownFields[i[3]]
            pair = i[0] + " " + i[1]
            dendCompts = self.elecid.compartmentsFromExpression[ pair ]
            spineCompts = self.elecid.spinesFromExpression[ pair ]
            dendObj, mooField = self._parseComptField( dendCompts, i, knownFields )
            spineObj, mooField2 = self._parseComptField( spineCompts, i, knownFields )
            assert( mooField == mooField2 )
            mooObj3 = dendObj + spineObj
            numMoogli = len( mooObj3 )
            #dendComptMap = self.dendCompt.elecComptMap
            #self.moogliViewer = rmoogli.makeMoogli( self, mooObj3, mooField )
            if len( i ) == 5:
                i.extend( kf[4:6] )
            elif len( i ) == 6:
                i.extend( [kf[5]] )
            #self.moogliViewer = rmoogli.makeMoogli( self, mooObj3, i, kf )
            self.moogNames.append( rmoogli.makeMoogli( self, mooObj3, i, kf ) )


    ################################################################
    # Here we display the plots and moogli
    ################################################################
    def displayMoogli( self, moogliDt, runtime, rotation = math.pi/500.0):
        rmoogli.displayMoogli( self, moogliDt, runtime, rotation )

    def display( self ):
        for i in self.plotNames:
            pylab.figure( i[2] )
            pylab.title( i[1] )
            pylab.xlabel( "Time (s)" )
            pylab.ylabel( i[4] )
            vtab = moose.vec( i[0] )
            t = np.arange( 0, vtab[0].vector.size, 1 ) * vtab[0].dt
            for j in vtab:
                pylab.plot( t, j.vector * i[3] )
        if len( self.moogList ) > 0:
            pylab.ion()
        pylab.show(block=True)
        self._save()                                             #This calls the _save function which saves only if the filenames have been specified

    ################################################################
    # Here we get the time-series data and write to various formats
    ################################################################        
    #[TO DO] Add NSDF output function
    '''
    The author of the functions -- [_savePlots(), _getTimeSeriesTable(), _writeXML(), _writeCSV(), _saveFormats(), _save()] is
    Sarthak Sharma. 
    Email address: sarthaks442@gmail.com
    ''' 
    
    def _savePlots( self ):
        
        knownFields = {
            'Vm':('CompartmentBase', 'getVm', 1000, 'Memb. Potential (mV)' ),
            'Im':('CompartmentBase', 'getIm', 1e9, 'Memb. current (nA)' ),
            'inject':('CompartmentBase', 'getInject', 1e9, 'inject current (nA)' ),
            'Gbar':('ChanBase', 'getGbar', 1e9, 'chan max conductance (nS)' ),
            'Gk':('ChanBase', 'getGk', 1e9, 'chan conductance (nS)' ),
            'Ik':('ChanBase', 'getIk', 1e9, 'chan current (nA)' ),
            'ICa':('NMDAChan', 'getICa', 1e9, 'Ca current (nA)' ),
            'Ca':('CaConcBase', 'getCa', 1e3, 'Ca conc (uM)' ),
            'n':('PoolBase', 'getN', 1, '# of molecules'),
            'conc':('PoolBase', 'getConc', 1000, 'Concentration (uM)' )
        }
        
        save_graphs = moose.Neutral( self.modelPath + '/save_graphs' )
        dummy = moose.element( '/' )
        k = 0
        
        for i in self.saveList:
            pair = i[0] + " " + i[1]
            dendCompts = self.elecid.compartmentsFromExpression[ pair ]
            spineCompts = self.elecid.spinesFromExpression[ pair ]
            plotObj, plotField = self._parseComptField( dendCompts, i, knownFields )
            plotObj2, plotField2 = self._parseComptField( spineCompts, i, knownFields )
            assert( plotField == plotField2 )
            plotObj3 = plotObj + plotObj2
            numPlots = sum( i != dummy for i in plotObj3 )
            if numPlots > 0:
                save_tabname = save_graphs.path + '/save_plot' + str(k)
                scale = knownFields[i[3]][2]
                units = knownFields[i[3]][3]
                self.saveNames.append( ( save_tabname, i[4], k, scale, units ) )
                k += 1
                if i[3] == 'n' or i[3] == 'conc':
                    save_tabs = moose.Table2( save_tabname, numPlots )
                else:
                    save_tabs = moose.Table( save_tabname, numPlots )
                save_vtabs = moose.vec( save_tabs )
                q = 0
                for p in [ x for x in plotObj3 if x != dummy ]:
                    moose.connect( save_vtabs[q], 'requestOut', p, plotField )
                    q += 1

    def _getTimeSeriesTable( self ):                                 
                                                               
        '''
        This function gets the list with all the details of the simulation
        required for plotting.
        This function adds flexibility in terms of the details 
        we wish to store.
        '''

        knownFields = {
            'Vm':('CompartmentBase', 'getVm', 1000, 'Memb. Potential (mV)' ),
            'Im':('CompartmentBase', 'getIm', 1e9, 'Memb. current (nA)' ),
            'inject':('CompartmentBase', 'getInject', 1e9, 'inject current (nA)' ),
            'Gbar':('ChanBase', 'getGbar', 1e9, 'chan max conductance (nS)' ),
            'Gk':('ChanBase', 'getGk', 1e9, 'chan conductance (nS)' ),
            'Ik':('ChanBase', 'getIk', 1e9, 'chan current (nA)' ),
            'ICa':('NMDAChan', 'getICa', 1e9, 'Ca current (nA)' ),
            'Ca':('CaConcBase', 'getCa', 1e3, 'Ca conc (uM)' ),
            'n':('PoolBase', 'getN', 1, '# of molecules'),
            'conc':('PoolBase', 'getConc', 1000, 'Concentration (uM)' )
        }    
        
        ''' 
        This takes data from plotList
        saveList is exactly like plotList but with a few additional arguments: 
        ->It will have a resolution option, i.e., the number of decimal figures to which the value should be rounded
        ->There is a list of "saveAs" formats
        With saveList, the user will able to set what all details he wishes to be saved.
        '''

        for i,ind in enumerate(self.saveNames):
            pair = self.saveList[i][0] + " " + self.saveList[i][1]
            dendCompts = self.elecid.compartmentsFromExpression[ pair ]
            spineCompts = self.elecid.spinesFromExpression[ pair ]
            # Here we get the object details from plotList
            savePlotObj, plotField = self._parseComptField( dendCompts, self.saveList[i], knownFields )
            savePlotObj2, plotField2 = self._parseComptField( spineCompts, self.saveList[i], knownFields )
            savePlotObj3 = savePlotObj + savePlotObj2                                    
            
            rowList = list(ind)                                       
            save_vtab = moose.vec( ind[0] )                                   
            t = np.arange( 0, save_vtab[0].vector.size, 1 ) * save_vtab[0].dt
            
            rowList.append(save_vtab[0].dt)
            rowList.append(t)
            rowList.append([jvec.vector * ind[3] for jvec in save_vtab])             #get values
            rowList.append(self.saveList[i][3])
            rowList.append(filter(lambda obj: obj.path != '/', savePlotObj3))        #this filters out dummy elements
            
            if (type(self.saveList[i][-1])==int):
                rowList.append(self.saveList[i][-1])
            else:
                rowList.append(12)
            
            self.tabForXML.append(rowList)
            rowList = []

        timeSeriesTable = self.tabForXML                                            # the list with all the details of plot
        return timeSeriesTable

    def _writeXML( self, filename, timeSeriesData ):                                #to write to XML file 

        plotData = timeSeriesData
        print("[CAUTION] The '%s' file might be very large if all the compartments are to be saved." % filename) 
        root = etree.Element("TimeSeriesPlot")
        parameters = etree.SubElement( root, "parameters" )
        if self.params == None:
            parameters.text = "None"
        else:
            assert(isinstance(self.params, dict)), "'params' should be a dictionary."
            for pkey, pvalue in self.params.items():
                parameter = etree.SubElement( parameters, str(pkey) )
                parameter.text = str(pvalue)

        #plotData contains all the details of a single plot
        title = etree.SubElement( root, "timeSeries" )
        title.set( 'title', str(plotData[1]))
        title.set( 'field', str(plotData[8]))
        title.set( 'scale', str(plotData[3]))
        title.set( 'units', str(plotData[4]))
        title.set( 'dt', str(plotData[5]))
        p = []
        assert(len(plotData[7]) == len(plotData[9]))
        
        res = plotData[10]
        for ind, jvec in enumerate(plotData[7]):
            p.append( etree.SubElement( title, "data"))
            p[-1].set( 'path', str(plotData[9][ind].path))
            p[-1].text = ''.join( str(round(value,res)) + ' ' for value in jvec )
            
        tree = etree.ElementTree(root)
        tree.write(filename)

    def _writeCSV(self, filename, timeSeriesData):

        plotData = timeSeriesData
        dataList = []
        header = []
        time = plotData[6]
        res = plotData[10]

        for ind, jvec in enumerate(plotData[7]):
            header.append(plotData[9][ind].path)    
            dataList.append([round(value,res) for value in jvec.tolist()])
        dl = [tuple(lst) for lst in dataList]
        rows = zip(tuple(time), *dl)
        header.insert(0, "time")

        with open(filename, 'wb') as f:
            writer = csv.writer(f, quoting=csv.QUOTE_MINIMAL)
            writer.writerow(header)
            for row in rows:
                writer.writerow(row)
        
    ##########****SAVING*****###############
    def _saveFormats(self, timeSeriesData, k, *filenames):
        "This takes in the filenames and writes to corresponding format."
        if filenames:
            for filename in filenames:
                for name in filename:
                    print (name)
                    if name[-4:] == '.xml':
                        self._writeXML(name, timeSeriesData)
                        print(name, " written")
                    elif name[-4:] == '.csv':
                        self._writeCSV(name, timeSeriesData)
                        print(name, " written")
                    else:
                        print("not possible")
                        pass
        else:
            pass


    def _save( self ):
        timeSeriesTable = self._getTimeSeriesTable()
        for i,sList in enumerate(self.saveList):

            if (len(sList) >= 6) and (type(sList[5]) != int):
                    self.saveAs.extend(filter(lambda fmt: type(fmt)!=int, sList[5:]))
                    try:
                        timeSeriesData = timeSeriesTable[i]
                    except IndexError:
                        print("The object to be plotted has all dummy elements.")
                        pass
                    self._saveFormats(timeSeriesData, i, self.saveAs)
                    self.saveAs=[]
            else:
                pass
        else:
            pass

    ################################################################
    # Here we set up the stims
    ################################################################
    def _buildStims( self ):
        knownFields = {
                'inject':('CompartmentBase', 'setInject'),
                'Ca':('CaConcBase', 'getCa'),
                'n':('PoolBase', 'setN'),
                'conc':('PoolBase''setConc')
        }
        stims = moose.Neutral( self.modelPath + '/stims' )
        k = 0
        for i in self.stimList:
            pair = i[0] + " " + i[1]
            dendCompts = self.elecid.compartmentsFromExpression[ pair ]
            spineCompts = self.elecid.spinesFromExpression[ pair ]
            stimObj, stimField = self._parseComptField( dendCompts, i, knownFields )
            stimObj2, stimField2 = self._parseComptField( spineCompts, i, knownFields )
            assert( stimField == stimField2 )
            stimObj3 = stimObj + stimObj2
            numStim = len( stimObj3 )
            if numStim > 0:
                funcname = stims.path + '/stim' + str(k)
                k += 1
                func = moose.Function( funcname )
                func.expr = i[4]
                for q in stimObj3:
                    moose.connect( func, 'valueOut', q, stimField )

    ################################################################
    # Utility function for setting up clocks.
    def _configureClocks( self ):
        if self.turnOffElec:
            elecDt = 1e6
            elecPlotDt = 1e6
            diffDt = 0.1    # Slow it down again because no multiscaling
            chemDt = 0.1    # Slow it down again because no multiscaling
        else:
            elecDt = self.elecDt
            diffDt = self.diffDt
            chemDt = self.chemDt
        for i in range( 0, 9 ):
            moose.setClock( i, elecDt )
        moose.setClock( 10, diffDt )
        for i in range( 11, 18 ):
            moose.setClock( i, chemDt )
        moose.setClock( 8, self.elecPlotDt )
        moose.setClock( 18, self.chemPlotDt )
        hsolve = moose.HSolve( self.elecid.path + '/hsolve' )
        hsolve.dt = elecDt
        hsolve.target = self.soma.path
    ################################################################
    ################################################################
    ################################################################

    def validateFromMemory( self, epath, cpath ):
        ret = self.validateChem()
        return ret

    #################################################################
    # assumes ePath is the parent element of the electrical model,
    # and cPath the parent element of the compts in the chem model
    def buildFromMemory( self, ePath, cPath, doCopy = False ):
        if not self.validateFromMemory( ePath, cPath ):
            return
        if doCopy:
            x = moose.copy( cPath, self.model )
            self.chemid = moose.element( x )
            self.chemid.name = 'chem'
            x = moose.copy( ePath, self.model )
            self.elecid = moose.element( x )
            self.elecid.name = 'elec'
        else:
            self.elecid = moose.element( ePath )
            self.chemid = moose.element( cPath )
            if self.elecid.path != self.model.path + '/elec':
                if ( self.elecid.parent != self.model ):
                    moose.move( self.elecid, self.model )
                self.elecid.name = 'elec'
            if self.chemid.path != self.model.path + '/chem':
                if ( self.chemid.parent != self.model ):
                    moose.move( self.chemid, self.model )
                self.chemid.name = 'chem'


        ep = self.elecid.path
        somaList = moose.wildcardFind( ep + '/#oma#[ISA=CompartmentBase]' )
        if len( somaList ) == 0:
            somaList = moose.wildcardFind( ep + '/#[ISA=CompartmentBase]' )
        assert( len( somaList ) > 0 )
        maxdia = 0.0
        for i in somaList:
            if ( i.diameter > maxdia ):
                self.soma = i
        #self.soma = self.comptList[0]
        self._decorateWithSpines()
        self.spineList = moose.wildcardFind( ep + '/#spine#[ISA=CompartmentBase],' + ep + '/#head#[ISA=CompartmentBase]' )
        if len( self.spineList ) == 0:
            self.spineList = moose.wildcardFind( ep + '/#head#[ISA=CompartmentBase]' )
        nmdarList = moose.wildcardFind( ep + '/##[ISA=NMDAChan]' )

        self.comptList = moose.wildcardFind( ep + '/#[ISA=CompartmentBase]')
        print("Rdesigneur: Elec model has ", len( self.comptList ),
            " compartments and ", len( self.spineList ),
            " spines with ", len( nmdarList ), " NMDARs")


        self._buildNeuroMesh()


        self._configureSolvers()
        for i in self.adaptorList:
            print(i)
            self._buildAdaptor( i[0],i[1],i[2],i[3],i[4],i[5],i[6] )

    ################################################################

    def buildFromFile( self, efile, cfile ):
        self.efile = efile
        self.cfile = cfile
        self._loadElec( efile, 'tempelec' )
        if len( self.chanDistrib ) > 0:
            self.elecid.channelDistribution = self.chanDistrib
            self.elecid.parseChanDistrib()
        self._loadChem( cfile, 'tempchem' )
        self.buildFromMemory( self.model.path + '/tempelec', self.model.path + '/tempchem' )

    ################################################################
    # Utility function to add a single spine to the given parent.

    # parent is parent compartment for this spine.
    # spineProto is just that.
    # pos is position (in metres ) along parent compartment
    # angle is angle (in radians) to rotate spine wrt x in plane xy.
    # Size is size scaling factor, 1 leaves as is.
    # x, y, z are unit vectors. Z is along the parent compt.
    # We first shift the spine over so that it is offset by the parent compt
    # diameter.
    # We then need to reorient the spine which lies along (i,0,0) to
    #   lie along x. X is a unit vector so this is done simply by
    #   multiplying each coord of the spine by x.
    # Finally we rotate the spine around the z axis by the specified angle
    # k is index of this spine.
    def _addSpine( self, parent, spineProto, pos, angle, x, y, z, size, k ):
        spine = moose.copy( spineProto, parent.parent, 'spine' + str(k) )
        kids = spine[0].children
        coords = []
        ppos = np.array( [parent.x0, parent.y0, parent.z0] )
        for i in kids:
            #print i.name, k
            j = i[0]
            j.name += str(k)
            #print 'j = ', j
            coords.append( [j.x0, j.y0, j.z0] )
            coords.append( [j.x, j.y, j.z] )
            self._scaleSpineCompt( j, size )
            moose.move( i, self.elecid )
        origin = coords[0]
        #print 'coords = ', coords
        # Offset it so shaft starts from surface of parent cylinder
        origin[0] -= parent.diameter / 2.0
        coords = np.array( coords )
        coords -= origin # place spine shaft base at origin.
        rot = np.array( [x, [0,0,0], [0,0,0]] )
        coords = np.dot( coords, rot )
        moose.delete( spine )
        moose.connect( parent, "raxial", kids[0], "axial" )
        self._reorientSpine( kids, coords, ppos, pos, size, angle, x, y, z )

    ################################################################
    ## The spineid is the parent object of the prototype spine. The
    ## spine prototype can include any number of compartments, and each
    ## can have any number of voltage and ligand-gated channels, as well
    ## as CaConc and other mechanisms.
    ## The parentList is a list of Object Ids for parent compartments for
    ## the new spines
    ## The spacingDistrib is the width of a normal distribution around
    ## the spacing. Both are in metre units.
    ## The reference angle of 0 radians is facing away from the soma.
    ## In all cases we assume that the spine will be rotated so that its
    ## axis is perpendicular to the axis of the dendrite.
    ## The simplest way to put the spine in any random position is to have
    ## an angleDistrib of 2 pi. The algorithm selects any angle in the
    ## linear range of the angle distrib to add to the specified angle.
    ## With each position along the dendrite the algorithm computes a new
    ## spine direction, using rotation to increment the angle.
    ################################################################
    def _decorateWithSpines( self ):
        args = []
        for i in self.addSpineList:
            if not moose.exists( '/library/' + i[0] ):
                print('Warning: _decorateWithSpines: spine proto ', i[0], ' not found.')
                continue
            s = ""
            for j in range( 9 ):
                s = s + str(i[j]) + ' '
            args.append( s )
        self.elecid.spineSpecification = args
        self.elecid.parseSpines()

    ################################################################

    def _loadElec( self, efile, elecname ):
        if ( efile[ len( efile ) - 2:] == ".p" ):
            self.elecid = moose.loadModel( efile, '/library/' + elecname)[0]
            print(self.elecid)
        elif ( efile[ len( efile ) - 4:] == ".swc" ):
            self.elecid = moose.loadModel( efile, '/library/' + elecname)[0]
        else:
            nm = NeuroML()
            print("in _loadElec, combineSegments = ", self.combineSegments)
            nm.readNeuroMLFromFile( efile, \
                    params = {'combineSegments': self.combineSegments, \
                    'createPotentialSynapses': True } )
            if moose.exists( '/cells' ):
                kids = moose.wildcardFind( '/cells/#' )
            else:
                kids = moose.wildcardFind( '/library/#[ISA=Neuron],/library/#[TYPE=Neutral]' )
                if ( kids[0].name == 'spine' ):
                    kids = kids[1:]

            assert( len( kids ) > 0 )
            self.elecid = kids[0]
            temp = moose.wildcardFind( self.elecid.path + '/#[ISA=CompartmentBase]' )

        transformNMDAR( self.elecid.path )
        kids = moose.wildcardFind( '/library/##[0]' )
        for i in kids:
            i.tick = -1


    #################################################################

    # This assumes that the chemid is located in self.parent.path+/chem
    # It moves the existing chem compartments into a NeuroMesh
    # For now this requires that we have a dend, a spine and a PSD,
    # with those names and volumes in decreasing order.
    def validateChem( self  ):
        cpath = self.chemid.path
        comptlist = moose.wildcardFind( cpath + '/#[ISA=ChemCompt]' )
        if len( comptlist ) == 0:
            raise BuildError( "validateChem: no compartment on: " + cpath )

        if len( comptlist ) == 1:
            return;

        # Sort comptlist in decreasing order of volume
        sortedComptlist = sorted( comptlist, key=lambda x: -x.volume )
        if ( len( sortedComptlist ) != 3 ):
            print(cpath, sortedComptlist)
            raise BuildError( "validateChem: Require 3 chem compartments, have: " + str( len( sortedComptlist ) ) )
        '''
        if not( sortedComptlist[0].name.lower() == 'dend' and \
            sortedComptlist[1].name.lower() == 'spine' and \
            sortedComptlist[2].name.lower() == 'psd' ):
            raise BuildError( "validateChem: Invalid compt names: require dend, spine and PSD.\nActual names = " \
                    + sortedComptList[0].name + ", " \
                    + sortedComptList[1].name + ", " \
                    + sortedComptList[2].name )
        '''

    #################################################################

    def _buildNeuroMesh( self ):
        comptlist = moose.wildcardFind( self.chemid.path + '/#[ISA=ChemCompt]' )
        sortedComptList = sorted( comptlist, key=lambda x: -x.volume )
        # A little juggling here to put the chem pathways onto new meshes.
        self.chemid.name = 'temp_chem'
        newChemid = moose.Neutral( self.model.path + '/chem' )
        self.dendCompt = moose.NeuroMesh( newChemid.path + '/dend' )
        self.dendCompt.geometryPolicy = 'cylinder'
        self.dendCompt.separateSpines = 0
        if len( sortedComptList ) == 3:
            self.dendCompt.separateSpines = 1
            self.spineCompt = moose.SpineMesh( newChemid.path + '/spine' )
            moose.connect( self.dendCompt, 'spineListOut', self.spineCompt, 'spineList' )
            self.psdCompt = moose.PsdMesh( newChemid.path + '/psd' )
            moose.connect( self.dendCompt, 'psdListOut', self.psdCompt, 'psdList','OneToOne')
        #Move the old reac systems onto the new compartments.
        self._moveCompt( sortedComptList[0], self.dendCompt )
        if len( sortedComptList ) == 3:
            self._moveCompt( sortedComptList[1], self.spineCompt )
            self._moveCompt( sortedComptList[2], self.psdCompt )
        self.dendCompt.diffLength = self.diffusionLength
        self.dendCompt.subTree = self.cellPortionElist
        moose.delete( self.chemid )
        self.chemid = newChemid

    #################################################################
    def _configureSolvers( self ) :
        if not hasattr( self, 'chemid' ):
            return
        if not hasattr( self, 'dendCompt' ):
            raise BuildError( "configureSolvers: no chem meshes defined." )
        dmksolve = moose.Ksolve( self.dendCompt.path + '/ksolve' )
        dmdsolve = moose.Dsolve( self.dendCompt.path + '/dsolve' )
        dmstoich = moose.Stoich( self.dendCompt.path + '/stoich' )
        dmstoich.compartment = self.dendCompt
        dmstoich.ksolve = dmksolve
        dmstoich.dsolve = dmdsolve
        dmstoich.path = self.dendCompt.path + "/##"
        # Below we have code that only applies if there are spines
        # Put in spine solvers. Note that these get info from the dendCompt
        if hasattr( self, 'spineCompt' ):
            if self.useGssa:
                smksolve = moose.Gsolve( self.spineCompt.path + '/ksolve' )
            else:
                smksolve = moose.Ksolve( self.spineCompt.path + '/ksolve' )
            smdsolve = moose.Dsolve( self.spineCompt.path + '/dsolve' )
            smstoich = moose.Stoich( self.spineCompt.path + '/stoich' )
            smstoich.compartment = self.spineCompt
            smstoich.ksolve = smksolve
            smstoich.dsolve = smdsolve
            smstoich.path = self.spineCompt.path + "/##"
            # Put in PSD solvers. Note that these get info from the dendCompt
            if self.useGssa:
                pmksolve = moose.Gsolve( self.psdCompt.path + '/ksolve' )
            else:
                pmksolve = moose.Ksolve( self.psdCompt.path + '/ksolve' )
            pmdsolve = moose.Dsolve( self.psdCompt.path + '/dsolve' )
            pmstoich = moose.Stoich( self.psdCompt.path + '/stoich' )
            pmstoich.compartment = self.psdCompt
            pmstoich.ksolve = pmksolve
            pmstoich.dsolve = pmdsolve
            pmstoich.path = self.psdCompt.path + "/##"

            # Put in cross-compartment diffusion between ksolvers
            dmdsolve.buildNeuroMeshJunctions( smdsolve, pmdsolve )
            # Put in cross-compartment reactions between ksolvers
            smstoich.buildXreacs( pmstoich )
            #pmstoich.buildXreacs( smstoich )
            smstoich.buildXreacs( dmstoich )
            dmstoich.filterXreacs()
            smstoich.filterXreacs()
            pmstoich.filterXreacs()

            # set up the connections so that the spine volume scaling can happen
            self.elecid.setSpineAndPsdMesh( self.spineCompt, self.psdCompt)
            self.elecid.setSpineAndPsdDsolve( smdsolve, pmdsolve )
    ################################################################

    def _loadChem( self, fname, chemName ):
        chem = moose.Neutral( '/library/' + chemName )
        modelId = moose.loadModel( fname, chem.path, 'ee' )
        comptlist = moose.wildcardFind( chem.path + '/#[ISA=ChemCompt]' )
        if len( comptlist ) == 0:
            print("loadChem: No compartment found in file: ", fname)
            return
        # Sort comptlist in decreasing order of volume
        sortedComptlist = sorted( comptlist, key=lambda x: -x.volume )
        if ( len( sortedComptlist ) != 3 ):
            print("loadChem: Require 3 chem compartments, have: ",\
                len( sortedComptlist ))
            return False
        sortedComptlist[0].name = 'dend'
        sortedComptlist[1].name = 'spine'
        sortedComptlist[2].name = 'psd'

    ################################################################

    def _moveCompt( self, a, b ):
        b.setVolumeNotRates( a.volume )
        for i in moose.wildcardFind( a.path + '/#' ):
            if ( i.name != 'mesh' ):
                moose.move( i, b )
        moose.delete( a )
    ################################################################
    def _buildAdaptor( self, meshName, elecRelPath, elecField, \
            chemRelPath, chemField, isElecToChem, offset, scale ):
        #print "offset = ", offset, ", scale = ", scale
        mesh = moose.element( '/model/chem/' + meshName )
        #elecComptList = mesh.elecComptList
        if elecRelPath == 'spine':
            elecComptList = moose.vec( mesh.elecComptList[0].path + '/../spine' )
        else:
            elecComptList = mesh.elecComptList

        '''
        for i in elecComptList:
            print i.diameter
        print len( elecComptList[0] )
        print elecComptList[0][0].parent.path
        print "--------------------------------------"
        spine = moose.vec( elecComptList[0].path + '/../spine' )
        for i in spine:
            print i.headDiameter

        moose.le( elecComptList[0][0].parent )
        '''
        if len( elecComptList ) == 0:
            raise BuildError( \
                "buildAdaptor: no elec compts in elecComptList on: " + \
                mesh.path )
        startVoxelInCompt = mesh.startVoxelInCompt
        endVoxelInCompt = mesh.endVoxelInCompt
        capField = elecField[0].capitalize() + elecField[1:]
        capChemField = chemField[0].capitalize() + chemField[1:]
        chemPath = mesh.path + '/' + chemRelPath
        if not( moose.exists( chemPath ) ):
            raise BuildError( \
                "Error: buildAdaptor: no chem obj in " + chemPath )
        chemObj = moose.element( chemPath )
        assert( chemObj.numData >= len( elecComptList ) )
        adName = '/adapt'
        for i in range( 1, len( elecRelPath ) ):
            if ( elecRelPath[-i] == '/' ):
                adName += elecRelPath[1-i]
                break
        ad = moose.Adaptor( chemObj.path + adName, len( elecComptList ) )
        #print 'building ', len( elecComptList ), 'adaptors ', adName, ' for: ', mesh.name, elecRelPath, elecField, chemRelPath
        av = ad.vec
        chemVec = moose.element( mesh.path + '/' + chemRelPath ).vec

        for i in zip( elecComptList, startVoxelInCompt, endVoxelInCompt, av ):
            i[3].inputOffset = 0.0
            i[3].outputOffset = offset
            i[3].scale = scale
            if elecRelPath == 'spine':
                elObj = i[0]
            else:
                ePath = i[0].path + '/' + elecRelPath
                if not( moose.exists( ePath ) ):
                    raise BuildError( \
                        "Error: buildAdaptor: no elec obj in " + ePath )
                elObj = moose.element( i[0].path + '/' + elecRelPath )
            if ( isElecToChem ):
                elecFieldSrc = 'get' + capField
                chemFieldDest = 'set' + capChemField
                #print ePath, elecFieldSrc, scale
                moose.connect( i[3], 'requestOut', elObj, elecFieldSrc )
                for j in range( i[1], i[2] ):
                    moose.connect( i[3], 'output', chemVec[j],chemFieldDest)
            else:
                chemFieldSrc = 'get' + capChemField
                elecFieldDest = 'set' + capField
                for j in range( i[1], i[2] ):
                    moose.connect( i[3], 'requestOut', chemVec[j], chemFieldSrc)
                msg = moose.connect( i[3], 'output', elObj, elecFieldDest )

