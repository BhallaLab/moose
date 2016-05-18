//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Sun Dec  7 15:51:46 2014
 
include kkit {argv 1}
 
FASTDT = 1e-05
SIMDT = 0.0001
CONTROLDT = 1
PLOTDT = 0.2
MAXTIME = 200
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 1
DEFAULT_VOL = 1e-19
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
simundump geometry /kinetics/geometry 0 1.6667e-21 3 sphere "" white black 5 \
  -4 0
simundump geometry /kinetics/geometry[1] 0 1e-19 3 sphere "" white black 5 -2 \
  0
simundump geometry /kinetics/geometry[2] 0 1e-20 3 sphere "" white black 2 2 \
  0
simundump geometry /kinetics/geometry[3] 0 1e-20 3 sphere "" white black 4 8 \
  0
simundump geometry /kinetics/geometry[4] 0 1e-20 3 sphere "" white black 2 \
  -17 0
simundump geometry /kinetics/geometry[5] 0 1e-20 3 sphere "" white black 5 \
  -13 0
simundump geometry /kinetics/geometry[6] 0 1e-19 3 sphere "" white black 0 7 \
  0
simundump geometry /kinetics/geometry[7] 0 1e-20 3 sphere "" white black 0 -3 \
  0
simundump geometry /kinetics/geometry[8] 0 1e-20 3 sphere "" white black -1 \
  -18 0
simundump geometry /kinetics/geometry[9] 0 1e-18 3 sphere "" white black 0 0 \
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
simundump text /kinetics/geometry[4]/notes 0 ""
call /kinetics/geometry[4]/notes LOAD \
""
simundump text /kinetics/geometry[5]/notes 0 ""
call /kinetics/geometry[5]/notes LOAD \
""
simundump text /kinetics/geometry[6]/notes 0 ""
call /kinetics/geometry[6]/notes LOAD \
""
simundump text /kinetics/geometry[7]/notes 0 ""
call /kinetics/geometry[7]/notes LOAD \
""
simundump text /kinetics/geometry[8]/notes 0 ""
call /kinetics/geometry[8]/notes LOAD \
""
simundump text /kinetics/geometry[9]/notes 0 ""
call /kinetics/geometry[9]/notes LOAD \
""
simundump kreac /kinetics/exocytosis 0 0.01 0 "" white black 4 -5 0
simundump text /kinetics/exocytosis/notes 0 ""
call /kinetics/exocytosis/notes LOAD \
""
simundump kreac /kinetics/endocytosis 0 0 4 "" white black -7 -5 0
simundump text /kinetics/endocytosis/notes 0 ""
call /kinetics/endocytosis/notes LOAD \
""
simundump group /kinetics/PSD 0 0 black x 0 1 "" PSD defaultfile.g 0 0 0 -10 \
  -3 0
simundump text /kinetics/PSD/notes 0 ""
call /kinetics/PSD/notes LOAD \
""
simundump kpool /kinetics/PSD/Rp 0 0 0 0 0 0 0 0 6 0 /kinetics/geometry[1] \
  blue 0 -4 1 0
simundump text /kinetics/PSD/Rp/notes 0 ""
call /kinetics/PSD/Rp/notes LOAD \
""
simundump kpool /kinetics/PSD/P 0 0 1 1 6 6 0 0 6 0 /kinetics/geometry[1] 61 \
  0 -4 -2 0
simundump text /kinetics/PSD/P/notes 0 ""
call /kinetics/PSD/P/notes LOAD \
""
simundump kenz /kinetics/PSD/P/P1 0 0 0 0 0 60 10.417 20 5 0 0 "" red 61 "" \
  -6 -1 0
simundump text /kinetics/PSD/P/P1/notes 0 ""
call /kinetics/PSD/P/P1/notes LOAD \
""
simundump kenz /kinetics/PSD/P/P2 0 0 0 0 0 60 10.417 20 5 0 0 "" red 61 "" \
  -2 -1 0
simundump text /kinetics/PSD/P/P2/notes 0 ""
call /kinetics/PSD/P/P2/notes LOAD \
""
simundump kpool /kinetics/PSD/KK 0 0 3 3 18 18 0 0 6 0 /kinetics/geometry[1] \
  49 0 -4 4 0
simundump text /kinetics/PSD/KK/notes 0 ""
call /kinetics/PSD/KK/notes LOAD \
""
simundump kenz /kinetics/PSD/KK/KK1 0 0 0 0 0 60 4.1667 40 10 0 0 "" red 49 \
  "" -6 3 0
simundump text /kinetics/PSD/KK/KK1/notes 0 ""
call /kinetics/PSD/KK/KK1/notes LOAD \
""
simundump kenz /kinetics/PSD/KK/KK2 0 0 0 0 0 60 4.1667 40 10 0 0 "" red 49 \
  "" -2 3 0
simundump text /kinetics/PSD/KK/KK2/notes 0 ""
call /kinetics/PSD/KK/KK2/notes LOAD \
""
simundump kpool /kinetics/PSD/Rpp 0 0 0 0 0 0 0 0 6 0 /kinetics/geometry[1] \
  28 0 0 1 0
simundump text /kinetics/PSD/Rpp/notes 0 ""
call /kinetics/PSD/Rpp/notes LOAD \
""
simundump kpool /kinetics/PSD/R 0 0 0 0 0 0 0 0 6 0 /kinetics/geometry[1] 4 0 \
  -8 1 0
simundump text /kinetics/PSD/R/notes 0 ""
call /kinetics/PSD/R/notes LOAD \
""
simundump kpool /kinetics/PSD/tot_PSD_R 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry[4] blue 0 -4 7 0
simundump text /kinetics/PSD/tot_PSD_R/notes 0 ""
call /kinetics/PSD/tot_PSD_R/notes LOAD \
""
simundump kpool /kinetics/PSD/Ca_PSD 0 0 0.1 0.1 0.6 0.6 0 0 6 0 \
  /kinetics/geometry[5] 53 0 -15 2 0
simundump text /kinetics/PSD/Ca_PSD/notes 0 ""
call /kinetics/PSD/Ca_PSD/notes LOAD \
""
simundump kpool /kinetics/PSD/psd_inact_CaN 0 0 1 1 6 6 0 0 6 0 \
  /kinetics/geometry[7] blue 0 -12 5 0
simundump text /kinetics/PSD/psd_inact_CaN/notes 0 ""
call /kinetics/PSD/psd_inact_CaN/notes LOAD \
""
simundump kreac /kinetics/PSD/psd_activate_CaN 0 0.27778 1 "" white 0 -11 1 0
simundump text /kinetics/PSD/psd_activate_CaN/notes 0 ""
call /kinetics/PSD/psd_activate_CaN/notes LOAD \
""
simundump kpool /kinetics/PSD/Ca.P 0 0 0 0 0 0 0 0 6 0 /kinetics/geometry[7] \
  blue 0 -4 -4 0
simundump text /kinetics/PSD/Ca.P/notes 0 ""
call /kinetics/PSD/Ca.P/notes LOAD \
""
simundump kenz /kinetics/PSD/Ca.P/Ca.P2 0 0 0 0 0 6 1.6667 40 10 0 0 "" red \
  blue "" -2 -3 0
simundump text /kinetics/PSD/Ca.P/Ca.P2/notes 0 ""
call /kinetics/PSD/Ca.P/Ca.P2/notes LOAD \
""
simundump kenz /kinetics/PSD/Ca.P/Ca.P1 0 0 0 0 0 6 1.6667 40 10 0 0 "" red \
  blue "" -6 -3 0
simundump text /kinetics/PSD/Ca.P/Ca.P1/notes 0 ""
call /kinetics/PSD/Ca.P/Ca.P1/notes LOAD \
""
simundump kpool /kinetics/PSD/Ca_input 0 0 0.1 0.1 0.6 0.6 0 0 6 4 \
  /kinetics/geometry[8] 60 black -12 9 0
simundump text /kinetics/PSD/Ca_input/notes 0 ""
call /kinetics/PSD/Ca_input/notes LOAD \
""
simundump kreac /kinetics/PSD/Ca_reac 0 100 100 "" white black -17 6 0
simundump text /kinetics/PSD/Ca_reac/notes 0 ""
call /kinetics/PSD/Ca_reac/notes LOAD \
""
simundump group /kinetics/Bulk 0 yellow black x 0 0 "" Bulk defaultfile.g 0 0 \
  0 -6 -16 0
simundump text /kinetics/Bulk/notes 0 ""
call /kinetics/Bulk/notes LOAD \
""
simundump kpool /kinetics/Bulk/iRpp 0 0 0 0 0 0 0 0 59.999 0 \
  /kinetics/geometry 29 yellow 2 -11 0
simundump text /kinetics/Bulk/iRpp/notes 0 ""
call /kinetics/Bulk/iRpp/notes LOAD \
""
simundump kpool /kinetics/Bulk/iRp 0 0 0 0 0 0 0 0 59.999 0 \
  /kinetics/geometry blue yellow -2 -11 0
simundump text /kinetics/Bulk/iRp/notes 0 ""
call /kinetics/Bulk/iRp/notes LOAD \
""
simundump kpool /kinetics/Bulk/iR 0 0 4.0001 4.0001 240 240 0 0 59.999 0 \
  /kinetics/geometry 0 yellow -6 -11 0
simundump text /kinetics/Bulk/iR/notes 0 "Same as Fus3\n"
call /kinetics/Bulk/iR/notes LOAD \
"Same as Fus3" \
""
simundump kpool /kinetics/Bulk/iKK 0 0 1 1 59.999 59.999 0 0 59.999 0 \
  /kinetics/geometry 50 yellow -2 -8 0
simundump text /kinetics/Bulk/iKK/notes 0 "Same as Ste7\n"
call /kinetics/Bulk/iKK/notes LOAD \
"Same as Ste7" \
""
simundump kenz /kinetics/Bulk/iKK/iKK1 0 0 0 0 0 59.999 0.83335 40 10 0 0 "" \
  red 50 "" -4 -9 0
simundump text /kinetics/Bulk/iKK/iKK1/notes 0 ""
call /kinetics/Bulk/iKK/iKK1/notes LOAD \
""
simundump kenz /kinetics/Bulk/iKK/iKK2 0 0 0 0 0 59.999 0.83335 40 10 0 0 "" \
  red 50 "" 0 -9 0
simundump text /kinetics/Bulk/iKK/iKK2/notes 0 ""
call /kinetics/Bulk/iKK/iKK2/notes LOAD \
""
simundump kpool /kinetics/Bulk/iP 0 0 0.50001 0.50001 30 30 0 0 59.999 0 \
  /kinetics/geometry 62 yellow -2 -14 0
simundump text /kinetics/Bulk/iP/notes 0 "Same as Msg5\n"
call /kinetics/Bulk/iP/notes LOAD \
"Same as Msg5" \
""
simundump kenz /kinetics/Bulk/iP/iP1 0 0 0 0 0 59.999 6.2501 60 15 0 0 "" red \
  62 "" -4 -13 0
simundump text /kinetics/Bulk/iP/iP1/notes 0 ""
call /kinetics/Bulk/iP/iP1/notes LOAD \
""
simundump kenz /kinetics/Bulk/iP/iP2 0 0 0 0 0 59.999 6.2501 60 15 0 0 "" red \
  62 "" 0 -13 0
simundump text /kinetics/Bulk/iP/iP2/notes 0 ""
call /kinetics/Bulk/iP/iP2/notes LOAD \
""
simundump kpool /kinetics/Bulk/PKA 0 0 0 0 0 0 0 0 60 0 /kinetics/geometry[3] \
  43 yellow -3 -5 0
simundump text /kinetics/Bulk/PKA/notes 0 ""
call /kinetics/Bulk/PKA/notes LOAD \
""
simundump kenz /kinetics/Bulk/PKA/PKA1 0 0 0 0 0 60 0.41667 40 10 0 0 "" red \
  43 "" -4 -7 0
simundump text /kinetics/Bulk/PKA/PKA1/notes 0 ""
call /kinetics/Bulk/PKA/PKA1/notes LOAD \
""
simundump kenz /kinetics/Bulk/PKA/PKA2 0 0 0 0 0 60 0.41667 40 10 0 0 "" red \
  43 "" 0 -7 0
simundump text /kinetics/Bulk/PKA/PKA2/notes 0 ""
call /kinetics/Bulk/PKA/PKA2/notes LOAD \
""
simundump kenz /kinetics/Bulk/PKA/PKA_exo 0 0 0 0 0 60 0.016667 8 2 0 0 "" \
  red 43 "" 1 -5 0
simundump text /kinetics/Bulk/PKA/PKA_exo/notes 0 ""
call /kinetics/Bulk/PKA/PKA_exo/notes LOAD \
""
simundump kreac /kinetics/Bulk/activate_PKA 0 0.0016667 5 "" white yellow -11 \
  -10 0
simundump text /kinetics/Bulk/activate_PKA/notes 0 ""
call /kinetics/Bulk/activate_PKA/notes LOAD \
""
simundump kpool /kinetics/Bulk/inact_PKA 0 0 1.6 1.6 96 96 0 0 60 0 \
  /kinetics/geometry[2] 51 yellow -12 -6 0
simundump text /kinetics/Bulk/inact_PKA/notes 0 ""
call /kinetics/Bulk/inact_PKA/notes LOAD \
""
simundump kpool /kinetics/Bulk/inact_CaN 0 0 0.2 0.2 12 12 0 0 60 0 \
  /kinetics/geometry[6] blue yellow -13 -18 0
simundump text /kinetics/Bulk/inact_CaN/notes 0 ""
call /kinetics/Bulk/inact_CaN/notes LOAD \
""
simundump kreac /kinetics/Bulk/activate_CaN 0 0.0055556 1 "" white yellow -10 \
  -16 0
simundump text /kinetics/Bulk/activate_CaN/notes 0 ""
call /kinetics/Bulk/activate_CaN/notes LOAD \
""
simundump kpool /kinetics/Bulk/Ca_Spine 0 0 0.1 0.1 6 6 0 0 60 0 \
  /kinetics/geometry[2] 50 yellow -15 -13 0
simundump text /kinetics/Bulk/Ca_Spine/notes 0 ""
call /kinetics/Bulk/Ca_Spine/notes LOAD \
""
simundump kreac /kinetics/Bulk/Ca_to_dend 0 10 1 "" white black -16 -22 0
simundump text /kinetics/Bulk/Ca_to_dend/notes 0 ""
call /kinetics/Bulk/Ca_to_dend/notes LOAD \
""
simundump kreac /kinetics/Ca_to_bulk 0 10 100 "" white black -15 -3 0
simundump text /kinetics/Ca_to_bulk/notes 0 ""
call /kinetics/Ca_to_bulk/notes LOAD \
""
simundump kpool /kinetics/Ca_dend 0 0 0 0 0 0 0 0 600 0 /kinetics/geometry[9] \
  1 black -19 -25 0
simundump text /kinetics/Ca_dend/notes 0 ""
call /kinetics/Ca_dend/notes LOAD \
""
simundump kpool /kinetics/Ca_dend_input 0 0 0 0 0 0 0 0 600 0 \
  /kinetics/geometry[9] 60 black -12 -28 0
simundump text /kinetics/Ca_dend_input/notes 0 ""
call /kinetics/Ca_dend_input/notes LOAD \
""
simundump kreac /kinetics/dend_Ca_reac 0 100 100 "" white black -16 -27 0
simundump text /kinetics/dend_Ca_reac/notes 0 ""
call /kinetics/dend_Ca_reac/notes LOAD \
""
simundump xgraph /graphs/conc1 0 0 800 9.6002e-07 0.10547 0
simundump xgraph /graphs/conc2 0 0 800 0 18.128 0
simundump xplot /graphs/conc1/Ca_PSD.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 53 0 0 1
simundump xplot /graphs/conc1/Ca_Bulk.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 41 0 0 1
simundump xplot /graphs/conc1/PKA.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 43 0 0 1
simundump xplot /graphs/conc1/Ca.P.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc1/Ca_dend.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 1 0 0 1
simundump xplot /graphs/conc2/tot_PSD_R.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc2/Rpp.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 28 0 0 1
simundump xgraph /moregraphs/conc3 0 0 800 0 33.241 0
simundump xgraph /moregraphs/conc4 0 0 800 0 2 0
simundump xcoredraw /edit/draw 0 -21 7 -30 11
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"26 Nov 2014. psd51.g: based on psd50.g which was" \
"based on psd41_back_dend.g." \
"07 Dec 2014. psd52.g: based on psd51. Slowed down Ca_to_dend by " \
"10x while retaining Keq."
addmsg /kinetics/Bulk/iRpp /kinetics/exocytosis SUBSTRATE n 
addmsg /kinetics/PSD/Rpp /kinetics/exocytosis PRODUCT n 
addmsg /kinetics/Bulk/iR /kinetics/endocytosis SUBSTRATE n 
addmsg /kinetics/PSD/R /kinetics/endocytosis PRODUCT n 
addmsg /kinetics/PSD/KK/KK1 /kinetics/PSD/Rp MM_PRD pA 
addmsg /kinetics/PSD/KK/KK2 /kinetics/PSD/Rp REAC sA B 
addmsg /kinetics/PSD/P/P2 /kinetics/PSD/Rp MM_PRD pA 
addmsg /kinetics/PSD/P/P1 /kinetics/PSD/Rp REAC sA B 
addmsg /kinetics/PSD/Ca.P/Ca.P2 /kinetics/PSD/Rp MM_PRD pA 
addmsg /kinetics/PSD/Ca.P/Ca.P1 /kinetics/PSD/Rp REAC sA B 
addmsg /kinetics/PSD/P/P1 /kinetics/PSD/P REAC eA B 
addmsg /kinetics/PSD/P/P2 /kinetics/PSD/P REAC eA B 
addmsg /kinetics/PSD/P /kinetics/PSD/P/P1 ENZYME n 
addmsg /kinetics/PSD/Rp /kinetics/PSD/P/P1 SUBSTRATE n 
addmsg /kinetics/PSD/P /kinetics/PSD/P/P2 ENZYME n 
addmsg /kinetics/PSD/Rpp /kinetics/PSD/P/P2 SUBSTRATE n 
addmsg /kinetics/PSD/KK/KK1 /kinetics/PSD/KK REAC eA B 
addmsg /kinetics/PSD/KK/KK2 /kinetics/PSD/KK REAC eA B 
addmsg /kinetics/PSD/KK /kinetics/PSD/KK/KK1 ENZYME n 
addmsg /kinetics/PSD/R /kinetics/PSD/KK/KK1 SUBSTRATE n 
addmsg /kinetics/PSD/KK /kinetics/PSD/KK/KK2 ENZYME n 
addmsg /kinetics/PSD/Rp /kinetics/PSD/KK/KK2 SUBSTRATE n 
addmsg /kinetics/PSD/KK/KK2 /kinetics/PSD/Rpp MM_PRD pA 
addmsg /kinetics/PSD/P/P2 /kinetics/PSD/Rpp REAC sA B 
addmsg /kinetics/exocytosis /kinetics/PSD/Rpp REAC B A 
addmsg /kinetics/Bulk/PKA/PKA_exo /kinetics/PSD/Rpp MM_PRD pA 
addmsg /kinetics/PSD/Ca.P/Ca.P2 /kinetics/PSD/Rpp REAC sA B 
addmsg /kinetics/PSD/KK/KK1 /kinetics/PSD/R REAC sA B 
addmsg /kinetics/PSD/P/P1 /kinetics/PSD/R MM_PRD pA 
addmsg /kinetics/PSD/Ca.P/Ca.P1 /kinetics/PSD/R MM_PRD pA 
addmsg /kinetics/endocytosis /kinetics/PSD/R REAC B A 
addmsg /kinetics/PSD/R /kinetics/PSD/tot_PSD_R SUMTOTAL n nInit 
addmsg /kinetics/PSD/Rp /kinetics/PSD/tot_PSD_R SUMTOTAL n nInit 
addmsg /kinetics/PSD/Rpp /kinetics/PSD/tot_PSD_R SUMTOTAL n nInit 
addmsg /kinetics/PSD/psd_activate_CaN /kinetics/PSD/Ca_PSD REAC A B 
addmsg /kinetics/PSD/psd_activate_CaN /kinetics/PSD/Ca_PSD REAC A B 
addmsg /kinetics/Ca_to_bulk /kinetics/PSD/Ca_PSD REAC B A 
addmsg /kinetics/PSD/Ca_reac /kinetics/PSD/Ca_PSD REAC B A 
addmsg /kinetics/PSD/psd_activate_CaN /kinetics/PSD/psd_inact_CaN REAC A B 
addmsg /kinetics/PSD/psd_inact_CaN /kinetics/PSD/psd_activate_CaN SUBSTRATE n 
addmsg /kinetics/PSD/Ca_PSD /kinetics/PSD/psd_activate_CaN SUBSTRATE n 
addmsg /kinetics/PSD/Ca_PSD /kinetics/PSD/psd_activate_CaN SUBSTRATE n 
addmsg /kinetics/PSD/Ca.P /kinetics/PSD/psd_activate_CaN PRODUCT n 
addmsg /kinetics/PSD/psd_activate_CaN /kinetics/PSD/Ca.P REAC B A 
addmsg /kinetics/PSD/Ca.P/Ca.P2 /kinetics/PSD/Ca.P REAC eA B 
addmsg /kinetics/PSD/Ca.P/Ca.P1 /kinetics/PSD/Ca.P REAC eA B 
addmsg /kinetics/PSD/Ca.P /kinetics/PSD/Ca.P/Ca.P2 ENZYME n 
addmsg /kinetics/PSD/Rpp /kinetics/PSD/Ca.P/Ca.P2 SUBSTRATE n 
addmsg /kinetics/PSD/Ca.P /kinetics/PSD/Ca.P/Ca.P1 ENZYME n 
addmsg /kinetics/PSD/Rp /kinetics/PSD/Ca.P/Ca.P1 SUBSTRATE n 
addmsg /kinetics/PSD/Ca_reac /kinetics/PSD/Ca_input REAC A B 
addmsg /kinetics/PSD/Ca_input /kinetics/PSD/Ca_reac SUBSTRATE n 
addmsg /kinetics/PSD/Ca_PSD /kinetics/PSD/Ca_reac PRODUCT n 
addmsg /kinetics/Bulk/iKK/iKK2 /kinetics/Bulk/iRpp MM_PRD pA 
addmsg /kinetics/Bulk/iP/iP2 /kinetics/Bulk/iRpp REAC sA B 
addmsg /kinetics/exocytosis /kinetics/Bulk/iRpp REAC A B 
addmsg /kinetics/Bulk/PKA/PKA2 /kinetics/Bulk/iRpp MM_PRD pA 
addmsg /kinetics/Bulk/PKA/PKA_exo /kinetics/Bulk/iRpp REAC sA B 
addmsg /kinetics/Bulk/iKK/iKK1 /kinetics/Bulk/iRp MM_PRD pA 
addmsg /kinetics/Bulk/iKK/iKK2 /kinetics/Bulk/iRp REAC sA B 
addmsg /kinetics/Bulk/iP/iP2 /kinetics/Bulk/iRp MM_PRD pA 
addmsg /kinetics/Bulk/iP/iP1 /kinetics/Bulk/iRp REAC sA B 
addmsg /kinetics/Bulk/PKA/PKA1 /kinetics/Bulk/iRp MM_PRD pA 
addmsg /kinetics/Bulk/PKA/PKA2 /kinetics/Bulk/iRp REAC sA B 
addmsg /kinetics/Bulk/iKK/iKK1 /kinetics/Bulk/iR REAC sA B 
addmsg /kinetics/Bulk/iP/iP1 /kinetics/Bulk/iR MM_PRD pA 
addmsg /kinetics/Bulk/PKA/PKA1 /kinetics/Bulk/iR REAC sA B 
addmsg /kinetics/endocytosis /kinetics/Bulk/iR REAC A B 
addmsg /kinetics/Bulk/iKK/iKK1 /kinetics/Bulk/iKK REAC eA B 
addmsg /kinetics/Bulk/iKK/iKK2 /kinetics/Bulk/iKK REAC eA B 
addmsg /kinetics/Bulk/iKK /kinetics/Bulk/iKK/iKK1 ENZYME n 
addmsg /kinetics/Bulk/iR /kinetics/Bulk/iKK/iKK1 SUBSTRATE n 
addmsg /kinetics/Bulk/iKK /kinetics/Bulk/iKK/iKK2 ENZYME n 
addmsg /kinetics/Bulk/iRp /kinetics/Bulk/iKK/iKK2 SUBSTRATE n 
addmsg /kinetics/Bulk/iP/iP1 /kinetics/Bulk/iP REAC eA B 
addmsg /kinetics/Bulk/iP/iP2 /kinetics/Bulk/iP REAC eA B 
addmsg /kinetics/Bulk/activate_CaN /kinetics/Bulk/iP REAC B A 
addmsg /kinetics/Bulk/iP /kinetics/Bulk/iP/iP1 ENZYME n 
addmsg /kinetics/Bulk/iRp /kinetics/Bulk/iP/iP1 SUBSTRATE n 
addmsg /kinetics/Bulk/iP /kinetics/Bulk/iP/iP2 ENZYME n 
addmsg /kinetics/Bulk/iRpp /kinetics/Bulk/iP/iP2 SUBSTRATE n 
addmsg /kinetics/Bulk/activate_PKA /kinetics/Bulk/PKA REAC B A 
addmsg /kinetics/Bulk/PKA/PKA1 /kinetics/Bulk/PKA REAC eA B 
addmsg /kinetics/Bulk/PKA/PKA2 /kinetics/Bulk/PKA REAC eA B 
addmsg /kinetics/Bulk/PKA/PKA_exo /kinetics/Bulk/PKA REAC eA B 
addmsg /kinetics/Bulk/PKA /kinetics/Bulk/PKA/PKA1 ENZYME n 
addmsg /kinetics/Bulk/iR /kinetics/Bulk/PKA/PKA1 SUBSTRATE n 
addmsg /kinetics/Bulk/PKA /kinetics/Bulk/PKA/PKA2 ENZYME n 
addmsg /kinetics/Bulk/iRp /kinetics/Bulk/PKA/PKA2 SUBSTRATE n 
addmsg /kinetics/Bulk/PKA /kinetics/Bulk/PKA/PKA_exo ENZYME n 
addmsg /kinetics/Bulk/iRpp /kinetics/Bulk/PKA/PKA_exo SUBSTRATE n 
addmsg /kinetics/Bulk/inact_PKA /kinetics/Bulk/activate_PKA SUBSTRATE n 
addmsg /kinetics/Bulk/Ca_Spine /kinetics/Bulk/activate_PKA SUBSTRATE n 
addmsg /kinetics/Bulk/PKA /kinetics/Bulk/activate_PKA PRODUCT n 
addmsg /kinetics/Bulk/Ca_Spine /kinetics/Bulk/activate_PKA SUBSTRATE n 
addmsg /kinetics/Bulk/activate_PKA /kinetics/Bulk/inact_PKA REAC A B 
addmsg /kinetics/Bulk/activate_CaN /kinetics/Bulk/inact_CaN REAC A B 
addmsg /kinetics/Bulk/inact_CaN /kinetics/Bulk/activate_CaN SUBSTRATE n 
addmsg /kinetics/Bulk/iP /kinetics/Bulk/activate_CaN PRODUCT n 
addmsg /kinetics/Bulk/Ca_Spine /kinetics/Bulk/activate_CaN SUBSTRATE n 
addmsg /kinetics/Bulk/Ca_Spine /kinetics/Bulk/activate_CaN SUBSTRATE n 
addmsg /kinetics/Bulk/activate_PKA /kinetics/Bulk/Ca_Spine REAC A B 
addmsg /kinetics/Bulk/activate_PKA /kinetics/Bulk/Ca_Spine REAC A B 
addmsg /kinetics/Bulk/activate_CaN /kinetics/Bulk/Ca_Spine REAC A B 
addmsg /kinetics/Bulk/activate_CaN /kinetics/Bulk/Ca_Spine REAC A B 
addmsg /kinetics/Ca_to_bulk /kinetics/Bulk/Ca_Spine REAC A B 
addmsg /kinetics/Bulk/Ca_to_dend /kinetics/Bulk/Ca_Spine REAC A B 
addmsg /kinetics/Bulk/Ca_Spine /kinetics/Bulk/Ca_to_dend SUBSTRATE n 
addmsg /kinetics/Ca_dend /kinetics/Bulk/Ca_to_dend PRODUCT n 
addmsg /kinetics/Bulk/Ca_Spine /kinetics/Ca_to_bulk SUBSTRATE n 
addmsg /kinetics/PSD/Ca_PSD /kinetics/Ca_to_bulk PRODUCT n 
addmsg /kinetics/dend_Ca_reac /kinetics/Ca_dend REAC B A 
addmsg /kinetics/Bulk/Ca_to_dend /kinetics/Ca_dend REAC B A 
addmsg /kinetics/dend_Ca_reac /kinetics/Ca_dend_input REAC A B 
addmsg /kinetics/Ca_dend_input /kinetics/dend_Ca_reac SUBSTRATE n 
addmsg /kinetics/Ca_dend /kinetics/dend_Ca_reac PRODUCT n 
addmsg /kinetics/PSD/Ca_PSD /graphs/conc1/Ca_PSD.Co PLOT Co *Ca_PSD.Co *53 
addmsg /kinetics/Bulk/Ca_Spine /graphs/conc1/Ca_Bulk.Co PLOT Co *Ca_Bulk.Co *41 
addmsg /kinetics/Bulk/PKA /graphs/conc1/PKA.Co PLOT Co *PKA.Co *43 
addmsg /kinetics/PSD/Ca.P /graphs/conc1/Ca.P.Co PLOT Co *Ca.P.Co *blue 
addmsg /kinetics/Ca_dend /graphs/conc1/Ca_dend.Co PLOT Co *Ca_dend.Co *1 
addmsg /kinetics/PSD/tot_PSD_R /graphs/conc2/tot_PSD_R.Co PLOT Co *tot_PSD_R.Co *blue 
addmsg /kinetics/PSD/Rpp /graphs/conc2/Rpp.Co PLOT Co *Rpp.Co *28 
enddump
// End of dump

call /kinetics/Bulk/iR/notes LOAD \
"Same as Fus3" \
""
call /kinetics/Bulk/iKK/notes LOAD \
"Same as Ste7" \
""
call /kinetics/Bulk/iP/notes LOAD \
"Same as Msg5" \
""
complete_loading
