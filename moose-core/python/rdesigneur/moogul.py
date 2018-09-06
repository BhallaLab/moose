# Moogul.py: MOOSE Graphics Using Lines
# This is a fallback graphics interface for displaying neurons using
# regular matplotlib routines.
# Put in because the GL versions like moogli need all sorts of difficult 
# libraries and dependencies.
# Copyright (C) Upinder S. Bhalla NCBS 2018
# This program is licensed under the GNU Public License version 3.
#

import moose
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Line3DCollection

class MoogulError( Exception ):
    def __init__( self, value ):
        self.value = value
    def __str__( self ):
        return repr( self.value )

class MooView:
    ''' The MooView class is a window in which to display one or more 
    moose cells, using the MooCell class.'''
    def __init__( self, swx = 10, swy = 12, hideAxis = True
    ):
        plt.ion()
        self.fig_ = plt.figure( figsize = (swx, swy) )
        self.ax = self.fig_.add_subplot(111, projection='3d' )
        self.drawables_ = []
        self.fig_.canvas.mpl_connect("key_press_event", self.moveView )
        plt.rcParams['keymap.xscale'] = ''
        plt.rcParams['keymap.yscale'] = ''
        plt.rcParams['keymap.zoom'] = ''
        plt.rcParams['keymap.back'] = ''
        plt.rcParams['keymap.home'] = ''
        plt.rcParams['keymap.forward'] = ''
        plt.rcParams['keymap.all_axes'] = ''
        self.hideAxis = hideAxis
        if self.hideAxis:
            self.ax.set_axis_off()
        #self.ax.margins( tight = True )
        self.ax.margins()
        self.sensitivity = 7.0 # degrees rotation
        self.zoom = 1.05

    def addDrawable( self, n ):
        self.drawables_.append( n )

    def firstDraw( self, rotation = 0.0, elev = 0.0, azim = 0.0 ):
        self.coordMax = 0.0 
        self.coordMin = 0.0
        if rotation == 0.0:
            self.doRotation = False
            self.rotation = 7 # default rotation per frame, in degrees.
        else:
            self.doRotation = True
            self.rotation = rotation * 180/np.pi # arg units: radians/frame
        
        self.azim = azim * 180/np.pi
        self.elev = elev * 180/np.pi
        for i in self.drawables_:
            cmax, cmin = i.drawForTheFirstTime( self.ax )
            self.coordMax = max( cmax, self.coordMax )
            self.coordMin = min( cmin, self.coordMin )
        if self.coordMin == self.coordMax:
            self.coordMax = 1+self.coordMin

            
        self.ax.set_xlim3d( self.coordMin, self.coordMax )
        self.ax.set_ylim3d( self.coordMin, self.coordMax )
        self.ax.set_zlim3d( self.coordMin, self.coordMax )
        self.ax.view_init( elev = self.elev, azim = self.azim )
        #self.ax.view_init( elev = -80.0, azim = 90.0 )
        #self.colorbar = plt.colorbar( self.drawables_[0].segments )
        self.colorbar = self.fig_.colorbar( self.drawables_[0].segments )
        self.colorbar.set_label( self.drawables_[0].fieldInfo[3])
        self.timeStr = self.ax.text2D( 0.05, 0.05, 
                "Time= 0.0", transform=self.ax.transAxes)
        self.fig_.canvas.draw()
        plt.show()

    def updateValues( self ):
        time = moose.element( '/clock' ).currentTime
        self.timeStr.set_text( "Time= " + str(time) )
        for i in self.drawables_:
            i.updateValues()
        if self.doRotation and abs( self.rotation ) < 120:
            self.ax.azim += self.rotation
        #self.fig_.canvas.draw()
        plt.pause(0.001)

    def moveView(self, event):
        x0 = self.ax.get_xbound()[0]
        x1 = self.ax.get_xbound()[1]
        xk = (x0 - x1) / self.sensitivity
        y0 = self.ax.get_ybound()[0]
        y1 = self.ax.get_ybound()[1]
        yk = (y0 - y1) / self.sensitivity
        z0 = self.ax.get_zbound()[0]
        z1 = self.ax.get_zbound()[1]
        zk = (z0 - z1) / self.sensitivity

        if event.key == "up" or event.key == "k":
            self.ax.set_ybound( y0 + yk, y1 + yk )
        if event.key == "down" or event.key == "j":
            self.ax.set_ybound( y0 - yk, y1 - yk )
        if event.key == "left" or event.key == "h":
            self.ax.set_xbound( x0 + xk, x1 + xk )
        if event.key == "right" or event.key == "l":
            self.ax.set_xbound( x0 - xk, x1 - xk )
        if event.key == "ctrl+up":
            self.ax.set_zbound( z0 + zk, z1 + zk )
        if event.key == "ctrl+down":
            self.ax.set_zbound( z0 - zk, z1 - zk )
        if event.key == "." or event.key == ">": # zoom in, bigger
            self.ax.set_xbound( x0/self.zoom, x1/self.zoom )
            self.ax.set_ybound( y0/self.zoom, y1/self.zoom )
            self.ax.set_zbound( z0/self.zoom, z1/self.zoom )
        if event.key == "," or event.key == "<": # zoom out, smaller
            self.ax.set_xbound( x0*self.zoom, x1*self.zoom )
            self.ax.set_ybound( y0*self.zoom, y1*self.zoom )
            self.ax.set_zbound( z0*self.zoom, z1*self.zoom )
        if event.key == "a": # autoscale to fill view.
            self.ax.set_xlim3d( self.coordMin, self.coordMax )
            self.ax.set_ylim3d( self.coordMin, self.coordMax )
            self.ax.set_zlim3d( self.coordMin, self.coordMax )
        if event.key == "p": # pitch
            self.ax.elev += self.sensitivity
        if event.key == "P":
            self.ax.elev -= self.sensitivity
        if event.key == "y": # yaw
            self.ax.azim += self.sensitivity
        if event.key == "Y":
            self.ax.azim -= self.sensitivity
        # Don't have anything for roll
        if event.key == "g":
            self.hideAxis = not self.hideAxis
            if self.hideAxis:
                self.ax.set_axis_off()
            else:
                self.ax.set_axis_on()
        if event.key == "t": # Turn on/off twisting/autorotate
            self.doRotation = not self.doRotation
        if event.key == "?": # Print out help for these commands
            self.printMoogulHelp()

        self.fig_.canvas.draw()

    def printMoogulHelp( self ):
        print( '''
            Key bindings for Moogul:
            Up or k:    pan object up
            Down or j:  pan object down
            left or h:  pan object left. Bug: direction depends on azimuth.
            right or l:  pan object right Bug: direction depends on azimuth
            . or >:     Zoom in: make object appear bigger
            , or <:     Zoom out: make object appear smaller
            a:          Autoscale to fill view
            p:          Pitch down
            P:          Pitch up
            y:          Yaw counterclockwise
            Y:          Yaw counterclockwise
            g:          Toggle visibility of grid
            t:          Toggle turn (rotation along long axis of cell)
            ?:          Print this help page.
        ''')

#####################################################################

class MooDrawable:
    ''' Base class for drawing things'''
    def __init__( self,
        fieldInfo, field, relativeObj, maxLineWidth,
        colormap,
        lenScale, diaScale, autoscale,
        valMin, valMax
    ):
        self.field = field
        self.relativeObj = relativeObj
        self.maxLineWidth = maxLineWidth
        self.lenScale = lenScale
        self.diaScale = diaScale
        self.colormap = colormap
        self.autoscale = autoscale
        self.valMin = valMin
        self.valMax = valMax
        self.fieldInfo = fieldInfo
        self.fieldScale = fieldInfo[2]
        #FieldInfo = [baseclass, fieldGetFunc, scale, axisText, min, max]

    def updateValues( self ):
        ''' Obtains values from the associated cell'''
        self.val = np.array([moose.getField( i, self.field, 'double' ) for i in self.activeObjs]) * self.fieldScale
        cmap = plt.get_cmap( self.colormap )
        if self.autoscale:
            valMin = min( self.val )
            valMax = max( self.val )
        else:
            valMin = self.valMin
            valMax = self.valMax
        scaleVal = (self.val - valMin) / (valMax - valMin)
        self.rgba = [ cmap(i) for i in scaleVal ]
        self.segments.set_color( self.rgba )
        return

    def drawForTheFirstTime( self, ax ):
        self.segments = Line3DCollection( self.activeCoords, 
                linewidths = self.linewidth, cmap = plt.get_cmap(self.colormap) )
        self.cax = ax.add_collection3d( self.segments )
        self.segments.set_array( self.valMin + np.array( range( len(self.activeCoords) ) ) * (self.valMax-self.valMin)/len(self.activeCoords) )
        return self.coordMax, self.coordMin




#####################################################################

class MooNeuron( MooDrawable ):
    ''' Draws collection of line segments of defined dia and color'''
    def __init__( self,
        neuronId,
        fieldInfo,
        field = 'Vm', 
        relativeObj = '.', 
        maxLineWidth = 20, 
        colormap = 'jet', 
        lenScale = 1e6, diaScale = 1e6, autoscale = False, 
        valMin = -0.1, valMax = 0.05,
    ):
        #self.isFieldOnCompt = 
            #field in ( 'Vm', 'Im', 'Rm', 'Cm', 'Ra', 'inject', 'diameter' )
        
        MooDrawable.__init__( self, fieldInfo, field = field, 
                relativeObj = relativeObj, maxLineWidth = maxLineWidth, 
                colormap = colormap, lenScale = lenScale, 
                diaScale = diaScale, autoscale = autoscale, 
                valMin = valMin, valMax = valMax )
        self.neuronId = neuronId
        self.updateCoords()

    def updateCoords( self ):
        ''' Obtains coords from the associated cell'''
        self.compts_ = moose.wildcardFind( self.neuronId.path + "/#[ISA=CompartmentBase]" )
        # Matplotlib3d isn't able to do full rotations about an y axis,
        # which is what the NeuroMorpho models use, so
        # here we shuffle the axes around. Should be an option.
        #coords = np.array([[[i.x0,i.y0,i.z0],[i.x,i.y,i.z]] 
            #for i in self.compts_])
        coords = np.array([[[i.z0,i.x0,i.y0],[i.z,i.x,i.y]] 
            for i in self.compts_])
        dia = np.array([i.diameter for i in self.compts_])
        if self.relativeObj == '.':
            self.activeCoords = coords
            self.activeDia = dia
            self.activeObjs = self.compts_
        else:
            self.activeObjs = []
            self.activeCoords = []
            self.activeDia = []
            for i,j,k in zip( self.compts_, coords, dia ):
                if moose.exists( i.path + '/' + self.relativeObj ):
                    elm = moose.element( i.path + '/' + self.relativeObj )
                    self.activeObjs.append( elm )
                    self.activeCoords.append( j )
                    self.activeDia.append( k )

        self.activeCoords = np.array( self.activeCoords ) * self.lenScale
        self.coordMax = np.amax( self.activeCoords )
        self.coordMin = np.amin( self.activeCoords )
        self.linewidth = np.array( [ min(self.maxLineWidth, 1 + int(i * self.diaScale )) for i in self.activeDia ] )

        return

#####################################################################
class MooReacSystem( MooDrawable ):
    ''' Draws collection of line segments of defined dia and color'''
    def __init__( self,
        mooObj, fieldInfo,
        field = 'conc', 
        relativeObj = '.', 
        maxLineWidth = 100, 
        colormap = 'jet', 
        lenScale = 1e6, diaScale = 20e6, autoscale = False, 
        valMin = 0.0, valMax = 1.0
    ):
        
        MooDrawable.__init__( self, fieldInfo, field = field, 
                relativeObj = relativeObj, maxLineWidth = maxLineWidth, 
                colormap = colormap, lenScale = lenScale, 
                diaScale = diaScale, autoscale = autoscale, 
                valMin = valMin, valMax = valMax )
        self.mooObj = mooObj
        self.updateCoords()

    def updateCoords( self ):
        ''' For now a dummy cylinder '''
        dx = 1e-6
        dummyDia = 20e-6
        numObj = len( self.mooObj )
        x = np.arange( 0, (numObj+1) * dx, dx )
        y = np.zeros( numObj + 1)
        z = np.zeros( numObj + 1)

        coords = np.array([[[i*dx,0,0],[(i+1)*dx,0,0]] for i in range( numObj )] )
        dia = np.ones( numObj ) * dummyDia
        self.activeCoords = coords
        self.activeDia = dia
        self.activeObjs = self.mooObj
        self.activeCoords = np.array( self.activeCoords ) * self.lenScale
        self.coordMax = np.amax( self.activeCoords )
        self.coordMin = np.amin( self.activeCoords )
        self.linewidth = np.array( [ min(self.maxLineWidth, 1 + int(i * self.diaScale )) for i in self.activeDia ] )

        return
