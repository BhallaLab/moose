# -*- coding: utf-8 -*-
"""params.py

Parameters used in this model

These parameters are from paper Miller et. al. "The stability of CaMKII
switch"

"""

__author__           = "Dilawar Singh"
__copyright__        = "Copyright 2015, Dilawar Singh and NCBS Bangalore"
__credits__          = ["NCBS Bangalore"]
__license__          = "GNU GPL"
__version__          = "1.0.0"
__maintainer__       = "Dilawar Singh"
__email__            = "dilawars@ncbs.res.in"
__status__           = "Development"


run_time             = 30
N_CaMK               = 10
N_PP1                = 100
num_switch           = 1
voxel_length         = 125e-9
num_voxels           = 1

diff_consts          = { 'x' : 1e-13, 'y' : 1e-13, 'pp1' : 1e-13 }

conc_i1_free         = 0.1e-3
act_CaN              = 1.0
act_PKA              = 1.0

# Michaelis constant of protein phosphatase.
# 0.4 um to 2.0 uM have been used. Miller uses 0.2 um. The switch shows
# bistability of these ranges. We have taken the largest Km (or slowest) first
# step in dephosphorylation.
K_M                  = 10e-3
k_2                  = 10.0

# Hill coefficientfor Ca++ activation of CaMKII
K_H1                 = 0.7e-3
K_H2                 = 0.3e-3

k_1                  = 1.5
k_3                  = 100e3
k_4                  = 0.001
K_I                  = 1e-6

rate_loosex          = 0.1
rate_loosey          = 0.1
rate_gainx           = 1
rate_gainy           = 1

turnover_rate        = 1/(30*3600.0)
v_1                  = 1.268e-5
v_2                  = 4.36e-3
phosphatase_inhibit  = 280.0
vi                   = phosphatase_inhibit

## Calcium input expression.
ca_basal             = 80e-6
ca_expr              = "(fmod(t,4)<2)?{0}:({0}*(1+0.5*rand(-1)))".format( ca_basal )
