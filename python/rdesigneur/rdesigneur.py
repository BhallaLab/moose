# -*- coding: utf-8 -*-
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
from __future__ import print_function, absolute_import 

import imp
import os
import moose
import numpy as np
import math
import sys
import time
import matplotlib.pyplot as plt

import rdesigneur.rmoogli as rmoogli
from rdesigneur.rdesigneurProtos import *
import moose.fixXreacs as fixXreacs

from moose.neuroml.NeuroML import NeuroML
from moose.neuroml.ChannelML import ChannelML

# In python3, cElementTree is deprecated. We do not plan to support python <2.7
# in future, so other imports have been removed.
try:
  from lxml import etree
except ImportError:
  import xml.etree.ElementTree as etree

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
            verbose = True,
            benchmark = False,
            addSomaChemCompt = False,  # Put a soma chemCompt on neuroMesh
            addEndoChemCompt = False,  # Put an endo compartment, typically for ER, on each of the NeuroMesh compartments.
            diffusionLength= 2e-6,
            meshLambda = -1.0,    #This is a backward compatibility hack
            temperature = 32,
            chemDt= 0.1,            # Much finer than MOOSE, for multiscale
            diffDt= 0.01,           # 10x finer than MOOSE, for multiscale
            elecDt= 50e-6,          # Same default as from MOOSE
            chemPlotDt = 1.0,       # Same default as from MOOSE
            elecPlotDt = 0.1e-3,    # Same default as from MOOSE
            funcDt = 0.1e-3,        # Used when turnOffElec is False.
                                    # Otherwise system uses chemDt.
            numWaveFrames = 100,    # Number of frames to use for waveplots
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
            plotList = [],  # elecpath, geom_expr, object, field, title ['wave' [min max]]
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
        self.verbose = verbose
        self.benchmark = benchmark
        self.addSomaChemCompt = addSomaChemCompt
        self.addEndoChemCompt = addEndoChemCompt
        self.diffusionLength= diffusionLength
        if meshLambda > 0.0:
            print("Warning: meshLambda argument is deprecated. Please use 'diffusionLength' instead.\nFor now rdesigneur will accept this argument.")
            self.diffusionLength = meshLambda
        self.temperature = temperature
        self.chemDt= chemDt
        self.diffDt= diffDt
        self.elecDt= elecDt
        self.elecPlotDt= elecPlotDt
        self.funcDt= funcDt
        self.chemPlotDt= chemPlotDt
        self.numWaveFrames = numWaveFrames

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
        try:
            self.stimList = [ rstim.convertArg(i) for i in stimList ]
            self.plotList = [ rplot.convertArg(i) for i in plotList ]
            self.moogList = [ rmoog.convertArg(i) for i in moogList ]
        except BuildError as msg:
            print("Error: rdesigneur: " + msg)
            quit()

        #self.saveList = plotList                    #ADDED BY Sarthak
        self.saveAs = []
        self.plotNames = []
        self.wavePlotNames = []
        self.saveNames = []
        self.moogNames = []
        self.cellPortionElist = []
        self.spineComptElist = []
        self.tabForXML = []
        self._endos = []

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
        if not self.verbose:
            return
        print("\nRdesigneur: Elec model has",
            self.elecid.numCompartments, "compartments and",
            self.elecid.numSpines, "spines on",
            len( self.cellPortionElist ), "compartments.")
        if hasattr( self , 'chemid' ):
            dmstoich = moose.element( self.dendCompt.path + '/stoich' )
            print("\tChem part of model has the following compartments: ")
            for j in moose.wildcardFind( '/model/chem/##[ISA=ChemCompt]'):
                s = moose.element( j.path + '/stoich' )
                print( "\t | In {}, {} voxels X {} pools".format( j.name, j.mesh.num, s.numAllPools ) )

    def buildModel( self, modelPath = '/model' ):
        if moose.exists( modelPath ):
            print("rdesigneur::buildModel: Build failed. Model '",
                modelPath, "' already exists.")
            return
        self.model = moose.Neutral( modelPath )
        self.modelPath = modelPath
        funcs = [ self.installCellFromProtos, self.buildPassiveDistrib
            , self.buildChanDistrib, self.buildSpineDistrib, self.buildChemDistrib
            , self._configureSolvers, self.buildAdaptors, self._buildStims
            , self._buildPlots, self._buildMoogli, self._configureHSolve
            , self._configureClocks, self._printModelStats 
            ]
        for i, _func in enumerate( funcs ):
            if self.benchmark:
                print( " + (%d/%d) executing %25s"%(i, len(funcs), _func.__name__), end=' ' )
            t0 = time.time()
            try:
                _func( )
            except BuildError as msg:
                print("Error: rdesigneur: model build failed:", msg)
                moose.delete( self.model )
                return
            t = time.time() - t0
            if self.benchmark:
                msg = r'    ... DONE'
                if t > 1:
                    msg += ' %.3f sec' % t
                print( msg )
            sys.stdout.flush()

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

        # . can be in path name as well. Find the last dot which is most likely
        # to be the function name.
        modPos = func.rfind( "." )
        if ( modPos != -1 ): # Function is in a file, load and check
            resolvedPath = os.path.realpath( func[0:modPos] )
            pathTokens = resolvedPath.split('/')
            pathTokens = ['/'] + pathTokens
            modulePath = os.path.realpath(os.path.join(*pathTokens[:-1]))
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
                    getattr( moose, protoName )( '/library/' + protoVec[1] )
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
                if self.verbose:
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
        # cellProtoList args:
        # Option 1: zero args: make standard soma, len and dia 500 um.
        # Option 2: [name, library_proto_name]: uses library proto
        # Option 3: [fname.suffix, cellname ]: Loads cell from file
        # Option 4: [moose<Classname>, cellname]: Makes proto of class
        # Option 5: [funcname, cellname]: Calls named function with specified name of cell to be made.
        # Option 6: [path, cellname]: Copies path to library as proto
        # Option 7: [libraryName, cellname]: Renames library entry as proto
        # Below two options only need the first two args, rest are optional
        # Defailt values are given.
        # Option 8: [somaProto,name, somaDia=5e-4, somaLen=5e-4]
        # Option 9: [ballAndStick,name, somaDia=10e-6, somaLen=10e-6, 
        #       dendDia=4e-6, dendLen=200e-6, numDendSeg=1]
        if len( self.cellProtoList ) == 0:
            ''' Make HH squid model sized compartment:
            len and dia 500 microns. CM = 0.01 F/m^2, RA =
            '''
            self.elecid = makePassiveHHsoma( name = 'cell' )
            assert( moose.exists( '/library/cell/soma' ) )
            self.soma = moose.element( '/library/cell/soma' )
            return

            '''
            self.elecid = moose.Neuron( '/library/cell' )
            dia = 500e-6
            self.soma = buildCompt( self.elecid, 'soma', dia, dia, 0.0,
                0.33333333, 3000, 0.01 )
            self.soma.initVm = -65e-3 # Resting of -65, from HH
            self.soma.Em = -54.4e-3 # 10.6 mV above resting of -65, from HH
            '''

        for i in self.cellProtoList:
            if i[0] == 'somaProto':
                self._buildElecSoma( i )
            elif i[0] == 'ballAndStick':
                self._buildElecBallAndStick( i )
            elif self.checkAndBuildProto( "cell", i, \
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
                raise BuildError( "chanProto: bad filename:" + name )
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
    def _buildElecSoma( self, args ):
        parms = [ 'somaProto', 'soma', 5e-4, 5e-4 ] # somaDia, somaLen
        for i in range( len(args) ):
            parms[i] = args[i]
        cell = moose.Neuron( '/library/' + parms[1] )
        buildCompt( cell, 'soma', dia = parms[2], dx = parms[3] )
        self.elecid = cell
        return cell
        
    ################################################################
    def _buildElecBallAndStick( self, args ):
        parms = [ 'ballAndStick', 'cell', 10e-6, 10e-6, 4e-6, 200e-6, 1 ] # somaDia, somaLen, dendDia, dendLen, dendNumSeg
        for i in range( len(args) ):
            parms[i] = args[i]
        if parms[6] <= 0:
            return self.buildElecSoma( parms[:4] )
        cell = moose.Neuron( '/library/' + parms[1] )
        prev = buildCompt( cell, 'soma', dia = args[2], dx = args[3] )
        dx = parms[5]/parms[6]
        x = prev.x
        for i in range( parms[6] ):
            compt = buildCompt( cell, 'dend' + str(i), x = x, dx = dx, dia = args[4] )
            moose.connect( prev, 'axial', compt, 'raxial' )
            prev = compt
            x += dx
        self.elecid = cell
        return cell

    ################################################################
    def _buildVclampOnCompt( self, dendCompts, spineCompts, stimInfo ):
        # stimInfo = [path, geomExpr, relPath, field, expr_string]
        stimObj = []
        for i in dendCompts + spineCompts:
            vclamp = make_vclamp( name = 'vclamp', parent = i.path )
            moose.connect( i, 'VmOut', vclamp, 'sensedIn' )
            moose.connect( vclamp, 'currentOut', i, 'injectMsg' )
            stimObj.append( vclamp )

        return stimObj

    def _buildSynInputOnCompt( self, dendCompts, spineCompts, stimInfo, doPeriodic = False ):
        # stimInfo = [path, geomExpr, relPath, field, expr_string]
        # Here we hack geomExpr to use it for the syn weight. We assume it
        # is just a number. In due course
        # it should be possible to actually evaluate it according to geom.
        synWeight = float( stimInfo.geom_expr )
        stimObj = []
        for i in dendCompts + spineCompts:
            path = i.path + '/' + stimInfo.relpath + '/sh/synapse[0]'
            if moose.exists( path ):
                synInput = make_synInput( name='synInput', parent=path )
                synInput.doPeriodic = doPeriodic
                moose.element(path).weight = synWeight
                moose.connect( synInput, 'spikeOut', path, 'addSpike' )
                stimObj.append( synInput )
        return stimObj
        
    ################################################################
    # Here we set up the distributions
    ################################################################
    def buildPassiveDistrib( self ):
	# [path field expr [field expr]...]
        # RM, RA, CM set specific values, per unit area etc.
        # Rm, Ra, Cm set absolute values.
        # Also does Em, Ek, initVm
	# Expression can use p, g, L, len, dia, maxP, maxG, maxL.
        temp = []
        for i in self.passiveDistrib:
            # Handle legacy format of ['.', path, field, expr [field expr]]
            if (len( i ) < 3) or (i[0] != '.' and len(i) %2 != 1):
                raise BuildError( "buildPassiveDistrib: Need 3 + N*2 arguments as (path field expr [field expr]...), have {}".format( len(i) ) )

            if not(( len(i) % 2 ) != 1 and i[0] == '.' ):
                temp.append( '.' )
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
        # Orig format [chem, elecPath, install, expr]
        #   where chem and install were not being used.
        # Modified format [chemLibPath, elecPath, newChemName, expr]
        # chemLibPath is name of chemCompt or even group on library
        # If chemLibPath has multiple compts on it, then the smaller ones
        # become endoMeshes, scaled as per original.
        # As a backward compatibility hack, if the newChemName == 'install'
        # we use the default naming.
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
            #print( 'LEN SPINECOMPTELIST =' + str( pair ) + ", " str( len( self.spineComptElist ) ) )
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

    # Utility function for doing lookups for objects.
    def _makeUniqueNameStr( self, obj ):
        # second one is faster than the former. 140 ns v/s 180 ns.
        #  return obj.name + " " + str( obj.index )
        return "%s %s" % (obj.name, obj.index)

    # Returns vector of source objects, and the field to use.
    # plotSpec is of the form
    #   [ region_wildcard, region_expr, path, field, title]
    def _parseComptField( self, comptList, plotSpec, knownFields ):
        # Put in stuff to go through fields if the target is a chem object
        field = plotSpec.field
        if not field in knownFields:
            print("Warning: Rdesigneur::_parseComptField: Unknown field '{}'".format( field ) )
            return (), ""

        kf = knownFields[field] # Find the field to decide type.
        if kf[0] in ['CaConcBase', 'ChanBase', 'NMDAChan', 'VClamp']:
            objList = self._collapseElistToPathAndClass( comptList, plotSpec.relpath, kf[0] )
            return objList, kf[1]
        elif field in [ 'n', 'conc', 'volume']:
            path = plotSpec.relpath
            pos = path.find( '/' )
            if pos == -1:   # Assume it is in the dend compartment.
                path  = 'dend/' + path
            pos = path.find( '/' )
            chemCompt = path[:pos]
            if chemCompt[-5:] == "_endo":
                chemCompt = chemCompt[0:-5]
            cc = moose.element( self.modelPath + '/chem/' + chemCompt)
            voxelVec = []
            temp = [ self._makeUniqueNameStr( i ) for i in comptList ]
            #print( temp )
            #print( "#####################" )
            comptSet = set( temp )
            #em = [ moose.element(i) for i in cc.elecComptMap ]
            em = sorted( [ self._makeUniqueNameStr(i[0]) for i in cc.elecComptMap ] )
            #print( em )
            #print( "=================================================" )

            voxelVec = [i for i in range(len( em ) ) if em[i] in comptSet ]
            # Here we collapse the voxelVec into objects to plot.
            allObj = moose.vec( self.modelPath + '/chem/' + plotSpec.relpath )
            #print "####### allObj=", self.modelPath + '/chem/' + plotSpec[2]
            if len( allObj ) >= len( voxelVec ):
                objList = [ allObj[int(j)] for j in voxelVec]
            else:
                objList = []
                print( "Warning: Rdesigneur::_parseComptField: unknown Object: '", plotSpec.relpath, "'" )
            #print "############", chemCompt, len(objList), kf[1]
            return objList, kf[1]

        else:
            return comptList, kf[1]


    def _buildPlots( self ):
        knownFields = {
            'Vm':('CompartmentBase', 'getVm', 1000, 'Memb. Potential (mV)' ),
            'spikeTime':('CompartmentBase', 'getVm', 1, 'Spike Times (s)'),
            'Im':('CompartmentBase', 'getIm', 1e9, 'Memb. current (nA)' ),
            'Cm':('CompartmentBase', 'getCm', 1e12, 'Memb. capacitance (pF)' ),
            'Rm':('CompartmentBase', 'getRm', 1e-9, 'Memb. Res (GOhm)' ),
            'Ra':('CompartmentBase', 'getRa', 1e-6, 'Axial Res (MOhm)' ),
            'inject':('CompartmentBase', 'getInject', 1e9, 'inject current (nA)' ),
            'Gbar':('ChanBase', 'getGbar', 1e9, 'chan max conductance (nS)' ),
            'modulation':('ChanBase', 'getModulation', 1, 'chan modulation (unitless)' ),
            'Gk':('ChanBase', 'getGk', 1e9, 'chan conductance (nS)' ),
            'Ik':('ChanBase', 'getIk', 1e9, 'chan current (nA)' ),
            'ICa':('NMDAChan', 'getICa', 1e9, 'Ca current (nA)' ),
            'Ca':('CaConcBase', 'getCa', 1e3, 'Ca conc (uM)' ),
            'n':('PoolBase', 'getN', 1, '# of molecules'),
            'conc':('PoolBase', 'getConc', 1000, 'Concentration (uM)' ),
            'volume':('PoolBase', 'getVolume', 1e18, 'Volume (um^3)' ),
            'current':('VClamp', 'getCurrent', 1e9, 'Holding Current (nA)')
        }
        graphs = moose.Neutral( self.modelPath + '/graphs' )
        dummy = moose.element( '/' )
        k = 0
        for i in self.plotList:
            pair = i.elecpath + ' ' + i.geom_expr
            dendCompts = self.elecid.compartmentsFromExpression[ pair ]
            spineCompts = self.elecid.spinesFromExpression[ pair ]
            plotObj, plotField = self._parseComptField( dendCompts, i, knownFields )
            plotObj2, plotField2 = self._parseComptField( spineCompts, i, knownFields )
            assert( plotField == plotField2 )
            plotObj3 = plotObj + plotObj2
            numPlots = sum( q != dummy for q in plotObj3 )
            #print( "PlotList: {0}: numobj={1}, field ={2}, nd={3}, ns={4}".format( pair, numPlots, plotField, len( dendCompts ), len( spineCompts ) ) )
            if numPlots > 0:
                tabname = graphs.path + '/plot' + str(k)
                scale = knownFields[i.field][2]
                units = knownFields[i.field][3]
                if i.mode == 'wave':
                    self.wavePlotNames.append( [ tabname, i.title, k, scale, units, i ] )
                else:
                    self.plotNames.append( [ tabname, i.title, k, scale, units, i.field, i.ymin, i.ymax ] )
                if len( i.saveFile ) > 4 and i.saveFile[-4] == '.xml' or i.saveFile:
                    self.saveNames.append( [ tabname, len(self.saveNames), scale, units, i ] )

                k += 1
                if i.field == 'n' or i.field == 'conc' or i.field == 'volume' or i.field == 'Gbar':
                    tabs = moose.Table2( tabname, numPlots )
                else:
                    tabs = moose.Table( tabname, numPlots )
                    if i.field == 'spikeTime':
                        tabs.vec.threshold = -0.02 # Threshold for classifying Vm as a spike.
                        tabs.vec.useSpikeMode = True # spike detect mode on

            vtabs = moose.vec( tabs )
            q = 0
            for p in [ x for x in plotObj3 if x != dummy ]:
                moose.connect( vtabs[q], 'requestOut', p, plotField )
                q += 1

    def _buildMoogli( self ):
        knownFields = {
            'Vm':('CompartmentBase', 'getVm', 1000, 'Memb. Potential (mV)', -80.0, 40.0 ),
            'initVm':('CompartmentBase', 'getInitVm', 1000, 'Init. Memb. Potl (mV)', -80.0, 40.0 ),
            'Im':('CompartmentBase', 'getIm', 1e9, 'Memb. current (nA)', -10.0, 10.0 ),
            'inject':('CompartmentBase', 'getInject', 1e9, 'inject current (nA)', -10.0, 10.0 ),
            'Gbar':('ChanBase', 'getGbar', 1e9, 'chan max conductance (nS)', 0.0, 1.0 ),
            'Gk':('ChanBase', 'getGk', 1e9, 'chan conductance (nS)', 0.0, 1.0 ),
            'Ik':('ChanBase', 'getIk', 1e9, 'chan current (nA)', -10.0, 10.0 ),
            'ICa':('NMDAChan', 'getICa', 1e9, 'Ca current (nA)', -10.0, 10.0 ),
            'Ca':('CaConcBase', 'getCa', 1e3, 'Ca conc (uM)', 0.0, 10.0 ),
            'n':('PoolBase', 'getN', 1, '# of molecules', 0.0, 200.0 ),
            'conc':('PoolBase', 'getConc', 1000, 'Concentration (uM)', 0.0, 2.0 ),
            'volume':('PoolBase', 'getVolume', 1e18, 'Volume (um^3)' )
        }
        moogliBase = moose.Neutral( self.modelPath + '/moogli' )
        k = 0
        for i in self.moogList:
            kf = knownFields[i.field]
            pair = i.elecpath + " " + i.geom_expr
            dendCompts = self.elecid.compartmentsFromExpression[ pair ]
            spineCompts = self.elecid.spinesFromExpression[ pair ]
            dendObj, mooField = self._parseComptField( dendCompts, i, knownFields )
            spineObj, mooField2 = self._parseComptField( spineCompts, i, knownFields )
            assert( mooField == mooField2 )
            mooObj3 = dendObj + spineObj
            numMoogli = len( mooObj3 )
            self.moogNames.append( rmoogli.makeMoogli( self, mooObj3, i, kf ) )


    ################################################################
    # Here we display the plots and moogli
    ################################################################

    def displayMoogli( self, moogliDt, runtime, rotation = math.pi/500.0, fullscreen = False, block = True, azim = 0.0, elev = 0.0 ):
        rmoogli.displayMoogli( self, moogliDt, runtime, rotation = rotation, fullscreen = fullscreen, azim = azim, elev = elev )
        pr = moose.PyRun( '/model/updateMoogli' )

        pr.runString = '''
import rdesigneur.rmoogli
rdesigneur.rmoogli.updateMoogliViewer()
'''
        moose.setClock( pr.tick, moogliDt )
        moose.reinit()
        moose.start( runtime )
        if block:
            self.display( len( self.moogNames ) + 1 )

    def display( self, startIndex = 0 ):
        for i in self.plotNames:
            plt.figure( i[2] + startIndex )
            plt.title( i[1] )
            plt.xlabel( "Time (s)" )
            plt.ylabel( i[4] )
            vtab = moose.vec( i[0] )
            if i[5] == 'spikeTime':
                k = 0
                tmax = moose.element( '/clock' ).currentTime
                for j in vtab: # Plot a raster
                    y = [k] * len( j.vector )
                    plt.plot( j.vector * i[3], y, linestyle = 'None', marker = '.', markersize = 10 )
                    plt.xlim( 0, tmax )
                
            else:
                t = np.arange( 0, vtab[0].vector.size, 1 ) * vtab[0].dt
                for j in vtab:
                    plt.plot( t, j.vector * i[3] )
        if len( self.moogList ) or len( self.wavePlotNames ) > 0:
            plt.ion()
        # Here we build the plots and lines for the waveplots
        self.initWavePlots( startIndex )
        if len( self.wavePlotNames ) > 0:
            for i in range( 3 ):
                self.displayWavePlots()
        plt.show( block=True )
        self._save()                                            
        

    def initWavePlots( self, startIndex ):
        self.frameDt = moose.element( '/clock' ).currentTime/self.numWaveFrames
        for wpn in range( len(self.wavePlotNames) ):
            i = self.wavePlotNames[wpn]
            vtab = moose.vec( i[0] )
            if len( vtab ) < 2:
                print( "Warning: Waveplot {} abandoned, only {} points".format( i[1], len( vtab ) ) )
                continue
            dFrame = len( vtab[0].vector ) / self.numWaveFrames
            if dFrame < 1:
                dFrame = 1
            vpts = np.array( [ [k.vector[j] for j in range( 0, len( k.vector ), dFrame ) ] for k in vtab] ).T * i[3]
            fig = plt.figure( i[2] + startIndex )
            ax = fig.add_subplot( 111 )
            plt.title( i[1] )
            plt.xlabel( "position (voxels)" )
            plt.ylabel( i[4] )
            plotinfo = i[5]
            if plotinfo.ymin == plotinfo.ymax:
                mn = np.min(vpts)
                mx = np.max(vpts)
                if mn/mx < 0.3:
                    mn = 0
            else:
                mn = plotinfo.ymin
                mx = plotinfo.ymax
            ax.set_ylim( mn, mx )
            line, = plt.plot( range( len( vtab ) ), vpts[0] )
            timeLabel = plt.text( len(vtab ) * 0.05, mn + 0.9*(mx-mn), 'time = 0' )
            self.wavePlotNames[wpn].append( [fig, line, vpts, timeLabel] )
            fig.canvas.draw()

    def displayWavePlots( self ):
        for f in range( self.numWaveFrames ):
            for i in self.wavePlotNames:
                wp = i[6]
                if len( wp[2] ) > f:
                    wp[1].set_ydata( wp[2][f] )
                    wp[3].set_text( "time = {:.1f}".format(f*self.frameDt) )
                    wp[0].canvas.draw()
            #plt.pause(0.001)
        
        #This calls the _save function which saves only if the filenames have been specified

    ################################################################
    # Here we get the time-series data and write to various formats
    ################################################################
    #[TO DO] Add NSDF output function
    '''
    The original author of the functions -- [_savePlots(), _writeXML(), _writeCSV(), _save()] is
    Sarthak Sharma.
    Email address: sarthaks442@gmail.com
    Heavily modified by U.S. Bhalla
    '''
    def _writeXML( self, plotData, time, vtab ): 
        tabname = plotData[0]
        idx = plotData[1]
        scale = plotData[2]
        units = plotData[3]
        rp = plotData[4]
        filename = rp.saveFile[:-4] + str(idx) + '.xml'
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
        title.set( 'title', rp.title)
        title.set( 'field', rp.field)
        title.set( 'scale', str(scale) )
        title.set( 'units', units)
        title.set( 'dt', str(vtab[0].dt) )
        res = rp.saveResolution
        p = []
        for t, v in zip( time, vtab ):
            p.append( etree.SubElement( title, "data"))
            p[-1].set( 'path', v.path )
            p[-1].text = ''.join( str(round(y,res)) + ' ' for y in v.vector )
        tree = etree.ElementTree(root)
        tree.write(filename)

    def _writeCSV( self, plotData, time, vtab ): 
        tabname = plotData[0]
        idx = plotData[1]
        scale = plotData[2]
        units = plotData[3]
        rp = plotData[4]
        filename = rp.saveFile[:-4] + str(idx) + '.csv'

        header = ["time",]
        valMatrix = [time,]
        header.extend( [ v.path for v in vtab ] )
        valMatrix.extend( [ v.vector for v in vtab ] )
        nv = np.array( valMatrix ).T
        with open(filename, 'wb') as f:
            writer = csv.writer(f, quoting=csv.QUOTE_MINIMAL)
            writer.writerow(header)
            for row in nv:
                writer.writerow(row)

    ##########****SAVING*****###############


    def _save( self ):
        for i in self.saveNames:
            tabname = i[0]
            idx = i[1]
            scale = i[2]
            units = i[3]
            rp = i[4] # The rplot data structure, it has the setup info.

            vtab = moose.vec( tabname )
            t = np.arange( 0, vtab[0].vector.size, 1 ) * vtab[0].dt
            ftype = rp.filename[-4:]
            if ftype == '.xml':
                self._writeXML( i, t, vtab )
            elif ftype == '.csv':
                self._writeCSV( i, t, vtab )
            else:
                print("Save format '{}' not known, please use .csv or .xml".format( ftype ) )

    ################################################################
    # Here we set up the stims
    ################################################################
    def _buildStims( self ):
        knownFields = {
                'inject':('CompartmentBase', 'setInject'),
                'Ca':('CaConcBase', 'setCa'),
                'n':('PoolBase', 'setN'),
                'conc':('PoolBase', 'setConc'),
                'nInit':('PoolBase', 'setNinit'),
                'concInit':('PoolBase', 'setConcInit'),
                'vclamp':('CompartmentBase', 'setInject'),
                'randsyn':('SynChan', 'addSpike'),
                'periodicsyn':('SynChan', 'addSpike')
        }
        stims = moose.Neutral( self.modelPath + '/stims' )
        k = 0
        # Stimlist = [path, geomExpr, relPath, field, expr_string]
        for i in self.stimList:
            pair = i.elecpath + " " + i.geom_expr
            dendCompts = self.elecid.compartmentsFromExpression[ pair ]
            spineCompts = self.elecid.spinesFromExpression[ pair ]
            #print( "pair = {}, numcompts = {},{} ".format( pair, len( dendCompts), len( spineCompts ) ) )
            if i.field == 'vclamp':
                stimObj3 = self._buildVclampOnCompt( dendCompts, spineCompts, i )
                stimField = 'commandIn'
            elif i.field == 'randsyn':
                stimObj3 = self._buildSynInputOnCompt( dendCompts, spineCompts, i )
                stimField = 'setRate'
            elif i.field == 'periodicsyn':
                stimObj3 = self._buildSynInputOnCompt( dendCompts, spineCompts, i, doPeriodic = True )
                stimField = 'setRate'
            else:
                stimObj, stimField = self._parseComptField( dendCompts, i, knownFields )
                stimObj2, stimField2 = self._parseComptField( spineCompts, i, knownFields )
                assert( stimField == stimField2 )
                stimObj3 = stimObj + stimObj2
            numStim = len( stimObj3 )
            if numStim > 0:
                funcname = stims.path + '/stim' + str(k)
                k += 1
                func = moose.Function( funcname )
                func.expr = i.expr
                #if i[3] == 'vclamp': # Hack to clean up initial condition
                func.doEvalAtReinit = 1
                for q in stimObj3:
                    moose.connect( func, 'valueOut', q, stimField )

    ################################################################
    def _configureHSolve( self ):
        if not self.turnOffElec:
            hsolve = moose.HSolve( self.elecid.path + '/hsolve' )
            hsolve.dt = self.elecDt
            hsolve.target = self.soma.path

# Utility function for setting up clocks.
    def _configureClocks( self ):
        if self.turnOffElec:
            elecDt = 1e6
            elecPlotDt = 1e6
        else:
            elecDt = self.elecDt
            elecPlotDt = self.elecPlotDt
        diffDt = self.diffDt
        chemDt = self.chemDt
        for i in range( 0, 9 ):     # Assign elec family of clocks
            moose.setClock( i, elecDt )
        moose.setClock( 8, elecPlotDt ) 
        moose.setClock( 10, diffDt )# Assign diffusion clock.
        for i in range( 11, 18 ):   # Assign the chem family of clocks.
            moose.setClock( i, chemDt )
        if not self.turnOffElec:    # Assign the Function clock
            moose.setClock( 12, self.funcDt )
        moose.setClock( 18, self.chemPlotDt )
        sys.stdout.flush()
    ################################################################
    ################################################################
    ################################################################

    def validateFromMemory( self, epath, cpath ):
        return self.validateChem()

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
            #  print(i)
            assert len(i) >= 8
            self._buildAdaptor( i[0],i[1],i[2],i[3],i[4],i[5],i[6],i[7] )

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

        return True

        '''
        if len( comptlist ) == 1:
            return;

        # Sort comptlist in decreasing order of volume
        sortedComptlist = sorted( comptlist, key=lambda x: -x.volume )
        if ( len( sortedComptlist ) != 3 ):
            print(cpath, sortedComptlist)
            raise BuildError( "validateChem: Require 3 chem compartments, have: " + str( len( sortedComptlist ) ) )
        '''
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

    def _isModelFromKkit( self ):
        for i in self.chemProtoList:
            if i[0][-2:] == ".g":
                return True
        return False

    def _assignComptNamesFromKkit( self ):
        '''
        Algorithm: Identify compts by volume. Assume a couple of standard
        orders depending on the addSomaChemCompt and addEndoChemCompt
        flags:\n
        ascc = 0, aecc = 0: dend, spine, psd.\n
        ascc = 0, aecc = 1: dend, dend_endo, spine, spine_endo, psd, psd_endo.\n
        ascc = 1, aecc = 0: soma, dend, spine, psd.\n
        ascc = 1, aecc = 1: soma, soma_endo, dend, dend_endo, spine, spine_endo, psd, psd_endo.\n
        In all cases, a shorter list of chem compartments will only fill
        up the list to the available length.\n
        soma_endo can be thought of as nucleus.\n
        psd_endo doesn't really make sense, as peri-synaptic region really
        needs to talk both to PSD and to spine bulk.
        '''
        comptList = moose.wildcardFind( self.chemid.path + '/#[ISA=ChemCompt]' )
        #print( "comptList = {}".format ( [i.name for i in comptList]) )
        if len( comptList ) < 2:
            if comptList[0].name != 'dend':
                comptList[0].name = 'dend'
            return comptList
        if not self._isModelFromKkit():
            print( "Not isModelfromKkit" )
            return comptList
        sortedComptList = sorted( comptList, key=lambda x: -x.volume )
        if self.addSomaChemCompt:
            if self.addEndoChemCompt:
                sortedNames = ["soma", "soma_endo", "dend", "dend_endo", "spine", "spine_endo", "psd", "psd_endo", ]
            else:
                sortedNames = ["soma", "dend", "spine","psd"]
        else:
            if self.addEndoChemCompt:
                sortedNames = ["dend", "dend_endo", "spine", "spine_endo", "psd", "psd_endo", ]
            else:
                sortedNames = ["dend", "spine","psd"]

        #print( "sortedNames = {}".format( sortedNames ) )
        for i in range(min( len( sortedComptList ), len( sortedNames ) ) ):
            #print( "SortedClist= {}".format( sortedComptList[i] ))
            if sortedComptList[i].name != sortedNames[i]:
                sortedComptList[i].name = sortedNames[i]
        return sortedComptList


    def _buildNeuroMesh( self ):
        # Address the following cases:
        #   - dend alone
        #   - dend with spine and PSD
        #   - above plus n endo meshes located as per name suffix: 
        #       er_1_dend, vesicle_2_spine, 
        #   - above plus presyn, e.g.:
        #       er_1_dend, vesicle_2_spine, 
        #   - above plus soma
        #   - soma alone
        #   - soma plus n endo meshes

        self.chemid.name = 'temp_chem'
        newChemid = moose.Neutral( self.model.path + '/chem' )
        comptlist = self._assignComptNamesFromKkit()
        comptdict = { i.name:i for i in comptlist }
        if len(comptdict) == 1 or 'dend' in comptdict:
            self.dendCompt = moose.NeuroMesh( newChemid.path + '/dend' )
            self.dendCompt.geometryPolicy = 'cylinder'
            self.dendCompt.separateSpines = 0
            self._moveCompt( comptdict['dend'], self.dendCompt )
            comptdict['dend'] = self.dendCompt

        if 'dend' in comptdict and 'spine' in comptdict:
            #print( "comptdict = {}".format (comptdict ) )
            self.dendCompt.separateSpines = 1
            self.spineCompt = moose.SpineMesh( newChemid.path + '/spine' )
            moose.connect( self.dendCompt, 'spineListOut', self.spineCompt, 'spineList' )
            self._moveCompt( comptdict['spine'], self.spineCompt )
            comptdict['spine'] = self.spineCompt
            # We need to make a PSD in the spine even if it is uninhabited.
            self.psdCompt = moose.PsdMesh( newChemid.path + '/psd' )
            moose.connect( self.dendCompt, 'psdListOut', self.psdCompt, 'psdList','OneToOne')
            if 'psd' in comptdict: # Shift stuff over if any.
                self._moveCompt( comptdict['psd'], self.psdCompt )
                comptdict['psd'] = self.psdCompt

        self.dendCompt.diffLength = self.diffusionLength
        self.dendCompt.subTree = self.cellPortionElist
        for i in comptdict:
            if len(i) > 5:
                if i[-5:] == '_endo':
                    endo = moose.EndoMesh( newChemid.path + '/' +i )
                    surround = comptdict[i[0:-5]]
                    self._endos.append( [endo, surround] )
                    #print( "{}****{}".format(i[0:-5], comptdict[i[0:-5]]))
                    self._moveCompt( comptdict[i], endo )
                    comptdict[i] = endo
        moose.delete( self.chemid )
        self.chemid = newChemid


    #################################################################
    def _configureSolvers( self ) :
        if not hasattr( self, 'chemid' ):
            return
        if not hasattr( self, 'dendCompt' ):
            raise BuildError( "configureSolvers: no chem meshes defined." )
        fixXreacs.fixXreacs( self.chemid.path )
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
            if len( moose.wildcardFind( 'self.psdCompt.path/##[ISA=PoolBase]' ) ) == 0:
                moose.Pool( self.psdCompt.path + '/dummy' )
            pmstoich.path = self.psdCompt.path + "/##"

            # Here we should test what kind of geom we have to use
            # Put in cross-compartment diffusion between ksolvers
            dmdsolve.buildNeuroMeshJunctions( smdsolve, pmdsolve )
            # All the Xreac stuff in the solvers is now eliminated.

            # set up the connections so that the spine volume scaling can happen
            self.elecid.setSpineAndPsdMesh( self.spineCompt, self.psdCompt)
            self.elecid.setSpineAndPsdDsolve( smdsolve, pmdsolve )
        for i in self._endos:
            i[0].isMembraneBound = True
            i[0].surround = i[1]
            #i[0].elecComptMap = i[1].elecComptMap
            path = i[0].path
            #print( "Doing endo {} inside {}".format( path, i[1].path ) )
            if self.useGssa:
                eksolve = moose.Gsolve( path + '/ksolve' )
            else:
                eksolve = moose.Ksolve( path + '/ksolve' )
            edsolve = moose.Dsolve( path + '/dsolve' )
            estoich = moose.Stoich( path + '/stoich' )
            estoich.compartment = i[0]
            estoich.ksolve = eksolve
            estoich.dsolve = edsolve
            estoich.path = path + "/##"
            edsolve.buildMeshJunctions( moose.element(i[1].path + '/dsolve' ))
    ################################################################

    def _loadChem( self, fname, chemName ):
        chem = moose.Neutral( '/library/' + chemName )
        modelId = moose.loadModel( fname, chem.path, 'ee' )
        comptlist = moose.wildcardFind( chem.path + '/#[ISA=ChemCompt]' )
        if len( comptlist ) == 0:
            print("loadChem: No compartment found in file: ", fname)
            return
        # Sort comptlist in decreasing order of volume
        '''
        sortedComptlist = sorted( comptlist, key=lambda x: -x.volume )
        if ( len( sortedComptlist ) >= 1 ):
            sortedComptlist[0].name = 'dend'
        if ( len( sortedComptlist ) >= 2 ):
            sortedComptlist[1].name = 'spine'
        if ( len( sortedComptlist ) >= 3 ):
            sortedComptlist[2].name = 'psd'
        '''

    ################################################################

    def _moveCompt( self, a, b ):
        b.setVolumeNotRates( a.volume )
        for i in moose.wildcardFind( a.path + '/#' ):
            if ( i.name != 'mesh' ):
                moose.move( i, b )
                #print( "Moving {} {} to {}".format( i.className, i.name, b.name ))
        moose.delete( a )
    ################################################################
    def _buildAdaptor( self, meshName, elecRelPath, elecField, \
            chemRelPath, chemField, isElecToChem, offset, scale ):
        #print "offset = ", offset, ", scale = ", scale
        mesh = moose.element( '/model/chem/' + meshName )
        #elecComptList = mesh.elecComptList
        if elecRelPath == 'spine':
            # This is nasty. The spine indexing is different from
            # the compartment indexing and the mesh indexing and the 
            # chem indexing. Need to fix at some time.
            #elecComptList = moose.vec( mesh.elecComptList[0].path + '/../spine' )
            elecComptList = moose.element( '/model/elec').spineIdsFromCompartmentIds[ mesh.elecComptList ]
            #print( len( mesh.elecComptList ) )
            # for i,j in zip( elecComptList, mesh.elecComptList ):
            #    print( "Lookup: {} {} {}; orig: {} {} {}".format( i.name, i.index, i.fieldIndex, j.name, j.index, j.fieldIndex ))
        else:
            elecComptList = mesh.elecComptList

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
        root = moose.element( '/' )

        for i in zip( elecComptList, startVoxelInCompt, endVoxelInCompt, av ):
            i[3].inputOffset = 0.0
            i[3].outputOffset = offset
            i[3].scale = scale
            if elecRelPath == 'spine':
                # Check needed in case there were unmapped entries in 
                # spineIdsFromCompartmentIds
                elObj = i[0]
                if elObj == root:
                    continue
            else:
                ePath = i[0].path + '/' + elecRelPath
                if not( moose.exists( ePath ) ):
                    continue
                    #raise BuildError( "Error: buildAdaptor: no elec obj in " + ePath )
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


#######################################################################
# Some helper classes, used to define argument lists.
#######################################################################

class baseplot:
    def __init__( self,
            elecpath='soma', geom_expr='1', relpath='.', field='Vm' ):
        self.elecpath = elecpath
        self.geom_expr = geom_expr
        self.relpath = relpath
        self.field = field

class rplot( baseplot ):
    def __init__( self,
        elecpath = 'soma', geom_expr = '1', relpath = '.', field = 'Vm', 
        title = 'Membrane potential', 
        mode = 'time', 
        ymin = 0.0, ymax = 0.0, 
        saveFile = "", saveResolution = 3, show = True ):
        baseplot.__init__( self, elecpath, geom_expr, relpath, field )
        self.title = title
        self.mode = mode # Options: time, wave, wave_still, raster
        self.ymin = ymin # If ymin == ymax, it autoscales.
        self.ymax = ymax
        if len( saveFile ) < 5:
            self.saveFile = ""
        else:
            f = saveFile.split('.')
            if len(f) < 2 or ( f[-1] != 'xml' and f[-1] != 'csv' ):
                raise BuildError( "rplot: Filetype is '{}', must be of type .xml or .csv.".format( f[-1] ) )
        self.saveFile = saveFile
        self.show = show

    def printme( self ):
        print( "{}, {}, {}, {}, {}, {}, {}, {}, {}, {}".format( 
            self.elecpath,
            self.geom_expr, self.relpath, self.field, self.title,
            self.mode, self.ymin, self.ymax, self.saveFile, self.show ) )

    @staticmethod
    def convertArg( arg ):
        if isinstance( arg, rplot ):
            return arg
        elif isinstance( arg, list ):
            return rplot( *arg )
        else:
            raise BuildError( "rplot initialization failed" )

class rmoog( baseplot ):
    def __init__( self,
        elecpath = 'soma', geom_expr = '1', relpath = '.', field = 'Vm', 
        title = 'Membrane potential', 
        ymin = 0.0, ymax = 0.0, 
        show = True ): # Could put in other display options.
        baseplot.__init__( self, elecpath, geom_expr, relpath, field )
        self.title = title
        self.ymin = ymin # If ymin == ymax, it autoscales.
        self.ymax = ymax
        self.show = show

    @staticmethod
    def convertArg( arg ):
        if isinstance( arg, rmoog ):
            return arg
        elif isinstance( arg, list ):
            return rmoog( *arg )
        else:
            raise BuildError( "rmoog initialization failed" )

        # Stimlist = [path, geomExpr, relPath, field, expr_string]
class rstim( baseplot ):
    def __init__( self,
            elecpath = 'soma', geom_expr = '1', relpath = '.', field = 'inject', expr = '0'):
        baseplot.__init__( self, elecpath, geom_expr, relpath, field )
        self.expr = expr

    def printme( self ):
        print( "{0}, {1}, {2}, {3}, {4}".format( 
            self.elecpath, self.geom_expr, self.relpath, self.field, self.expr
            ) )

    @staticmethod
    def convertArg( arg ):
        if isinstance( arg, rstim ):
            return arg
        elif isinstance( arg, list ):
            return rstim( *arg )
        else:
            raise BuildError( "rstim initialization failed" )

