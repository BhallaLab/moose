# mload.py ---
#
# Filename: mload.py
# Description:
# Author:
# Maintainer:
# Created: Fri Feb  8 09:38:40 2013 (+0530)
# Version:
# Last-Updated: Wed May 22 12:16:35 2013 (+0530)
#           By: subha
#     Update #: 213
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
# Utility to load models and detect filetype.
#
#

# Change log:
#
#
#
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
#
#

# Code:

import moose
from moose import neuroml
import mtypes
from mexception import FileLoadError
import posixpath
from os.path import basename
from os.path import splitext
from PyQt4 import QtGui, QtCore, Qt
from plugins.setsolver import *
from moose.SBML import *

def loadGenCsp(target,filename,solver="gsl"):
    target = target.replace(" ", "")
    path = '/'+target
    #Moving the model under /modelname/model and graphs under /model/graphs.
    #This is passed while loading-time which will be easy for setting the stoich path
    mpath = '/'+target+'/'+"model"
    if moose.exists(mpath):
        moose.delete(mpath)
    modelpath1 = moose.Neutral('%s' %(target))
    modelpath = moose.Neutral('%s/%s' %(modelpath1.path,"model"))
    model = moose.loadModel(filename, modelpath.path,solver)
    
    if not moose.exists(modelpath1.path+'/data'):
        graphspath = moose.Neutral('%s/%s' %(modelpath1.path,"data"))
    dataPath = moose.element(modelpath1.path+'/data')
    i =0
    nGraphs = moose.wildcardFind(modelpath.path+'/graphs/##[TYPE=Table2]')
    for graphs in nGraphs:
        if not moose.exists(dataPath.path+'/graph_'+str(i)):
            graphspath = moose.Neutral('%s/%s' %(dataPath.path,"graph_"+str(i)))
        else:
            graphspath = moose.element(dataPath.path+'/graph_'+str(i))
        moose.move(graphs.path,graphspath)

    if len(nGraphs) > 0:
        i = i+1
    
    for moregraphs in moose.wildcardFind(modelpath.path+'/moregraphs/##[TYPE=Table2]'):
        if not moose.exists(dataPath.path+'/graph_'+str(i)):
            graphspath = moose.Neutral('%s/%s' %(dataPath.path,"graph_"+str(i)))
        else:
            graphspath = moose.element(dataPath.path+'/graph_'+str(i))
        moose.move(moregraphs.path,graphspath)
    if moose.exists(modelpath.path+'/info'):
        AnnotatorOld =  moose.element(modelpath.path+'/info')
        AnnotatorNew = moose.Annotator(modelpath1.path+'/info')
        AnnotatorNew.runtime = AnnotatorOld.runtime
        AnnotatorNew.solver = AnnotatorOld.solver
        moose.delete(AnnotatorOld)
    moose.delete(modelpath.path+'/graphs')
    moose.delete(modelpath.path+'/moregraphs')
    return(modelpath1,modelpath1.path)

def loadFile(filename, target, solver="gsl", merge=True):
    """Try to load a model from specified `filename` under the element
    `target`.

    if `merge` is True, the contents are just loaded at target. If
    false, everything is deleted from the parent of target unless the
    parent is root.

    Returns
    -------
    a dict containing at least these three entries:

    modeltype: type of the loaded model.

    subtype: subtype of the loaded model, None if no specific subtype

    modelroot: root element of the model, None if could not be located - as is the case with Python scripts
    """
    num = 1
    newTarget = target
    while moose.exists(newTarget):
        newTarget = target + "-" + str(num)
        num = num + 1
    target = newTarget
    istext = True
    with open(filename, 'rb') as infile:
        istext = mtypes.istextfile(infile)
    if not istext:
        print 'Cannot handle any binary formats yet'
        return None
    # parent, child = posixpath.split(target)
    # p = moose.Neutral(parent)
    # if not merge and p.path != '/':
    #     for ch in p.children:
    #         moose.delete(ch)
    try:
        modeltype = mtypes.getType(filename)
        subtype = mtypes.getSubtype(filename, modeltype)
    except KeyError:
        raise FileLoadError('Do not know how to handle this filetype: %s' % (filename))
    pwe = moose.getCwe()
    #self.statusBar.showMessage('Loading model, please wait')
    # app = QtGui.qApp
    # app.setOverrideCursor(QtGui.QCursor(Qt.Qt.BusyCursor)) #shows a hourglass - or a busy/working arrow
    if modeltype == 'genesis':
        if subtype == 'kkit' or subtype == 'prototype':
            model,modelpath = loadGenCsp(target,filename,solver)
            if moose.exists(moose.element(modelpath).path):
                moose.Annotator(moose.element(modelpath).path+'/info').modeltype = "kkit"
            else:
                print " path doesn't exists"
            moose.le(modelpath)
        else:
            print 'Only kkit and prototype files can be loaded.'
    
    elif modeltype == 'cspace':
            model,modelpath = loadGenCsp(target,filename)
            if moose.exists(modelpath):
                moose.Annotator((moose.element(modelpath).path+'/info')).modeltype = "cspace"
            addSolver(modelpath,'gsl')

    elif modeltype == 'xml':
        if subtype == 'neuroml':
            popdict, projdict = neuroml.loadNeuroML_L123(filename)
            # Circus to get the container of populations from loaded neuroml
            for popinfo in popdict.values():
                for cell in popinfo[1].values():
                    solver = moose.HSolve(cell.path + "/hsolve")
                    solver.target = cell.path
                    # model = cell.parent
                    # break
                # break


            # Moving model to a new location under the model name
            # model name is the filename without extension

            model   = moose.Neutral("/" + splitext(basename(filename))[0])
            element = moose.Neutral(model.path + "/model")
            if(moose.exists("/cells"))  : moose.move("/cells"  , element.path)
            if(moose.exists("/elec"))   : moose.move("/elec"   , model.path)
            if(moose.exists("/library")): moose.move("/library", model.path)

            # moose.move("cells/", cell.path)
        elif subtype == 'sbml':
            if target != '/':
                if moose.exists(target):
                    moose.delete(target)
            model = mooseReadSBML(filename,target)
            if moose.exists(moose.element(model).path):
                moose.Annotator(moose.element(model).path+'/info').modeltype = "sbml"
            addSolver(target,'gsl')
    else:
        raise FileLoadError('Do not know how to handle this filetype: %s' % (filename))
    moose.setCwe(pwe) # The MOOSE loadModel changes the current working element to newly loaded model. We revert that behaviour

    # TODO: check with Aditya how to specify the target for
    # neuroml reader
    # app.restoreOverrideCursor()
    return {'modeltype': modeltype,
            'subtype': subtype,
            'model': model}



#
# mload.py ends here
