from __future__ import absolute_import, print_function
# rmoogli.py: rdesigneur Moogli interface
# This is a fallback version designed to work with moogul but using almost
# the same interface as far as rdesigneur is concerned.
# Put in because the GL versions like moogli need all sorts of difficult 
# libraries and dependencies.
# Copyright (C) Upinder S. Bhalla NCBS 2018
# This program is licensed under the GNU Public License version 3.

import rdesigneur.moogul as moogul
mooViews = []

def makeMoogli( rd, mooObj, args, fieldInfo ):
    #mooObj is currently poorly handled. Ideally it should simply be a 
    # vector of objects to plot, each with coords. This could be readily
    # used to build up the display in a neutral manner.
    # Another version is to have a vector of objects to plot, only as a 
    # way to get at their fields. We would separately need mapping to
    # neuron compartments and index along length.
    # Cleaner still would be to have the C code give a vector of values
    # For now it means something different for chem and elec displays.
    #moogliEntry = [elecPath,bool,whichObjToDisplay,FieldToDisplay,titleForDisplay,rangeMin,rangeMax]
    mooField = args.field
    relObjPath = args.relpath
    numMoogli = len( mooObj )
    if args.ymin != args.ymax:
        ymin = args.ymin
        ymax = args.ymax
    else:
        ymin = fieldInfo[4]
        ymax = fieldInfo[5]
    #print( "fieldinfo = {}, ymin = {}, ymax = {}".format( fieldInfo, ymin, ymax ))

    viewer = moogul.MooView()
    if mooField == 'n' or mooField == 'conc':
        #moogul.updateDiffCoords( mooObj )
        reacSystem = moogul.MooReacSystem( mooObj, fieldInfo, 
                field = mooField, relativeObj = relObjPath, 
                valMin = ymin, valMax = ymax )
        viewer.addDrawable( reacSystem )
    else:
        neuron = moogul.MooNeuron( rd.elecid, fieldInfo,
                field = mooField, relativeObj = relObjPath,
                valMin = ymin, valMax = ymax )
        print( "min = {}, max = {}".format(ymin, ymax) )
        viewer.addDrawable( neuron )

    return viewer

def updateMoogliViewer():
    for i in mooViews:
        i.updateValues()
    

def displayMoogli( rd, _dt, _runtime, rotation = 0.0, fullscreen = False, azim = 0.0, elev = 0.0 ):
    global mooViews
    mooViews = rd.moogNames
    for i in rd.moogNames:
        i.firstDraw( rotation = rotation, azim = azim, elev = elev ) 
        # rotation in radians/frame, azim, elev in radians.

