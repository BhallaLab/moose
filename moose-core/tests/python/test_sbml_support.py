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
# Thu 10 Mar 2016 10:52:27 AM IST
#  Dilawar: Modified for turning it into a test script.
# 

import moose
import matplotlib
import numpy as np
import matplotlib.pyplot as plt
import sys
import pylab
import os

script_dir = os.path.dirname( os.path.realpath( __file__) )

print( "Using moose from: %s" % moose.__file__ )

def main():
    """ This example illustrates loading, running of an SBML model defined in XML format.\n
	The model 00001-sbml-l3v1.xml is taken from l3v1 SBML testcase.\n
	Plots are setup.\n
	Model is run for 20sec.\n
	As a general rule we created model under '/path/model' and plots under '/path/graphs'.\n
    """

    mfile =  os.path.join( script_dir, 'chem_models/00001-sbml-l3v1.xml')
    runtime = 20.0
        
    # Loading the sbml file into MOOSE, models are loaded in path/model
    sbmlId = moose.readSBML(mfile,'sbml')
    

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

def displayPlots():
    # Display all plots.
    for x in moose.wildcardFind( '/sbml/graphs/#[TYPE=Table2]' ):
        t = np.arange( 0, x.vector.size, 1 ) #sec
        plt.plot( t, x.vector, label=x.name )
    
    pylab.legend()
    pylab.show()

    quit()
if __name__=='__main__':
    
    main()
    # displayPlots()
