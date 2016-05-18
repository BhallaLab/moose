# minimal.py --- 
# Upi Bhalla, NCBS Bangalore 2014.
#
# Commentary: 
# 
# Minimal model for loading rdesigneur: reac-diff elec signaling in neurons
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
# 

# Code:

import sys
sys.path.append('../../python')
import os
os.environ['NUMPTHREADS'] = '1'
import math
import numpy
import matplotlib.pyplot as plt
import moose
import proto18

EREST_ACT = -70e-3

def loadElec():
	library = moose.Neutral( '/library' )
	moose.setCwe( '/library' )
	proto18.make_Ca()
	proto18.make_Ca_conc()
	proto18.make_K_AHP()
	proto18.make_K_C()
	proto18.make_Na()
	proto18.make_K_DR()
	proto18.make_K_A()
	proto18.make_glu()
	proto18.make_NMDA()
	proto18.make_Ca_NMDA()
	proto18.make_NMDA_Ca_conc()
	proto18.make_axon()
	moose.setCwe( '/library' )
	model = moose.Neutral( '/model' )
	cellId = moose.loadModel( 'mincell2.p', '/model/elec', "Neutral" )
	return cellId

def loadChem( diffLength ):
	chem = moose.Neutral( '/model/chem' )
	neuroCompt = moose.NeuroMesh( '/model/chem/kinetics' )
	neuroCompt.separateSpines = 1
	neuroCompt.geometryPolicy = 'cylinder'
	spineCompt = moose.SpineMesh( '/model/chem/compartment_1' )
	moose.connect( neuroCompt, 'spineListOut', spineCompt, 'spineList', 'OneToOne' )
	psdCompt = moose.PsdMesh( '/model/chem/compartment_2' )
	#print 'Meshvolume[neuro, spine, psd] = ', neuroCompt.mesh[0].volume, spineCompt.mesh[0].volume, psdCompt.mesh[0].volume
	moose.connect( neuroCompt, 'psdListOut', psdCompt, 'psdList', 'OneToOne' )
	modelId = moose.loadModel( 'minimal.g', '/model/chem', 'ee' )
        neuroCompt.name = 'dend'
        spineCompt.name = 'spine'
        psdCompt.name = 'psd'


def makeNeuroMeshModel():
	diffLength = 6e-6 # Aim for 2 soma compartments.
	elec = loadElec()
	loadChem( diffLength )
        neuroCompt = moose.element( '/model/chem/dend' )
	neuroCompt.diffLength = diffLength
	neuroCompt.cellPortion( elec, '/model/elec/#' )
	for x in moose.wildcardFind( '/model/chem/##[ISA=PoolBase]' ):
		if (x.diffConst > 0):
			x.diffConst = 1e-11
	for x in moose.wildcardFind( '/model/chem/##/Ca' ):
		x.diffConst = 1e-10

	# Put in dend solvers
	ns = neuroCompt.numSegments
	ndc = neuroCompt.numDiffCompts
        print 'ns = ', ns, ', ndc = ', ndc
        assert( neuroCompt.numDiffCompts == neuroCompt.mesh.num )
	assert( ns == 1 ) # soma/dend only
	assert( ndc == 2 ) # split into 2.
	nmksolve = moose.Ksolve( '/model/chem/dend/ksolve' )
	nmdsolve = moose.Dsolve( '/model/chem/dend/dsolve' )
        nmstoich = moose.Stoich( '/model/chem/dend/stoich' )
        nmstoich.compartment = neuroCompt
        nmstoich.ksolve = nmksolve
        nmstoich.dsolve = nmdsolve
        nmstoich.path = "/model/chem/dend/##"
        print 'done setting path, numPools = ', nmdsolve.numPools
        assert( nmdsolve.numPools == 1 )
        assert( nmdsolve.numAllVoxels == 2 )
        assert( nmstoich.numAllPools == 1 )
	# oddly, numLocalFields does not work.
	ca = moose.element( '/model/chem/dend/DEND/Ca' )
	assert( ca.numData == ndc )
        
        # Put in spine solvers. Note that these get info from the neuroCompt
        spineCompt = moose.element( '/model/chem/spine' )
	sdc = spineCompt.mesh.num
        print 'sdc = ', sdc
	assert( sdc == 1 )
	smksolve = moose.Ksolve( '/model/chem/spine/ksolve' )
	smdsolve = moose.Dsolve( '/model/chem/spine/dsolve' )
        smstoich = moose.Stoich( '/model/chem/spine/stoich' )
        smstoich.compartment = spineCompt
        smstoich.ksolve = smksolve
        smstoich.dsolve = smdsolve
        smstoich.path = "/model/chem/spine/##"
        assert( smstoich.numAllPools == 3 )
        assert( smdsolve.numPools == 3 )
        assert( smdsolve.numAllVoxels == 1 )
        
        # Put in PSD solvers. Note that these get info from the neuroCompt
        psdCompt = moose.element( '/model/chem/psd' )
	pdc = psdCompt.mesh.num
	assert( pdc == 1 )
	pmksolve = moose.Ksolve( '/model/chem/psd/ksolve' )
	pmdsolve = moose.Dsolve( '/model/chem/psd/dsolve' )
        pmstoich = moose.Stoich( '/model/chem/psd/stoich' )
        pmstoich.compartment = psdCompt
        pmstoich.ksolve = pmksolve
        pmstoich.dsolve = pmdsolve
        pmstoich.path = "/model/chem/psd/##"
        assert( pmstoich.numAllPools == 3 )
        assert( pmdsolve.numPools == 3 )
        assert( pmdsolve.numAllVoxels == 1 )
        foo = moose.element( '/model/chem/psd/Ca' )
        print 'PSD: numfoo = ', foo.numData
        print 'PSD: numAllVoxels = ', pmksolve.numAllVoxels

        # Put in junctions between the diffusion solvers
        nmdsolve.buildNeuroMeshJunctions( smdsolve, pmdsolve )

	"""
	CaNpsd = moose.vec( '/model/chem/psdMesh/PSD/PP1_PSD/CaN' )
	print 'numCaN in PSD = ', CaNpsd.nInit, ', vol = ', CaNpsd.volume
	CaNspine = moose.vec( '/model/chem/spine/SPINE/CaN_BULK/CaN' )
	print 'numCaN in spine = ', CaNspine.nInit, ', vol = ', CaNspine.volume
	"""

	# set up adaptors
	aCa = moose.Adaptor( '/model/chem/dend/DEND/adaptCa', ndc )
	adaptCa = moose.vec( '/model/chem/dend/DEND/adaptCa' )
	chemCa = moose.vec( '/model/chem/dend/DEND/Ca' )
	print 'aCa = ', aCa, ' foo = ', foo, "len( ChemCa ) = ", len( chemCa ), ", numData = ", chemCa.numData, "len( adaptCa ) = ", len( adaptCa )
	assert( len( adaptCa ) == ndc )
	assert( len( chemCa ) == ndc )
	path = '/model/elec/soma/Ca_conc'
	elecCa = moose.element( path )
        print "=========="
        print elecCa
        print adaptCa
        print chemCa
	moose.connect( elecCa, 'concOut', adaptCa[0], 'input', 'Single' )
	moose.connect( adaptCa, 'output', chemCa, 'setConc', 'OneToOne' )
	adaptCa.inputOffset = 0.0	# 
	adaptCa.outputOffset = 0.00008	# 80 nM offset in chem.
   	adaptCa.scale = 1e-3	# 520 to 0.0052 mM
	#print adaptCa.outputOffset
	#print adaptCa.scale


def addPlot( objpath, field, plot ):
	#assert moose.exists( objpath )
	if moose.exists( objpath ):
		tab = moose.Table( '/graphs/' + plot )
		obj = moose.element( objpath )
		if obj.className == 'Neutral':
			print "addPlot failed: object is a Neutral: ", objpath
			return moose.element( '/' )
		else:
			#print "object was found: ", objpath, obj.className
			moose.connect( tab, 'requestOut', obj, field )
			return tab
	else:
		print "addPlot failed: object not found: ", objpath
		return moose.element( '/' )

def makeElecPlots():
    graphs = moose.Neutral( '/graphs' )
    elec = moose.Neutral( '/graphs/elec' )
    addPlot( '/model/elec/soma', 'getVm', 'elec/somaVm' )
    addPlot( '/model/elec/spine_head', 'getVm', 'elec/spineVm' )
    addPlot( '/model/elec/soma/Ca_conc', 'getCa', 'elec/somaCa' )

def makeChemPlots():
	graphs = moose.Neutral( '/graphs' )
	chem = moose.Neutral( '/graphs/chem' )
	addPlot( '/model/chem/psd/Ca_CaM', 'getConc', 'chem/psdCaCam' )
	addPlot( '/model/chem/psd/Ca', 'getConc', 'chem/psdCa' )
	addPlot( '/model/chem/spine/Ca_CaM', 'getConc', 'chem/spineCaCam' )
	addPlot( '/model/chem/spine/Ca', 'getConc', 'chem/spineCa' )
	addPlot( '/model/chem/dend/DEND/Ca', 'getConc', 'chem/dendCa' )

def testNeuroMeshMultiscale():
	elecDt = 50e-6
	chemDt = 0.01
	ePlotDt = 0.5e-3
	cPlotDt = 0.01
	plotName = 'nm.plot'

	makeNeuroMeshModel()
	print "after model is completely done"
	for i in moose.wildcardFind( '/model/chem/#/#/#/transloc#' ):
		print i[0].name, i[0].Kf, i[0].Kb, i[0].kf, i[0].kb

	"""
	for i in moose.wildcardFind( '/model/chem/##[ISA=PoolBase]' ):
		if ( i[0].diffConst > 0 ):
			grandpaname = i.parent[0].parent.name + '/'
			paname = i.parent[0].name + '/'
			print grandpaname + paname + i[0].name, i[0].diffConst
	print 'Neighbors:'
	for t in moose.element( '/model/chem/spine/ksolve/junction' ).neighbors['masterJunction']:
		print 'masterJunction <-', t.path
	for t in moose.wildcardFind( '/model/chem/#/ksolve' ):
		k = moose.element( t[0] )
		print k.path + ' localVoxels=', k.numLocalVoxels, ', allVoxels= ', k.numAllVoxels
	"""
        '''
        moose.useClock( 4, '/model/chem/dend/dsolve', 'process' )
        moose.useClock( 5, '/model/chem/dend/ksolve', 'process' )
        moose.useClock( 5, '/model/chem/spine/ksolve', 'process' )
        moose.useClock( 5, '/model/chem/psd/ksolve', 'process' )
        '''

	makeChemPlots()
	makeElecPlots()
	moose.setClock( 0, elecDt )
	moose.setClock( 1, elecDt )
	moose.setClock( 2, elecDt )
	moose.setClock( 4, chemDt )
	moose.setClock( 5, chemDt )
	moose.setClock( 6, chemDt )
	moose.setClock( 7, cPlotDt )
	moose.setClock( 8, ePlotDt )
	moose.useClock( 0, '/model/elec/##[ISA=Compartment]', 'init' )
	moose.useClock( 1, '/model/elec/##[ISA=Compartment]', 'process' )
	moose.useClock( 1, '/model/elec/##[ISA=SpikeGen]', 'process' )
	moose.useClock( 2, '/model/elec/##[ISA=ChanBase],/model/##[ISA=SynBase],/model/##[ISA=CaConc]','process')
	#moose.useClock( 5, '/model/chem/##[ISA=PoolBase],/model/##[ISA=ReacBase],/model/##[ISA=EnzBase]', 'process' )
	#moose.useClock( 4, '/model/chem/##[ISA=Adaptor]', 'process' )
	moose.useClock( 4, '/model/chem/#/dsolve', 'process' )
	moose.useClock( 5, '/model/chem/#/ksolve', 'process' )
	moose.useClock( 6, '/model/chem/dend/DEND/adaptCa', 'process' )
	moose.useClock( 7, '/graphs/chem/#', 'process' )
	moose.useClock( 8, '/graphs/elec/#', 'process' )
	#hsolve = moose.HSolve( '/model/elec/hsolve' )
	#moose.useClock( 1, '/model/elec/hsolve', 'process' )
	#hsolve.dt = elecDt
	#hsolve.target = '/model/elec/compt'
	#moose.reinit()
        moose.element( '/model/elec/spine_head' ).inject = 5e-12
        moose.element( '/model/chem/psd/Ca' ).concInit = 0.001
        moose.element( '/model/chem/spine/Ca' ).concInit = 0.002
        moose.element( '/model/chem/dend/DEND/Ca' ).concInit = 0.003
	moose.reinit()
	"""
	print 'pre'
	eca = moose.vec( '/model/chem/psd/PSD/CaM/Ca' )
	for i in range( 3 ):
		print eca[i].concInit, eca[i].conc, eca[i].nInit, eca[i].n, eca[i].volume
	print 'dend'
	eca = moose.vec( '/model/chem/dend/DEND/Ca' )
	#for i in ( 0, 1, 2, 30, 60, 90, 120, 144 ):
	for i in range( 13 ):
		print i, eca[i].concInit, eca[i].conc, eca[i].nInit, eca[i].n, eca[i].volume

	print 'PSD'
	eca = moose.vec( '/model/chem/psd/PSD/CaM/Ca' )
	for i in range( 3 ):
		print eca[i].concInit, eca[i].conc, eca[i].nInit, eca[i].n, eca[i].volume
	print 'spine'
	eca = moose.vec( '/model/chem/spine/SPINE/CaM/Ca' )
	for i in range( 3 ):
		print eca[i].concInit, eca[i].conc, eca[i].nInit, eca[i].n, eca[i].volume
	"""

	moose.start( 0.5 )
        plt.ion()
        fig = plt.figure( figsize=(8,8) )
        chem = fig.add_subplot( 211 )
        chem.set_ylim( 0, 0.004 )
        plt.ylabel( 'Conc (mM)' )
        plt.xlabel( 'time (seconds)' )
        for x in moose.wildcardFind( '/graphs/chem/#[ISA=Table]' ):
            pos = numpy.arange( 0, x.vector.size, 1 ) * cPlotDt
            line1, = chem.plot( pos, x.vector, label=x.name )
        plt.legend()

        elec = fig.add_subplot( 212 )
        plt.ylabel( 'Vm (V)' )
        plt.xlabel( 'time (seconds)' )
        for x in moose.wildcardFind( '/graphs/elec/#[ISA=Table]' ):
            pos = numpy.arange( 0, x.vector.size, 1 ) * ePlotDt
            line1, = elec.plot( pos, x.vector, label=x.name )
        plt.legend()

        fig.canvas.draw()
        raw_input()
                
        '''
        for x in moose.wildcardFind( '/graphs/##[ISA=Table]' ):
            t = numpy.arange( 0, x.vector.size, 1 )
            pylab.plot( t, x.vector, label=x.name )
        pylab.legend()
        pylab.show()
        '''

        pylab.show()
	print 'All done'


def main():
	testNeuroMeshMultiscale()

if __name__ == '__main__':
	main()

# 
# minimal.py ends here.
