# -*- coding: utf-8 -*-
# Utilties for loading NML and SBML models.
# Authored and maintained by Harsha Rani

from __future__ import absolute_import, print_function, division

import os
import moose._moose as _moose
import moose.utils as mu

import logging
logger_ = logging.getLogger('moose.model')

# sbml import.
sbmlImport_, sbmlError_ = True, ''
try:
    import moose.SBML.readSBML as _readSBML
    import moose.SBML.writeSBML as _writeSBML
except Exception as e:
    sbmlImport_ = False
    sbmlError_ = '%s' % e

# NeuroML2 import.
nml2Import_, nml2ImportError_ = True, ''
try:
    import moose.neuroml2 as _neuroml2
except Exception as e:
    nml2Import_ = False
    nml2ImportError_ = ' '.join( [
        "NML2 support is disabled because `libneuroml` and "
        , "`pyneuroml` modules are not found.\n"
        , "     $ pip install pyneuroml libneuroml \n"
        , " should fix it."
        , " Actual error: %s " % e ]
        )

chemImport_, chemError_ = True, ''
try:
    import moose.chemUtil as _chemUtil
except Exception as e:
    chemImport_ = False
    chemError_ = '%s' % e

kkitImport_, kkitImport_error_ = True, ''
try:
    import moose.genesis.writeKkit as _writeKkit
except ImportError as e:
    kkitImport_ = False
    kkitImport_err_ = '%s' % e

mergechemImport_, mergechemError_ = True, ''
try:
    import moose.chemMerge as _chemMerge
except Exception as e:
    mergechemImport_ = False
    mergechemError_ = '%s' % e

# SBML related functions.
def mooseReadSBML(filepath, loadpath, solver='ee', validate="on"):
    """Load SBML model.

    Parameter
    --------
    filepath: str
        filepath to be loaded.
    loadpath : str
        Root path for this model e.g. /model/mymodel
    solver : str
        Solver to use (default 'ee').
        Available options are "ee", "gsl", "stochastic", "gillespie"
            "rk", "deterministic"
            For full list see ??
    """
    global sbmlImport_
    if sbmlImport_:
        modelpath = _readSBML.mooseReadSBML(filepath, loadpath, solver, validate)
        sc = solver.lower()
        if sc in ["gssa","gillespie","stochastic","gsolve"]:
            method = "gssa"
        elif sc in ["gsl","runge kutta","deterministic","ksolve","rungekutta","rk5","rkf","rk"]:
            method = "gsl"
        elif sc in ["exponential euler","exponentialeuler","neutral", "ee"]:
            method = "ee"
        else:
            method = "ee"

        if method != 'ee':
            _chemUtil.add_Delete_ChemicalSolver.mooseAddChemSolver(modelpath[0].path, method)
        return modelpath
    else:
        logger_.error( sbmlError_ )
        return False

def mooseWriteSBML(modelpath, filepath, sceneitems={}):
    """mooseWriteSBML: Writes loaded model under modelpath to a file in SBML format.

    Parameters
    ----------
    modelpath : str
        model path in moose e.g /model/mymodel \n
    filepath : str
        Path of output file. \n
    sceneitems : dict
        UserWarning: user need not worry about this layout position is saved in
        Annotation field of all the moose Object (pool,Reaction,enzyme).
        If this function is called from
        * GUI - the layout position of moose object is passed
        * command line - NA
        * if genesis/kkit model is loaded then layout position is taken from the file
        * otherwise auto-coordinates is used for layout position.
    """
    if sbmlImport_:
        return _writeSBML.mooseWriteSBML(modelpath, filepath, sceneitems)
    else:
        logger_.error( sbmlError_ )
        return False


def mooseWriteKkit(modelpath, filepath, sceneitems={}):
    """Writes  loded model under modelpath to a file in Kkit format.

    Parameters
    ----------
    modelpath : str
        Model path in moose.
    filepath : str
        Path of output file.
    """
    global kkitImport_, kkitImport_err_
    if not kkitImport_:
        print( '[WARN] Could not import module to enable this function' )
        print( '\tError was %s' % kkitImport_error_ )
        return False
    return _writeKkit.mooseWriteKkit(modelpath, filepath,sceneitems)


def mooseDeleteChemSolver(modelpath):
    """mooseDeleteChemSolver
    deletes solver on all the compartment and its children.

    Notes
    -----
    This is neccesary while created a new moose object on a pre-existing modelpath,
    this should be followed by mooseAddChemSolver for add solvers on to compartment
    to simulate else default is Exponential Euler (ee)
    """
    if chemImport_:
        return _chemUtil.add_Delete_ChemicalSolver.mooseDeleteChemSolver(modelpath)
    else:
        print( chemError_ )
        return False


def mooseAddChemSolver(modelpath, solver):
    """mooseAddChemSolver:
    Add solver on chemical compartment and its children for calculation

    Parameters
    ----------

    modelpath : str
        Model path that is loaded into moose.
    solver : str
        Exponential Euler "ee" is default. Other options are Gillespie ("gssa"),
        Runge Kutta ("gsl") etc. Link to documentation?
    """
    if chemImport_:
        chemError_ = _chemUtil.add_Delete_ChemicalSolver.mooseAddChemSolver(modelpath, solver)
        return chemError_
    else:
        print( chemError_ )
        return False

def mergeChemModel(src, des):
    """mergeChemModel: Merges two chemical model.
    File or filepath can be passed source is merged to destination
    """
    #global mergechemImport_
    if mergechemImport_:
        return _chemMerge.merge.mergeChemModel(src,des)
    else:
        return False

# NML2 reader and writer function.
def mooseReadNML2( modelpath, verbose = False ):
    """Read NeuroML model (version 2) and return reader object.
    """
    global nml2Import_
    if not nml2Import_:
        mu.warn( nml2ImportError_ )
        raise RuntimeError( "Could not load NML2 support." )

    reader = _neuroml2.NML2Reader( verbose = verbose )
    reader.read( modelpath )
    return reader

def mooseWriteNML2( outfile ):
    raise NotImplementedError( "Writing to NML2 is not supported yet" )


def loadModel(filename, modelpath, solverclass="gsl"):
    """loadModel: Load model from a file to a specified path.

    Parameters
    ----------
    filename: str
        model description file.
    modelpath: str
        moose path for the top level element of the model to be created.
    method: str
        solver type to be used for simulating the model.
        TODO: Link to detailed description of solvers?

    Returns
    -------
    object
        moose.element if succcessful else None.
    """

    if not os.path.isfile( os.path.realpath(filename) ):
        mu.warn( "Model file '%s' does not exists or is not readable." % filename )
        return None

    extension = os.path.splitext(filename)[1]
    if extension in [".swc", ".p"]:
        return _moose.loadModelInternal(filename, modelpath, "Neutral" )

    if extension in [".g", ".cspace"]:
        # only if genesis or cspace file and method != ee then only
        # mooseAddChemSolver is called.
        ret = _moose.loadModelInternal(filename, modelpath, "ee")
        sc = solverclass.lower()
        if sc in ["gssa","gillespie","stochastic","gsolve"]:
            method = "gssa"
        elif sc in ["gsl","runge kutta","deterministic","ksolve","rungekutta","rk5","rkf","rk"]:
            method = "gsl"
        elif sc in ["exponential euler","exponentialeuler","neutral"]:
            method = "ee"
        else:
            method = "ee"

        if method != 'ee':
            _chemUtil.add_Delete_ChemicalSolver.mooseAddChemSolver(modelpath, method)
        return ret
    else:
        logger_.error( "Unknown model extenstion '%s'" % extension)
        return None

