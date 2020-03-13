//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Fri Mar 29 19:34:16 2019
 
include kkit {argv 1}
 
FASTDT = 0.001
SIMDT = 0.001
CONTROLDT = 0.1
PLOTDT = 0.1
MAXTIME = 100
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 0
DEFAULT_VOL = 3.1e-18
VERSION = 11.0
setfield /file/modpath value ~/scripts/modules
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
simundump geometry /kinetics/geometry 0 3.1115e-18 3 sphere "" white black 0 \
  0 0
simundump geometry /kinetics/geometry[1] 0 3.9e-19 3 sphere "" white black 0 \
  0 0
simundump text /kinetics/notes 0 ""
call /kinetics/notes LOAD \
""
simundump text /kinetics/geometry/notes 0 ""
call /kinetics/geometry/notes LOAD \
""
simundump text /kinetics/geometry[1]/notes 0 ""
call /kinetics/geometry[1]/notes LOAD \
""
simundump group /kinetics/compt 0 blue green x 0 0 "" defaultfile \
  defaultfile.g 0 0 0 1 2 0
simundump text /kinetics/compt/notes 0 ""
call /kinetics/compt/notes LOAD \
""
simundump kpool /kinetics/CaCyt 0 1e-13 0.079999 0.079999 149.35 149.35 0 0 \
  1866.9 0 /kinetics/geometry 23 black 7 4 0
simundump text /kinetics/CaCyt/notes 0 ""
call /kinetics/CaCyt/notes LOAD \
""
simundump kpool /kinetics/CaIP3_3_R 0 0.0 0 0 0 0 0 0 1866.9 0 \
  /kinetics/geometry 20 black 4 7 0
simundump text /kinetics/CaIP3_3_R/notes 0 ""
call /kinetics/CaIP3_3_R/notes LOAD \
""
simundump kpool /kinetics/CaER 0 0.0 408.59 408.59 95611 95611 0 0 234 0 \
  /kinetics/geometry 0 black 2 5 0
simundump text /kinetics/CaER/notes 0 ""
call /kinetics/CaER/notes LOAD \
""
simundump kpool /kinetics/Serca 0 0.0 1.9999 1.9999 3733.7 3733.7 0 0 1866.9 \
  0 /kinetics/geometry 31 black 4 1 0
simundump text /kinetics/Serca/notes 0 ""
call /kinetics/Serca/notes LOAD \
""
simundump kenz /kinetics/Serca/MMenz_SERCA 0 0 0 0 0 1866.9 0.14284 32 8 0 1 \
  "" black 42 "" 4 2 0
simundump text /kinetics/Serca/MMenz_SERCA/notes 0 ""
call /kinetics/Serca/MMenz_SERCA/notes LOAD \
""
simundump kpool /kinetics/CaM 0 0.0 0 0 0 0 0 0 1866.9 0 /kinetics/geometry \
  23 black 9 -1 0
simundump text /kinetics/CaM/notes 0 ""
call /kinetics/CaM/notes LOAD \
""
simundump kpool /kinetics/CaMCa 0 0.0 0 0 0 0 0 0 1866.9 0 /kinetics/geometry \
  23 black 9 0 0
simundump text /kinetics/CaMCa/notes 0 ""
call /kinetics/CaMCa/notes LOAD \
""
simundump kpool /kinetics/CaMCa2 0 0.0 0 0 0 0 0 0 1866.9 0 \
  /kinetics/geometry 55 black 9 1 0
simundump text /kinetics/CaMCa2/notes 0 ""
call /kinetics/CaMCa2/notes LOAD \
""
simundump kpool /kinetics/CaMCa3 0 0.0 0 0 0 0 0 0 1866.9 0 \
  /kinetics/geometry 27 black 9 2 0
simundump text /kinetics/CaMCa3/notes 0 ""
call /kinetics/CaMCa3/notes LOAD \
""
simundump kpool /kinetics/CaMCa4 0 0.0 0 0 0 0 0 0 1866.9 0 \
  /kinetics/geometry 55 black 9 3 0
simundump text /kinetics/CaMCa4/notes 0 ""
call /kinetics/CaMCa4/notes LOAD \
""
simundump kpool /kinetics/IP3_R 0 0.0 0.1 0.1 186.69 186.69 0 0 1866.9 0 \
  /kinetics/geometry 22 black 3 9 0
simundump text /kinetics/IP3_R/notes 0 ""
call /kinetics/IP3_R/notes LOAD \
""
simundump kpool /kinetics/IP3_3_R 0 0.0 0 0 0 0 0 0 1866.9 0 \
  /kinetics/geometry 1 black 4 7 0
simundump text /kinetics/IP3_3_R/notes 0 ""
call /kinetics/IP3_3_R/notes LOAD \
""
simundump kpool /kinetics/IP3 0 0.0 7.9999 7.9999 14935 14935 0 0 1866.9 4 \
  /kinetics/geometry 53 black 5 9 0
simundump text /kinetics/IP3/notes 0 ""
call /kinetics/IP3/notes LOAD \
""
simundump kpool /kinetics/Ca2_IP3_3_R 0 0.0 0 0 0 0 0 0 1866.9 0 \
  /kinetics/geometry 25 black 7 9 0
simundump text /kinetics/Ca2_IP3_3_R/notes 0 ""
call /kinetics/Ca2_IP3_3_R/notes LOAD \
""
simundump kpool /kinetics/Mirror_CaIP3_3_R 0 0.0 0 0 0 0 0 0 1866.9 0 \
  /kinetics/geometry 4 black -1 4 0
simundump text /kinetics/Mirror_CaIP3_3_R/notes 0 ""
call /kinetics/Mirror_CaIP3_3_R/notes LOAD \
""
simundump kpool /kinetics/ActIP3R 0 0.0 0 0 0 0 0 0 1866.9 0 \
  /kinetics/geometry 8 black 2 1 0
simundump text /kinetics/ActIP3R/notes 0 ""
call /kinetics/ActIP3R/notes LOAD \
""
simundump kchan /kinetics/ActIP3R/chan 0 8 0.1 0 1 0 "" brown 8 2 2 0
simundump text /kinetics/ActIP3R/chan/notes 0 ""
call /kinetics/ActIP3R/chan/notes LOAD \
""
simundump kreac /kinetics/CaMreac1 0 0.0045449 8.4853 "" white black 5 0 0
simundump text /kinetics/CaMreac1/notes 0 ""
call /kinetics/CaMreac1/notes LOAD \
""
simundump kreac /kinetics/CaMreac2 0 0.0045449 8.4853 "" white black 6 1 0
simundump text /kinetics/CaMreac2/notes 0 ""
call /kinetics/CaMreac2/notes LOAD \
""
simundump kreac /kinetics/CaMreac3 0 0.0019284 10 "" white black 7 2 0
simundump text /kinetics/CaMreac3/notes 0 ""
call /kinetics/CaMreac3/notes LOAD \
""
simundump kreac /kinetics/CaMreac4 0 0.00096419 10 "" white black 8 3 0
simundump text /kinetics/CaMreac4/notes 0 ""
call /kinetics/CaMreac4/notes LOAD \
""
simundump kreac /kinetics/Reac 0 0.0064279 8 "" white black 4 8 0
simundump text /kinetics/Reac/notes 0 ""
call /kinetics/Reac/notes LOAD \
""
simundump kreac /kinetics/Reac2 0 0.0080348 1.65 "" white black 6 6 0
simundump text /kinetics/Reac2/notes 0 ""
call /kinetics/Reac2/notes LOAD \
""
simundump kreac /kinetics/Reac4 0 0.00096422 0.21 "" white black 7 7 0
simundump text /kinetics/Reac4/notes 0 ""
call /kinetics/Reac4/notes LOAD \
""
simundump kreac /kinetics/Reac1 0 0.0001492 5 "" white black 0 0 0
simundump text /kinetics/Reac1/notes 0 ""
call /kinetics/Reac1/notes LOAD \
""
simundump kpool /kinetics/leakPool 0 0 1 1 234 234 0 0 234 0 \
  /kinetics/geometry[1] 45 black 4 4 0
simundump text /kinetics/leakPool/notes 0 ""
call /kinetics/leakPool/notes LOAD \
""
simundump kchan /kinetics/leakPool/leakChan 0 0.04 0.1 0 1 0 "" brown 45 4 5 \
  0
simundump text /kinetics/leakPool/leakChan/notes 0 ""
call /kinetics/leakPool/leakChan/notes LOAD \
""
simundump xgraph /graphs/conc1 0 0 100 0.001 0.999 0
simundump xgraph /graphs/conc2 0 0 100 0 1 0
simundump xplot /graphs/conc1/CaCyt.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 23 0 0 1
simundump xplot /graphs/conc1/ActIP3R.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 8 0 0 1
simundump xplot /graphs/conc1/CaIP3_3_R.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 20 0 0 1
simundump xplot /graphs/conc2/CaER.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 0 0 0 1
simundump xgraph /moregraphs/conc3 0 0 100 0 1 0
simundump xgraph /moregraphs/conc4 0 0 100 0 1 0
simundump xcoredraw /edit/draw 0 -1.5666 11.334 -1.8008 10.24
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
addmsg /kinetics/CaMreac1 /kinetics/CaCyt REAC A B 
addmsg /kinetics/CaMreac2 /kinetics/CaCyt REAC A B 
addmsg /kinetics/CaMreac3 /kinetics/CaCyt REAC A B 
addmsg /kinetics/CaMreac4 /kinetics/CaCyt REAC A B 
addmsg /kinetics/Reac2 /kinetics/CaCyt REAC A B 
addmsg /kinetics/Reac4 /kinetics/CaCyt REAC A B 
addmsg /kinetics/Serca/MMenz_SERCA /kinetics/CaCyt REAC sA B 
addmsg /kinetics/leakPool/leakChan /kinetics/CaCyt REAC B A 
addmsg /kinetics/ActIP3R/chan /kinetics/CaCyt REAC B A 
addmsg /kinetics/Reac2 /kinetics/CaIP3_3_R REAC B A 
addmsg /kinetics/Reac4 /kinetics/CaIP3_3_R REAC A B 
addmsg /kinetics/Serca/MMenz_SERCA /kinetics/CaER MM_PRD pA 
addmsg /kinetics/leakPool/leakChan /kinetics/CaER REAC A B 
addmsg /kinetics/ActIP3R/chan /kinetics/CaER REAC A B 
addmsg /kinetics/CaCyt /kinetics/Serca/MMenz_SERCA SUBSTRATE n 
addmsg /kinetics/Serca /kinetics/Serca/MMenz_SERCA ENZYME n 
addmsg /kinetics/CaMreac1 /kinetics/CaM REAC A B 
addmsg /kinetics/CaMreac1 /kinetics/CaMCa REAC B A 
addmsg /kinetics/CaMreac2 /kinetics/CaMCa REAC A B 
addmsg /kinetics/CaMreac2 /kinetics/CaMCa2 REAC B A 
addmsg /kinetics/CaMreac3 /kinetics/CaMCa2 REAC A B 
addmsg /kinetics/CaMreac3 /kinetics/CaMCa3 REAC B A 
addmsg /kinetics/CaMreac4 /kinetics/CaMCa3 REAC A B 
addmsg /kinetics/CaMreac4 /kinetics/CaMCa4 REAC B A 
addmsg /kinetics/Reac /kinetics/IP3_R REAC A B 
addmsg /kinetics/Reac /kinetics/IP3_3_R REAC B A 
addmsg /kinetics/Reac2 /kinetics/IP3_3_R REAC A B 
addmsg /kinetics/Reac /kinetics/IP3 REAC A B 
addmsg /kinetics/Reac4 /kinetics/Ca2_IP3_3_R REAC B A 
addmsg /kinetics/CaIP3_3_R /kinetics/Mirror_CaIP3_3_R SUMTOTAL n nInit 
addmsg /kinetics/Reac1 /kinetics/Mirror_CaIP3_3_R REAC A B 
addmsg /kinetics/Reac1 /kinetics/Mirror_CaIP3_3_R REAC A B 
addmsg /kinetics/Reac1 /kinetics/Mirror_CaIP3_3_R REAC A B 
addmsg /kinetics/Reac1 /kinetics/ActIP3R REAC B A 
addmsg /kinetics/ActIP3R /kinetics/ActIP3R/chan NUMCHAN n 
addmsg /kinetics/CaER /kinetics/ActIP3R/chan SUBSTRATE n vol 
addmsg /kinetics/CaCyt /kinetics/ActIP3R/chan PRODUCT n vol 
addmsg /kinetics/CaCyt /kinetics/CaMreac1 SUBSTRATE n 
addmsg /kinetics/CaM /kinetics/CaMreac1 SUBSTRATE n 
addmsg /kinetics/CaMCa /kinetics/CaMreac1 PRODUCT n 
addmsg /kinetics/CaMCa /kinetics/CaMreac2 SUBSTRATE n 
addmsg /kinetics/CaCyt /kinetics/CaMreac2 SUBSTRATE n 
addmsg /kinetics/CaMCa2 /kinetics/CaMreac2 PRODUCT n 
addmsg /kinetics/CaCyt /kinetics/CaMreac3 SUBSTRATE n 
addmsg /kinetics/CaMCa2 /kinetics/CaMreac3 SUBSTRATE n 
addmsg /kinetics/CaMCa3 /kinetics/CaMreac3 PRODUCT n 
addmsg /kinetics/CaCyt /kinetics/CaMreac4 SUBSTRATE n 
addmsg /kinetics/CaMCa3 /kinetics/CaMreac4 SUBSTRATE n 
addmsg /kinetics/CaMCa4 /kinetics/CaMreac4 PRODUCT n 
addmsg /kinetics/IP3_R /kinetics/Reac SUBSTRATE n 
addmsg /kinetics/IP3 /kinetics/Reac SUBSTRATE n 
addmsg /kinetics/IP3_3_R /kinetics/Reac PRODUCT n 
addmsg /kinetics/CaCyt /kinetics/Reac2 SUBSTRATE n 
addmsg /kinetics/IP3_3_R /kinetics/Reac2 SUBSTRATE n 
addmsg /kinetics/CaIP3_3_R /kinetics/Reac2 PRODUCT n 
addmsg /kinetics/CaCyt /kinetics/Reac4 SUBSTRATE n 
addmsg /kinetics/CaIP3_3_R /kinetics/Reac4 SUBSTRATE n 
addmsg /kinetics/Ca2_IP3_3_R /kinetics/Reac4 PRODUCT n 
addmsg /kinetics/Mirror_CaIP3_3_R /kinetics/Reac1 SUBSTRATE n 
addmsg /kinetics/Mirror_CaIP3_3_R /kinetics/Reac1 SUBSTRATE n 
addmsg /kinetics/Mirror_CaIP3_3_R /kinetics/Reac1 SUBSTRATE n 
addmsg /kinetics/ActIP3R /kinetics/Reac1 PRODUCT n 
addmsg /kinetics/leakPool /kinetics/leakPool/leakChan NUMCHAN n 
addmsg /kinetics/CaER /kinetics/leakPool/leakChan SUBSTRATE n vol 
addmsg /kinetics/CaCyt /kinetics/leakPool/leakChan PRODUCT n vol 
addmsg /kinetics/CaCyt /graphs/conc1/CaCyt.Co PLOT Co *CaCyt.Co *23 
addmsg /kinetics/ActIP3R /graphs/conc1/ActIP3R.Co PLOT Co *ActIP3R.Co *8 
addmsg /kinetics/CaIP3_3_R /graphs/conc1/CaIP3_3_R.Co PLOT Co *CaIP3_3_R.Co *20 
addmsg /kinetics/CaER /graphs/conc2/CaER.Co PLOT Co *CaER.Co *0 
enddump
// End of dump

complete_loading
