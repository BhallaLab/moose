# multi2.py --- 
# Upi Bhalla, NCBS Bangalore 2014.
#
# Commentary: 
# 
# This loads in a medium-detail model incorporating 
# reac-diff and elec signaling in neurons. The reac-diff model is
# psd_merged31d.g which has only Ca in the dendrites, but is quite
# interesting in the PSD and spine head. Problem is lots of 
# cross-compartment reactions.
# has in it, and there are no-cross-compartment
# reactions though Ca diffuses everywhere. The elec model controls the
# Ca levels in the chem compartments.
# This version uses solvers for both chem and electrical parts.
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
	cellId = moose.loadModel( 'ca1_asym.p', '/model/elec', "Neutral" )
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
	#modelId = moose.loadModel( 'separate_compts.g', '/model/chem', 'ee' )
	modelId = moose.loadModel( 'psd_merged31d.g', '/model/chem', 'ee' )
        neuroCompt.name = 'dend'
        spineCompt.name = 'spine'
        psdCompt.name = 'psd'

def makeNeuroMeshModel():
	diffLength = 10e-6 # Aim for 2 soma compartments.
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
	assert( ns == 36 ) # 
	assert( ndc == 278 ) # 
	nmksolve = moose.Ksolve( '/model/chem/dend/ksolve' )
	nmdsolve = moose.Dsolve( '/model/chem/dend/dsolve' )
        nmstoich = moose.Stoich( '/model/chem/dend/stoich' )
        nmstoich.compartment = neuroCompt
        nmstoich.ksolve = nmksolve
        nmstoich.dsolve = nmdsolve
        nmstoich.path = "/model/chem/dend/##"
        print 'done setting path, numPools = ', nmdsolve.numPools
        assert( nmdsolve.numPools == 1 )
        assert( nmdsolve.numAllVoxels == ndc )
        assert( nmstoich.numAllPools == 1 )
	# oddly, numLocalFields does not work.
	ca = moose.element( '/model/chem/dend/DEND/Ca' )
	assert( ca.numData == ndc )
        
        # Put in spine solvers. Note that these get info from the neuroCompt
        spineCompt = moose.element( '/model/chem/spine' )
	sdc = spineCompt.mesh.num
        print 'sdc = ', sdc
	assert( sdc == 13 )
	smksolve = moose.Ksolve( '/model/chem/spine/ksolve' )
	smdsolve = moose.Dsolve( '/model/chem/spine/dsolve' )
        smstoich = moose.Stoich( '/model/chem/spine/stoich' )
        smstoich.compartment = spineCompt
        smstoich.ksolve = smksolve
        smstoich.dsolve = smdsolve
        smstoich.path = "/model/chem/spine/##"
        print 'spine num Pools = ', smstoich.numAllPools, 'dsolve n = ', smdsolve.numPools
        assert( smstoich.numAllPools == 36 )
        assert( smdsolve.numPools == 31 )
        assert( smdsolve.numAllVoxels == sdc )
        
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
        print 'psd num Pools = ', pmstoich.numAllPools
        assert( pmstoich.numAllPools == 56 )
        assert( pmdsolve.numPools == 49 )
        assert( pmdsolve.numAllVoxels == pdc )
        foo = moose.element( '/model/chem/psd/Ca' )
        print 'PSD: numfoo = ', foo.numData
        print 'PSD: numAllVoxels = ', pmksolve.numAllVoxels

        # Put in junctions between the diffusion solvers
        nmdsolve.buildNeuroMeshJunctions( smdsolve, pmdsolve )

        # Put in cross-compartment reactions between ksolvers
        nmstoich.buildXreacs( smstoich )
        smstoich.buildXreacs( pmstoich )

	"""
	CaNpsd = moose.vec( '/model/chem/psdMesh/PSD/PP1_PSD/CaN' )
	print 'numCaN in PSD = ', CaNpsd.nInit, ', vol = ', CaNpsd.volume
	CaNspine = moose.vec( '/model/chem/spine/SPINE/CaN_BULK/CaN' )
	print 'numCaN in spine = ', CaNspine.nInit, ', vol = ', CaNspine.volume
	"""

        ##################################################################
	# set up adaptors
	aCa = moose.Adaptor( '/model/chem/spine/adaptCa', sdc )
	adaptCa = moose.vec( '/model/chem/spine/adaptCa' )
	chemCa = moose.vec( '/model/chem/spine/Ca' )
	#print 'aCa = ', aCa, ' foo = ', foo, "len( ChemCa ) = ", len( chemCa ), ", numData = ", chemCa.numData, "len( adaptCa ) = ", len( adaptCa )
	assert( len( adaptCa ) == sdc )
	assert( len( chemCa ) == sdc )
        for i in range( sdc ):
            elecCa = moose.element( '/model/elec/spine_head_14_' + str(i+1) + '/NMDA_Ca_conc' )
            #print elecCa
	    moose.connect( elecCa, 'concOut', adaptCa[i], 'input', 'Single' )
	moose.connect( adaptCa, 'output', chemCa, 'setConc', 'OneToOne' )
	adaptCa.inputOffset = 0.0	# 
	adaptCa.outputOffset = 0.00008	# 80 nM offset in chem.
   	adaptCa.scale = 5e-3	# 520 to 0.0052 mM
	#print adaptCa.outputOffset


        moose.le( '/model/chem/dend/DEND' )


        compts = neuroCompt.elecComptList
        begin = neuroCompt.startVoxelInCompt
        end = neuroCompt.endVoxelInCompt
	aCa = moose.Adaptor( '/model/chem/dend/DEND/adaptCa', len( compts))
	adaptCa = moose.vec( '/model/chem/dend/DEND/adaptCa' )
	chemCa = moose.vec( '/model/chem/dend/DEND/Ca' )
	#print 'aCa = ', aCa, ' foo = ', foo, "len( ChemCa ) = ", len( chemCa ), ", numData = ", chemCa.numData, "len( adaptCa ) = ", len( adaptCa )
	assert( len( chemCa ) == ndc )
        for i in zip( compts, adaptCa, begin, end ):
            name = i[0].path + '/Ca_conc'
            if ( moose.exists( name ) ):
                elecCa = moose.element( name )
                #print i[2], i[3], '   ', elecCa
                #print i[1]
                moose.connect( elecCa, 'concOut', i[1], 'input', 'Single' ) 
                for j in range( i[2], i[3] ):
                    moose.connect( i[1], 'output', chemCa[j], 'setConc', 'Single' )
	adaptCa.inputOffset = 0.0	# 
	adaptCa.outputOffset = 0.00008	# 80 nM offset in chem.
   	adaptCa.scale = 20e-6	# 10 arb units to 2 uM.

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

def makeCaPlots():
    graphs = moose.Neutral( '/graphs' )
    ca = moose.Neutral( '/graphs/ca' )
    addPlot( '/model/elec/soma/Ca_conc', 'getCa', 'ca/somaCa' )
    addPlot( '/model/elec/lat_11_2/Ca_conc', 'getCa', 'ca/lat11Ca' )
    addPlot( '/model/elec/spine_head_14_4/NMDA_Ca_conc', 'getCa', 'ca/spine4Ca' )
    addPlot( '/model/elec/spine_head_14_12/NMDA_Ca_conc', 'getCa', 'ca/spine12Ca' )

def makeElecPlots():
    graphs = moose.Neutral( '/graphs' )
    elec = moose.Neutral( '/graphs/elec' )
    addPlot( '/model/elec/soma', 'getVm', 'elec/somaVm' )
    addPlot( '/model/elec/spine_head_14_4', 'getVm', 'elec/spineVm' )

def makeChemPlots():
	graphs = moose.Neutral( '/graphs' )
	chem = moose.Neutral( '/graphs/chem' )
	addPlot( '/model/chem/psd/CaM_dash_Ca4', 'getConc', 'chem/psdCaCam' )
	addPlot( '/model/chem/psd/actCaMKII', 'getConc', 'chem/psdActCaMKII' )
	addPlot( '/model/chem/spine/CaM_dash_Ca4', 'getConc', 'chem/spineCaCam' )
	addPlot( '/model/chem/spine/CaM_dash_Ca4[12]', 'getConc', 'chem/spine12CaCam' )
	addPlot( '/model/chem/spine/actCaMKII', 'getConc', 'chem/spineActCaMKII' )
	addPlot( '/model/chem/spine/actCaMKII[11]', 'getConc', 'chem/spine12ActCaMKII' )
	addPlot( '/model/chem/psd/Ca', 'getConc', 'chem/psdCa' )
	addPlot( '/model/chem/psd/Ca[11]', 'getConc', 'chem/psd12Ca' )
	addPlot( '/model/chem/spine/Ca[3]', 'getConc', 'chem/spine4Ca' )
	addPlot( '/model/chem/spine/Ca[11]', 'getConc', 'chem/spine12Ca' )
	#addPlot( '/model/chem/dend/DEND/Ca', 'getConc', 'chem/dendCa' )
	#addPlot( '/model/chem/dend/DEND/Ca[20]', 'getConc', 'chem/dendCa20' )

def makeGraphics( cPlotDt, ePlotDt ):
        plt.ion()
        fig = plt.figure( figsize=(10,16) )
        chem = fig.add_subplot( 411 )
        chem.set_ylim( 0, 0.006 )
        plt.ylabel( 'Conc (mM)' )
        plt.xlabel( 'time (seconds)' )
        for x in moose.wildcardFind( '/graphs/chem/#[ISA=Table]' ):
            pos = numpy.arange( 0, x.vector.size, 1 ) * cPlotDt
            line1, = chem.plot( pos, x.vector, label=x.name )
        plt.legend()

        elec = fig.add_subplot( 412 )
        plt.ylabel( 'Vm (V)' )
        plt.xlabel( 'time (seconds)' )
        for x in moose.wildcardFind( '/graphs/elec/#[ISA=Table]' ):
            pos = numpy.arange( 0, x.vector.size, 1 ) * ePlotDt
            line1, = elec.plot( pos, x.vector, label=x.name )
        plt.legend()

        ca = fig.add_subplot( 413 )
        plt.ylabel( '[Ca] (mM)' )
        plt.xlabel( 'time (seconds)' )
        for x in moose.wildcardFind( '/graphs/ca/#[ISA=Table]' ):
            pos = numpy.arange( 0, x.vector.size, 1 ) * ePlotDt
            line1, = ca.plot( pos, x.vector, label=x.name )
        plt.legend()

        lenplot = fig.add_subplot( 414 )
        plt.ylabel( 'Ca (mM )' )
        plt.xlabel( 'Voxel#)' )

	spineCa = moose.vec( '/model/chem/spine/Ca' )
	dendCa = moose.vec( '/model/chem/dend/DEND/Ca' )
        line1, = lenplot.plot( range( len( spineCa ) ), spineCa.conc, label='spine' )
        line2, = lenplot.plot( range( len( dendCa ) ), dendCa.conc, label='dend' )

        ca = [ x.Ca * 0.0001 for x in moose.wildcardFind( '/model/elec/##[ISA=CaConcBase]') ]
        line3, = lenplot.plot( range( len( ca ) ), ca, label='elec' )

	spineCaM = moose.vec( '/model/chem/spine/CaM_dash_Ca4' )
        line4, = lenplot.plot( range( len( spineCaM ) ), spineCaM.conc, label='spineCaM' )
	psdCaM = moose.vec( '/model/chem/psd/CaM_dash_Ca4' )
        line5, = lenplot.plot( range( len( psdCaM ) ), psdCaM.conc, label='psdCaM' )
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

	print 'All done'

def testNeuroMeshMultiscale():
        useHsolve = 1
        runtime = 0.5
        if useHsolve:
	    elecDt = 50e-6
        else:
	    elecDt = 10e-6
	chemDt = 0.005
	ePlotDt = 0.5e-3
	cPlotDt = 0.005
	plotName = 'nm.plot'

	makeNeuroMeshModel()
	print "after model is completely done"
	for i in moose.wildcardFind( '/model/chem/#/#/#/transloc#' ):
		print i[0].name, i[0].Kf, i[0].Kb, i[0].kf, i[0].kb

	makeChemPlots()
	makeElecPlots()
	makeCaPlots()
	moose.setClock( 0, elecDt )
	moose.setClock( 1, elecDt )
	moose.setClock( 2, elecDt )
	moose.setClock( 4, chemDt )
	moose.setClock( 5, chemDt )
	moose.setClock( 6, chemDt )
	moose.setClock( 7, cPlotDt )
	moose.setClock( 8, ePlotDt )
        if useHsolve:
	    hsolve = moose.HSolve( '/model/elec/hsolve' )
	    moose.useClock( 1, '/model/elec/hsolve', 'process' )
	    hsolve.dt = elecDt
	    hsolve.target = '/model/elec/compt'
	    moose.reinit()
        else:
	    moose.useClock( 0, '/model/elec/##[ISA=Compartment]', 'init' )
	    moose.useClock( 1, '/model/elec/##[ISA=Compartment]', 'process' )
	    moose.useClock( 2, '/model/elec/##[ISA=ChanBase],/model/##[ISA=SynBase],/model/##[ISA=CaConc]','process')

	moose.useClock( 1, '/model/elec/##[ISA=SpikeGen]', 'process' )
	moose.useClock( 2, '/model/##[ISA=SynBase],/model/##[ISA=CaConc]','process')
	#moose.useClock( 5, '/model/chem/##[ISA=PoolBase],/model/##[ISA=ReacBase],/model/##[ISA=EnzBase]', 'process' )
	#moose.useClock( 4, '/model/chem/##[ISA=Adaptor]', 'process' )
	moose.useClock( 4, '/model/chem/#/dsolve', 'process' )
	moose.useClock( 4, '/model/chem/#/ksolve', 'init' )
	moose.useClock( 5, '/model/chem/#/ksolve', 'process' )
	moose.useClock( 6, '/model/chem/spine/adaptCa', 'process' )
	moose.useClock( 6, '/model/chem/dend/DEND/adaptCa', 'process' )
	moose.useClock( 7, '/graphs/chem/#', 'process' )
	moose.useClock( 8, '/graphs/elec/#,/graphs/ca/#', 'process' )
        moose.element( '/model/elec/soma' ).inject = 2e-10
        moose.element( '/model/chem/psd/Ca' ).concInit = 0.001
        moose.element( '/model/chem/spine/Ca' ).concInit = 0.002
        moose.element( '/model/chem/dend/DEND/Ca' ).concInit = 0.003
	moose.reinit()

	moose.start( runtime )
#        moose.element( '/model/elec/soma' ).inject = 0
#	moose.start( 0.25 )
        makeGraphics( cPlotDt, ePlotDt )


def main():
	testNeuroMeshMultiscale()

if __name__ == '__main__':
	main()

# 
# minimal.py ends here.
