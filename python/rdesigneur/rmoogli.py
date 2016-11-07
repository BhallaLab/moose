#########################################################################
## rdesigneur0_4.py ---
## This program is part of 'MOOSE', the
## Messaging Object Oriented Simulation Environment.
##           Copyright (C) 2014 Upinder S. Bhalla. and NCBS
## It is made available under the terms of the
## GNU General Public License version 2 or later.
## See the file COPYING.LIB for the full notice.
#########################################################################

import math
import matplotlib
import sys
import moose
import os

# Check if DISPLAY environment variable is properly set. If not, warn the user
# and continue.
hasDisplay = True
display = os.environ.get('DISPLAY',  '' )
if not display:
    hasDisplay = False
    print( "Warning: Environment variable DISPLAY is not set."
            " Did you forget to pass -X or -Y switch to ssh command?\n" 
            "Anyway, MOOSE will continue without graphics.\n"
            )

hasMoogli = True

if hasDisplay:
    try: 
        from PyQt4 import QtGui
        import moogli
        import moogli.extensions.moose
        app = QtGui.QApplication(sys.argv)
    except Exception as e:
        print( 'Warning: Moogli not found. All moogli calls will use dummy functions' )
        hasMoogli = False


runtime = 0.0
moogliDt = 1.0
rotation = math.pi / 500.0

def getComptParent( obj ):
    k = moose.element(obj)
    while not k.isA[ "CompartmentBase" ]:
        if k == moose.element( '/' ):
            return k.path
        k = moose.element( k.parent )
    return k.path

#######################################################################
## Here we set up the callback functions for the viewer
def prelude( view ):
    view.home()
    view.pitch( math.pi / 2.0 )
    view.zoom( 0.05 )
    #network.groups["soma"].set( "color", moogli.colors.RED )

# This func is used for the first viewer, it has to handle advancing time.
def interlude( view ):
    moose.start( moogliDt )
    val = [ moose.getField( i, view.mooField, "double" ) * view.mooScale for i in view.mooObj ]
    view.mooGroup.set("color", val, view.mapper)
    view.yaw( rotation )
    #print moogliDt, len( val ), runtime
    if moose.element("/clock").currentTime >= runtime:
        view.stop()

# This func is used for later viewers, that don't handle advancing time.
def interlude2( view ):
    val = [ moose.getField( i, view.mooField, "double" ) * view.mooScale for i in view.mooObj ]
    
    view.mooGroup.set("color", val, view.mapper)
    view.yaw( rotation )
    if moose.element("/clock").currentTime >= runtime:
        view.stop()

def postlude( view ):
    view.rd.display()

def makeMoogli( rd, mooObj, moogliEntry, fieldInfo ):
    if not hasMoogli:
        return None
    mooField = moogliEntry[3]
    numMoogli = len( mooObj )
    network = moogli.extensions.moose.read( path = rd.elecid.path, vertices=15)
    #print len( network.groups["spine"].shapes )
    #print len( network.groups["dendrite"].shapes )
    #print len( network.groups["soma"].shapes )
    #soma = network.groups["soma"].shapes[ '/model/elec/soma']
    #print network.groups["soma"].shapes

    soma = network.groups["soma"].shapes[ rd.elecid.path + '/soma[0]']
    if ( mooField == 'n' or mooField == 'conc' ):
        updateGroup = soma.subdivide( numMoogli )
        displayObj = mooObj
    else:
        shell = moose.element( '/' )
        displayObj = [i for i in mooObj if i != shell ]
        cpa = [getComptParent( i ) for i in displayObj ]
        updateGroup = moogli.Group( "update" )
        updateShapes = [network.shapes[i] for i in cpa]
        #print "########### Len( cpa, mooObj ) = ", len( cpa ), len( mooObj ), len( updateShapes )
        updateGroup.attach_shapes( updateShapes )

    normalizer = moogli.utilities.normalizer( 
                    moogliEntry[5], moogliEntry[6], 
                    clipleft =True,
                    clipright = True )
    colormap = moogli.colors.MatplotlibColorMap(matplotlib.cm.rainbow)
    mapper = moogli.utilities.mapper(colormap, normalizer)


    viewer = moogli.Viewer("Viewer")
    viewer.setWindowTitle( moogliEntry[4] )
    if ( mooField == 'n' or mooField == 'conc' ):
        viewer.attach_shapes( updateGroup.shapes.values())
        viewer.detach_shape(soma)
    else:
        viewer.attach_shapes(network.shapes.values())

    if len( rd.moogNames ) == 0:
        view = moogli.View("main-view",
                       prelude=prelude,
                       interlude=interlude,
                       postlude = postlude)
    else:
        view = moogli.View("main-view",
                       prelude=prelude,
                       interlude=interlude2)

    cb = moogli.widgets.ColorBar(id="cb",
                                 title=fieldInfo[3],
                                 text_color=moogli.colors.BLACK,
                                 position=moogli.geometry.Vec3f(0.975, 0.5, 0.0),
                                 size=moogli.geometry.Vec3f(0.30, 0.05, 0.0),
                                 text_font="/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-R.ttf",
                                 orientation=math.pi / 2.0,
                                 text_character_size=16,
                                 label_formatting_precision=0,
                                 colormap=moogli.colors.MatplotlibColorMap(matplotlib.cm.rainbow),
                                 color_resolution=100,
                                 scalar_range=moogli.geometry.Vec2f(
                                     moogliEntry[5],
                                     moogliEntry[6]))
    cb.set_num_labels(3)
    view.attach_color_bar(cb)
    view.rd = rd
    view.mooObj = displayObj
    view.mooGroup = updateGroup
    view.mooField = mooField
    view.mooScale = fieldInfo[2]
    view.mapper = mapper
    viewer.attach_view(view)
    return viewer

def displayMoogli( rd, _dt, _runtime, _rotation ):
    if not hasMoogli:
        return None
    global runtime
    global moogliDt
    global rotation
    runtime = _runtime
    moogliDt = _dt
    rotation = _rotation
    for i in rd.moogNames:
        i.show()
        i.start()
    #viewer.showMaximized()
    #viewer.show()
    #viewer.start()
    return app.exec_()
