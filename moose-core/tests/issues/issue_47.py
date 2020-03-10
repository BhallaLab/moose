# -*- coding: utf-8 -*-
# Issue 47 on moose-core
import moose
import os
sdir = os.path.dirname( __file__ )
print( '[INFO] Using moose from %s' % moose.__file__ )
print( '[WONTFIX] See https://github.com/BhallaLab/moose-core/issues/47')
if False:
    moose.loadModel(os.path.join(sdir,'../data/acc94.g'),'/acc94','gsl')
    #moose.loadModel(os.path.join(sdir,'../data/acc94.g'),'/acc94','gssa')
    moose.reinit()
    moose.start( 10 )
else:
    quit(0)
