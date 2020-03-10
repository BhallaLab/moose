# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import

# Author: Subhasis Ray
# Maintainer: Dilawar Singh, Harsha Rani, Upi Bhalla

import warnings
import os
import pydoc
import io
from contextlib import closing

import moose._moose as _moose
import moose.utils as mu


def about():
    """info: Return some 'about me' information.
    """
    return dict(path=os.path.dirname(__file__),
                version=_moose.VERSION,
                docs='https://moose.readthedocs.io/en/latest/')


# Version
def version():
    # Show user version.
    return _moose.VERSION


def pwe():
    """Print present working element. Convenience function for GENESIS
    users. If you want to retrieve the element in stead of printing
    the path, use moose.getCwe()

    >>> pwe()
    >>> '/'
    """
    pwe_ = _moose.getCwe()
    print(pwe_.path)
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
        el = _moose.getCwe()
    elif isinstance(el, str):
        if not _moose.exists(el):
            raise ValueError('no such element')
        el = _moose.element(el)
    elif isinstance(el, _moose.vec):
        el = el[0]
    print("Elements under '%s'" % el.path)
    for ch in el.children:
        print(" %s" % ch.path)
    return [child.path for child in el.children]


# ce is a GENESIS shorthand for change element.
ce = _moose.setCwe


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
    raise NotImplementedError(
        'The implementation is not working for IntFire - goes to invalid objects. \
First fix that issue with SynBase or something in that line.')
    if isinstance(target, str):
        if not _moose.exists(target):
            raise ValueError('%s: element does not exist.' % (target))
        target = _moose.vec(target)
        _moose.syncDataHandler(target)


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
    string

    """
    if isinstance(el, str):
        if not _moose.exists(el):
            raise ValueError('no such element: %s' % el)
        el = _moose.element(el)
    result = []
    if field == '*':
        value_field_dict = _moose.getFieldDict(el.className, 'valueFinfo')
        max_type_len = max(len(dtype) for dtype in value_field_dict.values())
        max_field_len = max(len(dtype) for dtype in value_field_dict.keys())
        result.append('\n[' + el.path + ']\n')
        for key, dtype in sorted(value_field_dict.items()):
            if dtype == 'bad' or key == 'this' or key == 'dummy' \
                or key == 'me' or dtype.startswith('vector') \
                or 'ObjId' in dtype:
                continue
            value = el.getField(key)
            if showtype:
                typestr = dtype.ljust(max_type_len + 4)
                # The following hack is for handling both Python 2 and
                # 3. Directly putting the print command in the if/else
                # clause causes syntax error in both systems.
                result.append(typestr + ' ')
            result.append(
                key.ljust(max_field_len + 4) + '=' + str(value) + '\n')
    else:
        try:
            result.append(field + '=' + el.getField(field))
        except AttributeError:
            pass  # Genesis silently ignores non existent fields
    print(''.join(result))
    return ''.join(result)


def showfields(el, showtype=False):
    """
    Print all fields on a a given element.
    """
    warnings.warn(
        'Deprecated. Use showfield(element, field="*", showtype=True) instead.',
        DeprecationWarning)
    return showfield(el, field='*', showtype=showtype)


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
    obj = _moose.element(el)
    ret = []
    for msg in obj.msgIn:
        ret.append(msg)
    for msg in obj.msgOut:
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
    obj = _moose.element(el)
    print('INCOMING:')
    for msg in obj.msgIn:
        print(msg.e2.path, msg.destFieldsOnE2, '<---', msg.e1.path,
              msg.srcFieldsOnE1)
    print('OUTGOING:')
    for msg in obj.msgOut:
        print(msg.e1.path, msg.srcFieldsOnE1, '--->', msg.e2.path,
              msg.destFieldsOnE2)


def getFieldDoc(tokens, indent=''):
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
    assert (len(tokens) > 1)
    classname = tokens[0]
    fieldname = tokens[1]
    while True:
        try:
            classelement = _moose.element('/classes/' + classname)
            for finfo in classelement.children:
                for fieldelement in finfo:
                    baseinfo = ''
                    if classname != tokens[0]:
                        baseinfo = ' (inherited from {})'.format(classname)
                    if fieldelement.fieldName == fieldname:
                        # The field elements are
                        # /classes/{ParentClass}[0]/{fieldElementType}[N].
                        finfotype = fieldelement.name
                        return u'{indent}{classname}.{fieldname}: type={type}, finfotype={finfotype}{baseinfo}\n\t{docs}\n'.format(
                            indent=indent,
                            classname=tokens[0],
                            fieldname=fieldname,
                            type=fieldelement.type,
                            finfotype=finfotype,
                            baseinfo=baseinfo,
                            docs=fieldelement.docs)
            classname = classelement.baseClass
        except ValueError:
            raise NameError('`%s` has no field called `%s`' %
                            (tokens[0], tokens[1]))

def _appendFinfoDocs(classname, docstring, indent):
    """Append list of finfos in class name to docstring"""
    try:
        classElem = _moose.element('/classes/%s' % (classname))
    except ValueError:
        raise NameError('class \'%s\' not defined.' % (classname))
    for ftype, rname in finfotypes:
        docstring.write(u'\n*%s*\n' % (rname.capitalize()))
        try:
            finfo = _moose.element('%s/%s' % (classElem.path, ftype))
            for field in finfo.vec:
                docstring.write(u'%s%s: %s\n' %
                                (indent, field.fieldName, field.type))
        except ValueError:
            docstring.write(u'%sNone\n' % (indent))



def _getMooseDoc(tokens, inherited=False):
    """Return MOOSE builtin documentation.
    """
    indent = '  '
    docstring = io.StringIO()
    with closing(docstring):
        if not tokens:
            return ""
        try:
            classElem = _moose.element('/classes/%s' % (tokens[0]))
        except ValueError:
            raise NameError("Name '%s' not defined." % (tokens[0]))

        if len(tokens) > 1:
            docstring.write(getFieldDoc(tokens))
            return docstring.getvalue()

        docstring.write(u'%s\n' % (classElem.docs))
        _appendFinfoDocs(tokens[0], docstring, indent)
        if not inherited:
            return docstring.getvalue()

        mro = eval('_moose.%s' % (tokens[0])).mro()
        for class_ in mro[1:]:
            if class_ == _moose.melement:
                break
            docstring.write(u"\n# Inherited from '%s'\n" % (class_.__name__))
            _appendFinfoDocs(class_.__name__, docstring, indent)
            if class_ == _moose.Neutral:
                break
        return docstring.getvalue()


__pager = None

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
    global __pager
    if paged and __pager is None:
        __pager = pydoc.pager
    tokens = []
    text = ''
    if isinstance(arg, str):
        tokens = arg.split('.')
        if tokens[0] in ['moose', '_moose']:
            tokens = tokens[1:]
    elif isinstance(arg, type):
        tokens = [arg.__name__]
    elif isinstance(arg, _moose.melement) or isinstance(arg, _moose.vec):
        text = '%s: %s\n\n' % (arg.path, arg.className)
        tokens = [arg.className]
    if tokens:
        text += _getMooseDoc(tokens, inherited=inherited)
    else:
        text += pydoc.getdoc(arg)
    if __pager:
        __pager(text)
    else:
        print(text)


#
# moose.py ends here
