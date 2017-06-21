#!/usr/bin/env python

"""Compartment.py: 

    A compartment in moose.

Last modified: Tue May 13, 2014  06:03PM

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2013, NCBS Bangalore"
__credits__          = ["NCBS Bangalore", "Bhalla Lab"]
__license__          = "GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"


import unittest
import math

import moose
import moose.utils as utils

class MooseCompartment():
    """A simple class for making MooseCompartment in moose"""

    def __init__(self, path, length, diameter, args):
        """ Initialize moose-compartment """
        self.mc_ = None
        self.path = path
        # Following values are taken from Upi's chapter on Rallpacks
        self.RM = args.get('RM', 4.0)
        self.RA = args.get('RA', 1.0)
        self.CM = args.get('CM', 0.01)
        self.Em = args.get('Em', -0.065)
        self.diameter = diameter
        self.compLength = length
        self.computeParams( )

        try:
            self.mc_ = moose.Compartment(self.path)
            self.mc_.length = self.compLength
            self.mc_.diameter = self.diameter
            self.mc_.Ra = self.Ra
            self.mc_.Rm = self.Rm
            self.mc_.Cm = self.Cm
            self.mc_.Em = self.Em
            self.mc_.initVm = self.Em
            
        except Exception as e:
            utils.dump("ERROR"
                    , [ "Can't create compartment with path %s " % path
                        , "Failed with error %s " % e
                        ]
                    )
            raise
        #utils.dump('DEBUG', [ 'Compartment: {}'.format( self ) ] )


    def __repr__( self ):
        msg = '{}: '.format( self.mc_.path )
        msg += '\n\t|- Length: {:1.4e}, Diameter: {:1.4e}'.format( 
                self.mc_.length, self.mc_.diameter
                )
#        msg += '\n\t|- Cross-section: {:1.4e}, SurfaceArea: {:1.4e}'.format(
#                self.crossSection, self.surfaceArea
#                )
        msg += '\n\t|- Ra: {:1.3e}, Rm: {:1.3e}, Cm: {:1.3e}, Em: {:1.3e}'.format( 
                self.mc_.Ra, self.mc_.Rm, self.mc_.Cm, self.mc_.Em
                )
        return msg

    def __str__( self ):
        return self.__repr__( )

    def computeParams( self ):
        '''Compute essentials paramters for compartment. '''

        self.surfaceArea = math.pi * self.compLength * self.diameter
        self.crossSection = ( math.pi * self.diameter * self.diameter ) / 4.0
        self.Ra = ( self.RA * self.compLength ) / self.crossSection
        self.Rm = ( self.RM / self.surfaceArea )
        self.Cm = ( self.CM * self.surfaceArea ) 


class TestCompartment( unittest.TestCase):
    ''' Test class '''

    def setUp( self ):
        self.dut = MooseCompartment()
        self.dut.createCompartment( path = '/dut1' )

    def test_creation( self ):
        m = MooseCompartment( )
        m.createCompartment( path = '/compartment1' )
        self.assertTrue( m.mc_
                , 'Always create compartments when parent is /.'
                )

        m = MooseCompartment( )
        m.createCompartment( path='/model/compartment1' )
        self.assertFalse ( m.mc_ 
                , 'Should not create compartment when parent does not exists.'
                )
    
    def test_properties( self ):
        m = MooseCompartment()
        m.createCompartment('/comp1')
        self.assertTrue( m.mc_.Em <= 0.0
                , "Em is initialized to some positive value."
                " Current value is %s " % m.mc_.Em 
                )
        self.assertTrue( m.mc_.Rm >= 0.0
                , "Rm should be initialized to non-zero positive float"
                 " Current value is: {}".format( m.mc_.Rm )
                )

    def test_repr ( self ):
        print( self.dut )

if __name__ == "__main__":
    unittest.main()
