# -*- coding: utf-8 -*-
# Issue 34 on github.
import moose
import moose.utils as mu

compt = moose.CubeMesh('/compt')
compt.volume = 1e-20

molecules = [ "ca", "S", "S1", "Sp" ]
pools = {}
tables = {}
for m in molecules:
    pools[m] = moose.Pool('/compt/%s' % m)
    t = moose.Table2("/table%s" % m)
    tables[m] = t
    moose.connect(t, 'requestOut', pools[m], 'getConc')

pools['ca'].nInit = 20

r_p0_p1 = moose.Reac('/compt/reacA')
funA = moose.Function('/compt/funA')
funA.x.num = 1
funA.expr = "{0}*(x0/{1})^6/(1+(x0/{1})^3)^2".format("1.5", "0.7e-3")
# moose.connect(funA.x[0], 'input',  pools['ca'], 'getN')
moose.connect(pools['ca'], 'nOut', funA.x[0], 'input')
moose.connect(funA, 'valueOut', pools['S1'], 'setN')
moose.connect(r_p0_p1, 'sub', pools['S'], 'reac')
moose.connect(r_p0_p1, 'prd', pools['S1'], 'reac')

r_p1_up = moose.Reac('/compt/reacB')
moose.connect(r_p1_up, 'sub', pools['S1'], 'reac')
moose.connect(r_p1_up, 'prd', pools['Sp'], 'reac')

# Change GSolve to Ksolve and bugs go away.
k = moose.Gsolve('/compt/ksolve')
s = moose.Stoich('/compt/stoich')
s.compartment = compt
s.ksolve = k
s.path = '/compt/##'

moose.reinit()
moose.start(10)
