//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Fri Sep  7 14:04:57 2012
 
include kkit {argv 1}
 
FASTDT = 0.01
SIMDT = 0.01
CONTROLDT = 5
PLOTDT = 1
MAXTIME = 250
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 1
DEFAULT_VOL = 1e-20
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
simobjdump text str
simundump geometry /kinetics/geometry 0 9.9998e-20 3 sphere "" white black 0 \
  0 0
simundump text /kinetics/notes 0 ""
call /kinetics/notes LOAD \
""
simundump text /kinetics/geometry/notes 0 ""
call /kinetics/geometry/notes LOAD \
""
simundump kpool /kinetics/a 0 0 1 1 6 6 0 0 6 0 /kinetics/geometry \
  red yellow 0 2 0
simundump text /kinetics/a/notes 0 ""
call /kinetics/a/notes LOAD \
""
simundump kpool /kinetics/b 0 0 0.1 0.1 0.6 0.6 0 0 6 0 /kinetics/geometry \
  darkgreen yellow -2 2 0
simundump text /kinetics/b/notes 0 ""
call /kinetics/b/notes LOAD \
""
simundump kenz /kinetics/b/autocat 0 0 0 0 0 6 1.6667 4 1 0 0 "" red green "" -2 3 \
  0
simundump text /kinetics/b/autocat/notes 0 ""
call /kinetics/b/autocat/notes LOAD \
""
simundump kenz /kinetics/b/inhib 0 0 0 0 0 6 1.6667 4 1 0 0 "" red green "" 2 1 \
  0
simundump text /kinetics/b/inhib/notes 0 ""
call /kinetics/b/inhib/notes LOAD \
""

simundump kpool /kinetics/c 0 0 0.1 0.1 0.6 0.6 0 0 6 0 /kinetics/geometry \
  blue yellow 2 2 0
simundump text /kinetics/c/notes 0 ""
call /kinetics/c/notes LOAD \
""
simundump kenz /kinetics/c/autocat 0 0 0 0 0 6 1.6667 4 1 0 0 "" red lightblue "" 2 3 \
  0
simundump text /kinetics/c/autocat/notes 0 ""
call /kinetics/c/autocat/notes LOAD \
""
simundump kenz /kinetics/c/inhib 0 0 0 0 0 6 1.6667 4 1 0 0 "" red lightblue "" -2 1 \
  0
simundump text /kinetics/c/inhib/notes 0 ""
call /kinetics/c/inhib/notes LOAD \
""

simundump kreac /kinetics/equil 0 0.05 0.05 "" white yellow 0 3 0
simundump text /kinetics/equil/notes 0 ""
call /kinetics/equil/notes LOAD \
""

simundump xgraph /graphs/conc1 0 0 2500 0.001 3 0
simundump xgraph /graphs/conc2 0 0 2500 0 5 0
simundump xplot /graphs/conc1/a.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 53 0 0 1
simundump xplot /graphs/conc1/b.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 47 0 0 1
simundump xplot /graphs/conc2/c.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 62 0 0 1
simundump xgraph /moregraphs/conc3 0 0 2500 0 1 0
simundump xgraph /moregraphs/conc4 0 0 2500 0 1 0
simundump xcoredraw /edit/draw 0 -9 6 -5 24
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"19 May 2014. M1719.g, implemented by hand." \
"This is the version start version."
addmsg /kinetics/b /kinetics/b/autocat ENZYME n 
addmsg /kinetics/b/autocat /kinetics/b REAC eA B 
addmsg /kinetics/a /kinetics/b/autocat SUBSTRATE n 
addmsg /kinetics/b/autocat /kinetics/a REAC sA B 
addmsg /kinetics/b/autocat /kinetics/b MM_PRD pA

addmsg /kinetics/b /kinetics/b/inhib ENZYME n 
addmsg /kinetics/b/inhib /kinetics/b REAC eA B 
addmsg /kinetics/c /kinetics/b/inhib SUBSTRATE n 
addmsg /kinetics/b/inhib /kinetics/c REAC sA B 
addmsg /kinetics/b/inhib /kinetics/a MM_PRD pA

addmsg /kinetics/c /kinetics/c/autocat ENZYME n 
addmsg /kinetics/c/autocat /kinetics/c REAC eA B 
addmsg /kinetics/a /kinetics/c/autocat SUBSTRATE n 
addmsg /kinetics/c/autocat /kinetics/a REAC sA B 
addmsg /kinetics/c/autocat /kinetics/c MM_PRD pA

addmsg /kinetics/c /kinetics/c/inhib ENZYME n 
addmsg /kinetics/c/inhib /kinetics/c REAC eA B 
addmsg /kinetics/b /kinetics/c/inhib SUBSTRATE n 
addmsg /kinetics/c/inhib /kinetics/b REAC sA B 
addmsg /kinetics/c/inhib /kinetics/a MM_PRD pA

addmsg /kinetics/b /kinetics/equil SUBSTRATE n
addmsg /kinetics/c /kinetics/equil PRODUCT n
addmsg /kinetics/equil /kinetics/b REAC A B 
addmsg /kinetics/equil /kinetics/c REAC B A 

addmsg /kinetics/a /graphs/conc1/a.Co PLOT Co *a.Co *red 
addmsg /kinetics/b /graphs/conc1/b.Co PLOT Co *b.Co *darkgreen 
addmsg /kinetics/c /graphs/conc2/c.Co PLOT Co *c.Co *blue 
enddump
// End of dump

complete_loading
