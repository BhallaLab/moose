# -*- coding: utf-8 -*-
# Issue 47 on moose-core
import moose
print( '[INFO] Using moose from %s' % moose.__file__ )
# moose.loadModel('../data/acc94.g','/acc94','gsl')
moose.loadModel('../data/acc94.g','/acc94','gssa')
moose.reinit()
moose.run( 10 )
