//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Sat Jul 28 22:51:00 2018
 
include kkit {argv 1}
 
FASTDT = 0.0001
SIMDT = 0.01
CONTROLDT = 5
PLOTDT = 1
MAXTIME = 100
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 1
DEFAULT_VOL = 1e-20
VERSION = 11.0
setfield /file/modpath value /home2/bhalla/scripts/modules
kparms
 
//genesis

initdump -version 3 -ignoreorphans 1
simobjdump doqcsinfo filename accessname accesstype transcriber developer \
  citation species tissue cellcompartment methodology sources \
  model_implementation model_validation x y z
simobjdump table input output alloced step_mode stepsize x y z
simobjdump xtree path script namemode sizescale
simobjdump xcoredraw xmin xmax ymin ymax
simobjdump xtext editable
simobjdump xgraph xmin xmax ymin ymax overlay
simobjdump xplot pixflags script fg ysquish do_slope wy
simobjdump group xtree_fg_req xtree_textfg_req plotfield expanded movealone \
  link savename file version md5sum mod_save_flag x y z
simobjdump geometry size dim shape outside xtree_fg_req xtree_textfg_req x y \
  z
simobjdump kpool DiffConst CoInit Co n nInit mwt nMin vol slave_enable \
  geomname xtree_fg_req xtree_textfg_req x y z
simobjdump kreac kf kb notes xtree_fg_req xtree_textfg_req x y z
simobjdump kenz CoComplexInit CoComplex nComplexInit nComplex vol k1 k2 k3 \
  keepconc usecomplex notes xtree_fg_req xtree_textfg_req link x y z
simobjdump stim level1 width1 delay1 level2 width2 delay2 baselevel trig_time \
  trig_mode notes xtree_fg_req xtree_textfg_req is_running x y z
simobjdump xtab input output alloced step_mode stepsize notes editfunc \
  xtree_fg_req xtree_textfg_req baselevel last_x last_y is_running x y z
simobjdump kchan perm gmax Vm is_active use_nernst notes xtree_fg_req \
  xtree_textfg_req x y z
simobjdump transport input output alloced step_mode stepsize dt delay clock \
  kf xtree_fg_req xtree_textfg_req x y z
simobjdump proto x y z
simobjdump text str
simundump geometry /kinetics/geometry 0 1e-19 3 sphere "" white black 0 0 0
simundump geometry /kinetics/geometry[1] 0 1e-18 3 sphere "" white black 0 0 \
  0
simundump text /kinetics/notes 0 ""
call /kinetics/notes LOAD \
""
simundump text /kinetics/geometry/notes 0 ""
call /kinetics/geometry/notes LOAD \
""
simundump text /kinetics/geometry[1]/notes 0 ""
call /kinetics/geometry[1]/notes LOAD \
""
simundump group /kinetics/PSD 0 yellow black x 0 0 "" PSD defaultfile.g 0 0 0 \
  4 4 0
simundump text /kinetics/PSD/notes 0 ""
call /kinetics/PSD/notes LOAD \
""
simundump kpool /kinetics/PSD/Ca_CaM_CaMKII 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry blue yellow 4 -5 0
simundump text /kinetics/PSD/Ca_CaM_CaMKII/notes 0 ""
call /kinetics/PSD/Ca_CaM_CaMKII/notes LOAD \
""
simundump kenz /kinetics/PSD/Ca_CaM_CaMKII/kinase 0 0 0 0 0 6 0.16667 8 2 0 0 \
  "" red blue "" 4 -4 0
simundump text /kinetics/PSD/Ca_CaM_CaMKII/kinase/notes 0 ""
call /kinetics/PSD/Ca_CaM_CaMKII/kinase/notes LOAD \
""
simundump kpool /kinetics/PSD/chan 0 0 100 100 600 600 0 0 6 0 \
  /kinetics/geometry 0 yellow 3 -1 0
simundump text /kinetics/PSD/chan/notes 0 ""
call /kinetics/PSD/chan/notes LOAD \
""
simundump kreac /kinetics/PSD/dephosph 0 1 0 "" white yellow 4 1 0
simundump text /kinetics/PSD/dephosph/notes 0 ""
call /kinetics/PSD/dephosph/notes LOAD \
""
simundump kpool /kinetics/PSD/chan_p 0 0 0 0 0 0 0 0 6 0 /kinetics/geometry 7 \
  yellow 5 -1 0
simundump text /kinetics/PSD/chan_p/notes 0 ""
call /kinetics/PSD/chan_p/notes LOAD \
""
simundump group /kinetics/SPINE 0 0 black x 0 0 "" SPINE defaultfile.g 0 0 0 \
  -3 -2 0
simundump text /kinetics/SPINE/notes 0 ""
call /kinetics/SPINE/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaM 0 1e-11 10 10 600 600 0 0 60 0 \
  /kinetics/geometry 55 0 -2 2 0
simundump text /kinetics/SPINE/CaM/notes 0 ""
call /kinetics/SPINE/CaM/notes LOAD \
""
simundump kpool /kinetics/SPINE/Ca 0 1e-10 0.08 0.08 4.8 4.8 0 0 60 0 \
  /kinetics/geometry blue 0 0 4 0
simundump text /kinetics/SPINE/Ca/notes 0 ""
call /kinetics/SPINE/Ca/notes LOAD \
""
simundump kpool /kinetics/SPINE/Ca_CaM 0 1e-11 0 0 0 0 0 0 60 0 \
  /kinetics/geometry blue 0 1 -1 0
simundump text /kinetics/SPINE/Ca_CaM/notes 0 ""
call /kinetics/SPINE/Ca_CaM/notes LOAD \
""
simundump kreac /kinetics/SPINE/Ca_bind_CaM 0 0.0027778 40 "" white 0 -1 0 0
simundump text /kinetics/SPINE/Ca_bind_CaM/notes 0 \
  "This should actually be 4th order in Ca. Using 2nd order here\nfor simplicity and to lessen numerical stiffness."
call /kinetics/SPINE/Ca_bind_CaM/notes LOAD \
"This should actually be 4th order in Ca. Using 2nd order here" \
"for simplicity and to lessen numerical stiffness."
simundump kpool /kinetics/SPINE/CaMKII 0 0 8.3333 8.3333 500 500 0 0 60 0 \
  /kinetics/geometry 7 0 -1 -3 0
simundump text /kinetics/SPINE/CaMKII/notes 0 ""
call /kinetics/SPINE/CaMKII/notes LOAD \
""
simundump kreac /kinetics/SPINE/CaM_bind_CaMKII 0 0.0016667 1 "" white 0 1 -4 \
  0
simundump text /kinetics/SPINE/CaM_bind_CaMKII/notes 0 ""
call /kinetics/SPINE/CaM_bind_CaMKII/notes LOAD \
""
simundump group /kinetics/DEND 0 35 black x 0 0 "" DEND defaultfile.g 0 0 0 \
  -3 -6 0
simundump text /kinetics/DEND/notes 0 ""
call /kinetics/DEND/notes LOAD \
""
simundump kpool /kinetics/DEND/Ca 0 1e-10 0.08 0.08 48 48 0 0 600 0 \
  /kinetics/geometry[1] blue 35 0 -6 0
simundump text /kinetics/DEND/Ca/notes 0 ""
call /kinetics/DEND/Ca/notes LOAD \
""
simundump xgraph /graphs/conc1 0 0 100 0 1 0
simundump xgraph /graphs/conc2 0 0 100 0 1.4023 0
simundump xplot /graphs/conc1/Ca.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc2/chan_p.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 7 0 0 1
simundump xplot /graphs/conc2/Ca_CaM_CaMKII.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xgraph /moregraphs/conc3 0 0 100 0 1 0
simundump xgraph /moregraphs/conc4 0 0 100 0 1 0
simundump xcoredraw /edit/draw 0 -5 7 -8 6
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"Very simplified calcium-to-kinase model in 3 compts, " \
"for a test multiscale" \
"model."
addmsg /kinetics/SPINE/CaM_bind_CaMKII /kinetics/PSD/Ca_CaM_CaMKII REAC B A 
addmsg /kinetics/PSD/Ca_CaM_CaMKII/kinase /kinetics/PSD/Ca_CaM_CaMKII REAC eA B 
addmsg /kinetics/PSD/Ca_CaM_CaMKII /kinetics/PSD/Ca_CaM_CaMKII/kinase ENZYME n 
addmsg /kinetics/PSD/chan /kinetics/PSD/Ca_CaM_CaMKII/kinase SUBSTRATE n 
addmsg /kinetics/PSD/Ca_CaM_CaMKII/kinase /kinetics/PSD/chan REAC sA B 
addmsg /kinetics/PSD/dephosph /kinetics/PSD/chan REAC B A 
addmsg /kinetics/PSD/chan_p /kinetics/PSD/dephosph SUBSTRATE n 
addmsg /kinetics/PSD/chan /kinetics/PSD/dephosph PRODUCT n 
addmsg /kinetics/PSD/Ca_CaM_CaMKII/kinase /kinetics/PSD/chan_p MM_PRD pA 
addmsg /kinetics/PSD/dephosph /kinetics/PSD/chan_p REAC A B 
addmsg /kinetics/SPINE/Ca_bind_CaM /kinetics/SPINE/CaM REAC A B 
addmsg /kinetics/SPINE/Ca_bind_CaM /kinetics/SPINE/Ca REAC A B 
addmsg /kinetics/SPINE/Ca_bind_CaM /kinetics/SPINE/Ca REAC A B 
addmsg /kinetics/SPINE/Ca_bind_CaM /kinetics/SPINE/Ca_CaM REAC B A 
addmsg /kinetics/SPINE/CaM_bind_CaMKII /kinetics/SPINE/Ca_CaM REAC A B 
addmsg /kinetics/SPINE/Ca /kinetics/SPINE/Ca_bind_CaM SUBSTRATE n 
addmsg /kinetics/SPINE/Ca /kinetics/SPINE/Ca_bind_CaM SUBSTRATE n 
addmsg /kinetics/SPINE/CaM /kinetics/SPINE/Ca_bind_CaM SUBSTRATE n 
addmsg /kinetics/SPINE/Ca_CaM /kinetics/SPINE/Ca_bind_CaM PRODUCT n 
addmsg /kinetics/SPINE/CaM_bind_CaMKII /kinetics/SPINE/CaMKII REAC A B 
addmsg /kinetics/SPINE/Ca_CaM /kinetics/SPINE/CaM_bind_CaMKII SUBSTRATE n 
addmsg /kinetics/SPINE/CaMKII /kinetics/SPINE/CaM_bind_CaMKII SUBSTRATE n 
addmsg /kinetics/PSD/Ca_CaM_CaMKII /kinetics/SPINE/CaM_bind_CaMKII PRODUCT n 
addmsg /kinetics/SPINE/Ca /graphs/conc1/Ca.Co PLOT Co *Ca.Co *blue 
addmsg /kinetics/PSD/chan_p /graphs/conc2/chan_p.Co PLOT Co *chan_p.Co *7 
addmsg /kinetics/PSD/Ca_CaM_CaMKII /graphs/conc2/Ca_CaM_CaMKII.Co PLOT Co *Ca_CaM_CaMKII.Co *blue 
enddump
// End of dump

call /kinetics/SPINE/Ca_bind_CaM/notes LOAD \
"This should actually be 4th order in Ca. Using 2nd order here" \
"for simplicity and to lessen numerical stiffness."
complete_loading
