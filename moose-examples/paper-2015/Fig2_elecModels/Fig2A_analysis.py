import os
import numpy as np
from pylab import *
from itertools import islice

if not os.path.isfile('wx.xplot'):
    print("Please run ./Fig2A.py to generate the data")
    quit()

fh = open('wt.xplot',mode='r')

fulltime = 1200         #s
plotdt = 1              #s
numpts = int(fulltime/plotdt)+1

print('reading')
wts = []
while True:
    next_line = ''
    while 'plotname' not in next_line:
        next_line = fh.readline()
        if next_line == '': break
    if next_line == '': break
    next_wt = [float(fh.readline()) for i in range(numpts)]
    wts.append(next_wt)
    print(('weight',len(wts)))


hiAve = np.zeros( len( wts[0] ) )
loAve = np.zeros( len( wts[0] ) )
numHi = 0
for i in wts:
    if i[0] > 0.5:
        hiAve += np.array( i )
        numHi += 1
    else:
        loAve += np.array( i )
    

hiAve /= numHi
loAve /= ( len( wts ) - numHi )

def dumpVec( f, name, vec ):
    f.write( '/newplot\n' )
    f.write( '/plotname ' + name  + '\n' )
    for i in vec:
        f.write( str( i ) + '\n' )

f = open( 'averageOfxplots', 'w' )
dumpVec( f, 'highAverage', hiAve )
dumpVec( f, 'lowAverage', loAve )
dumpVec( f, 'highExample', wts[4] )
dumpVec( f, 'lowExample', wts[5] )
f.close()


print(('numHi = ', numHi, ' plotting...'))
figure()
plot(transpose(wts))
plot( hiAve, linewidth=4 )
plot( loAve, linewidth=4 )
show()    
