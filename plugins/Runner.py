import math
import moose
from PyQt4 import Qt, QtGui, QtCore
from PyQt4.QtCore import QTimer
from PyQt4.QtCore import QObject
from PyQt4.QtCore import pyqtSignal

class Runner(QObject):
    """Helper class to control simulation execution

    See: http://doc.qt.digia.com/qq/qq27-responsive-guis.html :
    'Solving a Problem Step by Step' for design details.
    """

    simulationFinished   =   pyqtSignal(float)
    simulationStarted    =   pyqtSignal(float)
    simulationProgressed =   pyqtSignal(float)
    simulationReset      =   pyqtSignal()
    simulationContinued  =   pyqtSignal(float)

    def __init__(self):
        QtCore.QObject.__init__(self)
        self.runTime            = None
        self.updateInterval     = None
        self.simulationInterval = None
        self.runSequence        = None
        self.pause              = None
        self.clock              = moose.element("/clock")

    def resetSimulation(self, runTime, updateInterval, simulationInterval):
        self.runTime            = runTime
        self.updateInterval     = updateInterval
        self.simulationInterval = simulationInterval
        self.pause              = False
        moose.reinit()
        self.simulationReset.emit()

    def computeRunSequence(self, runTime, updateInterval, simulationInterval):
        # http://sourceforge.net/p/moose/bugs/147/
        runSequence = [ runTime / 20.0 ] * 6
        runSequence.extend([runTime / 10.0] * 7)
        # runSequence.append()
        # frac, whole         = math.modf(runTime / updateInterval)
        # runSequence         = [ updateInterval ] * int(whole)
        # remaining           = frac * updateInterval
        # if remaining > simulationInterval:
        #     runSequence.append(remaining)
        return runSequence

    def runSimulation(self, runTime):
        self.runTime = runTime
        self.runSequence = self.computeRunSequence( self.runTime
                                                  , self.updateInterval
                                                  , self.simulationInterval
                                                  )
        self.pause = False
        # print(self.runTime)
        # print(self.updateInterval)
        # print(self.simulationInterval)
        # print(self.runSequence)
        self.simulationStarted.emit(self.clock.currentTime + self.runTime)
        QTimer.singleShot(0, self.next)

    def next(self):
        if self.pause:
            return
        if len(self.runSequence) == 0:
            self.simulationFinished.emit(self.clock.currentTime)
            return
        moose.start(self.runSequence.pop(0))
        self.simulationProgressed.emit(self.clock.currentTime)
        QTimer.singleShot(0, self.next)

    def pauseSimulation(self):
        self.pause = True

    def unpauseSimulation(self):
        self.pause = False
        self.next()

    def togglePauseSimulation(self):
        if self.pause :
            self.pause = False
            self.next()
        else:
            self.pause = True

    def resetAndRunSimulation( self
                             , runTime
                             , updateInterval
                             , simulationInterval
                             ):
        self.resetSimulation(runTime, updateInterval, simulationInterval)
        self.runSimulation()
