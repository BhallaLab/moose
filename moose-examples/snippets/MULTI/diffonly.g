//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Tue Aug 27 21:53:07 2013
 
include kkit {argv 1}
 
FASTDT = 0.0001
SIMDT = 0.01
CONTROLDT = 5
PLOTDT = 1
MAXTIME = 100
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 0
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
simundump geometry /kinetics/geometry 0 1e-18 3 sphere "" white black 2 -6 0
simundump geometry /kinetics/geometry[1] 0 1e-19 3 sphere "" white black 0 -8 \
  0
simundump geometry /kinetics/geometry[2] 0 1e-20 3 sphere "" white black -3 \
  -6 0
simundump geometry /kinetics/geometry[3] 0 1e-18 3 sphere "" white black 0 0 \
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
simundump text /kinetics/geometry[3]/notes 0 ""
call /kinetics/geometry[3]/notes LOAD \
""
simundump group /kinetics/DEND 0 yellow black x 0 0 "" DEND defaultfile.g 0 0 \
  0 -3 -3 0
simundump text /kinetics/DEND/notes 0 ""
call /kinetics/DEND/notes LOAD \
""
simundump kpool /kinetics/DEND/Ca 0 1e-10 0.08 0.08 48 48 0 0 600 0 \
  /kinetics/geometry blue yellow -4 4 0
simundump text /kinetics/DEND/Ca/notes 0 ""
call /kinetics/DEND/Ca/notes LOAD \
""
simundump kpool /kinetics/DEND/Ca_CaM 0 1e-11 1 1 600 600 0 0 600 0 \
  /kinetics/geometry[3] blue yellow -2 -1 0
simundump text /kinetics/DEND/Ca_CaM/notes 0 ""
call /kinetics/DEND/Ca_CaM/notes LOAD \
""
simundump group /kinetics/SPINE 0 28 black x 0 0 "" SPINE defaultfile.g 0 0 0 \
  0 -3 0
simundump text /kinetics/SPINE/notes 0 ""
call /kinetics/SPINE/notes LOAD \
""
simundump kpool /kinetics/SPINE/Ca 0 1e-10 0.08 0.08 4.8 4.8 0 0 60 0 \
  /kinetics/geometry[1] blue 28 -2 4 0
simundump text /kinetics/SPINE/Ca/notes 0 ""
call /kinetics/SPINE/Ca/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaM 0 1e-11 5 5 300 300 0 0 60 0 \
  /kinetics/geometry[1] 55 28 -2 2 0
simundump text /kinetics/SPINE/CaM/notes 0 ""
call /kinetics/SPINE/CaM/notes LOAD \
""
simundump kpool /kinetics/SPINE/Ca_CaM 0 1e-11 0 0 0 0 0 0 60 0 \
  /kinetics/geometry[1] blue 28 1 -1 0
simundump text /kinetics/SPINE/Ca_CaM/notes 0 ""
call /kinetics/SPINE/Ca_CaM/notes LOAD \
""
simundump kreac /kinetics/SPINE/kreac 0 0.1 0.1 "" white 28 -1 0 0
simundump text /kinetics/SPINE/kreac/notes 0 ""
call /kinetics/SPINE/kreac/notes LOAD \
""
simundump group /kinetics/PSD 0 0 black x 0 0 "" PSD defaultfile.g 0 0 0 3 -3 \
  0
simundump text /kinetics/PSD/notes 0 ""
call /kinetics/PSD/notes LOAD \
""
simundump kpool /kinetics/PSD/Ca 0 1e-10 0.08 0.08 0.48 0.48 0 0 6 0 \
  /kinetics/geometry[2] blue 0 1 4 0
simundump text /kinetics/PSD/Ca/notes 0 ""
call /kinetics/PSD/Ca/notes LOAD \
""
simundump kpool /kinetics/PSD/CaM 0 1e-11 5 5 30 30 0 0 6 0 \
  /kinetics/geometry[2] 53 0 1 2 0
simundump text /kinetics/PSD/CaM/notes 0 ""
call /kinetics/PSD/CaM/notes LOAD \
""
simundump kpool /kinetics/PSD/Ca_CaM 0 1e-11 0 0 0 0 0 0 6 0 \
  /kinetics/geometry[2] blue 0 4 -1 0
simundump text /kinetics/PSD/Ca_CaM/notes 0 ""
call /kinetics/PSD/Ca_CaM/notes LOAD \
""
simundump kreac /kinetics/PSD/kreac 0 0.1 0.1 "" white 0 2 0 0
simundump text /kinetics/PSD/kreac/notes 0 ""
call /kinetics/PSD/kreac/notes LOAD \
""
simundump xgraph /graphs/conc1 0 0 100 0 1 0
simundump xgraph /graphs/conc2 0 0 100 0 1 0
simundump xgraph /moregraphs/conc3 0 0 100 0 1 0
simundump xgraph /moregraphs/conc4 0 0 100 0 1 0
simundump xcoredraw /edit/draw 0 -6 6 -10 6
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
addmsg /kinetics/SPINE/kreac /kinetics/SPINE/Ca REAC A B 
addmsg /kinetics/SPINE/kreac /kinetics/SPINE/CaM REAC A B 
addmsg /kinetics/SPINE/kreac /kinetics/SPINE/Ca_CaM REAC B A 
addmsg /kinetics/SPINE/Ca /kinetics/SPINE/kreac SUBSTRATE n 
addmsg /kinetics/SPINE/CaM /kinetics/SPINE/kreac SUBSTRATE n 
addmsg /kinetics/SPINE/Ca_CaM /kinetics/SPINE/kreac PRODUCT n 
addmsg /kinetics/PSD/kreac /kinetics/PSD/Ca REAC A B 
addmsg /kinetics/PSD/kreac /kinetics/PSD/CaM REAC A B 
addmsg /kinetics/PSD/kreac /kinetics/PSD/Ca_CaM REAC B A 
addmsg /kinetics/PSD/Ca /kinetics/PSD/kreac SUBSTRATE n 
addmsg /kinetics/PSD/CaM /kinetics/PSD/kreac SUBSTRATE n 
addmsg /kinetics/PSD/Ca_CaM /kinetics/PSD/kreac PRODUCT n 
enddump
// End of dump

complete_loading
