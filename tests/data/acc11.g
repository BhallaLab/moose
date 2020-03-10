//genesis
// kkit Version 8 flat dumpfile
 
// Saved on Mon Nov 12 13:36:11 2001
 
include kkit {argv 1}
 
FASTDT = 1e-05
SIMDT = 0.001
CONTROLDT = 1
PLOTDT = 0.1
MAXTIME = 400
TRANSIENT_TIME = 5
VARIABLE_DT_FLAG = 1
DEFAULT_VOL = 1.6667e-21
VERSION = 8.0
setfield /file/modpath value /home2/bhalla/scripts/modules
kparms
 
//genesis

initdump -version 3 -ignoreorphans 1
simobjdump table input output alloced step_mode stepsize x y z
simobjdump xtree path script namemode sizescale
simobjdump xcoredraw xmin xmax ymin ymax
simobjdump xtext editable
simobjdump xgraph xmin xmax ymin ymax overlay
simobjdump xplot pixflags script fg ysquish do_slope wy
simobjdump group xtree_fg_req xtree_textfg_req plotfield expanded movealone \
  link savename file version md5sum mod_save_flag x y z
simobjdump kpool CoTotal CoInit Co n nInit nTotal nMin vol slave_enable notes \
  xtree_fg_req xtree_textfg_req x y z
simobjdump kreac kf kb notes xtree_fg_req xtree_textfg_req x y z
simobjdump kenz CoComplexInit CoComplex nComplexInit nComplex vol k1 k2 k3 \
  keepconc usecomplex notes xtree_fg_req xtree_textfg_req link x y z
simobjdump stim level1 width1 delay1 level2 width2 delay2 baselevel trig_time \
  trig_mode notes xtree_fg_req xtree_textfg_req is_running x y z
simobjdump xtab input output alloced step_mode stepsize notes editfunc \
  xtree_fg_req xtree_textfg_req baselevel last_x last_y is_running x y z
simobjdump kchan perm gmax Vm is_active use_nernst notes xtree_fg_req \
  xtree_textfg_req x y z
simobjdump proto x y z
simobjdump linkinfo xtree_fg_req xtree_textfg_req uplink downlink x y z
simobjdump uplink xtree_fg_req xtree_textfg_req x y z
simobjdump downlink xtree_fg_req xtree_textfg_req x y z
simobjdump mirror notes xtree_fg_req x y z
simundump group /kinetics/neuroNOS 0 yellow black x 0 0 "" neuroNOS \
  /home2/bhalla/scripts/modules/neuroNOS_0.g 0 0 0 -19 -20 0
simundump kpool /kinetics/neuroNOS/nNOS 0 5 0.5 0.5 0.5 0.5 5 0 1 0 "" 27 \
  yellow -15 -17 0
simundump kpool /kinetics/neuroNOS/Ca-CaM_nNOS 0 0 0 0 0 0 0 0 1 0 "" 33 \
  yellow -17 -11 0
simundump kreac /kinetics/neuroNOS/Ca-CaMbind-nNOS 0 3.25 0.05 "" white \
  yellow -16 -14 0
simundump kreac /kinetics/neuroNOS/nNOSbindArg 0 0.5 81 "" white yellow -18 \
  -14 0
simundump kpool /kinetics/neuroNOS/L-Arg 0 100 100 100 100 100 100 0 1 0 "" 9 \
  yellow -17 -17 0
simundump kreac /kinetics/neuroNOS/k1 0 25 0 "" white yellow -20 -14 0
simundump kreac /kinetics/neuroNOS/k3 0 19 0 "" white yellow -22 -14 0
simundump kpool /kinetics/neuroNOS/OH.Arg 0 0 0 0 0 0 0 0 1 0 "" 34 yellow \
  -25 -11 0
simundump kpool /kinetics/neuroNOS/nNOS.Cit+NO 0 0 0 0 0 0 0 0 1 0 "" 62 \
  yellow -21 -17 0
simundump kpool /kinetics/neuroNOS/Cit 0 0 0 0 0 0 0 0 1 0 "" 12 yellow -23 \
  -17 0
simundump kpool /kinetics/neuroNOS/NO 0 5 0 0 0 0 5 0 1 0 "" 4 yellow -25 -17 \
  0
simundump kreac /kinetics/neuroNOS/k4 0 2.7 0 "" white yellow -22 -20 0
simundump kpool /kinetics/neuroNOS/nNOS.OH.Arg 0 0 0 0 0 0 0 0 1 0 "" 27 \
  yellow -21 -11 0
simundump kpool /kinetics/neuroNOS/nNOS.arg 0 0 0 0 0 0 0 0 1 0 "" 6 yellow \
  -19 -11 0
simundump kreac /kinetics/neuroNOS/k2 0 4.4 0 "" white yellow -24 -14 0
simundump kpool /kinetics/neuroNOS/CaM-Ca4 1 5.0003 0 0 0 0 5.0003 0 1 0 "" \
  blue yellow -15 -11 0
simundump xgraph /graphs/conc1 0 0 400 -0.05 0.05 0
simundump xgraph /graphs/conc2 0 0 400 0 0.0015779 0
simundump xplot /graphs/conc1/neurogranin.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0 0 1
simundump xplot /graphs/conc1/neurogranin*.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0 0 1
simundump xplot /graphs/conc2/CaM-Ca4.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xgraph /moregraphs/conc3 0 0 400 99.666 100 0
simundump xgraph /moregraphs/conc4 0 0 400 0 0.27092 0
simundump xplot /moregraphs/conc3/L-Arg.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 9 0 0 1
simundump xplot /moregraphs/conc4/nNOS.OH.Arg.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 27 0 0 1
simundump xplot /moregraphs/conc4/NO.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 4 0 0 1
simundump xcoredraw /edit/draw 0 -27 -13 -22 -9
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"Incorporated basal dephosph of " \
"neurogranin which gives around 20%" \
"phosph at basal PKC activity." \
"Deleted CONSERVE msg from Ca to" \
"itself."
addmsg /kinetics/neuroNOS/Ca-CaMbind-nNOS /kinetics/neuroNOS/nNOS REAC A B 
addmsg /kinetics/neuroNOS/Ca-CaMbind-nNOS /kinetics/neuroNOS/Ca-CaM_nNOS REAC B A 
addmsg /kinetics/neuroNOS/nNOSbindArg /kinetics/neuroNOS/Ca-CaM_nNOS REAC A B 
addmsg /kinetics/neuroNOS/Ca-CaM_nNOS /kinetics/neuroNOS/Ca-CaMbind-nNOS PRODUCT n 
addmsg /kinetics/neuroNOS/nNOS /kinetics/neuroNOS/Ca-CaMbind-nNOS SUBSTRATE n 
addmsg /kinetics/neuroNOS/CaM-Ca4 /kinetics/neuroNOS/Ca-CaMbind-nNOS SUBSTRATE n 
addmsg /kinetics/neuroNOS/L-Arg /kinetics/neuroNOS/nNOSbindArg SUBSTRATE n 
addmsg /kinetics/neuroNOS/nNOS.arg /kinetics/neuroNOS/nNOSbindArg PRODUCT n 
addmsg /kinetics/neuroNOS/Ca-CaM_nNOS /kinetics/neuroNOS/nNOSbindArg SUBSTRATE n 
addmsg /kinetics/neuroNOS/nNOSbindArg /kinetics/neuroNOS/L-Arg REAC A B 
addmsg /kinetics/neuroNOS/nNOS.arg /kinetics/neuroNOS/k1 SUBSTRATE n 
addmsg /kinetics/neuroNOS/nNOS.OH.Arg /kinetics/neuroNOS/k1 PRODUCT n 
addmsg /kinetics/neuroNOS/nNOS.OH.Arg /kinetics/neuroNOS/k3 SUBSTRATE n 
addmsg /kinetics/neuroNOS/nNOS.Cit+NO /kinetics/neuroNOS/k3 PRODUCT n 
addmsg /kinetics/neuroNOS/k2 /kinetics/neuroNOS/OH.Arg REAC B A 
addmsg /kinetics/neuroNOS/k3 /kinetics/neuroNOS/nNOS.Cit+NO REAC B A 
addmsg /kinetics/neuroNOS/k4 /kinetics/neuroNOS/nNOS.Cit+NO REAC A B 
addmsg /kinetics/neuroNOS/k4 /kinetics/neuroNOS/Cit REAC B A 
addmsg /kinetics/neuroNOS/k4 /kinetics/neuroNOS/NO REAC B A 
addmsg /kinetics/neuroNOS/nNOS.Cit+NO /kinetics/neuroNOS/k4 SUBSTRATE n 
addmsg /kinetics/neuroNOS/Cit /kinetics/neuroNOS/k4 PRODUCT n 
addmsg /kinetics/neuroNOS/NO /kinetics/neuroNOS/k4 PRODUCT n 
addmsg /kinetics/neuroNOS/k2 /kinetics/neuroNOS/nNOS.OH.Arg REAC A B 
addmsg /kinetics/neuroNOS/k3 /kinetics/neuroNOS/nNOS.OH.Arg REAC A B 
addmsg /kinetics/neuroNOS/k1 /kinetics/neuroNOS/nNOS.OH.Arg REAC B A 
addmsg /kinetics/neuroNOS/nNOSbindArg /kinetics/neuroNOS/nNOS.arg REAC B A 
addmsg /kinetics/neuroNOS/k1 /kinetics/neuroNOS/nNOS.arg REAC A B 
addmsg /kinetics/neuroNOS/nNOS.OH.Arg /kinetics/neuroNOS/k2 SUBSTRATE n 
addmsg /kinetics/neuroNOS/OH.Arg /kinetics/neuroNOS/k2 PRODUCT n 
addmsg /kinetics/neuroNOS/Ca-CaMbind-nNOS /kinetics/neuroNOS/CaM-Ca4 REAC A B 
addmsg /kinetics/neuroNOS/CaM-Ca4 /graphs/conc2/CaM-Ca4.Co PLOT Co *CaM-Ca4.Co *blue 
addmsg /kinetics/neuroNOS/L-Arg /moregraphs/conc3/L-Arg.Co PLOT Co *L-Arg.Co *9 
addmsg /kinetics/neuroNOS/nNOS.OH.Arg /moregraphs/conc4/nNOS.OH.Arg.Co PLOT Co *nNOS.OH.Arg.Co *27 
addmsg /kinetics/neuroNOS/NO /moregraphs/conc4/NO.Co PLOT Co *NO.Co *4 
enddump
// End of dump

call /kinetics/neuroNOS/nNOS/notes LOAD \
"neuronal Nitric Oxide Synthase." \
"Found in the neurons, activity is dependent on CaM binding," \
"in response to Ca levels." \
"" \
"Molecular weight ~160 kDa." \
""
call /kinetics/neuroNOS/Ca-CaM_nNOS/notes LOAD \
"The CaM-Ca bound nNOS, which is the acitve form."
call /kinetics/neuroNOS/Ca-CaMbind-nNOS/notes LOAD \
"Those binding CaM have a high Kd, including nNOS, ~ <=10 nM." \
"Calcium mediated nNOS activity is required for NO synthesis." \
"Rates from experiments...The binding of CaM to NOS has been " \
"demonstrated to act as the trigger necessary for electron " \
"transfer and catalytic activity.(Marletta, Biochemistry," \
"1997;36:12337-12345)" \
""
call /kinetics/neuroNOS/nNOSbindArg/notes LOAD \
"from Stuehr et al., Biochem,1999,38(38):12446-12451." \
"Steuhr et al., from stopped flow expts of sub binding to nNOS report" \
"0.5 (k-1) and 81 (k2)." \
"" \
"rates used straight from results." \
""
call /kinetics/neuroNOS/L-Arg/notes LOAD \
"The intracellular concentrations are atleast 100 uM." \
"Substrate for nNOS."
call /kinetics/neuroNOS/k1/notes LOAD \
"Rates from Iwanaga et al., Biochem, 1999;38(50):16629-16635." \
"Obtained from rapid quenching expts at 25 C." \
"" \
">>>>>> 25 +/- 2.0 "
call /kinetics/neuroNOS/k3/notes LOAD \
"Rate from Biochem., 1999, 38(50), 16629-16635." \
"Obtained from rapid queching expts at 25 C" \
"" \
">>>>>> 19 +/- 3.0"
call /kinetics/neuroNOS/nNOS.Cit+NO/notes LOAD \
"Citrulline and NO basically dissociate from the NOS enzyme " \
"complex, and this is the NOS bound with Citrulline and NO, and" \
"they dissociate in the subsequent step." \
""
call /kinetics/neuroNOS/NO/notes LOAD \
"Endogenously produced NO concentrations in the course of" \
"signal transduction processes are < 100 nM. (Varner et al.," \
"Nitric oxide in the nervous system, Academic press, London, UK," \
"pp.191-206.)"
call /kinetics/neuroNOS/k4/notes LOAD \
"Rate from Biochem., 1999, 38(50), 16629-16635." \
"from rapid quenching expts at 25 C" \
"" \
">>>>>> 2.7 +/- 0.3"
call /kinetics/neuroNOS/nNOS.OH.Arg/notes LOAD \
"This is the intermediate formed during the catalytic activity " \
"of NOS in synthesising NO, N-hydroxy arginine."
call /kinetics/neuroNOS/nNOS.arg/notes LOAD \
"Arginine binds to NOS first before the synthesis of NO from" \
"L-Arginine."
call /kinetics/neuroNOS/k2/notes LOAD \
"Rates from Biochem., 1999, 38(50), 16629-16635." \
"obtained from rapid quenching expts at 25 C...." \
"" \
">>>>>> 4.4 +/- 0.5"
complete_loading
