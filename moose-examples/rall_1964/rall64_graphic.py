# rall64_graphic.py --- 
# 
# Filename: rall64_graphic.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Thu May 29 13:18:12 2014 (+0530)
# Version: 
# Last-Updated: 
#           By: 
#     Update #: 0
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

"""Rall 1964 demo with fancy graphics"""

from matplotlib import pyplot as plt
from matplotlib import animation as anim
import numpy as np
from rall64 import *

fig = plt.figure('Rall 1964, Figure 7')
fig.suptitle('Rall 1964, Figure 7')
ax_model = fig.add_subplot(2, 1, 1)
ax_data = fig.add_subplot(2, 1, 2)

colors = ['darkblue', 'darkgreen', 'darkred']
active_colors = ['lightblue', 'lightgreen', 'lightpink']
radius = 0.5
length = 0.8
cables = []
lines = []
for yy in range(3):
    cables.append([])
    for xx in range(10):
        patch = ax_model.add_patch(plt.Rectangle((xx, 2*yy), length, 2 * radius, color=colors[yy]))
        cables[yy].append(patch)
        txt = ax_model.annotate('%d' % (xx), (xx, 2*yy), xytext=(xx + length/2.0, 2 * yy + radius), color='gray')
ax_model.set_xlim((-1, xx + 1))
ax_model.set_ylim((-1, 2 * yy + 3 * radius))
# ax_model.xaxis.set_visible(False)
# ax_model.yaxis.set_visible(False)
ax_model.set_axis_off()
stim_no = 0
stim_count = 4
clock = moose.element('/clock')
chan_list, vm_list = setup_model_fig7()
stim_t = 0.25 * tau
update_dt = dt
xdata = [[],[],[]]
ydata = [[],[],[]]

def update(xx):
    global stim_no, chan_list, vm_list, cables, clock
    if stim_no < stim_count:
        cables[0][2 * stim_no+1].set_facecolor(active_colors[0])
        cables[0][2 * stim_no+1].set_edgecolor('y')
        cables[0][2 * stim_no + 2].set_facecolor(active_colors[0])
        cables[0][2 * stim_no + 2].set_edgecolor('y')
        cables[1][- 2 * stim_no - 2].set_facecolor(active_colors[1])        
        cables[1][- 2 * stim_no - 2].set_edgecolor('y')        
        cables[1][- 2 * stim_no - 3].set_facecolor(active_colors[1])                
        cables[1][- 2 * stim_no - 3].set_edgecolor('y')
        for patch in cables[2][1:-1]:
            patch.set_facecolor(active_colors[2])
            patch.set_edgecolor('y')            
        if clock.currentTime < stim_t * (stim_no + 1):
            chan_list[0][2 * stim_no].Gk = 1 / Rm
            chan_list[0][2 * stim_no + 1].Gk = 1 / Rm
            chan_list[1][- 2 * stim_no - 1].Gk = 1 / Rm
            chan_list[1][- 2 * stim_no - 2].Gk = 1 / Rm
            for chan in chan_list[2]:
                chan.Gk = 0.25 / Rm
            moose.start(update_dt)
        else:
            stim_no += 1
            for chans in chan_list:
                for ch in chans:
                    ch.Gk = 0.0
            for ii in range(len(cables)):
                for patch in cables[ii]:
                    patch.set_color(colors[ii])
    elif clock.currentTime < simtime:
        
        moose.start(update_dt)
        for patch in cables[0]:
            patch.set_facecolor(colors[0])
        for patch in cables[1]:
            patch.set_facecolor(colors[1])
    for ii, (tmp_lines, vm, color) in enumerate(zip(lines, vm_list, colors)):
        for line in tmp_lines:
            line.set_data(np.linspace(0, clock.currentTime, len(vm.vector))/tau, (vm.vector - Em) / (Ek - Em))
    

#    """A variation of rall64.run_model_fig7 with visualization of the compartments."""
lines.append(ax_data.plot([0], [0], label='(1,2)->(3,4)->(5,6)->(7,8)', color=colors[0]))
lines.append(ax_data.plot([0], [0], label='(7,8)->(5,6)->(3,4)->(1,2)', color=colors[1]))
lines.append(ax_data.plot([0], [0], label='control', color=colors[2]))
ax_data.set_xlim(0, simtime/tau)
ax_data.set_ylim(0, 0.15)
ax_data.set_xlabel('Time (t/tau)')
ax_data.set_ylabel('Membrane voltage  (Vm - Em)/(Ek - Vm)')
print(lines)
plt.legend()
schedule()
fanim = anim.FuncAnimation(fig, update, fargs=None, interval=25, repeat=False)
plt.show()


# 
# rall64_graphic.py ends here
