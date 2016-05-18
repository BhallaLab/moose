# __DEPRECATED__ __BROKEN__
# minimal.py --- 
# Upi Bhalla, NCBS Bangalore 2013.
#
# Commentary: 
# 
# Testing system for loading in arbitrary multiscale models based on
# model definition files.
# This version has a minimal model with Ca in all 3 compartments,
# and CaM (just one step) in SPINE and PSD, with a nominal product Ca.CaM.
# In this diffusionOnly version there is no reaction. To be more precise,
# the loaded in minimal.g model has the Ca-binding reaction, but it is 
# deleted when the file is loaded in.
# Incoming Ca from synaptic events comes to the PSD.
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
	cellId = moose.loadModel( 'ca1_asym.p', '/model/elec', "hsolve" )
	return cellId

def addPlot( objpath, field, plot ):
	#assert moose.exists( objpath )
	if ( moose.exists( objpath ) ):
		tab = moose.Table( '/graphs/' + plot )
		obj = moose.element( objpath )
		moose.connect( tab, 'requestOut', obj, field )


def dumpPlots( fname ):
    if ( os.path.exists( fname ) ):
        os.remove( fname )
    for x in moose.wildcardFind( '/graphs/#[ISA=Table]' ):
        moose.element( x[0] ).xplot( fname, x[0].name )
    for x in moose.wildcardFind( '/graphs/elec/#[ISA=Table]' ):
        moose.element( x[0] ).xplot( fname, x[0].name + '_e' )

def moveCompt( path, oldParent, newParent ):
	meshEntries = moose.element( newParent.path + '/mesh' )
	# Set up vol messaging from new compts to all their child objects.
	for x in moose.wildcardFind( path + '/##[ISA=PoolBase]' ):
		moose.connect( meshEntries, 'mesh', x, 'mesh', 'OneToOne' )
	#print 'path = ', path, ', oldparent = ', oldParent.path
	orig = moose.element( path )
	moose.move( orig, newParent )
	moose.delete( moose.vec( oldParent.path ) )
	chem = moose.element( '/model/chem' )
	moose.move( newParent, chem )

def loadChem( neuroCompt, spineCompt, psdCompt ):
	# We need the compartments to come in with a volume of 1 to match the
	# original CubeMesh.
	assert( neuroCompt.volume == 1.0 )
	assert( spineCompt.volume == 1.0 )
	assert( psdCompt.volume == 1.0 )
	assert( neuroCompt.mesh.num == 1 )
	print('volume = ', neuroCompt.mesh[0].volume)
	#assert( neuroCompt.mesh[0].volume == 1.0 ) 
	#an unfortunate mismatch
	# So we'll have to resize the volumes of the current compartments to the
	# new ones.

	modelId = moose.loadModel( 'diffonly.g', '/model', 'ee' )
	#moose.le( '/model/model' )
	#moose.le( '/model/model/kinetics' )
	#moose.le( '/model/model/kinetics/PSD' )
	#moose.le( '/model/model/kinetics/SPINE' )
	moose.delete( moose.vec( '/model/model/kinetics/PSD/kreac' ) )
	moose.delete( moose.vec( '/model/model/kinetics/SPINE/kreac' ) )
	#moose.le( '/model/model/kinetics/PSD' )
	#moose.le( '/model/model/kinetics/SPINE' )
	pCaCaM = moose.element( '/model/model/kinetics/PSD/Ca_CaM' )
	pCaCaM.concInit = 0.001
	dCaCaM = moose.element( '/model/model/kinetics/PSD/Ca_CaM' )
	sCaCaM = moose.element( '/model/model/kinetics/SPINE/Ca_CaM' )
	print("CaCaM.concInit[p,s,d] = ", pCaCaM.concInit, sCaCaM.concInit, dCaCaM.concInit)
	#moose.delete( moose.vec( '/model/model/kinetics/SPINE/Ca_CaM' ) )
	#CaCaM2 = moose.element( '/model/model/kinetics/SPINE/Ca_CaM' )
	#CaCaM2.concInit = 0.001
	chem = moose.element( '/model/model' )
	chem.name = 'chem'
	oldS = moose.element( '/model/chem/compartment_1' )
	oldP = moose.element( '/model/chem/compartment_2' )
	oldN = moose.element( '/model/chem/kinetics' )
	print('oldvols[p,s,d] = ', oldP.volume, oldS.volume, oldN.volume)
	print('newvols[p,s,d] = ', psdCompt.mesh[0].volume, spineCompt.mesh[0].volume, neuroCompt.mesh[0].volume)
	oldN.volume = neuroCompt.mesh[0].volume
	oldS.volume = spineCompt.mesh[0].volume
	oldP.volume = psdCompt.mesh[0].volume
	print('after redoing vols')
	print("CaCaM.concInit[p,s,d] = ", pCaCaM.concInit, sCaCaM.concInit, dCaCaM.concInit)
	moveCompt( '/model/chem/kinetics/SPINE', oldS, spineCompt )
	moveCompt( '/model/chem/kinetics/PSD', oldP, psdCompt )
	# Need to do the DEND last because the oldN is /kinetics, 
	# and it will be deleted.
	moveCompt( '/model/chem/kinetics/DEND', oldN, neuroCompt )
	print('after moving to new compts')
	print("CaCaM.concInit[p,s,d] = ", pCaCaM.concInit, sCaCaM.concInit, dCaCaM.concInit)

def makeNeuroMeshModel():
	diffLength = 20e-6 # But we only want diffusion over part of the model.
	numSyn = 13
	elec = loadElec()
	synInput = moose.SpikeGen( '/model/elec/synInput' )
	synInput.refractT = 47e-3
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

	neuroCompt = moose.NeuroMesh( '/model/neuroMesh' )
	#print 'neuroMeshvolume = ', neuroCompt.mesh[0].volume
	neuroCompt.separateSpines = 1
	neuroCompt.diffLength = diffLength
	neuroCompt.geometryPolicy = 'cylinder'
	spineCompt = moose.SpineMesh( '/model/spineMesh' )
	#print 'spineMeshvolume = ', spineCompt.mesh[0].volume
	moose.connect( neuroCompt, 'spineListOut', spineCompt, 'spineList', 'OneToOne' )
	psdCompt = moose.PsdMesh( '/model/psdMesh' )
	#print 'psdMeshvolume = ', psdCompt.mesh[0].volume
	moose.connect( neuroCompt, 'psdListOut', psdCompt, 'psdList', 'OneToOne' )
	loadChem( neuroCompt, spineCompt, psdCompt )

	# Put in the solvers, see how they fare.
	nmksolve = moose.GslStoich( '/model/chem/neuroMesh/ksolve' )
	nmksolve.path = '/model/chem/neuroMesh/##'
	nmksolve.compartment = moose.element( '/model/chem/neuroMesh' )
	nmksolve.method = 'rk5'
	nm = moose.element( '/model/chem/neuroMesh/mesh' )
	moose.connect( nm, 'remesh', nmksolve, 'remesh' )
    #print "neuron: nv=", nmksolve.numLocalVoxels, ", nav=", nmksolve.numAllVoxels, nmksolve.numVarPools, nmksolve.numAllPools

    #print 'setting up smksolve'
	smksolve = moose.GslStoich( '/model/chem/spineMesh/ksolve' )
	smksolve.path = '/model/chem/spineMesh/##'
	smksolve.compartment = moose.element( '/model/chem/spineMesh' )
	smksolve.method = 'rk5'
	sm = moose.element( '/model/chem/spineMesh/mesh' )
	moose.connect( sm, 'remesh', smksolve, 'remesh' )
	#print "spine: nv=", smksolve.numLocalVoxels, ", nav=", smksolve.numAllVoxels, smksolve.numVarPools, smksolve.numAllPools
	#
	#print 'setting up pmksolve'
	pmksolve = moose.GslStoich( '/model/chem/psdMesh/ksolve' )
	pmksolve.path = '/model/chem/psdMesh/##'
	pmksolve.compartment = moose.element( '/model/chem/psdMesh' )
	pmksolve.method = 'rk5'
	pm = moose.element( '/model/chem/psdMesh/mesh' )
	moose.connect( pm, 'remesh', pmksolve, 'remesh' )
	#print "psd: nv=", pmksolve.numLocalVoxels, ", nav=", pmksolve.numAllVoxels, pmksolve.numVarPools, pmksolve.numAllPools
	#
	print('neuroMeshvolume = ', neuroCompt.mesh[0].volume)

	#print 'Assigning the cell model'
	# Now to set up the model.
	#neuroCompt.cell = elec
	neuroCompt.cellPortion( elec, '/model/elec/lat_14_#,/model/elec/spine_neck#,/model/elec/spine_head#' )
	"""
	ns = neuroCompt.numSegments
	#assert( ns == 11 ) # dend, 5x (shaft+head)
	ndc = neuroCompt.numDiffCompts
	#print 'numDiffCompts = ', ndc
	assert( ndc == 145 )
	ndc = neuroCompt.mesh.num
	#print 'NeuroMeshNum = ', ndc
	assert( ndc == 145 )

	sdc = spineCompt.mesh.num
	#print 'SpineMeshNum = ', sdc
	assert( sdc == 13 )
	pdc = psdCompt.mesh.num
	#print 'PsdMeshNum = ', pdc
	assert( pdc == 13 )
	"""

	mesh = moose.vec( '/model/chem/neuroMesh/mesh' )
	#for i in range( ndc ):
	#	print 's[', i, '] = ', mesh[i].volume
	mesh2 = moose.vec( '/model/chem/spineMesh/mesh' )
#	for i in range( sdc ):
#		print 's[', i, '] = ', mesh2[i].volume
	#print 'numPSD = ', moose.element( '/model/chem/psdMesh/mesh' ).localNumField
	mesh = moose.vec( '/model/chem/psdMesh/mesh' )
	#print 'psd mesh.volume = ', mesh.volume
	#for i in range( pdc ):
	#	print 's[', i, '] = ', mesh[i].volume
	#
	# We need to use the spine solver as the master for the purposes of
	# these calculations. This will handle the diffusion calculations
	# between head and dendrite, and between head and PSD.
	smksolve.addJunction( nmksolve )
	#print "spine: nv=", smksolve.numLocalVoxels, ", nav=", smksolve.numAllVoxels, smksolve.numVarPools, smksolve.numAllPools
	smksolve.addJunction( pmksolve )
	#print "psd: nv=", pmksolve.numLocalVoxels, ", nav=", pmksolve.numAllVoxels, pmksolve.numVarPools, pmksolve.numAllPools
	ndc = neuroCompt.numDiffCompts
	#print 'numDiffCompts = ', ndc
	assert( ndc == 13 )
	ndc = neuroCompt.mesh.num
	#print 'NeuroMeshNum = ', ndc
	assert( ndc == 13 )
	sdc = spineCompt.mesh.num
	#print 'SpineMeshNum = ', sdc
	assert( sdc == 13 )
	pdc = psdCompt.mesh.num
	#print 'PsdMeshNum = ', pdc
	assert( pdc == 13 )

	"""
	print 'neuroCompt'
	for i in range( ndc ):
			print i, neuroCompt.stencilIndex[i]
			print i, neuroCompt.stencilRate[i]

	print 'spineCompt'
	for i in range( sdc * 3 ):
			print i, spineCompt.stencilIndex[i]
			print i, spineCompt.stencilRate[i]

	print 'psdCompt'
	for i in range( pdc ):
			print i, psdCompt.stencilIndex[i]
			print i, psdCompt.stencilRate[i]
	print 'Spine parents:'
	pavoxel = spineCompt.parentVoxel
	for i in range( sdc ):
		print i, pavoxel[i]
	"""

	# oddly, numLocalFields does not work.
	#moose.le( '/model/chem/neuroMesh' )
	ca = moose.element( '/model/chem/neuroMesh/DEND/Ca' )
	assert( ca.lastDimension == ndc )
	"""
	CaNpsd = moose.vec( '/model/chem/psdMesh/PSD/PP1_PSD/CaN' )
	print 'numCaN in PSD = ', CaNpsd.nInit, ', vol = ', CaNpsd.volume
	CaNspine = moose.vec( '/model/chem/spineMesh/SPINE/CaN_BULK/CaN' )
	print 'numCaN in spine = ', CaNspine.nInit, ', vol = ', CaNspine.volume
	"""

	# set up adaptors
	aCa = moose.Adaptor( '/model/chem/psdMesh/adaptCa', pdc )
	adaptCa = moose.vec( '/model/chem/psdMesh/adaptCa' )
	chemCa = moose.vec( '/model/chem/psdMesh/PSD/Ca' )
	assert( len( adaptCa ) == pdc )
	assert( len( chemCa ) == pdc )
	for i in range( pdc ):
		path = '/model/elec/spine_head_14_' + str( i + 1 ) + '/NMDA_Ca_conc'
		elecCa = moose.element( path )
		moose.connect( elecCa, 'concOut', adaptCa[i], 'input', 'Single' )
	moose.connect( adaptCa, 'outputSrc', chemCa, 'setConc', 'OneToOne' )
	adaptCa.inputOffset = 0.0		# 
	adaptCa.outputOffset = 80e-6	# 80 nM offset in chem.
   	adaptCa.scale = 1e-5	# 520 to 0.0052 mM
	#print adaptCa.outputOffset
	#print adaptCa.scale

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
	spineMesh = moose.element( '/model/chem/spineMesh' )
	middleSpine = 6
	midSpineVoxel = spineMesh.parentVoxel[middleSpine]
	graphs = moose.Neutral( '/graphs' )
	addPlot( '/model/chem/psdMesh/PSD/Ca[0]', 'getConc', 'pCa0' )
	addPlot( '/model/chem/psdMesh/PSD/Ca[6]', 'getConc', 'pCa6' )
	addPlot( '/model/chem/psdMesh/PSD/Ca[12]', 'getConc', 'pCa12' )

	addPlot( '/model/chem/spineMesh/SPINE/Ca[0]', 'getConc', 'sCa0' )
	addPlot( '/model/chem/spineMesh/SPINE/Ca[6]', 'getConc', 'sCa6' )
	addPlot( '/model/chem/spineMesh/SPINE/Ca[12]', 'getConc', 'sCa12' )

	addPlot( '/model/chem/neuroMesh/DEND/Ca[0]', 'getConc', 'dend0Ca' )
	addPlot( '/model/chem/neuroMesh/DEND/Ca[' + str( midSpineVoxel ) + ']', 'getConc', 'dendMidCa' )
	addPlot( '/model/chem/neuroMesh/DEND/Ca[' + str( midSpineVoxel+2 ) + ']', 'getConc', 'dendMid2' )
	addPlot( '/model/chem/neuroMesh/DEND/Ca[' + str( midSpineVoxel+4 ) + ']', 'getConc', 'dendMid4' )
	addPlot( '/model/chem/neuroMesh/DEND/Ca[' + str( midSpineVoxel+6 ) + ']', 'getConc', 'dendMid6' )
	addPlot( '/model/chem/neuroMesh/DEND/Ca[144]', 'getConc', 'dend144Ca' )

	addPlot( '/model/chem/psdMesh/PSD/CaM[0]', 'getConc', 'pCaM0' )
	addPlot( '/model/chem/psdMesh/PSD/CaM[6]', 'getConc', 'pCaM6' )
	addPlot( '/model/chem/psdMesh/PSD/CaM[12]', 'getConc', 'pCaM12' )

	addPlot( '/model/chem/spineMesh/SPINE/CaM[0]', 'getConc', 'sCaM0' )
	addPlot( '/model/chem/spineMesh/SPINE/CaM[6]', 'getConc', 'sCaM6' )
	addPlot( '/model/chem/spineMesh/SPINE/CaM[12]', 'getConc', 'sCaM12' )

	addPlot( '/model/chem/psdMesh/PSD/Ca_CaM[0]', 'getConc', 'pCaCaM0' )
	addPlot( '/model/chem/psdMesh/PSD/Ca_CaM[6]', 'getConc', 'pCaCaM6' )
	addPlot( '/model/chem/psdMesh/PSD/Ca_CaM[12]', 'getConc', 'pCaCaM12' )

	addPlot( '/model/chem/spineMesh/SPINE/Ca_CaM[0]', 'getConc', 'sCaCaM0' )
	addPlot( '/model/chem/spineMesh/SPINE/Ca_CaM[6]', 'getConc', 'sCaCaM6' )
	addPlot( '/model/chem/spineMesh/SPINE/Ca_CaM[12]', 'getConc', 'sCaCaM12' )

	addPlot( '/model/chem/neuroMesh/DEND/Ca_CaM[0]', 'getConc', 'dCaCaM0' )
	addPlot( '/model/chem/neuroMesh/DEND/Ca_CaM[' + str( midSpineVoxel ) + ']', 'getConc', 'dendMidCaCaM' )
	addPlot( '/model/chem/neuroMesh/DEND/Ca_CaM[144]', 'getConc', 'dCaCaM144' )

def testNeuroMeshMultiscale():
	elecDt = 50e-6
	chemDt = 1e-4
	plotDt = 5e-4
	plotName = 'diffonly.plot'

	makeNeuroMeshModel()
	"""
	for i in moose.wildcardFind( '/model/chem/##[ISA=PoolBase]' ):
		if ( i[0].diffConst > 0 ):
			grandpaname = i.parent[0].parent.name + '/'
			paname = i.parent[0].name + '/'
			print grandpaname + paname + i[0].name, i[0].diffConst
	moose.le( '/model/chem/spineMesh/ksolve' )
	print 'Neighbors:'
	for t in moose.element( '/model/chem/spineMesh/ksolve/junction' ).neighbors['masterJunction']:
		print 'masterJunction <-', t.path
	for t in moose.wildcardFind( '/model/chem/#Mesh/ksolve' ):
		k = moose.element( t[0] )
		print k.path + ' localVoxels=', k.numLocalVoxels, ', allVoxels= ', k.numAllVoxels
	"""

	makeChemPlots()
	makeElecPlots()
	moose.setClock( 0, elecDt )
	moose.setClock( 1, elecDt )
	moose.setClock( 2, elecDt )
	moose.setClock( 5, chemDt )
	moose.setClock( 6, chemDt )
	moose.setClock( 7, plotDt )
	moose.setClock( 8, plotDt )
	moose.useClock( 0, '/model/elec/##[ISA=Compartment]', 'init' )
	moose.useClock( 1, '/model/elec/##[ISA=SpikeGen]', 'process' )
	moose.useClock( 2, '/model/elec/##[ISA=ChanBase],/model/##[ISA=SynBase],/model/##[ISA=CaConc]','process')
	moose.useClock( 5, '/model/chem/##[ISA=PoolBase],/model/##[ISA=ReacBase],/model/##[ISA=EnzBase]', 'process' )
	moose.useClock( 6, '/model/chem/##[ISA=Adaptor]', 'process' )
	moose.useClock( 7, '/graphs/#', 'process' )
	moose.useClock( 8, '/graphs/elec/#', 'process' )
	moose.useClock( 5, '/model/chem/#Mesh/ksolve', 'init' )
	moose.useClock( 6, '/model/chem/#Mesh/ksolve', 'process' )
	hsolve = moose.HSolve( '/model/elec/hsolve' )
	moose.useClock( 1, '/model/elec/hsolve', 'process' )
	hsolve.dt = elecDt
	hsolve.target = '/model/elec/compt'
	moose.reinit()
	moose.reinit()
	"""
	print 'pre'
	eca = moose.vec( '/model/chem/psdMesh/PSD/CaM/Ca' )
	for i in range( 3 ):
		print eca[i].concInit, eca[i].conc, eca[i].nInit, eca[i].n, eca[i].volume
	print 'dend'
	eca = moose.vec( '/model/chem/neuroMesh/DEND/Ca' )
	for i in ( 0, 1, 2, 30, 60, 90, 120, 144 ):
		print i, eca[i].concInit, eca[i].conc, eca[i].nInit, eca[i].n, eca[i].volume

	print 'PSD'
	eca = moose.vec( '/model/chem/psdMesh/PSD/CaM/Ca' )
	for i in range( 3 ):
		print eca[i].concInit, eca[i].conc, eca[i].nInit, eca[i].n, eca[i].volume
	print 'spine'
	eca = moose.vec( '/model/chem/spineMesh/SPINE/CaM/Ca' )
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
