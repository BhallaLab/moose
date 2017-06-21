//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Sun Aug 21 08:51:57 2016
 
include kkit {argv 1}
 
FASTDT = 0.0001
SIMDT = 0.001
CONTROLDT = 10
PLOTDT = 10
MAXTIME = 1000
TRANSIENT_TIME = 10
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
simundump geometry /kinetics/geometry 0 1e-20 3 sphere "" white black 0 0 0
simundump geometry /kinetics/geometry[1] 0 1e-18 3 sphere "" white black 0 0 \
  0
simundump geometry /kinetics/geometry[2] 0 9e-20 3 sphere "" white black 0 0 \
  0
simundump text /kinetics/notes 0 ""
call /kinetics/notes LOAD \
""
simundump group /kinetics/PSD 0 20 black x 0 0 "" PSD defaultfile.g 0 0 0 27 \
  12 0
simundump text /kinetics/PSD/notes 0 ""
call /kinetics/PSD/notes LOAD \
""
simundump kpool /kinetics/PSD/Ca 0 2e-11 0.1 0.1 0.6 0.6 0 0 6 0 \
  /kinetics/geometry 53 0 -16 6 0
simundump text /kinetics/PSD/Ca/notes 0 ""
call /kinetics/PSD/Ca/notes LOAD \
""
simundump kpool /kinetics/PSD/Ca_input 0 0 0.08 0.08 0.48 0.48 0 0 6 4 \
  /kinetics/geometry 55 black -10 2 0
simundump text /kinetics/PSD/Ca_input/notes 0 ""
call /kinetics/PSD/Ca_input/notes LOAD \
""
simundump kreac /kinetics/PSD/Ca_stim 0 50 1 "" white black -13 4 0
simundump text /kinetics/PSD/Ca_stim/notes 0 ""
call /kinetics/PSD/Ca_stim/notes LOAD \
""
simundump group /kinetics/DEND 0 13 black x 0 0 "" DEND defaultfile.g 0 0 0 \
  11 -30 0
simundump text /kinetics/DEND/notes 0 ""
call /kinetics/DEND/notes LOAD \
""
simundump kpool /kinetics/DEND/Ca 0 2e-11 0.08 0.08 48 48 0 0 600 0 \
  /kinetics/geometry 54 40 -8 -20 0
simundump text /kinetics/DEND/Ca/notes 0 ""
call /kinetics/DEND/Ca/notes LOAD \
""
simundump kpool /kinetics/DEND/Ca_input 0 0 0.08 0.08 48 48 0 0 600 4 \
  /kinetics/geometry 55 40 -2 -20 0
simundump text /kinetics/DEND/Ca_input/notes 0 ""
call /kinetics/DEND/Ca_input/notes LOAD \
""
simundump kreac /kinetics/DEND/Ca_stim 0 50 1 "" white 40 -5 -19 0
simundump text /kinetics/DEND/Ca_stim/notes 0 ""
call /kinetics/DEND/Ca_stim/notes LOAD \
""
simundump group /kinetics/SPINE 0 5 black x 0 0 "" SPINE defaultfile.g 0 0 0 \
  29 -23 0
simundump text /kinetics/SPINE/notes 0 ""
call /kinetics/SPINE/notes LOAD \
""
simundump kpool /kinetics/SPINE/Ca 0 2e-11 0.11111 0.11111 6 6 0 0 54 0 \
  /kinetics/geometry[1] 50 yellow -12 -11 0
simundump text /kinetics/SPINE/Ca/notes 0 ""
call /kinetics/SPINE/Ca/notes LOAD \
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
simundump xgraph /graphs/conc1 0 0 2923 0 38.53 0
simundump xgraph /graphs/conc2 0 0 2923 0 16.804 0
simundump xgraph /moregraphs/conc3 0 0 2923 0 4 0
simundump xgraph /moregraphs/conc4 0 0 2923 0 4 0
simundump xcoredraw /edit/draw 0 -18 31 -32 14
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"" \
"4 Aug 2016: CaM_3compt.g: Based on CaMKII_merged91.g, just have" \
"the CaM part in all 3 compartments, for merging." \
"CaM_3compt1.g: Eliminated Ca and Ca input too." \
"" \
"4 Aug 2016: Merged CaM_3compt1.g into NN_mapk6.g, some " \
"elimination of Ca buffer pools. All now handled by CaM." \
"" \
"15 Aug 2016: Doubled CaM levels throughout, use as a surrogate" \
"for other Ca buffers." \
"" \
"21 Aug 2016: Ca  in spine and dendrite for diffusive" \
"calculations." \
""
addmsg /kinetics/PSD/Ca_stim /kinetics/PSD/Ca REAC B A 
addmsg /kinetics/PSD/Ca_stim /kinetics/PSD/Ca_input REAC A B 
addmsg /kinetics/PSD/Ca_input /kinetics/PSD/Ca_stim SUBSTRATE n 
addmsg /kinetics/PSD/Ca /kinetics/PSD/Ca_stim PRODUCT n 
addmsg /kinetics/DEND/Ca_stim /kinetics/DEND/Ca REAC B A 
addmsg /kinetics/DEND/Ca_stim /kinetics/DEND/Ca_input REAC A B 
addmsg /kinetics/DEND/Ca_input /kinetics/DEND/Ca_stim SUBSTRATE n 
addmsg /kinetics/DEND/Ca /kinetics/DEND/Ca_stim PRODUCT n 
enddump
// End of dump

complete_loading
