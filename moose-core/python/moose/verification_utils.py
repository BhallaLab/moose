

"""verification_utils.py:

    IT contains a class which runs tests on moose internal data-structures to
    check if it is good for simulation.

Last modified: Sun Feb 15, 2015  12:21PM

"""
from __future__ import print_function
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import sys
from . import _moose as moose
import unittest
import inspect
from . import print_utils as debug
import numpy as np
from .backend import backend


class MooseTestCase( unittest.TestCase ):

    def dump(self, msg, end=''):
        ''' Dump the messages in test functions '''
        caller = inspect.stack()[1][3]
        if type(msg) == list:
            msg = '\n\t|- '.join(msg)
        print(('[VERIFY] {:80s}[{}]'.format(msg, caller)))
        
    def setUp(self):
        '''Initialize storehouse
        '''
        if not backend.moose_elems.filled:
            backend.moose_elems.populateStoreHouse()

        self.mooseElems = backend.moose_elems
        self.nonZeroClockIds = None

    def test_disconnected_compartments(self):
        '''Test if any comparment is not connected '''
        self.dump("Checking if any compartment is not connected ...")
        for c in self.mooseElems.compartments:
            if (c.neighbors['axial'] or c.neighbors['raxial']):
                continue
            elif c.neighbors['injectMsg']:
                continue
            else:
                msg = '%s is not connected with any other compartment' % c.path
                debug.dump('FAIL'
                        , [ msg
                            , 'Did you forget to use `moose.connect`?'
                            ]
                        )

    def test_isolated_pulse_gen(self):
        ''' Test if any pulse-generator is not injecting current to a
        compartment
        '''
        self.dump('Checking if any pulse-generator is floating')
        for pg in self.mooseElems.pulseGens:
            if pg.neighbors['output']:
                continue
            else:
                debug.dump(
                        'FAIL'
                        , [ 'Current source {} is floating'.format(pg.path)
                            , 'It is not injecting current to any compartment'
                            , 'Perhaps you forgot to use `moose.connect`?'
                            ]
                        )

    def test_synchans(self):
        self.dump("Checking if any synapse is dead")
        for synchan in self.mooseElems.synchans:
            if synchan.Gbar <= 0.0:
                debug.dump("WARN"
                        , [ synchan.path
                        , "Gbar value is zero or negative: %s" % synchan.Gbar
                        , "Not cool!"
                        ]
                        )
            # Check the output of synchan.
            if not synchan.neighbors['channel']:
                debug.dump("FAIL"
                        , [ "SynChan %s is not connected to post-compartment" % synchan.path
                            , " No connected 'channel'. " 
                             " Did you forget to connect compartment e.g." 
                             "moose.connect(synchan, 'channel', comp, 'channel')"
                             " where synchan is 'moose.SynChan' and comp is "
                             " 'moose.Compartment'?"
                            ]
                        )
            else:
                pass

            # Check if anyone is activating this synchan.
            synhandlers = synchan.neighbors['activation']
            if not synhandlers:
                debug.dump("FAIL"
                        , [ "No SynHandler is activating SynChan %s" % synchan.path
                            , " Did you forget to connect a SynHandler e.g. "
                            "moose.connect(synHandler, 'activationOut', synchan, 'activation'"
                            " where synchan is 'moose.SynChan' and synHandler is"
                            " moose.SynHandler."
                            ]
                        )
            else: [self.test_synhandler(x) for x in synhandlers]

    def test_synhandler(self, synhandlers):
        """A SynHandler object does not have incoming connections to itself.
        Rather it keeps an array of moose.Synapse inside it which recieves input
        of moose.SpikeGen.
        """
        if type(synhandlers) == moose.vec:
            if len(synhandlers) == 1:
                synhandler = synhandlers[0]
            else:
                [self.test_synhandler(x) for x in synhandlers]
        else:
            synhandler = synhandlers
        
        for synapses in synhandler.synapse:
            self.test_synapse(synapses)

    def test_synapse(self, synapses):
        if type(synapses) == moose.Synapse:
            synapse = synapses
        elif type(synapses) == moose.vec:
            if len(synapses) == 1:
                synapse = synapses[0]
            else:
                [ self.test_synapse(x) for x in synapses ]

        spikeGens = synapse.neighbors['addSpike']
        if not spikeGens:
            debug.dump('FAIL'
                    , [" Synapse %s has no incoming spikes" % synapse.path
                        , " Did you forget to connect a moose.SpikeGen e.g."
                        " moose.connect(spikegen, 'spikeOut', synapse, 'addSpike')" 
                        ]
                    )
        else: 
            [self.test_spikegen(x) for x in spikeGens]

    def test_spikegen(self, spikegens):
        spikeGen = None
        if len(spikegens) > 1:
            [self.test_spikegen(x) for x in spikegens]
        elif len(spikegens) == 1:
            spikeGen = spikegens[0]
        elif type(spikegens) == moose.SpikeGen:
            spikeGen = spikegens

        pre = spikeGen.neighbors['Vm']
        if not pre:
            debug.dump('FAIL', 
                    [ "SpikeGen %s is not reading Vm of any compartment " % spikeGen.path
                    , "Did you forget to connect Vm of a "
                    "compartment to this SpikeGen? "
                    " e.g. moose.connect(comp, 'VmOut', spikeGen, 'Vm')"
                    ]
                    )
        else: pass

    
    def test_unused_tables(self):
        '''Tests if any table is not reading data. Such tables remain empty.
        '''
        self.dump('Checking if any table is not connected')
        for table in self.mooseElems.tables:
            if table.neighbors['requestOut']:
                continue
            else:
                debug.dump(
                        'FAIL'
                        , [ 'Table {} is not reading data.'.format(table.path)
                            , ' Did you forget to use `moose.connect`?'
                            ]
                        )

    def test_clocks(self):
        """Tests if clocks are missing. """
        self.dump("Checking if clocks are available")
        try:
            clock = self.mooseElems.clocks[0]
        except:
            debug.dump("WARN", "Could not find any clock")
            return 
        clockDtList = clock.dts
        if np.count_nonzero(clockDtList) < 1:
            debug.dump("FATAL"
                    , [ "No clock is found with non-zero dt size. "
                        , "Did you forget to use `moose.setClock` function?"
                        , "Quitting..." 
                        ]
                    )
            sys.exit(0)
        else:
            self.nonZeroClockIds = np.nonzero(self.mooseElems.clocks)

    def test_methods_sensitivity(self):
        """Test if each compartment has process connected to a non-zero clock"""
        self.dump("Checking for insensitive processes")
        [ self.checkSentitivity( m, objs) 
                for m in ['process', 'init']  
                for objs in [self.mooseElems.compartments] 
                ]
        [self.checkSentitivity('process', objs)
                for objs in [self.mooseElems.tables, self.mooseElems.pulseGens]
                ]


    def checkSentitivity( self, methodName, objectList):
        """Check if a given method is sensitive to any non-zero clock 
        """
        assert type(methodName) == str
        insensitiveObjectList = []
        for obj in objectList:
            if not obj.neighbors[methodName]:
                insensitiveObjectList.append(obj)
            else:
                # Here we must check if method is made sensitive to a
                # zero-clock. Currently there is no way to test it in python.
                pass

        if len(insensitiveObjectList) > 0:
            msgList = [
                    "Method `%s` is insensitive to all clocks. " % methodName
                    , "Total {} out of {} object ({}) fails this test".format(
                        len(insensitiveObjectList)
                        , len(objectList)
                        , type(insensitiveObjectList[0])
                        )
                    ]
            debug.dump("FAIL", msgList)

def verify( *args, **kwargs):
    '''Verify the current moose setup. Emit errors and warnings 
    '''
    connectivitySuite = unittest.TestSuite()
    connectivitySuite.addTest(MooseTestCase('test_disconnected_compartments'))
    connectivitySuite.addTest(MooseTestCase('test_isolated_pulse_gen'))
    connectivitySuite.addTest(MooseTestCase('test_unused_tables'))
    connectivitySuite.addTest(MooseTestCase('test_synchans'))

    simulationSuite = unittest.TestSuite()
    simulationSuite.addTest(MooseTestCase('test_clocks'))
    simulationSuite.addTest(MooseTestCase('test_methods_sensitivity'))

    # We can replace self with run also and collect the result into a result
    # object.
    connectivitySuite.debug()
    simulationSuite.debug()

