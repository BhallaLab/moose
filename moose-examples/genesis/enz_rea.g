//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Mon Jul  9 11:42:22 2012
 
include kkit {argv 1}
 
FASTDT = 1e-06
SIMDT = 0.0001
CONTROLDT = 0.01
PLOTDT = 0.01
MAXTIME = 1
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 1
DEFAULT_VOL = 1.6667e-21
VERSION = 11.0
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
simundump geometry /kinetics/geometry 0 1.6667e-21 3 sphere "" white black 0 \
  0 0
simundump kpool /kinetics/a 0 0 1 1 1 1 0 0 1 0 /kinetics/geometry blue black \
  -2 1 0
simundump kpool /kinetics/b 0 0 1 1 1 1 0 0 1 0 /kinetics/geometry 60 black 2 \
  1 0
simundump kpool /kinetics/c 0 0 1 1 1 1 0 0 1 0 /kinetics/geometry 47 black 0 \
  -2 0
simundump kenz /kinetics/c/kenz 0 0 0 0 0 1 0.5 0.4 0.1 0 0 "" red 47 "" 0 -1 \
  0
simundump kreac /kinetics/kreac 0 0.01 0.1 "" white black 0 2 0
simundump xgraph /graphs/conc1 0 0 1 0 1 0
simundump xgraph /graphs/conc2 0 0 1 0 1 0
simundump xplot /graphs/conc1/a.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc2/b.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 60 0 0 1
simundump xplot /graphs/conc2/c.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 47 0 0 1
simundump xgraph /moregraphs/conc3 0 0 1 0 1 0
simundump xgraph /moregraphs/conc4 0 0 1 0 1 0
simundump xcoredraw /edit/draw 0 -4 4 -4 4
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
addmsg /kinetics/kreac /kinetics/a REAC A B 
addmsg /kinetics/c/kenz /kinetics/a REAC sA B 
addmsg /kinetics/kreac /kinetics/b REAC B A 
addmsg /kinetics/c/kenz /kinetics/b MM_PRD pA 
addmsg /kinetics/c/kenz /kinetics/c REAC eA B 
addmsg /kinetics/c /kinetics/c/kenz ENZYME n 
addmsg /kinetics/a /kinetics/c/kenz SUBSTRATE n 
addmsg /kinetics/a /kinetics/kreac SUBSTRATE n 
addmsg /kinetics/b /kinetics/kreac PRODUCT n 
addmsg /kinetics/a /graphs/conc1/a.Co PLOT Co *a.Co *blue 
addmsg /kinetics/b /graphs/conc2/b.Co PLOT Co *b.Co *60 
addmsg /kinetics/c /graphs/conc2/c.Co PLOT Co *c.Co *47 
enddump
// End of dump

complete_loading
