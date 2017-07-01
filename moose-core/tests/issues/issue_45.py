# -*- coding: utf-8 -*-
from __future__ import print_function
import moose

moose.Neutral('/model')
compt = moose.CubeMesh('/model/Compart')
compt.volume = 1.6667e-21

s = moose.Pool('/model/Compart/S')
s.concInit = 0.3
p = moose.Pool('/model/Compart/P')
r = moose.Reac('/model/Compart/Reac')
moose.connect(r, 'sub', s, 'reac')

moose.connect(r, 'prd', p, 'reac')

bf = moose.BufPool('/model/Compart/BufPool')
f = moose.Function('/model/Compart/BufPool/function')
moose.connect( f, 'valueOut', bf ,'setN' )

numVariables = f.numVars
f.numVars+=1
expr = ""
expr = (f.expr+'+'+'x'+str(numVariables))
expr = expr.lstrip("0 +")
expr = expr.replace(" ","")
f.expr = expr
moose.connect( s, 'nOut', f.x[numVariables], 'input' )

bf1 = moose.BufPool('/model/Compart/BufPool1')
f1 = moose.Function('/model/Compart/BufPool1/func')
moose.connect( f1, 'valueOut', bf1 ,'setN' )
numVariables = f1.numVars
expr = ""
expr = (f1.expr+'+'+'x'+str(numVariables))
expr = expr.lstrip("0 +")
expr = expr.replace(" ","")
f1.expr = expr
moose.connect( s, 'nOut', f1.x[numVariables], 'input' )

compts = moose.wildcardFind('/model/##[ISA=ChemCompt]')

print(" f name= ",f.name, f.tick)
print(" f1 name= ",f1.name, f1.tick)
for compt in compts:
    ksolve = moose.Ksolve( compt.path+'/ksolve' )
    stoich = moose.Stoich( compt.path+'/stoich' )
    stoich.compartment = compt
    stoich.ksolve = ksolve
    stoich.path = compt.path+"/##"

print("After solver is set")
print(" f name= ",f.name,f.tick)
print(" f1 name= ", f1.name, f1.tick)
