//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Wed Mar 19 11:08:42 2014
 
include kkit {argv 1}
 
FASTDT = 0.0001
SIMDT = 0.01
CONTROLDT = 5
PLOTDT = 0.1
MAXTIME = 20
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 0
DEFAULT_VOL = 1.6667e-21
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
simundump geometry /kinetics/geometry 0 1.6667e-21 3 sphere "" white black 3 \
  -7 0
simundump text /kinetics/notes 0 ""
call /kinetics/notes LOAD \
""
simundump text /kinetics/geometry/notes 0 ""
call /kinetics/geometry/notes LOAD \
""
simundump kpool /kinetics/B 0 0 0 0 0 0 0 0 1 0 /kinetics/geometry 62 black 1 \
  1 0
simundump text /kinetics/B/notes 0 ""
call /kinetics/B/notes LOAD \
""
simundump kpool /kinetics/A 0 0 2 2 2 2 0 0 1 0 /kinetics/geometry blue black \
  -3 1 0
simundump text /kinetics/A/notes 0 ""
call /kinetics/A/notes LOAD \
""
simundump kpool /kinetics/tot1 0 0 0 2 2 0 0 0 1 0 /kinetics/geometry 47 \
  black -1 -2 0
simundump text /kinetics/tot1/notes 0 ""
call /kinetics/tot1/notes LOAD \
""
simundump kpool /kinetics/C 0 0 0 0 0 0 0 0 1 0 /kinetics/geometry 56 black 5 \
  1 0
simundump text /kinetics/C/notes 0 ""
call /kinetics/C/notes LOAD \
""
simundump xtab /kinetics/xtab 0 0 0 1 1 0 "" edit_xtab "" red 0 0 0 1 -1 7 0
loadtab /kinetics/xtab table 1 100 0 10 \
 1 1.0628 1.1253 1.1874 1.2487 1.309 1.3681 1.4258 1.4817 1.5358 1.5878 \
 1.6374 1.6845 1.729 1.7705 1.809 1.8443 1.8763 1.9048 1.9298 1.951 1.9686 \
 1.9823 1.9921 1.998 2 1.998 1.9921 1.9823 1.9686 1.951 1.9298 1.9048 1.8763 \
 1.8443 1.809 1.7705 1.729 1.6846 1.6375 1.5878 1.5358 1.4818 1.4258 1.3681 \
 1.309 1.2487 1.1874 1.1254 1.0628 0.99999 0.93723 0.87462 0.81261 0.75133 \
 0.69103 0.63186 0.57423 0.51829 0.46416 0.41222 0.36261 0.31543 0.27104 \
 0.22951 0.19097 0.15567 0.12371 0.095158 0.070223 0.048953 0.031407 0.017712 \
 0.0078885 0.0019706 0 0.001972 0.0078787 0.017716 0.031413 0.048929 0.070231 \
 0.095168 0.12367 0.15569 0.19098 0.22946 0.27105 0.31545 0.36255 0.41224 \
 0.46417 0.51822 0.57425 0.63188 0.69096 0.75135 0.81263 0.87465 0.93716 1
simundump text /kinetics/xtab/notes 0 ""
call /kinetics/xtab/notes LOAD \
""
simundump kpool /kinetics/D 0 0 0 0 0 0 0 0 1 2 /kinetics/geometry 25 black \
  -3 5 0
simundump text /kinetics/D/notes 0 ""
call /kinetics/D/notes LOAD \
""
simundump kreac /kinetics/Reac1 0 0.2 0.1 "" white black -1 3 0
simundump text /kinetics/Reac1/notes 0 ""
call /kinetics/Reac1/notes LOAD \
""
simundump kreac /kinetics/Reac2 0 0.1 0 "" white black 3 3 0
simundump text /kinetics/Reac2/notes 0 ""
call /kinetics/Reac2/notes LOAD \
""
simundump kpool /kinetics/E 0 0 0 0 0 0 0 0 1 0 /kinetics/geometry 1 black 9 \
  1 0
simundump text /kinetics/E/notes 0 ""
call /kinetics/E/notes LOAD \
""
simundump kpool /kinetics/MassActionEnz 0 0 1 1 1 1 0 0 1 0 \
  /kinetics/geometry 28 black 7 5 0
simundump text /kinetics/MassActionEnz/notes 0 ""
call /kinetics/MassActionEnz/notes LOAD \
""
simundump kenz /kinetics/MassActionEnz/e1 0 0 0 0 0 1 1 4 1 0 0 "" red 28 "" \
  7 3 0
simundump text /kinetics/MassActionEnz/e1/notes 0 ""
call /kinetics/MassActionEnz/e1/notes LOAD \
""
simundump kpool /kinetics/MM_Enz 0 0 1 1 1 1 0 0 1 0 /kinetics/geometry 22 \
  black 6 -1 0
simundump text /kinetics/MM_Enz/notes 0 ""
call /kinetics/MM_Enz/notes LOAD \
""
simundump kenz /kinetics/MM_Enz/e2 0 0 0 0 0 1 1 4 1 0 1 "" red 22 "" 7 -1 0
simundump text /kinetics/MM_Enz/e2/notes 0 ""
call /kinetics/MM_Enz/e2/notes LOAD \
""
simundump kpool /kinetics/F 0 0 0 0 0 0 0 0 1 0 /kinetics/geometry blue black \
  9 -3 0
simundump text /kinetics/F/notes 0 ""
call /kinetics/F/notes LOAD \
""
simundump xgraph /graphs/conc1 0 0 20 0 2 0
simundump xgraph /graphs/conc2 0 0 20 0 2 0
simundump xplot /graphs/conc1/A.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc1/B.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 62 0 0 1
simundump xplot /graphs/conc2/tot1.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 47 0 0 1
simundump xplot /graphs/conc2/C.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 56 0 0 1
simundump xplot /graphs/conc2/D.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 25 0 0 1
simundump xplot /graphs/conc2/E.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 1 0 0 1
simundump xplot /graphs/conc2/F.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xgraph /moregraphs/conc3 0 0 20 0 1.2 0
simundump xgraph /moregraphs/conc4 0 0 20 0 1.2 0
simundump xcoredraw /edit/draw 0 -5 11 -9 9
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
addmsg /kinetics/Reac1 /kinetics/B REAC B A 
addmsg /kinetics/Reac2 /kinetics/B REAC A B 
addmsg /kinetics/Reac2 /kinetics/B REAC A B 
addmsg /kinetics/Reac1 /kinetics/A REAC A B 
addmsg /kinetics/A /kinetics/tot1 SUMTOTAL n nInit 
addmsg /kinetics/B /kinetics/tot1 SUMTOTAL n nInit 
addmsg /kinetics/Reac2 /kinetics/C REAC B A 
addmsg /kinetics/MassActionEnz/e1 /kinetics/C REAC sA B 
addmsg /kinetics/xtab /kinetics/D SLAVE output 
addmsg /kinetics/Reac1 /kinetics/D REAC A B 
addmsg /kinetics/A /kinetics/Reac1 SUBSTRATE n 
addmsg /kinetics/B /kinetics/Reac1 PRODUCT n 
addmsg /kinetics/D /kinetics/Reac1 SUBSTRATE n 
addmsg /kinetics/C /kinetics/Reac2 PRODUCT n 
addmsg /kinetics/B /kinetics/Reac2 SUBSTRATE n 
addmsg /kinetics/B /kinetics/Reac2 SUBSTRATE n 
addmsg /kinetics/MassActionEnz/e1 /kinetics/E MM_PRD pA 
addmsg /kinetics/MM_Enz/e2 /kinetics/E REAC sA B 
addmsg /kinetics/MassActionEnz/e1 /kinetics/MassActionEnz REAC eA B 
addmsg /kinetics/MassActionEnz /kinetics/MassActionEnz/e1 ENZYME n 
addmsg /kinetics/C /kinetics/MassActionEnz/e1 SUBSTRATE n 
addmsg /kinetics/MM_Enz /kinetics/MM_Enz/e2 ENZYME n 
addmsg /kinetics/E /kinetics/MM_Enz/e2 SUBSTRATE n 
addmsg /kinetics/MM_Enz/e2 /kinetics/F MM_PRD pA 
addmsg /kinetics/A /graphs/conc1/A.Co PLOT Co *A.Co *blue 
addmsg /kinetics/B /graphs/conc1/B.Co PLOT Co *B.Co *62 
addmsg /kinetics/tot1 /graphs/conc2/tot1.Co PLOT Co *tot1.Co *47 
addmsg /kinetics/C /graphs/conc2/C.Co PLOT Co *C.Co *56 
addmsg /kinetics/D /graphs/conc2/D.Co PLOT Co *D.Co *25 
addmsg /kinetics/E /graphs/conc2/E.Co PLOT Co *E.Co *1 
addmsg /kinetics/F /graphs/conc2/F.Co PLOT Co *F.Co *blue 
enddump
// End of dump

setfield /kinetics/xtab table->dx 0.1
setfield /kinetics/xtab table->invdx 10
complete_loading
