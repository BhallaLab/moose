import moose

a = moose.CubeMesh('/cube')
a.volume = 1

ca = moose.BufPool('/cube/ca')
ca.concInit = 1.0

f = moose.Function('/cube/ca/func')
f.expr = 'sin(0.1*3.14*t) > 0.81 ? 25e-3 : 0'
moose.connect(f, 'valueOut', ca, 'setConc')

# create a table.
tab = moose.Table2('/cube/ca/table')
moose.connect( tab, 'requestOut', ca, 'getConc' )

# refine clock
for i in range(10, 16):
    moose.setClock(i, 0.001)

moose.reinit()
moose.start(100)

import pylab
pylab.plot( tab.vector )
pylab.savefig('output.png')
