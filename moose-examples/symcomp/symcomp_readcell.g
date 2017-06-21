include compartments

float EREST_ACT = -0.06
float SIMTIME = 100e-3
float SIMDT = 1e-6
float NSTEPS = {SIMTIME/SIMDT}


create neutral /library
disable /library
pushe /library
make_cylind_symcompartment
pope

readcell symcomp.p /cell

create pulsegen pg
setfield pg level1 1e-6 delay1 10e-3 width1 20e-3 delay2 1e9
addmsg pg /cell/d1 INJECT output

create table soma_tab
setfield soma_tab step_mode 3
call soma_tab TABCREATE {NSTEPS} 0 {SIMTIME}
addmsg /cell/soma soma_tab INPUT Vm

create table d1_tab
setfield d1_tab step_mode 3
call d1_tab TABCREATE {NSTEPS} 0 {SIMTIME}
addmsg /cell/d1 d1_tab INPUT Vm

create table d2_tab
setfield d2_tab step_mode 3
call d2_tab TABCREATE {NSTEPS} 0 {SIMTIME}
addmsg /cell/d2 d2_tab INPUT Vm

setclock 0 {SIMDT}
useclock 0 /##
reset
step {SIMTIME} -t

tab2file genesis_readcell_soma_Vm.txt soma_tab table -mode xy -nentries {NSTEPS} -overwrite
tab2file genesis_readcell_d1_Vm.txt d1_tab table -mode xy -nentries {NSTEPS} -overwrite
tab2file genesis_readcell_d2_Vm.txt d2_tab table -mode xy -nentries {NSTEPS} -overwrite

