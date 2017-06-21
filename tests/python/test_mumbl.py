# This file is part of MOOSE simulator: http://moose.ncbs.res.in.

# MOOSE is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# MOOSE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with MOOSE.  If not, see <http://www.gnu.org/licenses/>.


"""test_mumbl.py: 

    A test script to test MUMBL support in MOOSE.

Last modified: Fri Jun 13, 2014  06:30PM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os

import moose
import moose.neuroml as nml
import moose.utils as utils
import moose.backend.graphviz as graphviz

# the model lives in the same directory as the test script
modeldir = os.path.dirname(__file__)

def main():
    utils.parser
    p = os.path.join(modeldir, 'two_cells_nml_1.8/two_cells.nml')
    nml.loadNeuroML_L123(p)
    #mumbl.loadMumbl("./two_cells_nml_1.8/mumbl.xml")
    table1 = utils.recordTarget('/tableA', '/cells/purkinjeGroup_0/Dend_37_41', 'vm')
    table2 = utils.recordTarget('/tableB', '/cells/granuleGroup_0/Soma_0', 'vm')
    moose.setClock(0, 5e-6)
    moose.useClock(0, '/##', 'process')
    moose.useClock(0, '/##', 'init')
    moose.reinit()
    utils.run(0.1, verify=True)
    graphviz.writeGraphviz('test_mumble.dot', ignore='/library')
    utils.plotRecords({ 'Dend 37' : table1, 'Soma 0' : table2 }
            , outfile = '%s.png' % sys.argv[0]
            , subplot = True
            )
    
if __name__ == '__main__':
    main()
