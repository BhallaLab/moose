# __DEPRECATED__ __BROKEN__
# loadMulti.py --- 
# Upi Bhalla, NCBS Bangalore 2013.
#
# Commentary: 
# 
# Testing system for loading in arbirary multiscale models based on
# model definition files.
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
import os
import math

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
	model = moose.Neutral( '/model' )
	cellId = moose.loadModel( 'ca1_asym.p', '/model/elec', "Neutral" )
	return cellId

def addPlot( objpath, field, plot ):
	#assert moose.exists( objpath )
	if moose.exists( objpath ):
		tab = moose.Table( '/graphs/' + plot )
		obj = moose.element( objpath )
		if obj.className == 'Neutral':
			print("addPlot failed: object is a Neutral: ", objpath)
			return moose.element( '/' )
		else:
			#print "object was found: ", objpath, obj.className
			moose.connect( tab, 'requestOut', obj, field )
			return tab
	else:
		print("addPlot failed: object not found: ", objpath)
		return moose.element( '/' )

def dumpPlots( fname ):
    if ( os.path.exists( fname ) ):
        os.remove( fname )
    for x in moose.wildcardFind( '/graphs/#[ISA=Table]' ):
        moose.element( x[0] ).xplot( fname, x[0].name )
    for x in moose.wildcardFind( '/graphs/elec/#[ISA=Table]' ):
        moose.element( x[0] ).xplot( fname, x[0].name + '_e' )

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
	modelId = moose.loadModel( 'psd_merged31d.g', '/model/chem', 'ee' )
        neuroCompt.name = 'dend'
        spineCompt.name = 'spine'
        psdCompt.name = 'psd'


def makeNeuroMeshModel():
	diffLength = 10e-6 # But we only want diffusion over part of the model.
	numSyn = 13
	elec = loadElec()
	synInput = moose.SpikeGen( '/model/elec/synInput' )
	synInput.threshold = -1.0
	synInput.edgeTriggered = 0
	synInput.Vm( 0 )
	synInput.refractT = 47e-3

	for i in range( numSyn ):
		name = '/model/elec/spine_head_14_' + str( i + 1 )
		r = moose.element( name + '/glu' )
		r.synapse.num = 1 
		syn = moose.element( r.path + '/synapse' )
		moose.connect( synInput, 'spikeOut', syn, 'addSpike', 'Single' )
		syn.weight = 0.2 * i * ( numSyn - 1 - i )
		syn.delay = i * 1.0e-3

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
        print('ns = ', ns, ', ndc = ', ndc)
        assert( neuroCompt.numDiffCompts == neuroCompt.mesh.num )
	assert( ns == 36 ) # dend, 5x (shaft+head)
	assert( ndc == 278 )
	nmksolve = moose.Ksolve( '/model/chem/dend/ksolve' )
	nmdsolve = moose.Dsolve( '/model/chem/dend/dsolve' )
        nmstoich = moose.Stoich( '/model/chem/dend/stoich' )
        nmstoich.compartment = neuroCompt
        nmstoich.ksolve = nmksolve
        nmstoich.dsolve = nmdsolve
        nmstoich.path = "/model/chem/dend/##"
        print('done setting path, numPools = ', nmdsolve.numPools)
        assert( nmdsolve.numPools == 1 )
	# oddly, numLocalFields does not work.
	ca = moose.element( '/model/chem/dend/DEND/Ca' )
	assert( ca.numData == ndc )
        
        # Put in spine solvers. Note that these get info from the neuroCompt
        spineCompt = moose.element( '/model/chem/spine' )
	sdc = spineCompt.mesh.num
        print('sdc = ', sdc)
	assert( sdc == 13 )
	smksolve = moose.Ksolve( '/model/chem/spine/ksolve' )
	smdsolve = moose.Dsolve( '/model/chem/spine/dsolve' )
        smstoich = moose.Stoich( '/model/chem/spine/stoich' )
        smstoich.compartment = spineCompt
        smstoich.ksolve = smksolve
        smstoich.dsolve = smdsolve
        smstoich.path = "/model/chem/spine/##"
        assert( smstoich.numAllPools == 36 )
        
        # Put in PSD solvers. Note that these get info from the neuroCompt
        psdCompt = moose.element( '/model/chem/psd' )
	pdc = psdCompt.mesh.num
	assert( pdc == 13 )
	pmksolve = moose.Ksolve( '/model/chem/psd/ksolve' )
	pmdsolve = moose.Dsolve( '/model/chem/psd/dsolve' )
        pmstoich = moose.Stoich( '/model/chem/psd/stoich' )
        pmstoich.compartment = psdCompt
        pmstoich.ksolve = pmksolve
        pmstoich.dsolve = pmdsolve
        pmstoich.path = "/model/chem/psd/##"
        print('numAllPools = ', pmstoich.numAllPools)
        assert( pmstoich.numAllPools == 56 )
        foo = moose.element( '/model/chem/psd/Ca' )
        bar = moose.element( '/model/chem/psd/I1_p' )
        print('PSD: numfoo = ', foo.numData, 'numbar = ', bar.numData)
        print('PSD: numAllVoxels = ', pmksolve.numAllVoxels)

        # Put in junctions between diffusion solvers
        nmdsolve.buildNeuroMeshJunctions( smdsolve, pmdsolve )

	"""
	CaNpsd = moose.vec( '/model/chem/psdMesh/PSD/PP1_PSD/CaN' )
	print 'numCaN in PSD = ', CaNpsd.nInit, ', vol = ', CaNpsd.volume
	CaNspine = moose.vec( '/model/chem/spine/SPINE/CaN_BULK/CaN' )
	print 'numCaN in spine = ', CaNspine.nInit, ', vol = ', CaNspine.volume
	"""

	# set up adaptors
	aCa = moose.Adaptor( '/model/chem/psd/adaptCa', pdc )
	adaptCa = moose.vec( '/model/chem/psd/adaptCa' )
	chemCa = moose.vec( '/model/chem/psd/Ca' )
	print('aCa = ', aCa, ' foo = ', foo, "len( ChemCa ) = ", len( chemCa ), ", numData = ", chemCa.numData)
	assert( len( adaptCa ) == pdc )
	assert( len( chemCa ) == pdc )
	for i in range( pdc ):
		path = '/model/elec/spine_head_14_' + str( i + 1 ) + '/NMDA_Ca_conc'
		elecCa = moose.element( path )
		moose.connect( elecCa, 'concOut', adaptCa[i], 'input', 'Single' )
		moose.connect( adaptCa, 'output', chemCa, 'setConc', 'OneToOne' )
	        adaptCa[i].inputOffset = 0.0	# 
	        adaptCa[i].outputOffset = 0.00008  # 80 nM offset in chem.
   	        adaptCa[i].scale = 1e-5	# 520 to 0.0052 mM
	#print adaptCa.outputOffset
	#print adaptCa.scale
        """
        """

	"""
	aGluR = moose.Adaptor( '/model/chem/psd/adaptGluR', 5 )
    adaptGluR = moose.vec( '/model/chem/psd/adaptGluR' )
	chemR = moose.vec( '/model/chem/psd/psdGluR' )
	assert( len( adaptGluR ) == 5 )
	for i in range( 5 ):
    	path = '/model/elec/head' + str( i ) + '/gluR'
		elecR = moose.element( path )
			moose.connect( adaptGluR[i], 'outputSrc', elecR, 'setGbar', 'Single' )
    #moose.connect( chemR, 'nOut', adaptGluR, 'input', 'OneToOne' )
	# Ksolve isn't sending nOut. Not good. So have to use requestOut.
    moose.connect( adaptGluR, 'requestOut', chemR, 'getN', 'OneToOne' )
    adaptGluR.outputOffset = 1e-7    # pS
    adaptGluR.scale = 1e-6 / 100     # from n to pS

    adaptK = moose.Adaptor( '/model/chem/dend/adaptK' )
    chemK = moose.element( '/model/chem/dend/kChan' )
    elecK = moose.element( '/model/elec/compt/K' )
	moose.connect( adaptK, 'requestOut', chemK, 'getConc', 'OneToAll' )
	moose.connect( adaptK, 'outputSrc', elecK, 'setGbar', 'Single' )
	adaptK.scale = 0.3               # from mM to Siemens
	"""

def makeElecPlots():
    graphs = moose.Neutral( '/graphs' )
    elec = moose.Neutral( '/graphs/elec' )
    addPlot( '/model/elec/soma', 'getVm', 'elec/somaVm' )
    addPlot( '/model/elec/soma/Ca_conc', 'getCa', 'elec/somaCa' )
    addPlot( '/model/elec/basal_3', 'getVm', 'elec/basal3Vm' )
    addPlot( '/model/elec/apical_14', 'getVm', 'elec/apical_14Vm' )
    addPlot( '/model/elec/apical_14/Ca_conc', 'getCa', 'elec/apical_14Ca' )
    addPlot( '/model/elec/spine_head_14_7', 'getVm', 'elec/spine_7Vm' )
    addPlot( '/model/elec/spine_head_14_7/NMDA_Ca_conc', 'getCa', 'elec/spine_7Ca' )
    addPlot( '/model/elec/spine_head_14_13/NMDA_Ca_conc', 'getCa', 'elec/spine_13Ca' )

def makeChemPlots():
	spineMesh = moose.element( '/model/chem/spine' )
	middleSpine = 6
	# handy lookup function to find which voxel # the spine is on.
	midSpineVoxel = spineMesh.parentVoxel[middleSpine]
        print(spineMesh.parentVoxel)
	graphs = moose.Neutral( '/graphs' )
	addPlot( '/model/chem/psd/tot_PSD_R[0]', 'getN', 'psd0R' )
	addPlot( '/model/chem/psd/tot_PSD_R[1]', 'getN', 'psd1R' )
	addPlot( '/model/chem/psd/tot_PSD_R[2]', 'getN', 'psd2R' )
	addPlot( '/model/chem/psd/CaM_dash_Ca4[0]', 'getConc', 'psdCaM0' )
	addPlot( '/model/chem/psd/CaM_dash_Ca4[6]', 'getConc', 'psdCaM6' )
	addPlot( '/model/chem/psd/CaM_CaN[0]', 'getConc', 'CaM_CaN' )
	addPlot( '/model/chem/psd/PP1_dash_active[0]', 'getConc', 'PP1_active0' )
	addPlot( '/model/chem/psd/PP1_dash_active[6]', 'getConc', 'psdPP1_active6' )
	addPlot( '/model/chem/psd/actCaMKII[6]', 'getConc', 'psdCaMKII6' )
	addPlot( '/model/chem/spine/actCaMKII[6]', 'getConc', 'spineCaMKII6' )
	addPlot( '/model/chem/psd/Ca[0]', 'getConc', 'psd0Ca' )
	addPlot( '/model/chem/psd/Ca[6]', 'getConc', 'psd6Ca' )
	addPlot( '/model/chem/psd/Ca[12]', 'getConc', 'psd12Ca' )

	addPlot( '/model/chem/spine/Ca[0]', 'getConc', 'spine0Ca' )
	addPlot( '/model/chem/spine/Ca[6]', 'getConc', 'spine6Ca' )
	addPlot( '/model/chem/spine/Ca[12]', 'getConc', 'spine12Ca' )

	addPlot( '/model/chem/dend/DEND/Ca[0]', 'getConc', 'dend0Ca' )
	path = '/model/chem/dend/DEND/Ca['
	addPlot( path + str( midSpineVoxel ) + ']', 'getConc', 'dendMidCa' )
	addPlot( path + str( midSpineVoxel+2 ) + ']', 'getConc', 'dendMid2Ca' )
	addPlot( path + str( midSpineVoxel+4 ) + ']', 'getConc', 'dendMid4Ca' )
	addPlot( path + str( midSpineVoxel+6 ) + ']', 'getConc', 'dendMid6Ca' )
	addPlot( '/model/chem/dend/DEND/Ca[144]', 'getConc', 'dend144Ca' )

	#addPlot( '/n/dend/Ca', 'getConc', 'dendCa' )
	#addPlot( '/n/dend/inact_kinase', 'getConc', 'inactDendKinase' )
	#addPlot( '/n/psd/psdGluR', 'getN', 'psdGluR' )
	addPlot( '/model/chem/psd/CaMKII_dash_CaM[6]', 'getConc', 'psdCaM_CaMKII6' )
	addPlot( '/model/chem/spine/CaMKII_dash_CaM[6]', 'getConc', 'spineCaM_CaMKII6' )
	addPlot( '/model/chem/spine/NMDAR[6]', 'getConc', 'psd_NMDAR6' )


def testNeuroMeshMultiscale():
	elecDt = 50e-6
	chemDt = 1e-2
	cplotDt = 1e-2
	eplotDt = 1e-3
	plotName = 'nm.plot'

	makeNeuroMeshModel()
	print("after model is completely done")
	for i in moose.wildcardFind( '/model/chem/#/#/#/transloc#' ):
		print(i[0].name, i[0].Kf, i[0].Kb, i[0].kf, i[0].kb)

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
	"""
        moose.useClock( 4, '/model/chem/dend/dsolve', 'process' )
        moose.useClock( 5, '/model/chem/dend/ksolve', 'process' )
        moose.useClock( 5, '/model/chem/spine/ksolve', 'process' )
        moose.useClock( 5, '/model/chem/psd/ksolve', 'process' )
	"""

	makeChemPlots()
	makeElecPlots()
	moose.setClock( 0, elecDt )
	moose.setClock( 1, elecDt )
	moose.setClock( 2, elecDt )
	moose.setClock( 4, chemDt )
	moose.setClock( 5, chemDt )
	moose.setClock( 6, chemDt )
	moose.setClock( 7, cplotDt )
	moose.setClock( 8, eplotDt )
	moose.useClock( 0, '/model/elec/##[ISA=Compartment]', 'init' )
	moose.useClock( 1, '/model/elec/##[ISA=Compartment]', 'process' )
	moose.useClock( 1, '/model/elec/##[ISA=SpikeGen]', 'process' )
	moose.useClock( 2, '/model/elec/##[ISA=ChanBase],/model/##[ISA=SynBase],/model/##[ISA=CaConc]','process')
	moose.useClock( 4, '/model/chem/#/dsolve', 'process' )
	moose.useClock( 5, '/model/chem/#/ksolve', 'process' )
	moose.useClock( 6, '/model/chem/psd/adaptCa', 'process' )
	moose.useClock( 7, '/graphs/#', 'process' )
	moose.useClock( 8, '/graphs/elec/#', 'process' )
	#hsolve = moose.HSolve( '/model/elec/hsolve' )
	#moose.useClock( 1, '/model/elec/hsolve', 'process' )
	#hsolve.dt = elecDt
	#hsolve.target = '/model/elec/compt'
	#moose.reinit()
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
	dumpPlots( plotName )
	print('All done')


def main():
	testNeuroMeshMultiscale()

if __name__ == '__main__':
	main()

# 
# loadMulti.py ends here.
