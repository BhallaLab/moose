//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Tue Jan 21 14:05:52 2014
 
include kkit {argv 1}
 
FASTDT = 0.0001
SIMDT = 0.005
CONTROLDT = 5
PLOTDT = 10
MAXTIME = 2500
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
simundump geometry /kinetics/geometry 0 9.9998e-20 3 sphere "" white black 0 \
  0 0
simundump geometry /kinetics/geometry[1] 0 9.9998e-21 3 sphere "" white black \
  0 0 0
simundump geometry /kinetics/geometry[2] 0 1e-20 3 sphere "" white black 0 0 \
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
simundump text /kinetics/geometry[2]/notes 0 ""
call /kinetics/geometry[2]/notes LOAD \
""
simundump kreac /kinetics/exo 0 0.005 0 "" white black 3 9 0
simundump text /kinetics/exo/notes 0 ""
call /kinetics/exo/notes LOAD \
""
simundump kreac /kinetics/endo 0 0.01 0 "" white black -6 11 0
simundump text /kinetics/endo/notes 0 ""
call /kinetics/endo/notes LOAD \
""
simundump group /kinetics/B 0 yellow black x 0 0 "" Bulk defaultfile.g 0 0 0 \
  -7 6 0
simundump text /kinetics/B/notes 0 ""
call /kinetics/B/notes LOAD \
""
simundump kpool /kinetics/B/P 0 0 0.1 0.1 0.6 0.6 0 0 6 0 /kinetics/geometry \
  4 yellow -2 2 0
simundump text /kinetics/B/P/notes 0 ""
call /kinetics/B/P/notes LOAD \
""
simundump kenz /kinetics/B/P/kenz 0 0 0 0 0 6 16.667 4 1 0 0 "" red 4 "" -2 0 \
  0
simundump text /kinetics/B/P/kenz/notes 0 ""
call /kinetics/B/P/kenz/notes LOAD \
""
simundump kreac /kinetics/B/basal 0 0.01 0 "" white yellow -4 -4 0
simundump text /kinetics/B/basal/notes 0 ""
call /kinetics/B/basal/notes LOAD \
""
simundump kpool /kinetics/B/M 0 0 0 0 0 0 0 0 6 0 /kinetics/geometry 62 \
  yellow -5 7 0
simundump text /kinetics/B/M/notes 0 ""
call /kinetics/B/M/notes LOAD \
""
simundump kpool /kinetics/B/M* 0 0 0 0 0 0 0 0 6 0 /kinetics/geometry 28 \
  yellow 2 2 0
simundump text /kinetics/B/M*/notes 0 ""
call /kinetics/B/M*/notes LOAD \
""
simundump kenz /kinetics/B/M*/kenz 0 0 0 0 0 59.999 0.41667 2 0.5 0 0 "" red \
  28 "" 1 6 0
simundump text /kinetics/B/M*/kenz/notes 0 ""
call /kinetics/B/M*/kenz/notes LOAD \
""
simundump group /kinetics/A 0 16 black x 0 0 "" PSD defaultfile.g 0 0 0 -5 16 \
  0
simundump text /kinetics/A/notes 0 ""
call /kinetics/A/notes LOAD \
""
simundump kreac /kinetics/A/basal 0 0.01 0 "" white 16 -1 8 0
simundump text /kinetics/A/basal/notes 0 ""
call /kinetics/A/basal/notes LOAD \
""
simundump kpool /kinetics/A/Stot 0 0 0 1.75 21 0 0 0 12 0 \
  /kinetics/geometry[2] 41 16 0 24 0
simundump text /kinetics/A/Stot/notes 0 ""
call /kinetics/A/Stot/notes LOAD \
""
simundump kpool /kinetics/A/P 0 0 0.049999 0.049999 0.59999 0.59999 0 0 12 0 \
  /kinetics/geometry[1] 3 16 -1 11 0
simundump text /kinetics/A/P/notes 0 ""
call /kinetics/A/P/notes LOAD \
""
simundump kenz /kinetics/A/P/kenz 0 0 0 0 0 5.9999 16.667 4 1 0 0 "" red 3 "" \
  -1 13 0
simundump text /kinetics/A/P/kenz/notes 0 ""
call /kinetics/A/P/kenz/notes LOAD \
""
simundump kpool /kinetics/A/M* 0 0 0 0 0 0 0 0 12 0 /kinetics/geometry 47 16 \
  4 19 0
simundump text /kinetics/A/M*/notes 0 ""
call /kinetics/A/M*/notes LOAD \
""
simundump kenz /kinetics/A/M*/kenz 0 0 0 0 0 5.9999 0.41667 2 0.5 0 0 "" red \
  57 "" -1 18 0
simundump text /kinetics/A/M*/kenz/notes 0 ""
call /kinetics/A/M*/kenz/notes LOAD \
""
simundump kpool /kinetics/A/M 0 0 1.75 1.75 21 21 0 0 12 0 /kinetics/geometry \
  53 16 -4 17 0
simundump text /kinetics/A/M/notes 0 ""
call /kinetics/A/M/notes LOAD \
""
simundump xgraph /graphs/conc1 0 0 2500 0.001 3 0
simundump xgraph /graphs/conc2 0 0 2500 0 5 0
simundump xplot /graphs/conc1/M.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 53 0 0 1
simundump xplot /graphs/conc1/M*.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 47 0 0 1
simundump xplot /graphs/conc2/M.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 62 0 0 1
simundump xplot /graphs/conc2/M*.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 28 0 0 1
simundump xgraph /moregraphs/conc3 0 0 2500 0 1 0
simundump xgraph /moregraphs/conc4 0 0 2500 0 1 0
simundump xcoredraw /edit/draw 0 -9 6 -6 26
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"21 Jan 2014." \
"Using " \
"different volumes for the two compartments, based on OSC_Cspace.g"
addmsg /kinetics/B/M* /kinetics/exo SUBSTRATE n 
addmsg /kinetics/A/M* /kinetics/exo PRODUCT n 
addmsg /kinetics/A/M /kinetics/endo SUBSTRATE n 
addmsg /kinetics/B/M /kinetics/endo PRODUCT n 
addmsg /kinetics/B/P/kenz /kinetics/B/P REAC eA B 
addmsg /kinetics/B/P /kinetics/B/P/kenz ENZYME n 
addmsg /kinetics/B/M* /kinetics/B/P/kenz SUBSTRATE n 
addmsg /kinetics/B/M /kinetics/B/basal SUBSTRATE n 
addmsg /kinetics/B/M* /kinetics/B/basal PRODUCT n 
addmsg /kinetics/B/basal /kinetics/B/M REAC A B 
addmsg /kinetics/B/P/kenz /kinetics/B/M MM_PRD pA 
addmsg /kinetics/B/M*/kenz /kinetics/B/M REAC sA B 
addmsg /kinetics/endo /kinetics/B/M REAC B A 
addmsg /kinetics/B/basal /kinetics/B/M* REAC B A 
addmsg /kinetics/B/P/kenz /kinetics/B/M* REAC sA B 
addmsg /kinetics/B/M*/kenz /kinetics/B/M* REAC eA B 
addmsg /kinetics/B/M*/kenz /kinetics/B/M* MM_PRD pA 
addmsg /kinetics/exo /kinetics/B/M* REAC A B 
addmsg /kinetics/B/M* /kinetics/B/M*/kenz ENZYME n 
addmsg /kinetics/B/M /kinetics/B/M*/kenz SUBSTRATE n 
addmsg /kinetics/A/M /kinetics/A/basal SUBSTRATE n 
addmsg /kinetics/A/M* /kinetics/A/basal PRODUCT n 
addmsg /kinetics/A/M /kinetics/A/Stot SUMTOTAL n nInit 
addmsg /kinetics/A/M* /kinetics/A/Stot SUMTOTAL n nInit 
addmsg /kinetics/A/M*/kenz /kinetics/A/Stot SUMTOTAL nComplex nComplexInit 
addmsg /kinetics/A/P/kenz /kinetics/A/Stot SUMTOTAL nComplex nComplexInit 
addmsg /kinetics/A/M*/kenz /kinetics/A/Stot SUMTOTAL nComplex nComplexInit 
addmsg /kinetics/A/P/kenz /kinetics/A/P REAC eA B 
addmsg /kinetics/A/P /kinetics/A/P/kenz ENZYME n 
addmsg /kinetics/A/M* /kinetics/A/P/kenz SUBSTRATE n 
addmsg /kinetics/A/M*/kenz /kinetics/A/M* MM_PRD pA 
addmsg /kinetics/A/P/kenz /kinetics/A/M* REAC sA B 
addmsg /kinetics/A/basal /kinetics/A/M* REAC B A 
addmsg /kinetics/A/M*/kenz /kinetics/A/M* REAC eA B 
addmsg /kinetics/exo /kinetics/A/M* REAC B A 
addmsg /kinetics/A/M /kinetics/A/M*/kenz SUBSTRATE n 
addmsg /kinetics/A/M* /kinetics/A/M*/kenz ENZYME n 
addmsg /kinetics/A/M*/kenz /kinetics/A/M REAC sA B 
addmsg /kinetics/A/P/kenz /kinetics/A/M MM_PRD pA 
addmsg /kinetics/A/basal /kinetics/A/M REAC A B 
addmsg /kinetics/endo /kinetics/A/M REAC A B 
addmsg /kinetics/A/M /graphs/conc1/M.Co PLOT Co *M.Co *53 
addmsg /kinetics/A/M* /graphs/conc1/M*.Co PLOT Co *M*.Co *47 
addmsg /kinetics/B/M /graphs/conc2/M.Co PLOT Co *M.Co *62 
addmsg /kinetics/B/M* /graphs/conc2/M*.Co PLOT Co *M*.Co *28 
enddump
// End of dump

complete_loading
