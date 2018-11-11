# -*- coding: utf-8 -*-
# hhfit.py ---
#
# Filename: hhfit.py
# Description:
# Author:
# Maintainer:
# Created: Tue May 21 16:31:56 2013 (+0530)
# Version:
# Last-Updated: Tue Jun 11 16:57:30 2013 (+0530)
#           By: subha
#     Update #: 34
# URL:
# Keywords:
# Compatibility:
#
#

# Commentary:
#
# Functions for fitting common equations for Hodgkin-Huxley type gate
# equations.
#
#

# Change log:
#
# Tue May 21 16:33:59 IST 2013 - Subha refactored the code from
# converter.py to hhfit.py.
#


#
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 3, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth
# Floor, Boston, MA 02110-1301, USA.
#
#

# Code:

import traceback
import warnings
import moose.utils as mu
import numpy as np

try:
    from scipy.optimize import curve_fit
except ImportError as e:
    mu.error( "To use this feature/module, please install python-scipy" )
    raise e

def exponential2(x, a, scale, x0, y0=0):
    res = a * np.exp((x - x0)/scale) + y0
    #print('============   Calculating exponential2 for %s, a=%s, scale=%s, x0=%s, y0=%s; = %s'%(x, a, scale, x0, y0, res))
    return res

def exponential(x, a, k, x0, y0=0):
    res = a * np.exp(k * (x - x0)) + y0
    #print('============   Calculating exponential for %s, a=%s, k=%s, x0=%s, y0=%s; = %s'%(x, a, k, x0, y0, res))
    return res

def sigmoid2(x, a, scale, x0, y0=0):
    res = a / (np.exp(-1*(x - x0)/scale) + 1.0) + y0
    #print('============   Calculating sigmoid for %s, a=%s, scale=%s, x0=%s, y0=%s; = %s'%(x, a, scale, x0, y0, res))
    return res

def sigmoid(x, a, k, x0, y0=0):
    res = a / (np.exp(k * (x - x0)) + 1.0) + y0
    #print('============   Calculating sigmoid for %s, a=%s, k=%s, x0=%s, y0=%s; = %s'%(x, a, k, x0, y0, res))
    return res

def linoid2(x, a, scale, x0, y0=0):
    """The so called linoid function. Called explinear in neuroml."""
    
    denominator = 1 - np.exp(-1 * (x - x0)/scale)
    # Linoid often includes a zero denominator - we need to fill those
    # points with interpolated values (interpolation is simpler than
    # finding limits).
    ret = (a/scale) *  (x - x0) / denominator
    infidx = np.flatnonzero((ret == np.inf) | (ret == -np.inf))
    if len(infidx) > 0:
        for ii in infidx:
            if ii == 0:
                ret[ii] = ret[ii+1] - (ret[ii+2] - ret[ii+1])
            elif ii == len(ret):
                ret[ii] = ret[ii-1] + (ret[ii-1] - ret[ii-2])
            else:
                ret[ii] = (ret[ii+1] + ret[ii+2]) * 0.5
    res = ret + y0
    #print('============   Calculating linoid2 for %s, a=%s, scale=%s, x0=%s, y0=%s; res=%s'%(x, a, scale, x0, y0,res))
    return res

def linoid(x, a, k, x0, y0=0):
    """The so called linoid function. Called explinear in neuroml."""
    
    denominator = np.exp(k * (x - x0)) - 1.0
    # Linoid often includes a zero denominator - we need to fill those
    # points with interpolated values (interpolation is simpler than
    # finding limits).
    ret = a * (x - x0) / denominator
    infidx = np.flatnonzero((ret == np.inf) | (ret == -np.inf))
    if len(infidx) > 0:
        for ii in infidx:
            if ii == 0:
                ret[ii] = ret[ii+1] - (ret[ii+2] - ret[ii+1])
            elif ii == len(ret):
                ret[ii] = ret[ii-1] + (ret[ii-1] - ret[ii-2])
            else:
                ret[ii] = (ret[ii+1] + ret[ii+2]) * 0.5
    res = ret + y0
    #print('============   Calculating linoid for %s, a=%s, k=%s, x0=%s, y0=%s; res=%s'%(x, a, k, x0, y0,res))
    return res

def double_exp(x, a, k1, x1, k2, x2, y0=0):
    """For functions of the form:

    a / (exp(k1 * (x - x1)) + exp(k2 * (x - x2)))

    """
    ret = np.zeros(len(x))
    try:
        ret = a / (np.exp(k1 * (x - x1)) + np.exp(k2 * (x - x2))) + y0
    except RuntimeWarning as e:
        traceback.print_exc()
    return ret

# Map from the above functions to corresponding neuroml class
fn_rate_map = {
    exponential: 'HHExpRate',
    sigmoid: 'HHSigmoidRate',
    linoid: 'HHExpLinearRate',
    double_exp: None,
}

# These are default starting parameter values
fn_p0_map = {
    exponential: (1.0, -100, 20e-3, 0.0),
    sigmoid: (1.0, 1.0, 0.0, 0.0),
    linoid: (1.0, 1.0, 0.0, 0.0),
    double_exp: (1e-3, -1.0, 0.0, 1.0, 0.0, 0.0),
}

def randomized_curve_fit(fn, x, y, maxiter=10, best=True):
    """Repeatedly search for a good fit for common gate functions for
    HHtype channels with randomly generated initial parameter
    set. This function first tries with default p0 for fn. If that
    fails to find a good fit, (correlation coeff returned by curve_fit
    being inf is an indication of this), it goes on to generate random
    p0 arrays and try scipy.optimize.curve_fit using this p0 until it
    finds a good fit or the number of iterations reaches maxiter.

    Ideally we should be doing something like stochastic gradient
    descent, but I don't know if that might have performance issue in
    pure python. The random parameterization in the present function
    uses uniformly distributed random numbers within the half-open
    interval [min(x), max(x)). The reason for choosing this: the
    offset used in the exponential parts of Boltzman-type/HH-type
    equations are usually within the domain of x. I also invert the
    second entry (p0[1], because it is always (one of) the scale
    factor(s) and usually 1/v for some v in the domain of x. I have
    not tested the utility of this inversion. Even without this
    inversion, with maxiter=100 this function is successful for the
    test cases.

    Parameters
    ----------
    x: ndarray
    values of the independent variable

    y: ndarray
    sample values of the dependent variable

    maxiter: int
    maximum number of iterations

    best: bool
    if true, repeat curve_fit for maxiter and return the case of least
    squared error.

    Returns
    -------
    The return value of scipy.optimize.curve_fit which succeed, or the
    last call to it if maxiter iterations is reached..

    """
    bad = True
    p0 = fn_p0_map[fn]
    p = None
    p_best = None
    min_err = 1e10 # A large value as placeholder
    for ii in range(maxiter):
        try:
            p = curve_fit(fn, x, y,  p0=p0, full_output=True)
        except (RuntimeError, RuntimeWarning) as e:
            p = None
        # The last entry returned by scipy.optimize.leastsq used by
        # curve_fit is 1, 2, 3 or 4 if it succeeds.
        bad = (p is None) or np.any(p[1] == np.inf) or (p[-1] not in [1, 2, 3, 4])
        if not bad:
            if not best:
                return p
            err = sum((y - fn(x, *tuple(p[0])))**2)
            if err < min_err:
                min_err = err
                p_best = p
        p0 = np.random.uniform(low=min(x), high=max(x), size=len(fn_p0_map[fn]))
        if p0[1] != 0.0:
            p0[1] = 1 / p0[1] # k = 1/v_scale - could help faster convergence
    if p_best is None:
        if p is not None:
            msg = p[-2]
        else:
            msg = ''
        warnings.warn('Maximum iteration %d reached. Could not find a decent fit. %s' % (maxiter, msg), RuntimeWarning)
    return p_best

def find_ratefn(x, y, **kwargs):
    """Find the function that fits the rate function best. This will try
    exponential, sigmoid and linoid and return the best fit.

    Needed until NeuroML2 supports tables or MOOSE supports
    functions.

    Parameters
    ----------
    x: 1D array
    independent variable.

    y: 1D array
    function values.

    **kwargs: keyword arguments
    passed to randomized_curve_fit.

    Returns
    -------
    best_fn: function
    the best fit function.

    best_p: tuple
    the optimal parameter values for the best fit function.

    """
    rms_error = 1e10 # arbitrarily setting this
    best_fn = None
    best_p = None
    for fn in fn_rate_map:
        p = randomized_curve_fit(fn, x, y, **kwargs)
        if p is None:
            continue
        popt = p[0]
        pcov = p[1]
        error = y - fn(x, *popt)
        erms = np.sqrt(np.mean(error**2))
        # Ideally I want a fuzzy selection criterion here - a
        # preference for fewer parameters, but if the errors are
        # really small then we go for functions with more number of
        # parameters. Some kind of weighted decision would have been
        # nice. I am arbitrarily setting less than 0.1% relative error
        # as a strong argument for taking a longer parameter function
        # as a really better fit. Even with 1%, double exponential
        # betters detected sigmoid for sigmoid curve in test case.
        if erms < rms_error and \
           ((best_p is None) or \
            len(popt) <= len(best_p) or \
            erms / (max(y) - min(y)) < 0.001):
            rms_error = erms
            best_fn = fn
            best_p = popt
    return (best_fn, best_p)



#
# hhfit.py ends here
