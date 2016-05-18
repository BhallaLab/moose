"""This code is under testing: checking the protocol for
FieldElements. Once that is cleaned up, this can be reused as a
demo.

>>> import moose
on node 0, numNodes = 1, numCores = 8
Info: Time to define moose classes:0
Info: Time to initialize module:0.05
>>> a = moose.IntFire('a')
Created 123 path=a numData=1 isGlobal=0 baseType=IntFire
>>> b = moose.Synapse('a/b')
Created 125 path=a/b numData=1 isGlobal=0 baseType=Synapse
>>> b.numSynapse = 10
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
AttributeError: 'moose.Synapse' object has no attribute 'numSynapse'
>>> a.numSynapse = 10
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
AttributeError: 'moose.IntFire' object has no attribute 'numSynapse'
>>> dir(a)
['__class__', '__delattr__', '__doc__', '__eq__', '__format__', '__ge__', '__getattribute__', '__gt__', '__hash__', '__init__', '__le__', '__lt__', '__ne__', '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', 'connect', 'ematrix', 'getDataIndex', 'getField', 'getFieldIndex', 'getFieldNames', 'getFieldType', 'getId', 'getLookupField', 'getNeighbors', 'neighbors', 'parentMsg', 'process', 'reinit', 'setDestField', 'setField', 'setLookupField', 'synapse']
>>> a.synapse
/a.synapse
>>> a.synapse[0]
Traceback (most recent call last):
  File "<stdin>", line 1, in <module>
IndexError: moose.ElementField.getItem: index out of bounds.
>>> a.synapse.num = 10
>>> a.synapse[0]
<moose.Synapse: id=124, dataId=0, path=/a/synapse[0]>
>>> a.synapse[1]
<moose.Synapse: id=124, dataId=1, path=/a/synapse[0]>
>>> 

----------------------
Subha, Tue Jan  7 11:58:10 IST 2014

Documentation/discussion on accessing synapses (ElementField):

A `synapse` is an ElementField of an IntFire element. The following
example is taken from Upi and modified to reflect the current API::

   network = moose.IntFire('/network', size)
   network.vec.buffferTime = [delayMax * 2] * size   # `vec` allows vectorized access to fields on all `element`s


IntFire elements have an ElementField called `synapse`. You can access
the first `synapse` element on the first IntFire element with the
following::

   synapse = moose.element('/network/synapse') 

This is equivalent to::

   synapse = moose.element('/network[0]/synapse[0]')

`synapse` is just another element but by default its length is 0
unless you set the number of elements in it either explicitly::

   synapse.num = 10

or in an indirect way::

mid = moose.connect( network, 'spike', synapse, 'addSpike', 'Sparse' )

mid.setRandomConnectivity( connectionProbability, 5489 )
network.vec.Vm = [(Vmax*random.random()) for r in range(size)]
network.vec.thresh = thresh
network.vec.refractoryPeriod = refractoryPeriod
numSynVec = network.numSynapses
numTotSym = sum( numSynVec )
netvec = network.vec
for i in range( size ):
        synvec = netvec[i].synapse.vec
        synvec.weight = [ (random.random() * weightMax) for r in range( synvec.len )]
        synvec.delay = [ (delayMin + random.random() * delayMax) for r in range( synvec.len )]

moose.useClock( '/network', 'process', 0 )
moose.setClock( 0, timestep )
moose.setClock( 9, timestep )
moose.reinit()
network.vec.Vm = [(Vmax*random.random()) for r in range(size)]
moose.start(runtime)
print network.vec[100].Vm, network.vec[900].Vm

"""
import sys
import os
sys.path.append('../../python')
import moose

# Create an IntFire vec containing 10 elements, a refers to alpha[0]
a = moose.IntFire('alpha', 10)
print 'a=', a
for i in range( 10 ):
    syn = moose.SimpleSynHandler( 'alpha[' + str(i) + ']/sh' )
    moose.connect( syn, 'activationOut', a.vec[i], 'activation' )

syn = moose.SimpleSynHandler( 'alpha/sh', 10 )
moose.connect( syn, 'activationOut', a, 'activation', 'OneToOne' )
###############################
# FieldElement identity
###############################
x = syn.synapse # x is an ElementField alpha[0].synapse
print 'x=',x 
print 'x.num=', x.num # Initially there are no synapses, so this will be 0
syn.synapse.num = 3 # We set number of field elements to 3
print 'x.num=', x.num  # x refers to a.synapse, so this should be 3
b = moose.element('alpha[0]/sh/synapse[1]') # We access x[1]
print 'b=',b
print 'x[1]=', x[1]
print 'b==x[1]?', b == x[1]

###############################
# Check fieldIndex and dataId
###############################
print 'syn.synapse[0]=', syn.synapse[0]
print 'syn.synapse[1]=', syn.synapse[1] # The fieldIndex should change, not dataId

#########################
# setVec call example
#########################
print 'alpha[0].synapse.delay=', x.delay
x.delay = [1.0, 2.0, 3.0] # This sets `delay` field in all elements via setVec call
print 'alpha[0].synapse.delay=', x.delay
x.delay = [1.141592] * len(x) # This is a Pythonic way of replicating the values in a list - ensures same length
print 'alpha[0].synapse.delay=', x.delay

#####################################################
# Play a little more with ObjId, FieldElement, Id
#####################################################
print 'Length of alpha[1]/synapse=', len(moose.element('/alpha[1]/sh').synapse)
c = moose.element('alpha[1]/sh/synapse[2]') # This should throw an error - alpha[1] does not have 3 synapses. 
print 'b=', b, 'numData=', b.numData
print 'c=', c, 'numData=', c.numData
try:
    print 'len(c)=', len(c)
except TypeError, e:
    print e
d = moose.element('/alpha[1]/sh')
try:
    print d.synapse[1]
except IndexError, e:
    print e
else:
    print 'Expected an IndexError. Length of synapse=', len(d.synapse)
# The fieldIndex should change, not dataId
x = moose.element(a.vec, 0, 1)
y = moose.element(a.vec, 1, 2)
print x, y
