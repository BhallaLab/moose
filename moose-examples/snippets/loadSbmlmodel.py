# -*- coding: utf-8 -*-
# Filename: loadSbmlmodel.py
# Description:
# Author: Harsha rani
# Maintainer:
# Created: Fri Jan 29 11:43:15 2016 (+0530)
# Version:
# Last-Updated:
#           By:
#     Update #: 0
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
#
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
import sys
import os.path

import numpy as np
import pylab

import moose
from moose.chemUtil.add_Delete_ChemicalSolver import *

def main():
    """
This example illustrates loading, running of an SBML model defined in XML format.
Default this  file load's 00001-sbml-l3v1.xml which is taken from l3v1 SBML testcase.
Plots are setup.
Model is run for 20sec.
As a general rule we created model under '/path/model' and plots under '/path/graphs'.
If someone wants to load anyother file then 
    `python loadSbmlmodel filepath runtime`
    """

    dfilepath = "../genesis/00001-sbml-l3v1.xml"
    druntime = 20.0
    msg = ""
    try:
        sys.argv[1]
    except IndexError:
        filepath = dfilepath
        
    else:
        filepath = sys.argv[1]
    if not os.path.exists(filepath):
        msg = "Filename or path does not exist",filepath,"loading default file",dfilepath
        filepath = dfilepath
        
    try:
        sys.argv[2]
    except :
        runtime = druntime
    else:
        runtime = float(sys.argv[2])
    sbmlId = moose.element('/')
    # Loading the sbml file into MOOSE, models are loaded in path/model
    sbmlId = moose.mooseReadSBML(filepath,'/sbml')
    if isinstance(sbmlId, (list, tuple)):
        print(sbmlId)

    elif sbmlId.path != '/':

        s1 = moose.element('/sbml/model/compartment/S1')
        s2= moose.element('/sbml/model/compartment/S2')

        # Creating MOOSE Table, Table2 is for the chemical model
        graphs = moose.Neutral( '/sbml/graphs' )
        outputs1 = moose.Table2 ( '/sbml/graphs/concS1')
        outputs2 = moose.Table2 ( '/sbml/graphs/concS2')

        # connect up the tables
        moose.connect( outputs1,'requestOut', s1, 'getConc' );
        moose.connect( outputs2,'requestOut', s2, 'getConc' );

        # gsl solver is added, default is ee
        moose.mooseaddChemSolver(sbmlId.path,"ee")

        # Reset and Run
        moose.reinit()
        moose.start(runtime)
        return sbmlId,True,msg
    return sbmlId,False,msg 

def displayPlots():
    # Display all plots.
    for x in moose.wildcardFind( '/sbml/graphs/#[TYPE=Table2]' ):
        t = np.arange( 0, x.vector.size, 1 ) #sec
        pylab.plot( t, x.vector, label=x.name )

    pylab.legend()
    pylab.show()

    quit()

if __name__=='__main__':
    modelPath = moose.element('/')
    modelpathexist = False
    msg = "" 
    modelPath, modelpathexist,msg = main()
    if msg:
        print (msg)
    if modelpathexist == True:
        displayPlots()

    
