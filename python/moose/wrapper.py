# -*- coding: utf-8 -*-
from __future__ import print_function, division, absolute_import

# Wrappers around _moose.so classes. These wrappers accomplish the following.
#
# 1. When `moose.MooseClass(path)` is called and pass already exsists,
#    `moose.element(path)` is returned.
# 2. `moose.connect(a, 'f1', b, 'f2')` can also be called as
#     a.connect('f1', b, 'fn2')

__author__ = "Dilawar Singh"
__copyright__ = "Copyright 2019-, Dilawar Singh"
__maintainer__ = "Dilawar Singh"
__email__ = "dilawars@ncbs.res.in"

import difflib
import moose._moose as _moose
import pprint

import logging

logger_ = logging.getLogger("moose")

# Provide a pretty printer.
pprint = pprint.pprint

sympyFound_ = False
try:
    import sympy

    sympy.init_printing(use_unicode=True)
    sympyFound_ = True
    # When sympy is found, pprint is sympy pprint.
    pprint = sympy.pprint
except ImportError:
    pass


def _didYouMean(v, options):
    """Did you mean this when you types v
    """
    return " or ".join(difflib.get_close_matches(v, list(options), n=2))


def _eval(expr):
    try:
        if isinstance(expr, (int, float)):
            return expr
        return eval(str(expr))
    except Exception:
        return expr


def _prettifyExpr(expr):
    global sympyFound_
    if not sympyFound_:
        return expr
    try:
        return sympy.pretty(sympy.simplify(expr, use_unicode=True))
    except Exception:
        return expr


# Generic wrapper around moose.Neutral
class Neutral(_moose.Neutral):
    def __init__(self, path, n=1, g=0, dtype="Neutral", **kwargs):
        super(Neutral, self).__init__(path, n, g, dtype)
        for k in kwargs:
            try:
                setattr(self, k, kwargs[k])
            except AttributeError:
                logging.warn("Attribute %s is not found. Ignoring..." % k)

    def __new__(cls, pathOrObject, n=1, g=0, dtype="Neutral", **kwargs):
        path = pathOrObject
        if not isinstance(pathOrObject, str):
            path = pathOrObject.path
        if _moose.exists(path):
            #  logger_.info("%s already exists. Returning old element."%path)
            return _moose.element(path)
        return super(Neutral, cls).__new__(cls, pathOrObject, n, g, dtype)

    def connect(self, srcField, dest, destField):
        """Wrapper around moose.connect.
        """
        allSrcFields = self.sourceFields
        allDestFields = dest.destFields

        if srcField not in allSrcFields:
            logger_.warn(
                "Could not find '{}' in {} sourceFields.".format(srcField, self)
            )
            dym = _didYouMean(srcField, allSrcFields)
            if dym:
                logger_.warn("\tDid you mean %s?" % dym)
            else:
                logger_.warn(": Available options: %s" % ", ".join(allSrcFields))
            raise NameError("Failed to connect")
        if destField not in allDestFields:
            logger_.error(
                "Could not find '{0}' in {1} destFields.".format(destField, dest)
            )
            dym = _didYouMean(destField, allDestFields)
            if dym:
                logger_.warn("\tDid you mean %s?" % dym)
            else:
                logger_.warn(": Available options: %s" % ", ".join(allDestFields))
            raise NameError("Failed to connect")

        # Everything looks fine. Connect now.
        _moose.connect(self, srcField, dest, destField)


class Function(_moose.Function, Neutral):
    """Overides moose._Function

    Provides a convinient way to set expression and connect variables.
    """

    __expr = ""

    def __init__(self, path, n=1, g=0, dtype="Function", **kwargs):
        super(Function, self).__init__(path, n, g, dtype)
        for k in kwargs:
            try:
                setattr(self, k, kwargs[k])
            except AttributeError:
                logging.warn("Attribute %s is not found. Ignoring..." % k)

    def __getitem__(self, key):
        """Override [] operator. It returns the linked variable by name.
        """
        assert self.numVars > 0
        return self.x[self.xindex[key]]

    def compile(self, expr, constants={}, variables=[], mode=0, **kwargs):
        """Add an expression to a given function.
        """
        __expr = expr
        # Replace constants.
        constants = {k: v for k, v in sorted(constants.items(), key=lambda x: len(x))}
        for i, constName in enumerate(constants):
            # replace constName with 'c%d' % i
            mooseConstName = "c%d" % i
            expr = expr.replace(constName, mooseConstName)
            self.c[mooseConstName] = _eval(constants[constName])

        self.expr = expr
        self.mode = mode
        if kwargs.get("independent", ""):
            self.independent = kwargs["independent"]

        if __expr != expr:
            msg = "Expression has been changed to MOOSE's form."
            msg += "\nFrom,\n"
            msg += _prettifyExpr(__expr)
            msg += "\nto, \n"
            msg += _prettifyExpr(expr)
            logging.warn(msg.replace("\n", "\n ﹅ "))

    # alias
    setExpr = compile

    def sympyExpr(self):
        import sympy

        return sympy.simplify(self.expr)

    def printUsingSympy(self):
        """Print function expression using sympy.
        """
        import sympy

        sympy.pprint(self.sympyExpr())


def addAttrib(mooseObject, **kwargs):
    for k, v in kwargs.items():
        try:
            setattr(mooseObject, k, v)
        except AttributeError:
            logger_.warn("Attribute {0} not found on {1}".format(k, v))


class StimulusTable(_moose.StimulusTable, Neutral):
    """StimulusTable
    Wrapper around _moose.StimulusTable
    """

    def __init__(self, path, n=1, g=0, dtype="StimulusTable", **kwargs):
        super(StimulusTable, self).__init__(path, n, g, dtype)
        addAttrib(self, **kwargs)


class IntFire(_moose.IntFire, Neutral):
    """IntFire
    Wrapper around _moose.IntFire
    """

    def __init__(self, path, n=1, g=0, dtype="IntFire", **kwargs):
        super(IntFire, self).__init__(path, n, g, dtype)
        addAttrib(self, **kwargs)


class PulseGen(_moose.PulseGen, Neutral):
    """PulseGen
    Wrapper around _moose.PulseGen
    """

    def __init__(self, path, n=1, g=0, dtype="PulseGen", **kwargs):
        super(PulseGen, self).__init__(path, n, g, dtype)
        addAttrib(self, **kwargs)
