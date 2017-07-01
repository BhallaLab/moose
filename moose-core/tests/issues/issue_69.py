# -*- coding: utf-8 -*-
# Github issue #69, Also BhallaLab/moose-gui#3.

import moose
print( '[INFO] Importing moose from %s' % moose.__file__ )
print( '[INFO] Version : %s' % moose.__version__ )
moose.loadModel('../data/acc27.g','/acc27_1',"gsl")
compts = moose.wildcardFind('/acc27_1/##[ISA=ChemCompt]')
for compt in compts:
    if moose.exists(compt.path+'/stoich'):
        st = moose.element(compt.path+'/stoich')
        #print " stoich ksolve ",st.ksolve.path
        if moose.exists((st.ksolve).path):
            moose.delete(st.ksolve)
            moose.delete(st)
