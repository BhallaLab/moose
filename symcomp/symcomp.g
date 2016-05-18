float SIMTIME = 100e-3
float SIMDT = 1e-6
float NSTEPS = {SIMTIME/SIMDT}

create symcompartment soma
setfield soma Em -60e-3 Rm 1e9 Cm 1e-11 Ra 1e6
create symcompartment d1
setfield d1 Em -60e-3 Rm 1e8 Cm 1e-10 Ra 1e7
create symcompartment d2
setfield d2 Em -60e-3 Rm 1e8 Cm 1e-10 Ra 2e7
addmsg d1 soma CONNECTTAIL Ra Vm
addmsg d2 soma CONNECTTAIL Ra Vm
addmsg soma d1 CONNECTHEAD Ra Vm
addmsg soma d2 CONNECTHEAD Ra Vm
addmsg d1 d2 CONNECTCROSS Ra Vm
addmsg d2 d1 CONNECTCROSS Ra Vm

create pulsegen pg
setfield pg level1 1e-6 delay1 10e-3 width1 20e-3 delay2 1e9
addmsg pg d1 INJECT output

create table soma_tab
setfield soma_tab step_mode 3
call soma_tab TABCREATE {NSTEPS} 0 {SIMTIME}
addmsg soma soma_tab INPUT Vm

create table d1_tab
setfield d1_tab step_mode 3
call d1_tab TABCREATE {NSTEPS} 0 {SIMTIME}
addmsg d1 d1_tab INPUT Vm

create table d2_tab
setfield d2_tab step_mode 3
call d2_tab TABCREATE {NSTEPS} 0 {SIMTIME}
addmsg d2 d2_tab INPUT Vm

setclock 0 {SIMDT}
useclock 0 /##
reset
step {SIMTIME} -t

tab2file genesis_soma_Vm.txt soma_tab table -mode xy -nentries {NSTEPS} -overwrite
tab2file genesis_d1_Vm.txt d1_tab table -mode xy -nentries {NSTEPS} -overwrite
tab2file genesis_d2_Vm.txt d2_tab table -mode xy -nentries {NSTEPS} -overwrite

// quit
