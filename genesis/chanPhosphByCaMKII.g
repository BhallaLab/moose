//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Thu Sep 11 18:01:11 2014
 
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
simundump text /kinetics/notes 0 ""
call /kinetics/notes LOAD \
""
simundump text /kinetics/geometry/notes 0 ""
call /kinetics/geometry/notes LOAD \
""
simundump kpool /kinetics/CaM 0 1e-11 5 5 300 300 0 0 60 0 /kinetics/geometry \
  55 28 -2 2 0
simundump text /kinetics/CaM/notes 0 ""
call /kinetics/CaM/notes LOAD \
""
simundump kpool /kinetics/Ca_CaM 0 1e-11 0 0 0 0 0 0 60 0 /kinetics/geometry \
  blue 28 1 -1 0
simundump text /kinetics/Ca_CaM/notes 0 ""
call /kinetics/Ca_CaM/notes LOAD \
""
simundump kpool /kinetics/Ca 0 1e-10 0.08 0.08 4.8 4.8 0 0 60 4 \
  /kinetics/geometry blue 28 0 4 0
simundump text /kinetics/Ca/notes 0 ""
call /kinetics/Ca/notes LOAD \
""
simundump kreac /kinetics/CaM_bind_CaMKII 0 0.0016667 1 "" white 28 1 -4 0
simundump text /kinetics/CaM_bind_CaMKII/notes 0 ""
call /kinetics/CaM_bind_CaMKII/notes LOAD \
""
simundump kpool /kinetics/CaMKII 0 0 8.3333 8.3333 500 500 0 0 60 0 \
  /kinetics/geometry 7 28 -1 -3 0
simundump text /kinetics/CaMKII/notes 0 ""
call /kinetics/CaMKII/notes LOAD \
""
simundump kpool /kinetics/Ca_CaM_CaMKII 0 0 0 0 0 0 0 0 60 0 \
  /kinetics/geometry blue 0 4 -5 0
simundump text /kinetics/Ca_CaM_CaMKII/notes 0 ""
call /kinetics/Ca_CaM_CaMKII/notes LOAD \
""
simundump kenz /kinetics/Ca_CaM_CaMKII/kinase 0 0 0 0 0 6 0.16667 8 2 0 0 "" \
  red blue "" 4 -4 0
simundump text /kinetics/Ca_CaM_CaMKII/kinase/notes 0 ""
call /kinetics/Ca_CaM_CaMKII/kinase/notes LOAD \
""
simundump kpool /kinetics/chan 0 0 1 1 60 60 0 0 60 0 /kinetics/geometry 0 \
  black 3 -1 0
simundump text /kinetics/chan/notes 0 ""
call /kinetics/chan/notes LOAD \
""
simundump kpool /kinetics/chan_p 0 0 0 0 0 0 0 0 60 0 /kinetics/geometry 7 \
  black 5 -1 0
simundump text /kinetics/chan_p/notes 0 ""
call /kinetics/chan_p/notes LOAD \
""
simundump kreac /kinetics/Ca_bind_CaM 0 0.0027778 40 "" white 28 -1 0 0
simundump text /kinetics/Ca_bind_CaM/notes 0 \
  "This should actually be 4th order in Ca. Using 2nd order here\nfor simplicity and to lessen numerical stiffness."
call /kinetics/Ca_bind_CaM/notes LOAD \
"This should actually be 4th order in Ca. Using 2nd order here" \
"for simplicity and to lessen numerical stiffness."
simundump kreac /kinetics/dephosph 0 1 0 "" white black 4 1 0
simundump text /kinetics/dephosph/notes 0 ""
call /kinetics/dephosph/notes LOAD \
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
simundump xcoredraw /edit/draw 0 -4 7 -7 6
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"Very simplified calcium-to-kinase model for a test multiscale" \
"model."
addmsg /kinetics/Ca_bind_CaM /kinetics/CaM REAC A B 
addmsg /kinetics/Ca_bind_CaM /kinetics/Ca_CaM REAC B A 
addmsg /kinetics/CaM_bind_CaMKII /kinetics/Ca_CaM REAC A B 
addmsg /kinetics/Ca_bind_CaM /kinetics/Ca REAC A B 
addmsg /kinetics/Ca_bind_CaM /kinetics/Ca REAC A B 
addmsg /kinetics/Ca_CaM /kinetics/CaM_bind_CaMKII SUBSTRATE n 
addmsg /kinetics/CaMKII /kinetics/CaM_bind_CaMKII SUBSTRATE n 
addmsg /kinetics/Ca_CaM_CaMKII /kinetics/CaM_bind_CaMKII PRODUCT n 
addmsg /kinetics/CaM_bind_CaMKII /kinetics/CaMKII REAC A B 
addmsg /kinetics/CaM_bind_CaMKII /kinetics/Ca_CaM_CaMKII REAC B A 
addmsg /kinetics/Ca_CaM_CaMKII/kinase /kinetics/Ca_CaM_CaMKII REAC eA B 
addmsg /kinetics/Ca_CaM_CaMKII /kinetics/Ca_CaM_CaMKII/kinase ENZYME n 
addmsg /kinetics/chan /kinetics/Ca_CaM_CaMKII/kinase SUBSTRATE n 
addmsg /kinetics/Ca_CaM_CaMKII/kinase /kinetics/chan REAC sA B 
addmsg /kinetics/dephosph /kinetics/chan REAC B A 
addmsg /kinetics/Ca_CaM_CaMKII/kinase /kinetics/chan_p MM_PRD pA 
addmsg /kinetics/dephosph /kinetics/chan_p REAC A B 
addmsg /kinetics/Ca /kinetics/Ca_bind_CaM SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/Ca_bind_CaM SUBSTRATE n 
addmsg /kinetics/CaM /kinetics/Ca_bind_CaM SUBSTRATE n 
addmsg /kinetics/Ca_CaM /kinetics/Ca_bind_CaM PRODUCT n 
addmsg /kinetics/chan_p /kinetics/dephosph SUBSTRATE n 
addmsg /kinetics/chan /kinetics/dephosph PRODUCT n 
addmsg /kinetics/Ca /graphs/conc1/Ca.Co PLOT Co *Ca.Co *blue 
addmsg /kinetics/chan_p /graphs/conc2/chan_p.Co PLOT Co *chan_p.Co *7 
addmsg /kinetics/Ca_CaM_CaMKII /graphs/conc2/Ca_CaM_CaMKII.Co PLOT Co *Ca_CaM_CaMKII.Co *blue 
enddump
// End of dump

call /kinetics/Ca_bind_CaM/notes LOAD \
"This should actually be 4th order in Ca. Using 2nd order here" \
"for simplicity and to lessen numerical stiffness."
complete_loading
