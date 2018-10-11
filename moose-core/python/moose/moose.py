# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import

# Author: Subhasis Ray
# Maintainer: Dilawar Singh, Harsha Rani, Upi Bhalla

from __future__ import print_function, division, absolute_import

from contextlib import closing
import warnings
import pydoc
from io import StringIO
from os.path import splitext
import moose
import moose.utils as mu

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


# Import function from C++ module into moose namespace.
from moose._moose import *

#`loadModel` is deleted from global import,
# this is to bypass the call from c++ module which is due to fixXreacs() which is
# now written in python and readKkit.cpp will not be possible to set/call the solver due to this

del globals()['loadModel']

def loadModel(filename, target,method=None):
    solverClass = 'Neutral'
    if method != None:
        solverClass = method
    try:
        f = open(filename,'r')
        f.close()
    except IOError as e:
        print (e)
        return
    else:
        file_name,extension = splitext(filename)
        if extension in [".swc",".p"]:
            ret = moose._moose.loadModel(filename,target,"Neutral")
        elif extension in [".g",".cspace"]:
            #only if genesis or cspace file, then mooseAddChemSolver is called
            ret = moose._moose.loadModel(filename,target,"ee")
          
            method = "ee"
            if solverClass.lower() in ["gssa","gillespie","stochastic","gsolve"]:
                method = "gssa"
            elif solverClass.lower() in ["gsl","runge kutta","deterministic","ksolve","rungekutta","rk5","rkf","rk"]:
                method = "gsl"
            elif solverClass.lower() in ["exponential euler","exponentialeuler","neutral"]:
                method = "ee"
            
            if method != 'ee':
                chemError_ = _chemUtil.add_Delete_ChemicalSolver.mooseAddChemSolver(target,method)
        return ret
        
def version( ):
    return VERSION

# Tests
from moose.moose_test import test

sequence_types = ['vector<double>',
                  'vector<int>',
                  'vector<long>',
                  'vector<unsigned int>',
                  'vector<float>',
                  'vector<unsigned long>',
                  'vector<short>',
                  'vector<Id>',
                  'vector<ObjId>']

known_types = ['void',
               'char',
               'short',
               'int',
               'unsigned int',
               'double',
               'float',
               'long',
               'unsigned long',
               'string',
               'vec',
               'melement'] + sequence_types

# SBML related functions.
def mooseReadSBML(filepath, loadpath, solver='ee'):
    """Load SBML model.

    keyword arguments: \n

    filepath -- filepath to be loaded \n
    loadpath -- Root path for this model e.g. /model/mymodel \n
    solver   -- Solver to use (default 'ee' ) \n

    """
    global sbmlImport_
    if sbmlImport_:
        return _readSBML.mooseReadSBML( filepath, loadpath, solver )
    else:
        print( sbmlError_ )
        return False


def mooseWriteSBML(modelpath, filepath, sceneitems={}):
    """Writes loaded model under modelpath to a file in SBML format.

    keyword arguments:\n

    modelpath -- model path in moose e.g /model/mymodel \n
    filepath -- Path of output file. \n
    sceneitems -- dictlist (UserWarning: user need not worry about this) \n
                    layout position is saved in Annotation field of all the moose Object (pool,Reaction,enzyme)\n
                    If this function is called from \n
                        -- GUI, the layout position of moose object is passed \n
                        -- command line, \n
                            ---if genesis/kkit model is loaded then layout position is taken from the file \n
                            --- else, auto-coordinates is used for layout position and passed

    """
    if sbmlImport_:
        return _writeSBML.mooseWriteSBML(modelpath, filepath, sceneitems)
    else:
        print( sbmlError_ )
        return False


def mooseWriteKkit(modelpath, filepath,sceneitems={}):
    """Writes  loded model under modelpath to a file in Kkit format.

    keyword arguments:\n

    modelpath -- model path in moose \n
    filepath -- Path of output file.
    """
    global kkitImport_, kkitImport_err_
    if not kkitImport_:
        print( '[WARN] Could not import module to enable this function' )
        print( '\tError was %s' % kkitImport_error_ )
        return False
    return _writeKkit.mooseWriteKkit(modelpath, filepath,sceneitems)


def mooseDeleteChemSolver(modelpath):
    """ deletes solver on all the compartment and its children.
        This is neccesary while created a new moose object on a pre-existing modelpath,\n
        this should be followed by mooseAddChemSolver for add solvers on to compartment to simulate else
        default is Exponential Euler (ee)
    """
    if chemImport_:
        return _chemUtil.add_Delete_ChemicalSolver.mooseDeleteChemSolver(modelpath)
    else:
        print( chemError_ )
        return False


def mooseAddChemSolver(modelpath, solver):
    """ Add solver on chemical compartment and its children for calculation

    keyword arguments:\n

    modelpath -- model path that is loaded into moose \n
    solver -- "Exponential Euler" (ee) (default), \n
              "Gillespie"         ("gssa"), \n
              "Runge Kutta"       ("gsl")

    """
    if chemImport_:
        chemError_ = _chemUtil.add_Delete_ChemicalSolver.mooseAddChemSolver(modelpath, solver)
        return chemError_
    else:
        print( chemError_ )
        return False

def mergeChemModel(src, des):
    """ Merges two chemical model, \n
        File or filepath can be passed
        source is merged to destination
    """
    #global mergechemImport_
    if mergechemImport_:
        return _chemMerge.merge.mergeChemModel(src,des)
    else:
        return False

# NML2 reader and writer function.
def mooseReadNML2( modelpath ):
    """Read NeuroML model (version 2) and return reader object.
    """
    global nml2Import_
    if not nml2Import_:
        mu.warn( nml2ImportError_ )
        raise RuntimeError( "Could not load NML2 support." )

    reader = _neuroml2.NML2Reader( )
    reader.read( modelpath )
    return reader

def mooseWriteNML2( outfile ):
    raise NotImplementedError( "Writing to NML2 is not supported yet" )

################################################################
# Wrappers for global functions
################################################################
def pwe():
    """Print present working element. Convenience function for GENESIS
    users. If you want to retrieve the element in stead of printing
    the path, use moose.getCwe()

    """
    pwe_ = moose.getCwe()
    print(pwe_.getPath())
    return pwe_


def le(el=None):
    """List elements under `el` or current element if no argument
    specified.

    Parameters
    ----------
    el : str/melement/vec/None
        The element or the path under which to look. If `None`, children
         of current working element are displayed.

    Returns
    -------
    List of path of child elements

    """
    if el is None:
        el = getCwe()
    elif isinstance(el, str):
        if not exists(el):
            raise ValueError('no such element')
        el = element(el)
    elif isinstance(el, vec):
        el = el[0]
    print('Elements under', el.path)
    for ch in el.children:
        print(ch.path)
    return [child.path for child in el.children]

ce = setCwe  # ce is a GENESIS shorthand for change element.


def syncDataHandler(target):
    """Synchronize data handlers for target.

    Parameters
    ----------
    target : melement/vec/str
        Target element or vec or path string.

    Raises
    ------
    NotImplementedError
        The call to the underlying C++ function does not work.

    Notes
    -----
    This function is defined for completeness, but currently it does not work.

    """
    raise NotImplementedError('The implementation is not working for IntFire - goes to invalid objects. \
First fix that issue with SynBase or something in that line.')
    if isinstance(target, str):
        if not moose.exists(target):
            raise ValueError('%s: element does not exist.' % (target))
        target = vec(target)
        moose.syncDataHandler(target)


def showfield(el, field='*', showtype=False):
    """Show the fields of the element `el`, their data types and
    values in human readable format. Convenience function for GENESIS
    users.

    Parameters
    ----------
    el : melement/str
        Element or path of an existing element.

    field : str
        Field to be displayed. If '*' (default), all fields are displayed.

    showtype : bool
        If True show the data type of each field. False by default.

    Returns
    -------
    None

    """
    if isinstance(el, str):
        if not exists(el):
            raise ValueError('no such element')
        el = element(el)
    if field == '*':
        value_field_dict = getFieldDict(el.className, 'valueFinfo')
        max_type_len = max(len(dtype) for dtype in value_field_dict.values())
        max_field_len = max(len(dtype) for dtype in value_field_dict.keys())
        print('\n[', el.path, ']')
        for key, dtype in sorted(value_field_dict.items()):
            if dtype == 'bad' or key == 'this' or key == 'dummy' or key == 'me' or dtype.startswith(
                    'vector') or 'ObjId' in dtype:
                continue
            value = el.getField(key)
            if showtype:
                typestr = dtype.ljust(max_type_len + 4)
                # The following hack is for handling both Python 2 and
                # 3. Directly putting the print command in the if/else
                # clause causes syntax error in both systems.
                print(typestr, end=' ')
            print(key.ljust(max_field_len + 4), '=', value)
    else:
        try:
            print(field, '=', el.getField(field))
        except AttributeError:
            pass  # Genesis silently ignores non existent fields


def showfields(el, showtype=False):
    """Convenience function. Should be deprecated if nobody uses it.

    """
    warnings.warn(
        'Deprecated. Use showfield(element, field="*", showtype=True) instead.',
        DeprecationWarning)
    showfield(el, field='*', showtype=showtype)

# Predefined field types and their human readable names
finfotypes = [('valueFinfo', 'value field'),
              ('srcFinfo', 'source message field'),
              ('destFinfo', 'destination message field'),
              ('sharedFinfo', 'shared message field'),
              ('lookupFinfo', 'lookup field')]


def listmsg(el):
    """Return a list containing the incoming and outgoing messages of
    `el`.

    Parameters
    ----------
    el : melement/vec/str
        MOOSE object or path of the object to look into.

    Returns
    -------
    msg : list
        List of Msg objects corresponding to incoming and outgoing
        connections of `el`.

    """
    obj = element(el)
    ret = []
    for msg in obj.inMsg:
        ret.append(msg)
    for msg in obj.outMsg:
        ret.append(msg)
    return ret


def showmsg(el):
    """Print the incoming and outgoing messages of `el`.

    Parameters
    ----------
    el : melement/vec/str
        Object whose messages are to be displayed.

    Returns
    -------
    None

    """
    obj = element(el)
    print('INCOMING:')
    for msg in obj.msgIn:
        print(
            msg.e2.path,
            msg.destFieldsOnE2,
            '<---',
            msg.e1.path,
            msg.srcFieldsOnE1)
    print('OUTGOING:')
    for msg in obj.msgOut:
        print(
            msg.e1.path,
            msg.srcFieldsOnE1,
            '--->',
            msg.e2.path,
            msg.destFieldsOnE2)


def getfielddoc(tokens, indent=''):
    """Return the documentation for field specified by `tokens`.

    Parameters
    ----------
    tokens : (className, fieldName) str
        A sequence whose first element is a MOOSE class name and second
        is the field name.

    indent : str
        indentation (default: empty string) prepended to builtin
        documentation string.

    Returns
    -------
    docstring : str
        string of the form
        `{indent}{className}.{fieldName}: {datatype} - {finfoType}\n{Description}\n`

    Raises
    ------
    NameError
        If the specified fieldName is not present in the specified class.
    """
    assert(len(tokens) > 1)
    classname = tokens[0]
    fieldname = tokens[1]
    while True:
        try:
            classelement = moose.element('/classes/' + classname)
            for finfo in classelement.children:
                for fieldelement in finfo:
                    baseinfo = ''
                    if classname != tokens[0]:
                        baseinfo = ' (inherited from {})'.format(classname)
                    if fieldelement.fieldName == fieldname:
                        # The field elements are
                        # /classes/{ParentClass}[0]/{fieldElementType}[N].
                        finfotype = fieldelement.name
                        return '{indent}{classname}.{fieldname}: type={type}, finfotype={finfotype}{baseinfo}\n\t{docs}\n'.format(
                            indent=indent, classname=tokens[0],
                            fieldname=fieldname,
                            type=fieldelement.type,
                            finfotype=finfotype,
                            baseinfo=baseinfo,
                            docs=fieldelement.docs)
            classname = classelement.baseClass
        except ValueError:
            raise NameError('`%s` has no field called `%s`'
                            % (tokens[0], tokens[1]))


def toUnicode(v, encoding='utf8'):
    # if isinstance(v, str):
        # return v
    try:
        return v.decode(encoding)
    except (AttributeError, UnicodeEncodeError):
        return str(v)


def getmoosedoc(tokens, inherited=False):
    """Return MOOSE builtin documentation.

    Parameters
    ----------
    tokens : (className, [fieldName])
        tuple containing one or two strings specifying class name
        and field name (optional) to get documentation for.

    inherited: bool (default: False)
        include inherited fields.

    Returns
    -------
    docstring : str
        Documentation string for class `className`.`fieldName` if both
        are specified, for the class `className` if fieldName is not
        specified. In the latter case, the fields and their data types
        and finfo types are listed.

    Raises
    ------
    NameError
        If class or field does not exist.

    """
    indent = '    '
    with closing(StringIO()) as docstring:
        if not tokens:
            return ""
        try:
            class_element = moose.element('/classes/%s' % (tokens[0]))
        except ValueError:
            raise NameError('name \'%s\' not defined.' % (tokens[0]))
        if len(tokens) > 1:
            docstring.write(toUnicode(getfielddoc(tokens)))
        else:
            docstring.write(toUnicode('%s\n' % (class_element.docs)))
            append_finfodocs(tokens[0], docstring, indent)
            if inherited:
                mro = eval('moose.%s' % (tokens[0])).mro()
                for class_ in mro[1:]:
                    if class_ == moose.melement:
                        break
                    docstring.write(toUnicode(
                        '\n\n#Inherited from %s#\n' % (class_.__name__)))
                    append_finfodocs(class_.__name__, docstring, indent)
                    if class_ == moose.Neutral:    # Neutral is the toplevel moose class
                        break
        return docstring.getvalue()


def append_finfodocs(classname, docstring, indent):
    """Append list of finfos in class name to docstring"""
    try:
        class_element = moose.element('/classes/%s' % (classname))
    except ValueError:
        raise NameError('class \'%s\' not defined.' % (classname))
    for ftype, rname in finfotypes:
        docstring.write(toUnicode('\n*%s*\n' % (rname.capitalize())))
        try:
            finfo = moose.element('%s/%s' % (class_element.path, ftype))
            for field in finfo.vec:
                docstring.write(toUnicode(
                    '%s%s: %s\n' % (indent, field.fieldName, field.type)))
        except ValueError:
            docstring.write(toUnicode('%sNone\n' % (indent)))


# the global pager is set from pydoc even if the user asks for paged
# help once. this is to strike a balance between GENESIS user's
# expectation of control returning to command line after printing the
# help and python user's expectation of seeing the help via more/less.
pager = None


def doc(arg, inherited=True, paged=True):
    """Display the documentation for class or field in a class.

    Parameters
    ----------
    arg : str/class/melement/vec
        A string specifying a moose class name and a field name
        separated by a dot. e.g., 'Neutral.name'. Prepending `moose.`
        is allowed. Thus moose.doc('moose.Neutral.name') is equivalent
        to the above.
        It can also be string specifying just a moose class name or a
        moose class or a moose object (instance of melement or vec
        or there subclasses). In that case, the builtin documentation
        for the corresponding moose class is displayed.

    paged: bool
        Whether to display the docs via builtin pager or print and
        exit. If not specified, it defaults to False and
        moose.doc(xyz) will print help on xyz and return control to
        command line.

    Returns
    -------
    None

    Raises
    ------
    NameError
        If class or field does not exist.

    """
    # There is no way to dynamically access the MOOSE docs using
    # pydoc. (using properties requires copying all the docs strings
    # from MOOSE increasing the loading time by ~3x). Hence we provide a
    # separate function.
    global pager
    if paged and pager is None:
        pager = pydoc.pager
    tokens = []
    text = ''
    if isinstance(arg, str):
        tokens = arg.split('.')
        if tokens[0] == 'moose':
            tokens = tokens[1:]
    elif isinstance(arg, type):
        tokens = [arg.__name__]
    elif isinstance(arg, melement) or isinstance(arg, vec):
        text = '%s: %s\n\n' % (arg.path, arg.className)
        tokens = [arg.className]
    if tokens:
        text += getmoosedoc(tokens, inherited=inherited)
    else:
        text += pydoc.getdoc(arg)
    if pager:
        pager(text)
    else:
        print(text)


#
# moose.py ends here
