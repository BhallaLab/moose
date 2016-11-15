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

import moose
import matplotlib
import numpy as np
import matplotlib.pyplot as plt
import sys
import pylab
from moose.SBML import *
import os.path

def main():
    """ This example illustrates loading, running of an SBML model defined in XML format.\n
	The model 00001-sbml-l3v1.xml is taken from l3v1 SBML testcase.\n
	Plots are setup.\n
	Model is run for 20sec.\n
	As a general rule we created model under '/path/model' and plots under '/path/graphs'.\n
    """

    mfile = "../genesis/00001-sbml-l3v1.xml"
    try:
        sys.argv[1]
    except:
        pass
    else:
        mfile = sys.argv[1]
    
    try:
        sys.argv[2]
    except:
        runtime = 20.0
    else:
        runtime = float(sys.argv[2])
    
    # Loading the sbml file into MOOSE, models are loaded in path/model
    sbmlId = mooseReadSBML(mfile,'/sbml')
    if isinstance(sbmlId, (list, tuple)):
	    print sbmlId
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

            
        # Reset and Run
        moose.reinit()
        moose.start(runtime)
        return sbmlId,True
    return sbmlId,False

def displayPlots():
    # Display all plots.
    for x in moose.wildcardFind( '/sbml/graphs/#[TYPE=Table2]' ):
        t = np.arange( 0, x.vector.size, 1 ) #sec
        plt.plot( t, x.vector, label=x.name )
    
    pylab.legend()
    pylab.show()

    quit()
if __name__=='__main__':
    
    modelPath, modelpathexist = main()
    if modelpathexist == True:
        displayPlots()
