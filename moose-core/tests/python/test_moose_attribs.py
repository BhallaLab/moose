"""test_moose_attribs.py: 

"""
    
__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2017-, Dilawar Singh"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"

import sys
import os
import moose

attribs = ['AdExIF', 'AdThreshIF', 'Adaptor', 'Annotator', 'Arith', 'BufPool',
        'CaConc', 'CaConcBase', 'ChanBase', 'ChemCompt', 'Cinfo', 'Clock',
        'Compartment', 'CompartmentBase', 'ConcChan', 'CplxEnzBase', 'CubeMesh',
        'CylMesh', 'DestField', 'DiagonalMsg', 'DifBuffer', 'DifBufferBase',
        'DifShell', 'DifShellBase', 'DiffAmp', 'Dsolve', 'ElementField',
        'EndoMesh', 'Enz', 'EnzBase', 'ExIF', 'Finfo', 'Function',
        'GapJunction', 'GraupnerBrunel2012CaPlasticitySynHandler', 'Group',
        'Gsolve', 'HHChannel', 'HHChannel2D', 'HHChannelBase', 'HHGate',
        'HHGate2D', 'HSolve', 'INFINITE', 'IntFire', 'IntFireBase', 'Interpol',
        'Interpol2D', 'IzhIF', 'IzhikevichNrn', 'Ksolve', 'LIF', 'Leakage',
        'LookupField', 'MMPump', 'MMenz', 'MarkovChannel' ,
        # 'MarkovGslSolver', # This is GSL specific.
        'MarkovRateTable', 'MarkovSolver', 'MarkovSolverBase', 'MeshEntry',
        'MgBlock', 'Msg', 'Mstring', 'NMDAChan', 'Nernst', 'NeuroMesh',
        'Neuron', 'Neutral', 'OneToAllMsg', 'OneToOneDataIndexMsg',
        'OneToOneMsg', 'PIDController', 'Pool', 'PoolBase',
        'PostMaster', 'PsdMesh', 'PulseGen', 'PyRun', 'QIF', 'RC', 'RandSpike',
        'Reac', 'ReacBase', 'SBML', 'STDPSynHandler', 'STDPSynapse',
        'SeqSynHandler', 'Shell', 'SimpleSynHandler', 'SingleMsg', 'SparseMsg',
        'Species', 'SpikeGen', 'SpikeStats', 'Spine', 'SpineMesh', 'Stats',
        'SteadyState', 'StimulusTable', 'Stoich', 'Streamer', 'StringIO',
        'SymCompartment', 'SynChan', 'SynHandlerBase', 'Synapse', 'Table',
        'Table2', 'TableBase', 'TimeTable', 'VClamp', 'VERSION', 'Variable',
        'VectorTable', 'ZombieBufPool', 'ZombieCaConc', 'ZombieCompartment',
        'ZombieEnz', 'ZombieFunction', 'ZombieHHChannel', 'ZombieMMenz',
        'ZombiePool', 'ZombieReac', '_moose', 
        'append_finfodocs', 'ce', 'chemMerge',
        'chemUtil', 'closing', 'connect', 'copy', 'delete', 'division', 'doc',
        'element', 'exists', 'finfotypes', 'fixXreacs', 'genesis', 'getCwe',
        'getField', 'getFieldDict', 'getFieldNames', 'getfielddoc',
        'getmoosedoc', 'isRunning', 
        'known_types', 'le', 'listmsg', 'loadModelInternal', 'melement',
        'mergeChemModel', 'moose',
        'mooseAddChemSolver', 'mooseDeleteChemSolver', 'mooseReadNML2',
        'mooseReadSBML', 'mooseWriteKkit', 'mooseWriteNML2', 'mooseWriteSBML',
        'moose_constants', 'moose_test', 'move',
        'nml2Import_', 'pager', 'print_utils',
        'pwe', 'pydoc', 'rand', 'reinit', 
        'seed', 'sequence_types', 'setClock', 'setCwe', 'showfield',
        'showfields', 'showmsg', 'start', 'stop', 'syncDataHandler',
        'test', 'testSched', 'toUnicode', 'useClock', 'utils', 'vec', 'version',
        'warnings', 'wildcardFind']

def main():
    global attribs
    for at in attribs:
        assert hasattr( moose, at ), 'Attrib %s not found' % at 
        print( getattr(moose, at ) )

if __name__ == '__main__':
    main()

