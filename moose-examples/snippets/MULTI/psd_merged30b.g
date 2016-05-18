//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Mon Aug 12 22:34:03 2013
 
include kkit {argv 1}
 
FASTDT = 0.0001
SIMDT = 0.001
CONTROLDT = 10
PLOTDT = 10
MAXTIME = 2000
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
simundump geometry /kinetics/geometry[1] 0 9e-20 3 sphere "" white black 0 0 \
  0
simundump geometry /kinetics/geometry[2] 0 1e-20 3 sphere "" white black 0 0 \
  0
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
simundump group /kinetics/PSD 0 20 black x 0 0 "" PSD defaultfile.g 0 0 0 27 \
  12 0
simundump text /kinetics/PSD/notes 0 ""
call /kinetics/PSD/notes LOAD \
""
simundump kpool /kinetics/PSD/tot_PSD_R 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry blue 0 -4 7 0
simundump text /kinetics/PSD/tot_PSD_R/notes 0 ""
call /kinetics/PSD/tot_PSD_R/notes LOAD \
""
simundump kpool /kinetics/PSD/actCaMKII 0 0 2 2 12 12 0 0 6 0 \
  /kinetics/geometry 12 20 70 33 0
simundump text /kinetics/PSD/actCaMKII/notes 0 ""
call /kinetics/PSD/actCaMKII/notes LOAD \
""
simundump kenz /kinetics/PSD/actCaMKII/CaMKII_1 0 0 0 0 0 6 0.033332 8 2 0 0 \
  "" red 35 "" -6 3 0
simundump text /kinetics/PSD/actCaMKII/CaMKII_1/notes 0 ""
call /kinetics/PSD/actCaMKII/CaMKII_1/notes LOAD \
""
simundump kenz /kinetics/PSD/actCaMKII/CaMKII_2 0 0 0 0 0 6 0.033332 8 2 0 0 \
  "" red 35 "" -2 3 0
simundump text /kinetics/PSD/actCaMKII/CaMKII_2/notes 0 ""
call /kinetics/PSD/actCaMKII/CaMKII_2/notes LOAD \
""
simundump group /kinetics/PSD/PP1_PSD 1 yellow 20 x 0 0 "" PP1_PSD \
  /home2/bhalla/scripts/modules/PP1_PSD_0.g 0 \
  62a600ae10e53f567e47decb4f2b6488 0 12 27 0
simundump text /kinetics/PSD/PP1_PSD/notes 0 ""
call /kinetics/PSD/PP1_PSD/notes LOAD \
""
simundump kpool /kinetics/PSD/PP1_PSD/PP1-active 1 0 4 4 24 24 0 0 6 0 \
  /kinetics/geometry cyan yellow 44 25 0
simundump text /kinetics/PSD/PP1_PSD/PP1-active/notes 0 \
  "Cohen et al Meth Enz 159 390-408 is main source of info\nconc  = 1.8 uM"
call /kinetics/PSD/PP1_PSD/PP1-active/notes LOAD \
"Cohen et al Meth Enz 159 390-408 is main source of info" \
"conc  = 1.8 uM"
simundump kenz /kinetics/PSD/PP1_PSD/PP1-active/PP1_2 0 0 0 0 0 6 0.0010416 \
  0.2 0.05 0 0 "" red blue "" -2 -4 0
simundump text /kinetics/PSD/PP1_PSD/PP1-active/PP1_2/notes 0 ""
call /kinetics/PSD/PP1_PSD/PP1-active/PP1_2/notes LOAD \
""
simundump kenz /kinetics/PSD/PP1_PSD/PP1-active/PP1_1 0 0 0 0 0 6 0.0010416 \
  0.2 0.05 0 0 "" red blue "" -6 -4 0
simundump text /kinetics/PSD/PP1_PSD/PP1-active/PP1_1/notes 0 ""
call /kinetics/PSD/PP1_PSD/PP1-active/PP1_1/notes LOAD \
""
simundump kenz /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286 0 0 0 0 0 54.001 \
  0.083333 2 0.5 0 0 "" red cyan "" 61 25 0
simundump text /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286/notes 0 ""
call /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286/notes LOAD \
""
simundump kenz /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286b 0 0 0 0 0 54.001 \
  0.083333 2 0.5 0 0 "" red cyan "" 71 25 0
simundump text /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286b/notes 0 ""
call /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286b/notes LOAD \
""
simundump kenz /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305 0 0 0 0 0 54.001 \
  0.083333 2 0.5 0 0 "" red cyan "" 78 25 0
simundump text /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305/notes 0 ""
call /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305/notes LOAD \
""
simundump kenz /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286c 0 0 0 0 0 54.001 \
  0.020833 0.5 0.125 0 0 "" red cyan "" 88 25 0
simundump text /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286c/notes 0 ""
call /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286c/notes LOAD \
""
simundump kenz /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305a 0 0 0 0 0 54.001 \
  0.083333 2 0.5 0 0 "" red cyan "" 83 25 0
simundump text /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305a/notes 0 ""
call /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305a/notes LOAD \
""
simundump kpool /kinetics/PSD/PP1_PSD/I1 1 0 4 4 24 24 0 0 6 0 \
  /kinetics/geometry orange yellow 35.35 21.88 0
simundump text /kinetics/PSD/PP1_PSD/I1/notes 0 \
  "I1 is a 'mixed' inhibitor, but at high enz concs it looks like a non-compet\ninhibitor (Foulkes et al Eur J Biochem 132 309-313 9183).\nWe treat it as non-compet, so it just turns the enz off\nwithout interacting with the binding site.\nCohen et al ann rev bioch refer to results where conc is \n1.5 to 1.8 uM. In order to get complete inhib of PP1, which is at 1.8 uM,\nwe need >= 1.8 uM.\n\n"
call /kinetics/PSD/PP1_PSD/I1/notes LOAD \
"I1 is a 'mixed' inhibitor, but at high enz concs it looks like a non-compet" \
"inhibitor (Foulkes et al Eur J Biochem 132 309-313 9183)." \
"We treat it as non-compet, so it just turns the enz off" \
"without interacting with the binding site." \
"Cohen et al ann rev bioch refer to results where conc is " \
"1.5 to 1.8 uM. In order to get complete inhib of PP1, which is at 1.8 uM," \
"we need >= 1.8 uM." \
"" \
""
simundump kpool /kinetics/PSD/PP1_PSD/PP1-I1 1 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry brown yellow 36 29 0
simundump text /kinetics/PSD/PP1_PSD/PP1-I1/notes 0 ""
call /kinetics/PSD/PP1_PSD/PP1-I1/notes LOAD \
""
simundump kreac /kinetics/PSD/PP1_PSD/dissoc-PP1-I1 1 1 0 "" white yellow 33 \
  26 0
simundump text /kinetics/PSD/PP1_PSD/dissoc-PP1-I1/notes 0 \
  "Let us assume that the equil in this case is very far over to the\nright. This is probably safe.\n"
call /kinetics/PSD/PP1_PSD/dissoc-PP1-I1/notes LOAD \
"Let us assume that the equil in this case is very far over to the" \
"right. This is probably safe." \
""
simundump kreac /kinetics/PSD/PP1_PSD/Inact-PP1 1 83.33 0.1 "" white yellow \
  17 26 0
simundump text /kinetics/PSD/PP1_PSD/Inact-PP1/notes 0 \
  "K inhib = 1nM from Cohen Ann Rev Bioch 1989, \n4 nM from Foukes et al \nAssume 2 nM. kf /kb = 8.333e-4"
call /kinetics/PSD/PP1_PSD/Inact-PP1/notes LOAD \
"K inhib = 1nM from Cohen Ann Rev Bioch 1989, " \
"4 nM from Foukes et al " \
"Assume 2 nM. kf /kb = 8.333e-4"
simundump kpool /kinetics/PSD/PP1_PSD/PP1-I1_p 1 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry brown yellow 22 29 0
simundump text /kinetics/PSD/PP1_PSD/PP1-I1_p/notes 0 ""
call /kinetics/PSD/PP1_PSD/PP1-I1_p/notes LOAD \
""
simundump kpool /kinetics/PSD/PP1_PSD/I1_p 1 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry orange yellow 21 22 0
simundump text /kinetics/PSD/PP1_PSD/I1_p/notes 0 \
  "Dephosph is mainly by PP2B"
call /kinetics/PSD/PP1_PSD/I1_p/notes LOAD \
"Dephosph is mainly by PP2B"
simundump kpool /kinetics/PSD/PP1_PSD/CaN 0 1e-12 1 1 6 6 0 0 6 0 \
  /kinetics/geometry 1 yellow -1 33 0
simundump text /kinetics/PSD/PP1_PSD/CaN/notes 0 ""
call /kinetics/PSD/PP1_PSD/CaN/notes LOAD \
""
simundump kreac /kinetics/PSD/PP1_PSD/CaM-Bind-CaN 0 7.6667 0.002 "" white \
  yellow 21 40 0
simundump text /kinetics/PSD/PP1_PSD/CaM-Bind-CaN/notes 0 ""
call /kinetics/PSD/PP1_PSD/CaM-Bind-CaN/notes LOAD \
""
simundump kpool /kinetics/PSD/PP1_PSD/Ca2_CaN 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry[2] blue yellow 12 33 0
simundump text /kinetics/PSD/PP1_PSD/Ca2_CaN/notes 0 ""
call /kinetics/PSD/PP1_PSD/Ca2_CaN/notes LOAD \
""
simundump kreac /kinetics/PSD/PP1_PSD/Ca_bind_CaN 0 0.11111 1 "" white yellow \
  7 40 0
simundump text /kinetics/PSD/PP1_PSD/Ca_bind_CaN/notes 0 ""
call /kinetics/PSD/PP1_PSD/Ca_bind_CaN/notes LOAD \
""
simundump kreac /kinetics/PSD/move_to_PSD 0 0.1 3 "" white 0 -8 -5 0
simundump text /kinetics/PSD/move_to_PSD/notes 0 ""
call /kinetics/PSD/move_to_PSD/notes LOAD \
""
simundump kpool /kinetics/PSD/R_S2 0 0 0 0 0 0 0 0 6 0 /kinetics/geometry 4 0 \
  -8 1 0
simundump text /kinetics/PSD/R_S2/notes 0 ""
call /kinetics/PSD/R_S2/notes LOAD \
""
simundump kpool /kinetics/PSD/R_SpS 0 0 0 0 0 0 0 0 6 0 /kinetics/geometry \
  blue 0 -4 1 0
simundump text /kinetics/PSD/R_SpS/notes 0 ""
call /kinetics/PSD/R_SpS/notes LOAD \
""
simundump kpool /kinetics/PSD/R_SpSp 0 0 0 0 0 0 0 0 6 0 /kinetics/geometry \
  28 0 0 1 0
simundump text /kinetics/PSD/R_SpSp/notes 0 ""
call /kinetics/PSD/R_SpSp/notes LOAD \
""
simundump kpool /kinetics/PSD/basal_phosphatase 0 0 1 1 6 6 0 0 6 0 \
  /kinetics/geometry 61 0 -4 -2 0
simundump text /kinetics/PSD/basal_phosphatase/notes 0 \
  "There isn't any clear info for this. I had originally called \nit PP2A, but that causes odd interactions with other pathways.\n"
call /kinetics/PSD/basal_phosphatase/notes LOAD \
"There isn't any clear info for this. I had originally called " \
"it PP2A, but that causes odd interactions with other pathways." \
""
simundump kenz /kinetics/PSD/basal_phosphatase/P1 0 0 0 0 0 60 0.46295 \
  0.44443 0.11111 0 0 "" red 61 "" -6 -1 0
simundump text /kinetics/PSD/basal_phosphatase/P1/notes 0 ""
call /kinetics/PSD/basal_phosphatase/P1/notes LOAD \
""
simundump kenz /kinetics/PSD/basal_phosphatase/P2 0 0 0 0 0 60 0.46295 \
  0.44443 0.11111 0 0 "" red 61 "" -2 -1 0
simundump text /kinetics/PSD/basal_phosphatase/P2/notes 0 ""
call /kinetics/PSD/basal_phosphatase/P2/notes LOAD \
""
simundump kpool /kinetics/PSD/PKA-active 1 0 0.02 0.02 0.12 0.12 0 0 6 0 \
  /kinetics/geometry yellow 20 14 17 0
simundump text /kinetics/PSD/PKA-active/notes 0 ""
call /kinetics/PSD/PKA-active/notes LOAD \
""
simundump kenz /kinetics/PSD/PKA-active/PKA-phosph-I1 0 0 0 0 0 54.001 \
  0.11111 36 9 0 0 "" red 27 "" 27 19 0
simundump text /kinetics/PSD/PKA-active/PKA-phosph-I1/notes 0 ""
call /kinetics/PSD/PKA-active/PKA-phosph-I1/notes LOAD \
""
simundump group /kinetics/PSD/CaMKII_PSD 0 33 20 x 0 0 "" CaMKII_PSD \
  defaultfile.g 0 0 0 39 32 0
simundump text /kinetics/PSD/CaMKII_PSD/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/NMDAR 0 0 40 40 240 240 0 0 6 0 \
  /kinetics/geometry blue 33 67 8 0
simundump text /kinetics/PSD/CaMKII_PSD/NMDAR/notes 0 \
  "The stochiometry is a bit off here. Each NMDAR actually\nbinds to a holoenzyme, about 12 CaMKII subunits. But\nour CaMKII calculations are in terms of individual\nsubunits. So as a hack, we put in much more NMDAR than\nis actually there.\n\nDec 2011: Reconsidered this, now only 120 NMDARs.\n\nJune 02 2012. Consider these as anchor points for a dodecamer.\nThere are far more effected CaMKII bindin...."
call /kinetics/PSD/CaMKII_PSD/NMDAR/notes LOAD \
"The stochiometry is a bit off here. Each NMDAR actually" \
"binds to a holoenzyme, about 12 CaMKII subunits. But" \
"our CaMKII calculations are in terms of individual" \
"subunits. So as a hack, we put in much more NMDAR than" \
"is actually there." \
"" \
"Dec 2011: Reconsidered this, now only 120 NMDARs." \
"" \
"June 02 2012. Consider these as anchor points for a dodecamer." \
"There are far more effected CaMKII binding sites then, than" \
"individual molecules of NMDAR. Raised limit to 40."
simundump kpool /kinetics/PSD/CaMKII_PSD/tot-auto 0 0 2 1 6 12 0 0 6 0 \
  /kinetics/geometry 29 33 74 36 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-auto/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/tot-auto/notes LOAD \
""
simundump kenz /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph 0 0 0 0 0 7.732 \
  0.0033333 8 2 0 0 "" red 29 "" 72 23 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph/notes LOAD \
""
simundump kenz /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286 0 0 0 0 0 7.732 \
  0.00083333 2 0.5 0 0 "" red 29 "" 59 23 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286/notes LOAD \
""
simundump kenz /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305 0 0 0 0 0 54.001 \
  0.01 24 6 0 0 "" red 29 "" 78 23 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/tot_CaMKII 0 0 2 2 12 12 0 0 6 0 \
  /kinetics/geometry 23 33 84 36 0
simundump text /kinetics/PSD/CaMKII_PSD/tot_CaMKII/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/tot_CaMKII/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/286P 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry 59 33 52 33 0
simundump text /kinetics/PSD/CaMKII_PSD/286P/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/286P/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry 47 33 61 33 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/notes LOAD \
""
simundump kenz /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph 0 0 0 0 \
  0 7.732 0.0052083 8 2 0 0 "" red 47 "" 72 20 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph/notes 0 \
  ""
call /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph/notes LOAD \
""
simundump kenz /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305 0 0 0 0 0 \
  54.001 0.015625 24 6 0 0 "" red 47 "" 78 20 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305/notes LOAD \
""
simundump kenz /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286 0 0 0 0 0 \
  7.732 0.0013021 2 0.5 0 0 "" red 47 "" 60 20 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/basal_CaMKII 0 0 1 1 6 6 0 0 6 0 \
  /kinetics/geometry blue 33 81 33 0
simundump text /kinetics/PSD/CaMKII_PSD/basal_CaMKII/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/basal_CaMKII/notes LOAD \
""
simundump kreac /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM 0 10.945 \
  0.0022 "" white 33 54 28 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM/notes 0 \
  "Same values as for the main compartment\nCan the main compartment pool of Ca/CaM be used?"
call /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM/notes LOAD \
"Same values as for the main compartment" \
"Can the main compartment pool of Ca/CaM be used?"
simundump kreac /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM 0 10.945 2.2 "" \
  white 33 65 28 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM 0 0 0 0 0 0 0 0 6 \
  0 /kinetics/geometry 47 33 56 22 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/CaMKII-CaM 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry 50 33 63 22 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII-CaM/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/CaMKII-CaM/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry 57 33 69 22 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII-thr286/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/CaMKII-thr286/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/CaMKII 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry 62 33 75 22 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/CaMKII/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/CaMKII-thr305 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry 30 33 88 22 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII-thr305/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/CaMKII-thr305/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/CaMKII_p_p_p 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry 0 33 81 22 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII_p_p_p/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/CaMKII_p_p_p/notes LOAD \
""
simundump kreac /kinetics/PSD/CaMKII_PSD/transloc_1 0 0.0001 0 "" white 33 63 \
  12 0
simundump text /kinetics/PSD/CaMKII_PSD/transloc_1/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/transloc_1/notes LOAD \
""
simundump kreac /kinetics/PSD/CaMKII_PSD/back_1 0 0.04 6.6667e-06 "" white 33 \
  75 12 0
simundump text /kinetics/PSD/CaMKII_PSD/back_1/notes 0 \
  "Rates set by the translocation experiments of \nShen and Meyer, Science 1999."
call /kinetics/PSD/CaMKII_PSD/back_1/notes LOAD \
"Rates set by the translocation experiments of " \
"Shen and Meyer, Science 1999."
simundump kreac /kinetics/PSD/CaMKII_PSD/transloc_2 0 0.0001 0 "" white 33 56 \
  12 0
simundump text /kinetics/PSD/CaMKII_PSD/transloc_2/notes 0 \
  "Same as for transloc_1\n\n\n"
call /kinetics/PSD/CaMKII_PSD/transloc_2/notes LOAD \
"Same as for transloc_1" \
"" \
"" \
""
simundump kreac /kinetics/PSD/CaMKII_PSD/back_2 0 0.04 6.6667e-06 "" white 33 \
  89 13 0
simundump text /kinetics/PSD/CaMKII_PSD/back_2/notes 0 "Same as for back_1\n"
call /kinetics/PSD/CaMKII_PSD/back_2/notes LOAD \
"Same as for back_1" \
""
simundump kpool /kinetics/PSD/PP2A 1 0 1 1 6 6 0 0 6 0 /kinetics/geometry red \
  20 45 17 0
simundump text /kinetics/PSD/PP2A/notes 0 ""
call /kinetics/PSD/PP2A/notes LOAD \
""
simundump kenz /kinetics/PSD/PP2A/PP2A-dephosph-PP1-I_p 1 0.0012791 0.0012791 \
  0.06907 0.06907 54.001 0.0013289 0.92593 0.22222 0 0 "" red red "" 29 27 0
simundump text /kinetics/PSD/PP2A/PP2A-dephosph-PP1-I_p/notes 0 \
  "k1 changed from 3.3e-6 to 6.6e-6\n"
call /kinetics/PSD/PP2A/PP2A-dephosph-PP1-I_p/notes LOAD \
"k1 changed from 3.3e-6 to 6.6e-6" \
""
simundump kenz /kinetics/PSD/PP2A/PP2A-dephosph-I1 1 2.2155e-07 2.2155e-07 \
  1.1964e-05 1.1964e-05 54.001 0.0013289 0.92593 0.22222 0 0 "" red red "" 27 \
  20 0
simundump text /kinetics/PSD/PP2A/PP2A-dephosph-I1/notes 0 \
  "PP2A does most of the dephosph of I1 at basal Ca levels. See\nthe review by Cohen in Ann Rev Biochem 1989.\nFor now, lets halve Km. k1 was 3.3e-6, now 6.6e-6\n"
call /kinetics/PSD/PP2A/PP2A-dephosph-I1/notes LOAD \
"PP2A does most of the dephosph of I1 at basal Ca levels. See" \
"the review by Cohen in Ann Rev Biochem 1989." \
"For now, lets halve Km. k1 was 3.3e-6, now 6.6e-6" \
""
simundump group /kinetics/PSD/CaM 1 blue 20 x 0 0 "" defaultfile \
  defaultfile.g 0 fbb0ff81553508bc01f3dd51428742fb 0 31 37 0
simundump text /kinetics/PSD/CaM/notes 0 ""
call /kinetics/PSD/CaM/notes LOAD \
""
simundump kpool /kinetics/PSD/CaM/CaM 1 1e-12 20 20 120 120 0 0 6 0 \
  /kinetics/geometry pink blue 35 39 0
simundump text /kinetics/PSD/CaM/CaM/notes 0 \
  "There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)\nSay 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying\nit is comparable to CaMK levels. \n"
call /kinetics/PSD/CaM/CaM/notes LOAD \
"There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)" \
"Say 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying" \
"it is comparable to CaMK levels. " \
""
simundump kreac /kinetics/PSD/CaM/CaM-Ca3-bind-Ca 1 0.077501 10 "" white blue \
  49 36 0
simundump text /kinetics/PSD/CaM/CaM-Ca3-bind-Ca/notes 0 \
  "Use K3 = 21.5 uM here from Stemmer and Klee table 3.\nkb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10"
call /kinetics/PSD/CaM/CaM-Ca3-bind-Ca/notes LOAD \
"Use K3 = 21.5 uM here from Stemmer and Klee table 3." \
"kb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10"
simundump kpool /kinetics/PSD/CaM/CaM-Ca3 1 1e-12 0 0 0 0 0 0 6 0 \
  /kinetics/geometry hotpink blue 47 39 0
simundump text /kinetics/PSD/CaM/CaM-Ca3/notes 0 ""
call /kinetics/PSD/CaM/CaM-Ca3/notes LOAD \
""
simundump kreac /kinetics/PSD/CaM/CaM-bind-Ca 1 1.4141 8.4853 "" white blue \
  37 36 0
simundump text /kinetics/PSD/CaM/CaM-bind-Ca/notes 0 \
  "Lets use the fast rate consts here. Since the rates are so different, I am not\nsure whether the order is relevant. These correspond to the TR2C fragment.\nWe use the Martin et al rates here, plus the Drabicowski binding consts.\nAll are scaled by 3X to cell temp.\nkf = 2e-10 kb = 72\nStemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11.\nIf kb=72, kf = 2e-10 (Exactly the same !)...."
call /kinetics/PSD/CaM/CaM-bind-Ca/notes LOAD \
"Lets use the fast rate consts here. Since the rates are so different, I am not" \
"sure whether the order is relevant. These correspond to the TR2C fragment." \
"We use the Martin et al rates here, plus the Drabicowski binding consts." \
"All are scaled by 3X to cell temp." \
"kf = 2e-10 kb = 72" \
"Stemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11." \
"If kb=72, kf = 2e-10 (Exactly the same !)" \
"" \
"19 May 2006. Splitting the old CaM-TR2-bind-Ca reaction into" \
"two steps, each binding 1 Ca. This improves numerical" \
"stability and is conceptually better too." \
"" \
"Overall rates are the same, so each kf and kb is the " \
"square root of the earlier ones. So" \
"kf = 1.125e-4, kb = 8.4853" \
""
simundump kpool /kinetics/PSD/CaM/CaM-Ca2 1 1e-12 0 0 0 0 0 0 6 0 \
  /kinetics/geometry pink blue 43 39 0
simundump text /kinetics/PSD/CaM/CaM-Ca2/notes 0 \
  "This is the intermediate where the TR2 end (the high-affinity end) has\nbound the Ca but the TR1 end has not."
call /kinetics/PSD/CaM/CaM-Ca2/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
simundump kreac /kinetics/PSD/CaM/CaM-Ca2-bind-Ca 1 0.60001 10 "" white blue \
  45 36 0
simundump text /kinetics/PSD/CaM/CaM-Ca2-bind-Ca/notes 0 \
  "K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get\nkb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10\n"
call /kinetics/PSD/CaM/CaM-Ca2-bind-Ca/notes LOAD \
"K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get" \
"kb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10" \
""
simundump kreac /kinetics/PSD/CaM/CaM-Ca-bind-Ca 1 1.4141 8.4853 "" white \
  blue 41 36 0
simundump text /kinetics/PSD/CaM/CaM-Ca-bind-Ca/notes 0 \
  "Lets use the fast rate consts here. Since the rates are so different, I am not\nsure whether the order is relevant. These correspond to the TR2C fragment.\nWe use the Martin et al rates here, plus the Drabicowski binding consts.\nAll are scaled by 3X to cell temp.\nkf = 2e-10 kb = 72\nStemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11.\nIf kb=72, kf = 2e-10 (Exactly the same !)...."
call /kinetics/PSD/CaM/CaM-Ca-bind-Ca/notes LOAD \
"Lets use the fast rate consts here. Since the rates are so different, I am not" \
"sure whether the order is relevant. These correspond to the TR2C fragment." \
"We use the Martin et al rates here, plus the Drabicowski binding consts." \
"All are scaled by 3X to cell temp." \
"kf = 2e-10 kb = 72" \
"Stemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11." \
"If kb=72, kf = 2e-10 (Exactly the same !)" \
"" \
"19 May 2006. Splitting the old CaM-TR2-bind-Ca reaction into" \
"two steps, each binding 1 Ca. This improves numerical" \
"stability and is conceptually better too." \
"" \
"Overall rates are the same, so each kf and kb is the " \
"square root of the earlier ones. So" \
"kf = 1.125e-4, kb = 8.4853" \
""
simundump kpool /kinetics/PSD/CaM/CaM-Ca 1 1e-12 0 0 0 0 0 0 6 0 \
  /kinetics/geometry pink blue 39 39 0
simundump text /kinetics/PSD/CaM/CaM-Ca/notes 0 \
  "This is the intermediate where the TR2 end (the high-affinity end) has\nbound the Ca but the TR1 end has not."
call /kinetics/PSD/CaM/CaM-Ca/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
simundump kpool /kinetics/PSD/CaM/Ca 1 1e-11 0.08 0.08 0.48 0.48 0 0 6 0 \
  /kinetics/geometry red blue 43 33 0
simundump text /kinetics/PSD/CaM/Ca/notes 0 ""
call /kinetics/PSD/CaM/Ca/notes LOAD \
""
simundump kpool /kinetics/PSD/CaM/CaM-Ca4 1 1e-12 0 0 0 0 0 0 6 0 \
  /kinetics/geometry blue blue 52 39 0
simundump text /kinetics/PSD/CaM/CaM-Ca4/notes 0 ""
call /kinetics/PSD/CaM/CaM-Ca4/notes LOAD \
""
simundump kpool /kinetics/PSD/CaM_CaN 1 1e-12 0 0 0 0 0 0 6 0 \
  /kinetics/geometry 1 20 30 33 0
simundump text /kinetics/PSD/CaM_CaN/notes 0 ""
call /kinetics/PSD/CaM_CaN/notes LOAD \
""
simundump kenz /kinetics/PSD/CaM_CaN/dephosph-PP1-I_p 1 0 0 0 0 54.001 \
  0.0063334 1.36 0.34 0 0 "" white 1 "" 29 30 0
simundump text /kinetics/PSD/CaM_CaN/dephosph-PP1-I_p/notes 0 ""
call /kinetics/PSD/CaM_CaN/dephosph-PP1-I_p/notes LOAD \
""
simundump kenz /kinetics/PSD/CaM_CaN/dephosph_inhib1 1 0 0 0 0 54.001 \
  0.0063334 1.36 0.34 0 0 "" red 1 "" 29 24 0
simundump text /kinetics/PSD/CaM_CaN/dephosph_inhib1/notes 0 ""
call /kinetics/PSD/CaM_CaN/dephosph_inhib1/notes LOAD \
""
simundump group /kinetics/DEND 0 13 black x 0 0 "" DEND defaultfile.g 0 0 0 8 \
  -30 0
simundump text /kinetics/DEND/notes 0 ""
call /kinetics/DEND/notes LOAD \
""
simundump kpool /kinetics/DEND/Ca 0 1e-11 0.08 0.08 48 48 0 0 600 0 \
  /kinetics/geometry[3] 62 13 13 -27 0
simundump text /kinetics/DEND/Ca/notes 0 ""
call /kinetics/DEND/Ca/notes LOAD \
""
simundump group /kinetics/SPINE 0 5 black x 0 0 "" SPINE defaultfile.g 0 0 0 \
  29 -23 0
simundump text /kinetics/SPINE/notes 0 ""
call /kinetics/SPINE/notes LOAD \
""
simundump kpool /kinetics/SPINE/iR 0 0 4.4444 4.4444 240 240 0 0 54 0 \
  /kinetics/geometry 0 yellow -8 -10 0
simundump text /kinetics/SPINE/iR/notes 0 "Same as Fus3\n"
call /kinetics/SPINE/iR/notes LOAD \
"Same as Fus3" \
""
simundump group /kinetics/SPINE/CaM 1 blue 5 x 0 0 "" defaultfile \
  defaultfile.g 0 fbb0ff81553508bc01f3dd51428742fb 0 33 2 0
simundump text /kinetics/SPINE/CaM/notes 0 ""
call /kinetics/SPINE/CaM/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaM/CaM 1 1e-12 20 20 1080 1080 0 0 54 0 \
  /kinetics/geometry pink blue 37 4 0
simundump text /kinetics/SPINE/CaM/CaM/notes 0 \
  "There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)\nSay 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying\nit is comparable to CaMK levels. \n"
call /kinetics/SPINE/CaM/CaM/notes LOAD \
"There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)" \
"Say 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying" \
"it is comparable to CaMK levels. " \
""
simundump kreac /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca 1 0.0086112 10 "" white \
  blue 51 1 0
simundump text /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca/notes 0 \
  "Use K3 = 21.5 uM here from Stemmer and Klee table 3.\nkb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10"
call /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca/notes LOAD \
"Use K3 = 21.5 uM here from Stemmer and Klee table 3." \
"kb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10"
simundump kpool /kinetics/SPINE/CaM/CaM-Ca3 1 1e-12 0 0 0 0 0 0 54 0 \
  /kinetics/geometry hotpink blue 49 4 0
simundump text /kinetics/SPINE/CaM/CaM-Ca3/notes 0 ""
call /kinetics/SPINE/CaM/CaM-Ca3/notes LOAD \
""
simundump kreac /kinetics/SPINE/CaM/CaM-bind-Ca 1 0.15712 8.4853 "" white \
  blue 39 1 0
simundump text /kinetics/SPINE/CaM/CaM-bind-Ca/notes 0 \
  "Lets use the fast rate consts here. Since the rates are so different, I am not\nsure whether the order is relevant. These correspond to the TR2C fragment.\nWe use the Martin et al rates here, plus the Drabicowski binding consts.\nAll are scaled by 3X to cell temp.\nkf = 2e-10 kb = 72\nStemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11.\nIf kb=72, kf = 2e-10 (Exactly the same !)...."
call /kinetics/SPINE/CaM/CaM-bind-Ca/notes LOAD \
"Lets use the fast rate consts here. Since the rates are so different, I am not" \
"sure whether the order is relevant. These correspond to the TR2C fragment." \
"We use the Martin et al rates here, plus the Drabicowski binding consts." \
"All are scaled by 3X to cell temp." \
"kf = 2e-10 kb = 72" \
"Stemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11." \
"If kb=72, kf = 2e-10 (Exactly the same !)" \
"" \
"19 May 2006. Splitting the old CaM-TR2-bind-Ca reaction into" \
"two steps, each binding 1 Ca. This improves numerical" \
"stability and is conceptually better too." \
"" \
"Overall rates are the same, so each kf and kb is the " \
"square root of the earlier ones. So" \
"kf = 1.125e-4, kb = 8.4853" \
""
simundump kpool /kinetics/SPINE/CaM/CaM-Ca2 1 1e-12 0 0 0 0 0 0 54 0 \
  /kinetics/geometry pink blue 45 4 0
simundump text /kinetics/SPINE/CaM/CaM-Ca2/notes 0 \
  "This is the intermediate where the TR2 end (the high-affinity end) has\nbound the Ca but the TR1 end has not."
call /kinetics/SPINE/CaM/CaM-Ca2/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
simundump kreac /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca 1 0.066668 10 "" white \
  blue 47 1 0
simundump text /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca/notes 0 \
  "K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get\nkb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10\n"
call /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca/notes LOAD \
"K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get" \
"kb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10" \
""
simundump kreac /kinetics/SPINE/CaM/CaM-Ca-bind-Ca 1 0.15712 8.4853 "" white \
  blue 43 1 0
simundump text /kinetics/SPINE/CaM/CaM-Ca-bind-Ca/notes 0 \
  "Lets use the fast rate consts here. Since the rates are so different, I am not\nsure whether the order is relevant. These correspond to the TR2C fragment.\nWe use the Martin et al rates here, plus the Drabicowski binding consts.\nAll are scaled by 3X to cell temp.\nkf = 2e-10 kb = 72\nStemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11.\nIf kb=72, kf = 2e-10 (Exactly the same !)...."
call /kinetics/SPINE/CaM/CaM-Ca-bind-Ca/notes LOAD \
"Lets use the fast rate consts here. Since the rates are so different, I am not" \
"sure whether the order is relevant. These correspond to the TR2C fragment." \
"We use the Martin et al rates here, plus the Drabicowski binding consts." \
"All are scaled by 3X to cell temp." \
"kf = 2e-10 kb = 72" \
"Stemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11." \
"If kb=72, kf = 2e-10 (Exactly the same !)" \
"" \
"19 May 2006. Splitting the old CaM-TR2-bind-Ca reaction into" \
"two steps, each binding 1 Ca. This improves numerical" \
"stability and is conceptually better too." \
"" \
"Overall rates are the same, so each kf and kb is the " \
"square root of the earlier ones. So" \
"kf = 1.125e-4, kb = 8.4853" \
""
simundump kpool /kinetics/SPINE/CaM/CaM-Ca 1 1e-12 0 0 0 0 0 0 54 0 \
  /kinetics/geometry pink blue 41 4 0
simundump text /kinetics/SPINE/CaM/CaM-Ca/notes 0 \
  "This is the intermediate where the TR2 end (the high-affinity end) has\nbound the Ca but the TR1 end has not."
call /kinetics/SPINE/CaM/CaM-Ca/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
simundump kpool /kinetics/SPINE/CaM/Ca 1 1e-11 0.08 0.08 4.32 4.32 0 0 54 0 \
  /kinetics/geometry red blue 45 -2 0
simundump text /kinetics/SPINE/CaM/Ca/notes 0 ""
call /kinetics/SPINE/CaM/Ca/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaM/CaM-Ca4 1 1e-12 0 0 0 0 0 0 54 0 \
  /kinetics/geometry blue blue 54 4 0
simundump text /kinetics/SPINE/CaM/CaM-Ca4/notes 0 ""
call /kinetics/SPINE/CaM/CaM-Ca4/notes LOAD \
""
simundump group /kinetics/SPINE/CaMKII_BULK 0 33 5 x 0 0 "" CaMKII_BULK \
  defaultfile.g 0 0 0 41 -3 0
simundump text /kinetics/SPINE/CaMKII_BULK/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/tot-auto 0 0 2 1 54 108 0 0 54 0 \
  /kinetics/geometry 29 33 76 1 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-auto/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/tot-auto/notes LOAD \
""
simundump kenz /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-autoph 0 0 0 0 0 \
  69.588 0.00037037 8 2 0 0 "" red 29 "" 74 -12 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-autoph/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-autoph/notes LOAD \
""
simundump kenz /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-286 0 0 0 0 0 \
  69.588 9.2592e-05 2 0.5 0 0 "" red 29 "" 61 -12 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-286/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-286/notes LOAD \
""
simundump kenz /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-305 0 0 0 0 0 \
  486.01 0.0011111 24 6 0 0 "" red 29 "" 80 -12 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-305/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-305/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/tot_CaMKII 0 0 2 22 1188 108 0 0 \
  54 0 /kinetics/geometry 23 33 86 1 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot_CaMKII/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/tot_CaMKII/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/286P 0 0 0 0 0 0 0 0 54 0 \
  /kinetics/geometry 59 33 54 -2 0
simundump text /kinetics/SPINE/CaMKII_BULK/286P/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/286P/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII 0 0 0 0 0 0 0 0 54 \
  0 /kinetics/geometry 47 33 63 -2 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/notes LOAD \
""
simundump kenz /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph 0 0 \
  0 0 0 69.588 0.0005787 8 2 0 0 "" red 47 "" 74 -15 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph/notes 0 \
  ""
call /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph/notes LOAD \
""
simundump kenz /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 0 0 0 0 \
  0 486.01 0.0017361 24 6 0 0 "" red 47 "" 80 -15 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305/notes 0 \
  ""
call /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305/notes LOAD \
""
simundump kenz /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 0 0 0 0 \
  0 69.588 0.00014468 2 0.5 0 0 "" red 47 "" 62 -15 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286/notes 0 \
  ""
call /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/basal_CaMKII 0 0 1 1 54 54 0 0 54 \
  0 /kinetics/geometry blue 33 83 -2 0
simundump text /kinetics/SPINE/CaMKII_BULK/basal_CaMKII/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/basal_CaMKII/notes LOAD \
""
simundump kreac /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-bind-CaM 0 1.2161 \
  0.0022 "" white 33 56 -7 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-bind-CaM/notes 0 \
  "Same values as for the main compartment\nCan the main compartment pool of Ca/CaM be used?"
call /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-bind-CaM/notes LOAD \
"Same values as for the main compartment" \
"Can the main compartment pool of Ca/CaM be used?"
simundump kreac /kinetics/SPINE/CaMKII_BULK/CaMKII-bind-CaM 0 1.2161 2.2 "" \
  white 33 67 -7 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII-bind-CaM/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII-bind-CaM/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM 0 0 0 0 0 0 0 0 \
  54 0 /kinetics/geometry 47 33 58 -13 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM 0 0 0 0 0 0 0 0 54 0 \
  /kinetics/geometry 50 33 65 -13 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 0 0 0 0 0 0 0 0 54 \
  0 /kinetics/geometry 57 33 71 -13 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/CaMKII 0 0 20 20 1080 1080 0 0 54 \
  0 /kinetics/geometry 62 33 77 -13 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305 0 0 0 0 0 0 0 0 54 \
  0 /kinetics/geometry 30 33 90 -13 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p 0 0 0 0 0 0 0 0 54 0 \
  /kinetics/geometry 0 33 83 -13 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaM_CaN 1 1e-12 0 0 0 0 0 0 54 0 \
  /kinetics/geometry 1 5 32 -2 0
simundump text /kinetics/SPINE/CaM_CaN/notes 0 ""
call /kinetics/SPINE/CaM_CaN/notes LOAD \
""
simundump kpool /kinetics/SPINE/actCaMKII 0 0 2 2 108 108 0 0 54 0 \
  /kinetics/geometry 12 5 72 -2 0
simundump text /kinetics/SPINE/actCaMKII/notes 0 ""
call /kinetics/SPINE/actCaMKII/notes LOAD \
""
simundump kpool /kinetics/SPINE/PP2A 1 0 4 4 216 216 0 0 54 0 \
  /kinetics/geometry 62 5 46 -10 0
simundump text /kinetics/SPINE/PP2A/notes 0 \
  "Strack et al JBC 1997 show that PP2A is the primary\nphosphatase acting on CaMKII in the bulk."
call /kinetics/SPINE/PP2A/notes LOAD \
"Strack et al JBC 1997 show that PP2A is the primary" \
"phosphatase acting on CaMKII in the bulk."
simundump kenz /kinetics/SPINE/PP2A/Deph-thr286 0 0 0 0 0 486.01 0.0092593 2 \
  0.5 0 0 "" red 62 "" 63 -10 0
simundump text /kinetics/SPINE/PP2A/Deph-thr286/notes 0 ""
call /kinetics/SPINE/PP2A/Deph-thr286/notes LOAD \
""
simundump kenz /kinetics/SPINE/PP2A/Deph-thr286b 0 0 0 0 0 486.01 0.0092593 2 \
  0.5 0 0 "" red 62 "" 73 -10 0
simundump text /kinetics/SPINE/PP2A/Deph-thr286b/notes 0 ""
call /kinetics/SPINE/PP2A/Deph-thr286b/notes LOAD \
""
simundump kenz /kinetics/SPINE/PP2A/Deph-thr305 0 0 0 0 0 486.01 0.0092593 2 \
  0.5 0 0 "" red 62 "" 80 -10 0
simundump text /kinetics/SPINE/PP2A/Deph-thr305/notes 0 ""
call /kinetics/SPINE/PP2A/Deph-thr305/notes LOAD \
""
simundump kenz /kinetics/SPINE/PP2A/Deph-thr286c 0 0 0 0 0 486.01 0.0023148 \
  0.5 0.125 0 0 "" red 62 "" 90 -10 0
simundump text /kinetics/SPINE/PP2A/Deph-thr286c/notes 0 ""
call /kinetics/SPINE/PP2A/Deph-thr286c/notes LOAD \
""
simundump kenz /kinetics/SPINE/PP2A/Deph-thr305a 0 0 0 0 0 486.01 0.0092593 2 \
  0.5 0 0 "" red 62 "" 85 -10 0
simundump text /kinetics/SPINE/PP2A/Deph-thr305a/notes 0 ""
call /kinetics/SPINE/PP2A/Deph-thr305a/notes LOAD \
""
simundump group /kinetics/SPINE/CaN_BULK 1 yellow 5 x 0 0 "" CaN_BULK \
  /home2/bhalla/scripts/modules/PP1_PSD_0.g 0 \
  62a600ae10e53f567e47decb4f2b6488 0 14 -8 0
simundump text /kinetics/SPINE/CaN_BULK/notes 0 ""
call /kinetics/SPINE/CaN_BULK/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaN_BULK/CaN 0 1e-12 1 1 54 54 0 0 54 0 \
  /kinetics/geometry 1 yellow 4 -2 0
simundump text /kinetics/SPINE/CaN_BULK/CaN/notes 0 ""
call /kinetics/SPINE/CaN_BULK/CaN/notes LOAD \
""
simundump kreac /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN 0 0.85185 0.002 "" \
  white yellow 23 5 0
simundump text /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN/notes 0 \
  "From Quintana et al 2005"
call /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN/notes LOAD \
"From Quintana et al 2005"
simundump kreac /kinetics/SPINE/CaN_BULK/Ca_bind_CaN 0 0.0013717 1 "" white \
  yellow 8 5 0
simundump text /kinetics/SPINE/CaN_BULK/Ca_bind_CaN/notes 0 ""
call /kinetics/SPINE/CaN_BULK/Ca_bind_CaN/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaN_BULK/Ca2_CaN 0 0 0 0 0 0 0 0 54 0 \
  /kinetics/geometry[1] blue yellow 15 -2 0
simundump text /kinetics/SPINE/CaN_BULK/Ca2_CaN/notes 0 ""
call /kinetics/SPINE/CaN_BULK/Ca2_CaN/notes LOAD \
""
simundump xgraph /graphs/conc1 0 0 2000 0 4 0
simundump xgraph /graphs/conc2 0 0 2000 0 16.804 0
simundump xplot /graphs/conc1/tot_PSD_R.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc1/R.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 4 0 0 1
simundump xplot /graphs/conc1/Ca.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0 0 1
simundump xplot /graphs/conc1/PP1-active.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" cyan 0 0 1
simundump xplot /graphs/conc1/CaM_CaN.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 1 0 0 1
simundump xplot /graphs/conc1/tot-CaM-CaMKII.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 47 0 0 1
simundump xplot /graphs/conc1/tot-auto.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 29 0 0 1
simundump xplot /graphs/conc1/CaM-Ca4.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc2/iR.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 0 0 0 1
simundump xplot /graphs/conc2/Rpp.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 28 0 0 1
simundump xplot /graphs/conc2/Ca.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0 0 1
simundump xplot /graphs/conc2/CaM_CaN.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 1 0 0 1
simundump xplot /graphs/conc2/tot-CaM-CaMKII.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 47 0 0 1
simundump xplot /graphs/conc2/tot-auto.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 29 0 0 1
simundump xplot /graphs/conc2/CaM-Ca4.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xgraph /moregraphs/conc3 0 0 2000 0 4 0
simundump xgraph /moregraphs/conc4 0 0 2000 0 4 0
simundump xcoredraw /edit/draw 0 -10 92 -32 42
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"23 Dec 2011" \
"CaMKII_merged16.g" \
"Added Ca-binding step to the CaN activation pathway." \
"Based on CaMKII_merged15.g" \
"" \
"CaMKII_merged16a.g" \
"Halved the Kb for CaN-bind-CaM" \
"" \
"CaMKII_merged17.g" \
" Used more rates from Saucerman and Bers BPJ 2008" \
"" \
"CaMKII_merged17a.g" \
"Fix to init conc of PP1-active in PSD: from 2 to 4." \
"" \
"CaMKII_merged17b.g" \
"Raised PP2A CoInit from 0.1111 uM to 1 uM, to avoid sub-molecular" \
"levels in PSD. Scaled kcats down to match." \
"" \
"CaMKII_merged20.g" \
"First pass at a version with a reasonable basal PKA and a less" \
"saturating effect on PP1-active of CaMKII." \
"" \
"CaMKII_merged20c.g: This version turns on but does not go off" \
"afterwards. " \
"" \
"CaMKII_merged20d.g: 10x higher Km and kcat for PP1 on CaMKII." \
"" \
"CaMKII_merged20e.g: Km = 5 and kcat =0.5" \
"" \
"02 June 2012. CaMKII_merged20g.g: NMDAR = 40, effective # of" \
"sites given that CaMKII is a dodecamer." \
"" \
"CaMKII_merged20h.g: Lowered Kb for CaM-Bind-CaN from 0.006 to " \
"0.002/sec." \
"" \
"CaMKII_merged20i.g: Set PKA-active in both compts to be 0.02 uM." \
"" \
"CaMKII_merged20j.g: Faster CaN diffusion, by 6x. Now matches" \
"CaM with 6:54." \
"" \
"CaMKII_merged20k.g: Halved CaMKII affinity for NMDAR." \
"" \
"CaMKII_merged21.g: Removed bulk PP1. Use PP2A to dephosphorylate" \
"CaMKII in bulk." \
"" \
"19 July 2013. psd_merged30.g: Variant for signeur loading." \
""
addmsg /kinetics/PSD/R_S2 /kinetics/PSD/tot_PSD_R SUMTOTAL n nInit 
addmsg /kinetics/PSD/R_SpS /kinetics/PSD/tot_PSD_R SUMTOTAL n nInit 
addmsg /kinetics/PSD/R_SpSp /kinetics/PSD/tot_PSD_R SUMTOTAL n nInit 
addmsg /kinetics/PSD/actCaMKII/CaMKII_1 /kinetics/PSD/actCaMKII REAC eA B 
addmsg /kinetics/PSD/actCaMKII/CaMKII_2 /kinetics/PSD/actCaMKII REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM /kinetics/PSD/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-CaM /kinetics/PSD/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto /kinetics/PSD/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/actCaMKII /kinetics/PSD/actCaMKII/CaMKII_1 ENZYME n 
addmsg /kinetics/PSD/R_S2 /kinetics/PSD/actCaMKII/CaMKII_1 SUBSTRATE n 
addmsg /kinetics/PSD/actCaMKII /kinetics/PSD/actCaMKII/CaMKII_2 ENZYME n 
addmsg /kinetics/PSD/R_SpS /kinetics/PSD/actCaMKII/CaMKII_2 SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/PP1_2 /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/PP1_1 /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286 /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286b /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305 /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305a /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286c /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/dissoc-PP1-I1 /kinetics/PSD/PP1_PSD/PP1-active REAC B A 
addmsg /kinetics/PSD/PP1_PSD/Inact-PP1 /kinetics/PSD/PP1_PSD/PP1-active REAC A B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/PP1_2 ENZYME n 
addmsg /kinetics/PSD/R_SpSp /kinetics/PSD/PP1_PSD/PP1-active/PP1_2 SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/PP1_1 ENZYME n 
addmsg /kinetics/PSD/R_SpS /kinetics/PSD/PP1_PSD/PP1-active/PP1_1 SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286 ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286 SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286b ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286b SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305 ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII_p_p_p /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305 SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286c ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII_p_p_p /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286c SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305a ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr305 /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305a SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/dissoc-PP1-I1 /kinetics/PSD/PP1_PSD/I1 REAC B A 
addmsg /kinetics/PSD/PKA-active/PKA-phosph-I1 /kinetics/PSD/PP1_PSD/I1 REAC sA B 
addmsg /kinetics/PSD/CaM_CaN/dephosph_inhib1 /kinetics/PSD/PP1_PSD/I1 MM_PRD pA 
addmsg /kinetics/PSD/PP2A/PP2A-dephosph-I1 /kinetics/PSD/PP1_PSD/I1 MM_PRD pA 
addmsg /kinetics/PSD/CaM_CaN/dephosph-PP1-I_p /kinetics/PSD/PP1_PSD/PP1-I1 MM_PRD pA 
addmsg /kinetics/PSD/PP2A/PP2A-dephosph-PP1-I_p /kinetics/PSD/PP1_PSD/PP1-I1 MM_PRD pA 
addmsg /kinetics/PSD/PP1_PSD/dissoc-PP1-I1 /kinetics/PSD/PP1_PSD/PP1-I1 REAC A B 
addmsg /kinetics/PSD/PP1_PSD/I1 /kinetics/PSD/PP1_PSD/dissoc-PP1-I1 PRODUCT n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/dissoc-PP1-I1 PRODUCT n 
addmsg /kinetics/PSD/PP1_PSD/PP1-I1 /kinetics/PSD/PP1_PSD/dissoc-PP1-I1 SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/I1_p /kinetics/PSD/PP1_PSD/Inact-PP1 SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-I1_p /kinetics/PSD/PP1_PSD/Inact-PP1 PRODUCT n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/Inact-PP1 SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/Inact-PP1 /kinetics/PSD/PP1_PSD/PP1-I1_p REAC B A 
addmsg /kinetics/PSD/CaM_CaN/dephosph-PP1-I_p /kinetics/PSD/PP1_PSD/PP1-I1_p REAC sA B 
addmsg /kinetics/PSD/PP2A/PP2A-dephosph-PP1-I_p /kinetics/PSD/PP1_PSD/PP1-I1_p REAC sA B 
addmsg /kinetics/PSD/PP1_PSD/Inact-PP1 /kinetics/PSD/PP1_PSD/I1_p REAC A B 
addmsg /kinetics/PSD/PKA-active/PKA-phosph-I1 /kinetics/PSD/PP1_PSD/I1_p MM_PRD pA 
addmsg /kinetics/PSD/CaM_CaN/dephosph_inhib1 /kinetics/PSD/PP1_PSD/I1_p REAC sA B 
addmsg /kinetics/PSD/PP2A/PP2A-dephosph-I1 /kinetics/PSD/PP1_PSD/I1_p REAC sA B 
addmsg /kinetics/PSD/PP1_PSD/Ca_bind_CaN /kinetics/PSD/PP1_PSD/CaN REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca4 /kinetics/PSD/PP1_PSD/CaM-Bind-CaN SUBSTRATE n 
addmsg /kinetics/PSD/CaM_CaN /kinetics/PSD/PP1_PSD/CaM-Bind-CaN PRODUCT n 
addmsg /kinetics/PSD/PP1_PSD/Ca2_CaN /kinetics/PSD/PP1_PSD/CaM-Bind-CaN SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/Ca_bind_CaN /kinetics/PSD/PP1_PSD/Ca2_CaN REAC B A 
addmsg /kinetics/PSD/PP1_PSD/CaM-Bind-CaN /kinetics/PSD/PP1_PSD/Ca2_CaN REAC A B 
addmsg /kinetics/PSD/PP1_PSD/CaN /kinetics/PSD/PP1_PSD/Ca_bind_CaN SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/Ca2_CaN /kinetics/PSD/PP1_PSD/Ca_bind_CaN PRODUCT n 
addmsg /kinetics/PSD/CaM/Ca /kinetics/PSD/PP1_PSD/Ca_bind_CaN SUBSTRATE n 
addmsg /kinetics/PSD/CaM/Ca /kinetics/PSD/PP1_PSD/Ca_bind_CaN SUBSTRATE n 
addmsg /kinetics/SPINE/iR /kinetics/PSD/move_to_PSD SUBSTRATE n 
addmsg /kinetics/PSD/R_S2 /kinetics/PSD/move_to_PSD PRODUCT n 
addmsg /kinetics/PSD/basal_phosphatase/P1 /kinetics/PSD/R_S2 MM_PRD pA 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/PP1_1 /kinetics/PSD/R_S2 MM_PRD pA 
addmsg /kinetics/PSD/move_to_PSD /kinetics/PSD/R_S2 REAC B A 
addmsg /kinetics/PSD/actCaMKII/CaMKII_1 /kinetics/PSD/R_S2 REAC sA B 
addmsg /kinetics/PSD/basal_phosphatase/P2 /kinetics/PSD/R_SpS MM_PRD pA 
addmsg /kinetics/PSD/basal_phosphatase/P1 /kinetics/PSD/R_SpS REAC sA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/PP1_2 /kinetics/PSD/R_SpS MM_PRD pA 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/PP1_1 /kinetics/PSD/R_SpS REAC sA B 
addmsg /kinetics/PSD/actCaMKII/CaMKII_1 /kinetics/PSD/R_SpS MM_PRD pA 
addmsg /kinetics/PSD/actCaMKII/CaMKII_2 /kinetics/PSD/R_SpS REAC sA B 
addmsg /kinetics/PSD/basal_phosphatase/P2 /kinetics/PSD/R_SpSp REAC sA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/PP1_2 /kinetics/PSD/R_SpSp REAC sA B 
addmsg /kinetics/PSD/actCaMKII/CaMKII_2 /kinetics/PSD/R_SpSp MM_PRD pA 
addmsg /kinetics/PSD/basal_phosphatase/P1 /kinetics/PSD/basal_phosphatase REAC eA B 
addmsg /kinetics/PSD/basal_phosphatase/P2 /kinetics/PSD/basal_phosphatase REAC eA B 
addmsg /kinetics/PSD/basal_phosphatase /kinetics/PSD/basal_phosphatase/P1 ENZYME n 
addmsg /kinetics/PSD/R_SpS /kinetics/PSD/basal_phosphatase/P1 SUBSTRATE n 
addmsg /kinetics/PSD/basal_phosphatase /kinetics/PSD/basal_phosphatase/P2 ENZYME n 
addmsg /kinetics/PSD/R_SpSp /kinetics/PSD/basal_phosphatase/P2 SUBSTRATE n 
addmsg /kinetics/PSD/PKA-active/PKA-phosph-I1 /kinetics/PSD/PKA-active REAC eA B 
addmsg /kinetics/PSD/PKA-active /kinetics/PSD/PKA-active/PKA-phosph-I1 ENZYME n 
addmsg /kinetics/PSD/PP1_PSD/I1 /kinetics/PSD/PKA-active/PKA-phosph-I1 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/transloc_2 /kinetics/PSD/CaMKII_PSD/NMDAR REAC A B 
addmsg /kinetics/PSD/CaMKII_PSD/transloc_1 /kinetics/PSD/CaMKII_PSD/NMDAR REAC A B 
addmsg /kinetics/PSD/CaMKII_PSD/back_1 /kinetics/PSD/CaMKII_PSD/NMDAR REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/back_2 /kinetics/PSD/CaMKII_PSD/NMDAR REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 /kinetics/PSD/CaMKII_PSD/tot-auto SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII_p_p_p /kinetics/PSD/CaMKII_PSD/tot-auto SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305 /kinetics/PSD/CaMKII_PSD/tot-auto REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286 /kinetics/PSD/CaMKII_PSD/tot-auto REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/basal_CaMKII /kinetics/PSD/CaMKII_PSD/tot-auto SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph /kinetics/PSD/CaMKII_PSD/tot-auto REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286 ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-CaM /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305 ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305 SUBSTRATE n 
addmsg /kinetics/PSD/actCaMKII /kinetics/PSD/CaMKII_PSD/tot_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII /kinetics/PSD/CaMKII_PSD/tot_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr305 /kinetics/PSD/CaMKII_PSD/tot_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM /kinetics/PSD/CaMKII_PSD/286P SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 /kinetics/PSD/CaMKII_PSD/286P SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-CaM /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305 /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286 /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305 ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286 ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-CaM /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM PRODUCT n 
addmsg /kinetics/PSD/CaM/CaM-Ca4 /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca4 /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-CaM /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM PRODUCT n 
addmsg /kinetics/PSD/CaMKII_PSD/transloc_2 /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286 /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286 /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM MM_PRD pA 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286 /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/transloc_1 /kinetics/PSD/CaMKII_PSD/CaMKII-CaM REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286 /kinetics/PSD/CaMKII_PSD/CaMKII-CaM REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286 /kinetics/PSD/CaMKII_PSD/CaMKII-CaM REAC sA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286 /kinetics/PSD/CaMKII_PSD/CaMKII-CaM MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM /kinetics/PSD/CaMKII_PSD/CaMKII-CaM REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305 /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305 /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 REAC A B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286b /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 REAC sA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305 /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/back_1 /kinetics/PSD/CaMKII_PSD/CaMKII REAC A B 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM /kinetics/PSD/CaMKII_PSD/CaMKII REAC A B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305a /kinetics/PSD/CaMKII_PSD/CaMKII MM_PRD pA 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286b /kinetics/PSD/CaMKII_PSD/CaMKII MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph /kinetics/PSD/CaMKII_PSD/CaMKII REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph /kinetics/PSD/CaMKII_PSD/CaMKII REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/back_2 /kinetics/PSD/CaMKII_PSD/CaMKII-thr305 REAC A B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305a /kinetics/PSD/CaMKII_PSD/CaMKII-thr305 REAC sA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286c /kinetics/PSD/CaMKII_PSD/CaMKII-thr305 MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305 /kinetics/PSD/CaMKII_PSD/CaMKII_p_p_p MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305 /kinetics/PSD/CaMKII_PSD/CaMKII_p_p_p MM_PRD pA 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305 /kinetics/PSD/CaMKII_PSD/CaMKII_p_p_p REAC sA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286c /kinetics/PSD/CaMKII_PSD/CaMKII_p_p_p REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-CaM /kinetics/PSD/CaMKII_PSD/transloc_1 PRODUCT n 
addmsg /kinetics/PSD/CaMKII_PSD/NMDAR /kinetics/PSD/CaMKII_PSD/transloc_1 SUBSTRATE n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM /kinetics/PSD/CaMKII_PSD/transloc_1 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII /kinetics/PSD/CaMKII_PSD/back_1 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/NMDAR /kinetics/PSD/CaMKII_PSD/back_1 PRODUCT n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII /kinetics/PSD/CaMKII_PSD/back_1 PRODUCT n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM /kinetics/PSD/CaMKII_PSD/transloc_2 PRODUCT n 
addmsg /kinetics/PSD/CaMKII_PSD/NMDAR /kinetics/PSD/CaMKII_PSD/transloc_2 SUBSTRATE n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM /kinetics/PSD/CaMKII_PSD/transloc_2 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr305 /kinetics/PSD/CaMKII_PSD/back_2 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/NMDAR /kinetics/PSD/CaMKII_PSD/back_2 PRODUCT n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305 /kinetics/PSD/CaMKII_PSD/back_2 PRODUCT n 
addmsg /kinetics/PSD/PP2A/PP2A-dephosph-I1 /kinetics/PSD/PP2A REAC eA B 
addmsg /kinetics/PSD/PP2A/PP2A-dephosph-PP1-I_p /kinetics/PSD/PP2A REAC eA B 
addmsg /kinetics/PSD/PP2A /kinetics/PSD/PP2A/PP2A-dephosph-PP1-I_p ENZYME n 
addmsg /kinetics/PSD/PP1_PSD/PP1-I1_p /kinetics/PSD/PP2A/PP2A-dephosph-PP1-I_p SUBSTRATE n 
addmsg /kinetics/PSD/PP2A /kinetics/PSD/PP2A/PP2A-dephosph-I1 ENZYME n 
addmsg /kinetics/PSD/PP1_PSD/I1_p /kinetics/PSD/PP2A/PP2A-dephosph-I1 SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-bind-Ca /kinetics/PSD/CaM/CaM REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca3 /kinetics/PSD/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/Ca /kinetics/PSD/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca4 /kinetics/PSD/CaM/CaM-Ca3-bind-Ca PRODUCT n 
addmsg /kinetics/PSD/CaM/CaM-Ca2-bind-Ca /kinetics/PSD/CaM/CaM-Ca3 REAC B A 
addmsg /kinetics/PSD/CaM/CaM-Ca3-bind-Ca /kinetics/PSD/CaM/CaM-Ca3 REAC A B 
addmsg /kinetics/PSD/CaM/CaM /kinetics/PSD/CaM/CaM-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/Ca /kinetics/PSD/CaM/CaM-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca /kinetics/PSD/CaM/CaM-bind-Ca PRODUCT n 
addmsg /kinetics/PSD/CaM/CaM-Ca2-bind-Ca /kinetics/PSD/CaM/CaM-Ca2 REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca-bind-Ca /kinetics/PSD/CaM/CaM-Ca2 REAC B A 
addmsg /kinetics/PSD/CaM/CaM-Ca2 /kinetics/PSD/CaM/CaM-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/Ca /kinetics/PSD/CaM/CaM-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca3 /kinetics/PSD/CaM/CaM-Ca2-bind-Ca PRODUCT n 
addmsg /kinetics/PSD/CaM/CaM-Ca /kinetics/PSD/CaM/CaM-Ca-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/Ca /kinetics/PSD/CaM/CaM-Ca-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca2 /kinetics/PSD/CaM/CaM-Ca-bind-Ca PRODUCT n 
addmsg /kinetics/PSD/CaM/CaM-bind-Ca /kinetics/PSD/CaM/CaM-Ca REAC B A 
addmsg /kinetics/PSD/CaM/CaM-Ca-bind-Ca /kinetics/PSD/CaM/CaM-Ca REAC A B 
addmsg /kinetics/PSD/CaM/CaM-bind-Ca /kinetics/PSD/CaM/Ca REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca2-bind-Ca /kinetics/PSD/CaM/Ca REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca3-bind-Ca /kinetics/PSD/CaM/Ca REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca-bind-Ca /kinetics/PSD/CaM/Ca REAC A B 
addmsg /kinetics/PSD/PP1_PSD/Ca_bind_CaN /kinetics/PSD/CaM/Ca REAC A B 
addmsg /kinetics/PSD/PP1_PSD/Ca_bind_CaN /kinetics/PSD/CaM/Ca REAC A B 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM /kinetics/PSD/CaM/CaM-Ca4 REAC A B 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM /kinetics/PSD/CaM/CaM-Ca4 REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca3-bind-Ca /kinetics/PSD/CaM/CaM-Ca4 REAC B A 
addmsg /kinetics/PSD/PP1_PSD/CaM-Bind-CaN /kinetics/PSD/CaM/CaM-Ca4 REAC A B 
addmsg /kinetics/PSD/CaM_CaN/dephosph_inhib1 /kinetics/PSD/CaM_CaN REAC eA B 
addmsg /kinetics/PSD/CaM_CaN/dephosph-PP1-I_p /kinetics/PSD/CaM_CaN REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/CaM-Bind-CaN /kinetics/PSD/CaM_CaN REAC B A 
addmsg /kinetics/PSD/CaM_CaN /kinetics/PSD/CaM_CaN/dephosph-PP1-I_p ENZYME n 
addmsg /kinetics/PSD/PP1_PSD/PP1-I1_p /kinetics/PSD/CaM_CaN/dephosph-PP1-I_p SUBSTRATE n 
addmsg /kinetics/PSD/CaM_CaN /kinetics/PSD/CaM_CaN/dephosph_inhib1 ENZYME n 
addmsg /kinetics/PSD/PP1_PSD/I1_p /kinetics/PSD/CaM_CaN/dephosph_inhib1 SUBSTRATE n 
addmsg /kinetics/PSD/move_to_PSD /kinetics/SPINE/iR REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-bind-Ca /kinetics/SPINE/CaM/CaM REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-Ca3 /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/Ca /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-Ca4 /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca PRODUCT n 
addmsg /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca /kinetics/SPINE/CaM/CaM-Ca3 REAC B A 
addmsg /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca /kinetics/SPINE/CaM/CaM-Ca3 REAC A B 
addmsg /kinetics/SPINE/CaM/CaM /kinetics/SPINE/CaM/CaM-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/Ca /kinetics/SPINE/CaM/CaM-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-Ca /kinetics/SPINE/CaM/CaM-bind-Ca PRODUCT n 
addmsg /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca /kinetics/SPINE/CaM/CaM-Ca2 REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-Ca-bind-Ca /kinetics/SPINE/CaM/CaM-Ca2 REAC B A 
addmsg /kinetics/SPINE/CaM/CaM-Ca2 /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/Ca /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-Ca3 /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca PRODUCT n 
addmsg /kinetics/SPINE/CaM/CaM-Ca /kinetics/SPINE/CaM/CaM-Ca-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/Ca /kinetics/SPINE/CaM/CaM-Ca-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-Ca2 /kinetics/SPINE/CaM/CaM-Ca-bind-Ca PRODUCT n 
addmsg /kinetics/SPINE/CaM/CaM-bind-Ca /kinetics/SPINE/CaM/CaM-Ca REAC B A 
addmsg /kinetics/SPINE/CaM/CaM-Ca-bind-Ca /kinetics/SPINE/CaM/CaM-Ca REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-bind-Ca /kinetics/SPINE/CaM/Ca REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca /kinetics/SPINE/CaM/Ca REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca /kinetics/SPINE/CaM/Ca REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-Ca-bind-Ca /kinetics/SPINE/CaM/Ca REAC A B 
addmsg /kinetics/SPINE/CaN_BULK/Ca_bind_CaN /kinetics/SPINE/CaM/Ca REAC A B 
addmsg /kinetics/SPINE/CaN_BULK/Ca_bind_CaN /kinetics/SPINE/CaM/Ca REAC A B 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-bind-CaM /kinetics/SPINE/CaM/CaM-Ca4 REAC A B 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-bind-CaM /kinetics/SPINE/CaM/CaM-Ca4 REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca /kinetics/SPINE/CaM/CaM-Ca4 REAC B A 
addmsg /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN /kinetics/SPINE/CaM/CaM-Ca4 REAC A B 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 /kinetics/SPINE/CaMKII_BULK/tot-auto SUMTOTAL n nInit 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p /kinetics/SPINE/CaMKII_BULK/tot-auto SUMTOTAL n nInit 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-305 /kinetics/SPINE/CaMKII_BULK/tot-auto REAC eA B 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-286 /kinetics/SPINE/CaMKII_BULK/tot-auto REAC eA B 
addmsg /kinetics/SPINE/CaMKII_BULK/basal_CaMKII /kinetics/SPINE/CaMKII_BULK/tot-auto SUMTOTAL n nInit 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-autoph /kinetics/SPINE/CaMKII_BULK/tot-auto REAC eA B 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-autoph ENZYME n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-autoph SUBSTRATE n 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-286 ENZYME n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-286 SUBSTRATE n 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-305 ENZYME n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-305 SUBSTRATE n 
addmsg /kinetics/SPINE/actCaMKII /kinetics/SPINE/CaMKII_BULK/tot_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII /kinetics/SPINE/CaMKII_BULK/tot_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305 /kinetics/SPINE/CaMKII_BULK/tot_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM /kinetics/SPINE/CaMKII_BULK/286P SUMTOTAL n nInit 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 /kinetics/SPINE/CaMKII_BULK/286P SUMTOTAL n nInit 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII SUMTOTAL n nInit 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII SUMTOTAL n nInit 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII REAC eA B 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII REAC eA B 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII REAC eA B 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph ENZYME n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph SUBSTRATE n 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 ENZYME n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 SUBSTRATE n 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 ENZYME n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 SUBSTRATE n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-bind-CaM SUBSTRATE n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-bind-CaM PRODUCT n 
addmsg /kinetics/SPINE/CaM/CaM-Ca4 /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-bind-CaM SUBSTRATE n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII /kinetics/SPINE/CaMKII_BULK/CaMKII-bind-CaM SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-Ca4 /kinetics/SPINE/CaMKII_BULK/CaMKII-bind-CaM SUBSTRATE n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM /kinetics/SPINE/CaMKII_BULK/CaMKII-bind-CaM PRODUCT n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-bind-CaM /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM REAC B A 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM MM_PRD pA 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-286 /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM MM_PRD pA 
addmsg /kinetics/SPINE/PP2A/Deph-thr286 /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/transloc_2 /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM REAC A B 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM REAC sA B 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-286 /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM REAC sA B 
addmsg /kinetics/SPINE/PP2A/Deph-thr286 /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM MM_PRD pA 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-bind-CaM /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/transloc_1 /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM REAC A B 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 REAC sA B 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-305 /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 REAC sA B 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-bind-CaM /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 REAC A B 
addmsg /kinetics/SPINE/PP2A/Deph-thr286b /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 REAC sA B 
addmsg /kinetics/SPINE/PP2A/Deph-thr305 /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 MM_PRD pA 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 MM_PRD pA 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-autoph /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 MM_PRD pA 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-bind-CaM /kinetics/SPINE/CaMKII_BULK/CaMKII REAC A B 
addmsg /kinetics/SPINE/PP2A/Deph-thr305a /kinetics/SPINE/CaMKII_BULK/CaMKII MM_PRD pA 
addmsg /kinetics/SPINE/PP2A/Deph-thr286b /kinetics/SPINE/CaMKII_BULK/CaMKII MM_PRD pA 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph /kinetics/SPINE/CaMKII_BULK/CaMKII REAC sA B 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-autoph /kinetics/SPINE/CaMKII_BULK/CaMKII REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/back_1 /kinetics/SPINE/CaMKII_BULK/CaMKII REAC B A 
addmsg /kinetics/SPINE/PP2A/Deph-thr305a /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305 REAC sA B 
addmsg /kinetics/SPINE/PP2A/Deph-thr286c /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305 MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/back_2 /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305 REAC B A 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p MM_PRD pA 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-305 /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p MM_PRD pA 
addmsg /kinetics/SPINE/PP2A/Deph-thr305 /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p REAC sA B 
addmsg /kinetics/SPINE/PP2A/Deph-thr286c /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p REAC sA B 
addmsg /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN /kinetics/SPINE/CaM_CaN REAC B A 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM /kinetics/SPINE/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM /kinetics/SPINE/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto /kinetics/SPINE/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/SPINE/PP2A/Deph-thr286 /kinetics/SPINE/PP2A REAC eA B 
addmsg /kinetics/SPINE/PP2A/Deph-thr286b /kinetics/SPINE/PP2A REAC eA B 
addmsg /kinetics/SPINE/PP2A/Deph-thr305 /kinetics/SPINE/PP2A REAC eA B 
addmsg /kinetics/SPINE/PP2A/Deph-thr305a /kinetics/SPINE/PP2A REAC eA B 
addmsg /kinetics/SPINE/PP2A/Deph-thr286c /kinetics/SPINE/PP2A REAC eA B 
addmsg /kinetics/SPINE/PP2A /kinetics/SPINE/PP2A/Deph-thr286 ENZYME n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM /kinetics/SPINE/PP2A/Deph-thr286 SUBSTRATE n 
addmsg /kinetics/SPINE/PP2A /kinetics/SPINE/PP2A/Deph-thr286b ENZYME n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 /kinetics/SPINE/PP2A/Deph-thr286b SUBSTRATE n 
addmsg /kinetics/SPINE/PP2A /kinetics/SPINE/PP2A/Deph-thr305 ENZYME n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p /kinetics/SPINE/PP2A/Deph-thr305 SUBSTRATE n 
addmsg /kinetics/SPINE/PP2A /kinetics/SPINE/PP2A/Deph-thr286c ENZYME n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p /kinetics/SPINE/PP2A/Deph-thr286c SUBSTRATE n 
addmsg /kinetics/SPINE/PP2A /kinetics/SPINE/PP2A/Deph-thr305a ENZYME n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305 /kinetics/SPINE/PP2A/Deph-thr305a SUBSTRATE n 
addmsg /kinetics/SPINE/CaN_BULK/Ca_bind_CaN /kinetics/SPINE/CaN_BULK/CaN REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-Ca4 /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN SUBSTRATE n 
addmsg /kinetics/SPINE/CaM_CaN /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN PRODUCT n 
addmsg /kinetics/SPINE/CaN_BULK/Ca2_CaN /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN SUBSTRATE n 
addmsg /kinetics/SPINE/CaN_BULK/CaN /kinetics/SPINE/CaN_BULK/Ca_bind_CaN SUBSTRATE n 
addmsg /kinetics/SPINE/CaN_BULK/Ca2_CaN /kinetics/SPINE/CaN_BULK/Ca_bind_CaN PRODUCT n 
addmsg /kinetics/SPINE/CaM/Ca /kinetics/SPINE/CaN_BULK/Ca_bind_CaN SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/Ca /kinetics/SPINE/CaN_BULK/Ca_bind_CaN SUBSTRATE n 
addmsg /kinetics/SPINE/CaN_BULK/Ca_bind_CaN /kinetics/SPINE/CaN_BULK/Ca2_CaN REAC B A 
addmsg /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN /kinetics/SPINE/CaN_BULK/Ca2_CaN REAC A B 
addmsg /kinetics/PSD/tot_PSD_R /graphs/conc1/tot_PSD_R.Co PLOT Co *tot_PSD_R.Co *blue 
addmsg /kinetics/PSD/R_S2 /graphs/conc1/R.Co PLOT Co *R.Co *4 
addmsg /kinetics/PSD/CaM/Ca /graphs/conc1/Ca.Co PLOT Co *Ca.Co *red 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /graphs/conc1/PP1-active.Co PLOT Co *PP1-active.Co *cyan 
addmsg /kinetics/PSD/CaM_CaN /graphs/conc1/CaM_CaN.Co PLOT Co *CaM_CaN.Co *1 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII /graphs/conc1/tot-CaM-CaMKII.Co PLOT Co *tot-CaM-CaMKII.Co *47 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto /graphs/conc1/tot-auto.Co PLOT Co *tot-auto.Co *29 
addmsg /kinetics/PSD/CaM/CaM-Ca4 /graphs/conc1/CaM-Ca4.Co PLOT Co *CaM-Ca4.Co *blue 
addmsg /kinetics/SPINE/iR /graphs/conc2/iR.Co PLOT Co *iR.Co *0 
addmsg /kinetics/PSD/R_SpSp /graphs/conc2/Rpp.Co PLOT Co *Rpp.Co *28 
addmsg /kinetics/SPINE/CaM/Ca /graphs/conc2/Ca.Co PLOT Co *Ca.Co *red 
addmsg /kinetics/SPINE/CaM_CaN /graphs/conc2/CaM_CaN.Co PLOT Co *CaM_CaN.Co *1 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII /graphs/conc2/tot-CaM-CaMKII.Co PLOT Co *tot-CaM-CaMKII.Co *47 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto /graphs/conc2/tot-auto.Co PLOT Co *tot-auto.Co *29 
addmsg /kinetics/SPINE/CaM/CaM-Ca4 /graphs/conc2/CaM-Ca4.Co PLOT Co *CaM-Ca4.Co *blue 
enddump
// End of dump

call /kinetics/PSD/PP1_PSD/PP1-active/notes LOAD \
"Cohen et al Meth Enz 159 390-408 is main source of info" \
"conc  = 1.8 uM"
call /kinetics/PSD/PP1_PSD/I1/notes LOAD \
"I1 is a 'mixed' inhibitor, but at high enz concs it looks like a non-compet" \
"inhibitor (Foulkes et al Eur J Biochem 132 309-313 9183)." \
"We treat it as non-compet, so it just turns the enz off" \
"without interacting with the binding site." \
"Cohen et al ann rev bioch refer to results where conc is " \
"1.5 to 1.8 uM. In order to get complete inhib of PP1, which is at 1.8 uM," \
"we need >= 1.8 uM." \
"" \
""
call /kinetics/PSD/PP1_PSD/dissoc-PP1-I1/notes LOAD \
"Let us assume that the equil in this case is very far over to the" \
"right. This is probably safe." \
""
call /kinetics/PSD/PP1_PSD/Inact-PP1/notes LOAD \
"K inhib = 1nM from Cohen Ann Rev Bioch 1989, " \
"4 nM from Foukes et al " \
"Assume 2 nM. kf /kb = 8.333e-4"
call /kinetics/PSD/PP1_PSD/I1_p/notes LOAD \
"Dephosph is mainly by PP2B"
call /kinetics/PSD/basal_phosphatase/notes LOAD \
"There isn't any clear info for this. I had originally called " \
"it PP2A, but that causes odd interactions with other pathways." \
""
call /kinetics/PSD/CaMKII_PSD/NMDAR/notes LOAD \
"The stochiometry is a bit off here. Each NMDAR actually" \
"binds to a holoenzyme, about 12 CaMKII subunits. But" \
"our CaMKII calculations are in terms of individual" \
"subunits. So as a hack, we put in much more NMDAR than" \
"is actually there." \
"" \
"Dec 2011: Reconsidered this, now only 120 NMDARs." \
"" \
"June 02 2012. Consider these as anchor points for a dodecamer." \
"There are far more effected CaMKII binding sites then, than" \
"individual molecules of NMDAR. Raised limit to 40."
call /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM/notes LOAD \
"Same values as for the main compartment" \
"Can the main compartment pool of Ca/CaM be used?"
call /kinetics/PSD/CaMKII_PSD/back_1/notes LOAD \
"Rates set by the translocation experiments of " \
"Shen and Meyer, Science 1999."
call /kinetics/PSD/CaMKII_PSD/transloc_2/notes LOAD \
"Same as for transloc_1" \
"" \
"" \
""
call /kinetics/PSD/CaMKII_PSD/back_2/notes LOAD \
"Same as for back_1" \
""
call /kinetics/PSD/PP2A/PP2A-dephosph-PP1-I_p/notes LOAD \
"k1 changed from 3.3e-6 to 6.6e-6" \
""
call /kinetics/PSD/PP2A/PP2A-dephosph-I1/notes LOAD \
"PP2A does most of the dephosph of I1 at basal Ca levels. See" \
"the review by Cohen in Ann Rev Biochem 1989." \
"For now, lets halve Km. k1 was 3.3e-6, now 6.6e-6" \
""
call /kinetics/PSD/CaM/CaM/notes LOAD \
"There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)" \
"Say 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying" \
"it is comparable to CaMK levels. " \
""
call /kinetics/PSD/CaM/CaM-Ca3-bind-Ca/notes LOAD \
"Use K3 = 21.5 uM here from Stemmer and Klee table 3." \
"kb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10"
call /kinetics/PSD/CaM/CaM-bind-Ca/notes LOAD \
"Lets use the fast rate consts here. Since the rates are so different, I am not" \
"sure whether the order is relevant. These correspond to the TR2C fragment." \
"We use the Martin et al rates here, plus the Drabicowski binding consts." \
"All are scaled by 3X to cell temp." \
"kf = 2e-10 kb = 72" \
"Stemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11." \
"If kb=72, kf = 2e-10 (Exactly the same !)" \
"" \
"19 May 2006. Splitting the old CaM-TR2-bind-Ca reaction into" \
"two steps, each binding 1 Ca. This improves numerical" \
"stability and is conceptually better too." \
"" \
"Overall rates are the same, so each kf and kb is the " \
"square root of the earlier ones. So" \
"kf = 1.125e-4, kb = 8.4853" \
""
call /kinetics/PSD/CaM/CaM-Ca2/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
call /kinetics/PSD/CaM/CaM-Ca2-bind-Ca/notes LOAD \
"K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get" \
"kb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10" \
""
call /kinetics/PSD/CaM/CaM-Ca-bind-Ca/notes LOAD \
"Lets use the fast rate consts here. Since the rates are so different, I am not" \
"sure whether the order is relevant. These correspond to the TR2C fragment." \
"We use the Martin et al rates here, plus the Drabicowski binding consts." \
"All are scaled by 3X to cell temp." \
"kf = 2e-10 kb = 72" \
"Stemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11." \
"If kb=72, kf = 2e-10 (Exactly the same !)" \
"" \
"19 May 2006. Splitting the old CaM-TR2-bind-Ca reaction into" \
"two steps, each binding 1 Ca. This improves numerical" \
"stability and is conceptually better too." \
"" \
"Overall rates are the same, so each kf and kb is the " \
"square root of the earlier ones. So" \
"kf = 1.125e-4, kb = 8.4853" \
""
call /kinetics/PSD/CaM/CaM-Ca/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
call /kinetics/SPINE/iR/notes LOAD \
"Same as Fus3" \
""
call /kinetics/SPINE/CaM/CaM/notes LOAD \
"There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)" \
"Say 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying" \
"it is comparable to CaMK levels. " \
""
call /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca/notes LOAD \
"Use K3 = 21.5 uM here from Stemmer and Klee table 3." \
"kb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10"
call /kinetics/SPINE/CaM/CaM-bind-Ca/notes LOAD \
"Lets use the fast rate consts here. Since the rates are so different, I am not" \
"sure whether the order is relevant. These correspond to the TR2C fragment." \
"We use the Martin et al rates here, plus the Drabicowski binding consts." \
"All are scaled by 3X to cell temp." \
"kf = 2e-10 kb = 72" \
"Stemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11." \
"If kb=72, kf = 2e-10 (Exactly the same !)" \
"" \
"19 May 2006. Splitting the old CaM-TR2-bind-Ca reaction into" \
"two steps, each binding 1 Ca. This improves numerical" \
"stability and is conceptually better too." \
"" \
"Overall rates are the same, so each kf and kb is the " \
"square root of the earlier ones. So" \
"kf = 1.125e-4, kb = 8.4853" \
""
call /kinetics/SPINE/CaM/CaM-Ca2/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
call /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca/notes LOAD \
"K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get" \
"kb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10" \
""
call /kinetics/SPINE/CaM/CaM-Ca-bind-Ca/notes LOAD \
"Lets use the fast rate consts here. Since the rates are so different, I am not" \
"sure whether the order is relevant. These correspond to the TR2C fragment." \
"We use the Martin et al rates here, plus the Drabicowski binding consts." \
"All are scaled by 3X to cell temp." \
"kf = 2e-10 kb = 72" \
"Stemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11." \
"If kb=72, kf = 2e-10 (Exactly the same !)" \
"" \
"19 May 2006. Splitting the old CaM-TR2-bind-Ca reaction into" \
"two steps, each binding 1 Ca. This improves numerical" \
"stability and is conceptually better too." \
"" \
"Overall rates are the same, so each kf and kb is the " \
"square root of the earlier ones. So" \
"kf = 1.125e-4, kb = 8.4853" \
""
call /kinetics/SPINE/CaM/CaM-Ca/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
call /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-bind-CaM/notes LOAD \
"Same values as for the main compartment" \
"Can the main compartment pool of Ca/CaM be used?"
call /kinetics/SPINE/PP2A/notes LOAD \
"Strack et al JBC 1997 show that PP2A is the primary" \
"phosphatase acting on CaMKII in the bulk."
call /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN/notes LOAD \
"From Quintana et al 2005"
complete_loading
