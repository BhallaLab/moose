# compare_genesis.py --- 
# 
# Filename: compare_genesis.py
# Description: 
# Author: 
# Maintainer: 
# Created: Fri Jun 21 15:31:01 2013 (+0530)
# Version: 
# Last-Updated: Wed Jun 26 14:38:58 2013 (+0530)
#           By: subha
#     Update #: 30
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# 
# 
# 

# Change log:
# 
# 
# 
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
# 

# Code:

from pylab import *

moose_soma = loadtxt('symcompartment.txt')
plot(moose_soma[:,0], moose_soma[:,1], color='#114477', ls='', marker='o', label='moose-soma', alpha=0.5)
plot(moose_soma[:,0], moose_soma[:,2], color='#4477AA', ls='', marker='o', label='moose-d1', alpha=0.5)
plot(moose_soma[:,0], moose_soma[:,3], color='#77AADD', ls='', marker='o', label='moose-d2', alpha=0.5)

moose_soma = loadtxt('symcompartment_readcell.txt')
plot(moose_soma[:,0], moose_soma[:,1], color='#117744', ls='', marker='x', label='moose-readcell-soma', alpha=0.5)
plot(moose_soma[:,0], moose_soma[:,2], color='#44AAAA', ls='', marker='x', label='moose-readcell-d1', alpha=0.5)
plot(moose_soma[:,0], moose_soma[:,3], color='#77CCCC', ls='', marker='x', label='moose-readcell-d2', alpha=0.5)

gen_d1 = loadtxt('genesis_d1_Vm.txt')
gen_soma = loadtxt('genesis_soma_Vm.txt')
gen_d2 = loadtxt('genesis_d2_Vm.txt')
plot(gen_soma[:, 0], gen_soma[:, 1], color='#771122', ls='', marker='x', label='gen-soma', alpha=0.5)
plot(gen_d1[:,0], gen_d1[:,1], color='#AA4455', ls='', marker='x', label='gen-d1', alpha=0.5)
plot(gen_d2[:,0], gen_d2[:,1], color='#771155', ls='', marker='x', label='gen-d2', alpha=0.5)

gen_d1 = loadtxt('genesis_readcell_d1_Vm.txt')
gen_soma = loadtxt('genesis_readcell_soma_Vm.txt')
gen_d2 = loadtxt('genesis_readcell_d2_Vm.txt')
plot(gen_soma[:, 0], gen_soma[:, 1], color='#774411', ls='--', label='gen-readcell-soma', alpha=0.5)
plot(gen_d1[:,0], gen_d1[:,1], color='#AA7744', ls='--', label='gen-readcell-d1', alpha=0.5)
plot(gen_d2[:,0], gen_d2[:,1], color='#DDAA77', ls='--', label='gen-readcell-d2', alpha=0.5)

legend()
show()

# 
# compare_genesis.py ends here
