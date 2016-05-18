//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Sun Nov 15 09:10:36 2015
 
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
simundump geometry /kinetics/geometry 0 1e-20 3 sphere "" white black -4 -26 \
  0
simundump geometry /kinetics/geometry[1] 0 1e-18 3 sphere "" white black 1 \
  -24 0
simundump geometry /kinetics/geometry[2] 0 9e-20 3 sphere "" white black 4 \
  -20 0
simundump geometry /kinetics/geometry[3] 0 1e-20 3 sphere "" white black 3 -6 \
  0
simundump geometry /kinetics/geometry[4] 0 1e-18 3 sphere "" white black -2 \
  -12 0
simundump geometry /kinetics/geometry[5] 0 1e-18 3 sphere "" white black 9 \
  -11 0
simundump geometry /kinetics/geometry[6] 0 9e-20 3 sphere "" white black 10 \
  -15 0
simundump geometry /kinetics/geometry[7] 0 1e-20 3 sphere "" white black 0 0 \
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
simundump kenz /kinetics/PSD/actCaMKII/CaMKII_1 0 0 0 0 0 6 0.083325 8 2 0 0 \
  "" red 35 "" -6 3 0
simundump text /kinetics/PSD/actCaMKII/CaMKII_1/notes 0 ""
call /kinetics/PSD/actCaMKII/CaMKII_1/notes LOAD \
""
simundump kenz /kinetics/PSD/actCaMKII/CaMKII_2 0 0 0 0 0 6 0.083325 8 2 0 0 \
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
simundump kpool /kinetics/PSD/PP1_PSD/CaN 0 0 1 1 6 6 0 0 6 0 \
  /kinetics/geometry 1 yellow -1 33 0
simundump text /kinetics/PSD/PP1_PSD/CaN/notes 0 ""
call /kinetics/PSD/PP1_PSD/CaN/notes LOAD \
""
simundump kreac /kinetics/PSD/PP1_PSD/CaM-Bind-CaN 0 7.6667 0.5 "" white \
  yellow 21 40 0
simundump text /kinetics/PSD/PP1_PSD/CaM-Bind-CaN/notes 0 ""
call /kinetics/PSD/PP1_PSD/CaM-Bind-CaN/notes LOAD \
""
simundump kpool /kinetics/PSD/PP1_PSD/Ca2_CaN 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry blue yellow 12 33 0
simundump text /kinetics/PSD/PP1_PSD/Ca2_CaN/notes 0 ""
call /kinetics/PSD/PP1_PSD/Ca2_CaN/notes LOAD \
""
simundump kreac /kinetics/PSD/PP1_PSD/Ca_bind_CaN 0 0.69444 1 "" white yellow \
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
simundump kenz /kinetics/PSD/basal_phosphatase/P1 0 0 0 0 0 60 0.62499 \
  0.59998 0.15 0 0 "" red 61 "" -6 -1 0
simundump text /kinetics/PSD/basal_phosphatase/P1/notes 0 ""
call /kinetics/PSD/basal_phosphatase/P1/notes LOAD \
""
simundump kenz /kinetics/PSD/basal_phosphatase/P2 0 0 0 0 0 60 0.62499 \
  0.59998 0.15 0 0 "" red 61 "" -2 -1 0
simundump text /kinetics/PSD/basal_phosphatase/P2/notes 0 ""
call /kinetics/PSD/basal_phosphatase/P2/notes LOAD \
""
simundump kpool /kinetics/PSD/PKA-active 1 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry yellow 20 14 17 0
simundump text /kinetics/PSD/PKA-active/notes 0 ""
call /kinetics/PSD/PKA-active/notes LOAD \
""
simundump kenz /kinetics/PSD/PKA-active/PKA-phosph-I1 0 0 0 0 0 54.001 \
  0.012345 4 1 0 0 "" red 27 "" 27 19 0
simundump text /kinetics/PSD/PKA-active/PKA-phosph-I1/notes 0 ""
call /kinetics/PSD/PKA-active/PKA-phosph-I1/notes LOAD \
""
simundump group /kinetics/PSD/CaMKII_PSD 0 33 20 x 0 0 "" CaMKII_PSD \
  defaultfile.g 0 0 0 39 32 0
simundump text /kinetics/PSD/CaMKII_PSD/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/tot-auto 0 0 2 0.5 3 12 0 0 6 0 \
  /kinetics/geometry 29 33 74 36 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-auto/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/tot-auto/notes LOAD \
""
simundump kenz /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph 0 0 0 0 0 7.732 \
  0.066667 8 2 0 0 "" red 29 "" 72 23 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph/notes LOAD \
""
simundump kenz /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286 0 0 0 0 0 7.732 \
  0.016667 2 0.5 0 0 "" red 29 "" 59 23 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286/notes LOAD \
""
simundump kenz /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305 0 0 0 0 0 54.001 \
  0.2 24 6 0 0 "" red 29 "" 78 23 0
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
  0 7.732 0.10417 8 2 0 0 "" red 47 "" 72 20 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph/notes 0 \
  ""
call /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph/notes LOAD \
""
simundump kenz /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305 0 0 0 0 0 \
  54.001 0.3125 24 6 0 0 "" red 47 "" 78 20 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305/notes LOAD \
""
simundump kenz /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286 0 0 0 0 0 \
  7.732 0.026042 2 0.5 0 0 "" red 47 "" 60 20 0
simundump text /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/basal_CaMKII 0 0 0.5 0.5 3 3 0 0 6 0 \
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
simundump kreac /kinetics/PSD/CaMKII_PSD/transloc_2 0 0.002 0 "" white 33 56 \
  12 0
simundump text /kinetics/PSD/CaMKII_PSD/transloc_2/notes 0 \
  "Same as for transloc_1\n\n\n"
call /kinetics/PSD/CaMKII_PSD/transloc_2/notes LOAD \
"Same as for transloc_1" \
"" \
"" \
""
simundump kreac /kinetics/PSD/CaMKII_PSD/transloc_1 0 0.002 0 "" white 33 63 \
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
simundump kreac /kinetics/PSD/CaMKII_PSD/back_2 0 0.04 6.6667e-06 "" white 33 \
  89 13 0
simundump text /kinetics/PSD/CaMKII_PSD/back_2/notes 0 "Same as for back_1\n"
call /kinetics/PSD/CaMKII_PSD/back_2/notes LOAD \
"Same as for back_1" \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM 0 0 0 0 0 0 0 0 6 \
  0 /kinetics/geometry 47 33 56 22 0
simundump text /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry 50 33 63 22 0
simundump text /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/sCaMKII 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry 62 33 75 22 0
simundump text /kinetics/PSD/CaMKII_PSD/sCaMKII/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/sCaMKII/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/sCaMKII_p_p_p 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry 0 33 81 22 0
simundump text /kinetics/PSD/CaMKII_PSD/sCaMKII_p_p_p/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/sCaMKII_p_p_p/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/sCaMKII-thr305 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry 30 33 88 22 0
simundump text /kinetics/PSD/CaMKII_PSD/sCaMKII-thr305/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/sCaMKII-thr305/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/CaMKII-CaM 0 1e-12 0 0 0 0 0 0 6 0 \
  /kinetics/geometry[7] 50 black 66 17 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII-CaM/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/CaMKII-CaM/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/CaMKII_p_p_p 0 1e-12 0 0 0 0 0 0 6 0 \
  /kinetics/geometry[7] 2 black 84 17 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII_p_p_p/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/CaMKII_p_p_p/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM 0 1e-12 0 0 0 0 0 \
  0 6 0 /kinetics/geometry[7] 46 black 58 17 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/CaMKII 0 5e-13 0 0 0 0 0 0 6 0 \
  /kinetics/geometry[7] 61 black 78 17 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/CaMKII/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/CaMKII-thr305 0 1e-12 0 0 0 0 0 0 6 \
  0 /kinetics/geometry[7] 36 black 91 17 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII-thr305/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/CaMKII-thr305/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry 57 33 69 22 0
simundump text /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286/notes LOAD \
""
simundump kpool /kinetics/PSD/CaMKII_PSD/CaMKII-thr286 0 1e-12 0 0 0 0 0 0 6 \
  0 /kinetics/geometry[7] 54 black 73 17 0
simundump text /kinetics/PSD/CaMKII_PSD/CaMKII-thr286/notes 0 ""
call /kinetics/PSD/CaMKII_PSD/CaMKII-thr286/notes LOAD \
""
simundump kpool /kinetics/PSD/PP2A 1 0 1 1 6 6 0 0 6 0 /kinetics/geometry red \
  20 45 17 0
simundump text /kinetics/PSD/PP2A/notes 0 ""
call /kinetics/PSD/PP2A/notes LOAD \
""
simundump kenz /kinetics/PSD/PP2A/PP2A-dephosph-PP1-I_p 1 0 0 0 0 54.001 \
  0.0013289 0.92593 0.22222 0 0 "" red red "" 29 27 0
simundump text /kinetics/PSD/PP2A/PP2A-dephosph-PP1-I_p/notes 0 \
  "k1 changed from 3.3e-6 to 6.6e-6\n"
call /kinetics/PSD/PP2A/PP2A-dephosph-PP1-I_p/notes LOAD \
"k1 changed from 3.3e-6 to 6.6e-6" \
""
simundump kenz /kinetics/PSD/PP2A/PP2A-dephosph-I1 1 0 0 0 0 54.001 0.0013289 \
  0.92593 0.22222 0 0 "" red red "" 27 20 0
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
simundump kpool /kinetics/PSD/CaM/CaM 1 5e-13 20 20 120 120 0 0 6 0 \
  /kinetics/geometry pink blue 35 39 0
simundump text /kinetics/PSD/CaM/CaM/notes 0 \
  "There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)\nSay 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying\nit is comparable to CaMK levels. \n"
call /kinetics/PSD/CaM/CaM/notes LOAD \
"There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)" \
"Say 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying" \
"it is comparable to CaMK levels. " \
""
simundump kreac /kinetics/PSD/CaM/CaM-Ca3-bind-Ca 1 0.6 10 "" white blue 49 \
  36 0
simundump text /kinetics/PSD/CaM/CaM-Ca3-bind-Ca/notes 0 \
  "Use K3 = 21.5 uM here from Stemmer and Klee table 3.\nkb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10\n24 Sep 2015\nThis is a bit too low affinity. Changing to match K2 at \nKd = 2.8."
call /kinetics/PSD/CaM/CaM-Ca3-bind-Ca/notes LOAD \
"Use K3 = 21.5 uM here from Stemmer and Klee table 3." \
"kb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10" \
"24 Sep 2015" \
"This is a bit too low affinity. Changing to match K2 at " \
"Kd = 2.8."
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
simundump kpool /kinetics/PSD/CaM/Ca_input 0 0 0.08 0.08 0.48 0.48 0 0 6 4 \
  /kinetics/geometry[3] blue black 49 33 0
simundump text /kinetics/PSD/CaM/Ca_input/notes 0 ""
call /kinetics/PSD/CaM/Ca_input/notes LOAD \
""
simundump kreac /kinetics/PSD/CaM/Ca_stim 0 50 50 "" white black 46 31 0
simundump text /kinetics/PSD/CaM/Ca_stim/notes 0 ""
call /kinetics/PSD/CaM/Ca_stim/notes LOAD \
""
simundump kpool /kinetics/PSD/CaM_CaN 1 0 0 0 0 0 0 0 6 0 /kinetics/geometry \
  1 20 30 33 0
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
simundump kenz /kinetics/PSD/CaM_CaN/CaN_1 0 0 0 0 0 6 0.033333 8 2 0 0 "" \
  red pink "" -6 -4 0
simundump text /kinetics/PSD/CaM_CaN/CaN_1/notes 0 ""
call /kinetics/PSD/CaM_CaN/CaN_1/notes LOAD \
""
simundump kenz /kinetics/PSD/CaM_CaN/CaN_2 0 0 0 0 0 6 0.033333 8 2 0 0 "" \
  red pink "" -2 -4 0
simundump text /kinetics/PSD/CaM_CaN/CaN_2/notes 0 ""
call /kinetics/PSD/CaM_CaN/CaN_2/notes LOAD \
""
simundump group /kinetics/DEND 0 13 black x 0 0 "" DEND defaultfile.g 0 0 0 \
  11 -30 0
simundump text /kinetics/DEND/notes 0 ""
call /kinetics/DEND/notes LOAD \
""
simundump group /kinetics/DEND/CaM 1 blue 13 x 0 0 "" defaultfile \
  defaultfile.g 0 fbb0ff81553508bc01f3dd51428742fb 0 34 -32 0
simundump text /kinetics/DEND/CaM/notes 0 ""
call /kinetics/DEND/CaM/notes LOAD \
""
simundump kpool /kinetics/DEND/CaM/CaM 1 5e-13 1 1 600 600 0 0 600 0 \
  /kinetics/geometry pink blue 38 -30 0
simundump text /kinetics/DEND/CaM/CaM/notes 0 \
  "There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)\nSay 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying\nit is comparable to CaMK levels. \n"
call /kinetics/DEND/CaM/CaM/notes LOAD \
"There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)" \
"Say 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying" \
"it is comparable to CaMK levels. " \
""
simundump kreac /kinetics/DEND/CaM/CaM-Ca3-bind-Ca 1 0.006 10 "" white blue \
  52 -33 0
simundump text /kinetics/DEND/CaM/CaM-Ca3-bind-Ca/notes 0 \
  "Use K3 = 21.5 uM here from Stemmer and Klee table 3.\nkb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10"
call /kinetics/DEND/CaM/CaM-Ca3-bind-Ca/notes LOAD \
"Use K3 = 21.5 uM here from Stemmer and Klee table 3." \
"kb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10"
simundump kpool /kinetics/DEND/CaM/CaM-Ca3 1 1e-12 0 0 0 0 0 0 600 0 \
  /kinetics/geometry hotpink blue 50 -30 0
simundump text /kinetics/DEND/CaM/CaM-Ca3/notes 0 ""
call /kinetics/DEND/CaM/CaM-Ca3/notes LOAD \
""
simundump kreac /kinetics/DEND/CaM/CaM-bind-Ca 1 0.014141 8.4853 "" white \
  blue 40 -33 0
simundump text /kinetics/DEND/CaM/CaM-bind-Ca/notes 0 \
  "Lets use the fast rate consts here. Since the rates are so different, I am not\nsure whether the order is relevant. These correspond to the TR2C fragment.\nWe use the Martin et al rates here, plus the Drabicowski binding consts.\nAll are scaled by 3X to cell temp.\nkf = 2e-10 kb = 72\nStemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11.\nIf kb=72, kf = 2e-10 (Exactly the same !)...."
call /kinetics/DEND/CaM/CaM-bind-Ca/notes LOAD \
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
simundump kpool /kinetics/DEND/CaM/CaM-Ca2 1 1e-12 0 0 0 0 0 0 600 0 \
  /kinetics/geometry pink blue 46 -30 0
simundump text /kinetics/DEND/CaM/CaM-Ca2/notes 0 \
  "This is the intermediate where the TR2 end (the high-affinity end) has\nbound the Ca but the TR1 end has not."
call /kinetics/DEND/CaM/CaM-Ca2/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
simundump kreac /kinetics/DEND/CaM/CaM-Ca2-bind-Ca 1 0.006 10 "" white blue \
  48 -33 0
simundump text /kinetics/DEND/CaM/CaM-Ca2-bind-Ca/notes 0 \
  "K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get\nkb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10\n"
call /kinetics/DEND/CaM/CaM-Ca2-bind-Ca/notes LOAD \
"K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get" \
"kb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10" \
""
simundump kreac /kinetics/DEND/CaM/CaM-Ca-bind-Ca 1 0.014141 8.4853 "" white \
  blue 44 -33 0
simundump text /kinetics/DEND/CaM/CaM-Ca-bind-Ca/notes 0 \
  "Lets use the fast rate consts here. Since the rates are so different, I am not\nsure whether the order is relevant. These correspond to the TR2C fragment.\nWe use the Martin et al rates here, plus the Drabicowski binding consts.\nAll are scaled by 3X to cell temp.\nkf = 2e-10 kb = 72\nStemmer & Klee: K1=.9, K2=1.1. Assume 1.0uM for both. kb/kf=3.6e11.\nIf kb=72, kf = 2e-10 (Exactly the same !)...."
call /kinetics/DEND/CaM/CaM-Ca-bind-Ca/notes LOAD \
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
simundump kpool /kinetics/DEND/CaM/CaM-Ca 1 1e-12 0 0 0 0 0 0 600 0 \
  /kinetics/geometry pink blue 42 -30 0
simundump text /kinetics/DEND/CaM/CaM-Ca/notes 0 \
  "This is the intermediate where the TR2 end (the high-affinity end) has\nbound the Ca but the TR1 end has not."
call /kinetics/DEND/CaM/CaM-Ca/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
simundump kpool /kinetics/DEND/CaM/CaM-Ca4 1 1e-12 0 0 0 0 0 0 600 0 \
  /kinetics/geometry blue blue 55 -30 0
simundump text /kinetics/DEND/CaM/CaM-Ca4/notes 0 ""
call /kinetics/DEND/CaM/CaM-Ca4/notes LOAD \
""
simundump kreac /kinetics/DEND/CaM/CaM_x2 0 10 10 "" white black 28 -30 0
simundump text /kinetics/DEND/CaM/CaM_x2/notes 0 ""
call /kinetics/DEND/CaM/CaM_x2/notes LOAD \
""
simundump kpool /kinetics/DEND/CaM/CaM_xchange 0 2e-11 0 0 0 0 0 0 600 0 \
  /kinetics/geometry[1] blue blue 19 -32 0
simundump text /kinetics/DEND/CaM/CaM_xchange/notes 0 ""
call /kinetics/DEND/CaM/CaM_xchange/notes LOAD \
""
simundump group /kinetics/DEND/CaMKII_BULK 0 33 13 x 0 0 "" CaMKII_BULK \
  defaultfile.g 0 0 0 42 -37 0
simundump text /kinetics/DEND/CaMKII_BULK/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/notes LOAD \
""
simundump kpool /kinetics/DEND/CaMKII_BULK/tot-auto 0 0 0.18 0.5 300 108 0 0 \
  600 0 /kinetics/geometry 29 33 77 -33 0
simundump text /kinetics/DEND/CaMKII_BULK/tot-auto/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/tot-auto/notes LOAD \
""
simundump kenz /kinetics/DEND/CaMKII_BULK/tot-auto/auton-autoph 0 0 0 0 0 \
  69.588 0.00066667 8 2 0 0 "" red 29 "" 75 -46 0
simundump text /kinetics/DEND/CaMKII_BULK/tot-auto/auton-autoph/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/tot-auto/auton-autoph/notes LOAD \
""
simundump kenz /kinetics/DEND/CaMKII_BULK/tot-auto/auton-286 0 0 0 0 0 69.588 \
  0.00016667 2 0.5 0 0 "" red 29 "" 62 -46 0
simundump text /kinetics/DEND/CaMKII_BULK/tot-auto/auton-286/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/tot-auto/auton-286/notes LOAD \
""
simundump kenz /kinetics/DEND/CaMKII_BULK/tot-auto/auton-305 0 0 0 0 0 486.01 \
  0.002 24 6 0 0 "" red 29 "" 81 -46 0
simundump text /kinetics/DEND/CaMKII_BULK/tot-auto/auton-305/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/tot-auto/auton-305/notes LOAD \
""
simundump kpool /kinetics/DEND/CaMKII_BULK/tot_CaMKII 0 0 0.18 1.18 708 108 0 \
  0 600 0 /kinetics/geometry 23 33 87 -33 0
simundump text /kinetics/DEND/CaMKII_BULK/tot_CaMKII/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/tot_CaMKII/notes LOAD \
""
simundump kpool /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII 0 0 0 0 0 0 0 0 600 \
  0 /kinetics/geometry 47 33 64 -36 0
simundump text /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/notes LOAD \
""
simundump kenz /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph 0 0 0 \
  0 0 69.588 0.0010417 8 2 0 0 "" red 47 "" 75 -49 0
simundump text /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph/notes 0 \
  ""
call /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph/notes LOAD \
""
simundump kenz /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 0 0 0 0 \
  0 486.01 0.003125 24 6 0 0 "" red 47 "" 81 -49 0
simundump text /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305/notes 0 \
  ""
call /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305/notes LOAD \
""
simundump kenz /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 0 0 0 0 \
  0 69.588 0.00026042 2 0.5 0 0 "" red 47 "" 63 -49 0
simundump text /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286/notes 0 \
  ""
call /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286/notes LOAD \
""
simundump kpool /kinetics/DEND/CaMKII_BULK/basal_CaMKII 0 0 0.5 0.5 300 300 0 \
  0 600 0 /kinetics/geometry blue 33 84 -36 0
simundump text /kinetics/DEND/CaMKII_BULK/basal_CaMKII/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/basal_CaMKII/notes LOAD \
""
simundump kreac /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-bind-CaM 0 0.10945 \
  0.0022 "" white 33 57 -41 0
simundump text /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-bind-CaM/notes 0 \
  "Same values as for the main compartment\nCan the main compartment pool of Ca/CaM be used?"
call /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-bind-CaM/notes LOAD \
"Same values as for the main compartment" \
"Can the main compartment pool of Ca/CaM be used?"
simundump kreac /kinetics/DEND/CaMKII_BULK/CaMKII-bind-CaM 0 0.10945 2.2 "" \
  white 33 68 -41 0
simundump text /kinetics/DEND/CaMKII_BULK/CaMKII-bind-CaM/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/CaMKII-bind-CaM/notes LOAD \
""
simundump kpool /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-CaM 0 1e-12 0 0 0 0 \
  0 0 600 0 /kinetics/geometry 47 33 59 -47 0
simundump text /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-CaM/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-CaM/notes LOAD \
""
simundump kpool /kinetics/DEND/CaMKII_BULK/CaMKII-CaM 0 1e-12 0 0 0 0 0 0 600 \
  0 /kinetics/geometry 50 33 66 -47 0
simundump text /kinetics/DEND/CaMKII_BULK/CaMKII-CaM/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/CaMKII-CaM/notes LOAD \
""
simundump kpool /kinetics/DEND/CaMKII_BULK/CaMKII-thr286 0 1e-12 0 0 0 0 0 0 \
  600 0 /kinetics/geometry 57 33 72 -47 0
simundump text /kinetics/DEND/CaMKII_BULK/CaMKII-thr286/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/CaMKII-thr286/notes LOAD \
""
simundump kpool /kinetics/DEND/CaMKII_BULK/CaMKII 0 5e-13 1 1 600 600 0 0 600 \
  0 /kinetics/geometry 62 33 78 -47 0
simundump text /kinetics/DEND/CaMKII_BULK/CaMKII/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/CaMKII/notes LOAD \
""
simundump kpool /kinetics/DEND/CaMKII_BULK/CaMKII-thr305 0 1e-12 0 0 0 0 0 0 \
  600 0 /kinetics/geometry 30 33 91 -47 0
simundump text /kinetics/DEND/CaMKII_BULK/CaMKII-thr305/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/CaMKII-thr305/notes LOAD \
""
simundump kpool /kinetics/DEND/CaMKII_BULK/CaMKII_p_p_p 0 1e-12 0 0 0 0 0 0 \
  600 0 /kinetics/geometry 0 33 84 -47 0
simundump text /kinetics/DEND/CaMKII_BULK/CaMKII_p_p_p/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/CaMKII_p_p_p/notes LOAD \
""
simundump kreac /kinetics/DEND/CaMKII_BULK/CaMKII_x2 0 10 10 "" white black \
  95 -49 0
simundump text /kinetics/DEND/CaMKII_BULK/CaMKII_x2/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/CaMKII_x2/notes LOAD \
""
simundump kpool /kinetics/DEND/CaMKII_BULK/CaMKII_xchange 0 2e-11 0 0 0 0 0 0 \
  600 0 /kinetics/geometry[5] blue black 99 -47 0
simundump text /kinetics/DEND/CaMKII_BULK/CaMKII_xchange/notes 0 ""
call /kinetics/DEND/CaMKII_BULK/CaMKII_xchange/notes LOAD \
""
simundump kpool /kinetics/DEND/actCaMKII 0 0 0.18 0.18 108 108 0 0 600 0 \
  /kinetics/geometry 12 5 73 -36 0
simundump text /kinetics/DEND/actCaMKII/notes 0 ""
call /kinetics/DEND/actCaMKII/notes LOAD \
""
simundump kpool /kinetics/DEND/PP2A 1 0 1.2 1.2 720 720 0 0 600 0 \
  /kinetics/geometry 62 5 47 -44 0
simundump text /kinetics/DEND/PP2A/notes 0 \
  "Strack et al JBC 1997 show that PP2A is the primary\nphosphatase acting on CaMKII in the bulk."
call /kinetics/DEND/PP2A/notes LOAD \
"Strack et al JBC 1997 show that PP2A is the primary" \
"phosphatase acting on CaMKII in the bulk."
simundump kenz /kinetics/DEND/PP2A/Deph-thr286 0 0 0 0 0 486.01 0.0033334 8 2 \
  0 0 "" red 62 "" 64 -44 0
simundump text /kinetics/DEND/PP2A/Deph-thr286/notes 0 ""
call /kinetics/DEND/PP2A/Deph-thr286/notes LOAD \
""
simundump kenz /kinetics/DEND/PP2A/Deph-thr286b 0 0 0 0 0 486.01 0.0033334 8 \
  2 0 0 "" red 62 "" 74 -44 0
simundump text /kinetics/DEND/PP2A/Deph-thr286b/notes 0 ""
call /kinetics/DEND/PP2A/Deph-thr286b/notes LOAD \
""
simundump kenz /kinetics/DEND/PP2A/Deph-thr305 0 0 0 0 0 486.01 0.0033334 8 2 \
  0 0 "" red 62 "" 81 -44 0
simundump text /kinetics/DEND/PP2A/Deph-thr305/notes 0 ""
call /kinetics/DEND/PP2A/Deph-thr305/notes LOAD \
""
simundump kenz /kinetics/DEND/PP2A/Deph-thr286c 0 0 0 0 0 486.01 0.00083332 2 \
  0.5 0 0 "" red 62 "" 91 -44 0
simundump text /kinetics/DEND/PP2A/Deph-thr286c/notes 0 ""
call /kinetics/DEND/PP2A/Deph-thr286c/notes LOAD \
""
simundump kenz /kinetics/DEND/PP2A/Deph-thr305a 0 0 0 0 0 486.01 0.0033334 8 \
  2 0 0 "" red 62 "" 86 -44 0
simundump text /kinetics/DEND/PP2A/Deph-thr305a/notes 0 ""
call /kinetics/DEND/PP2A/Deph-thr305a/notes LOAD \
""
simundump kpool /kinetics/DEND/Ca_input 0 0 0.08 0.08 48 48 0 0 600 4 \
  /kinetics/geometry[4] blue black 53 -37 0
simundump text /kinetics/DEND/Ca_input/notes 0 ""
call /kinetics/DEND/Ca_input/notes LOAD \
""
simundump kreac /kinetics/DEND/Ca_stim 0 50 50 "" white black 49 -38 0
simundump text /kinetics/DEND/Ca_stim/notes 0 ""
call /kinetics/DEND/Ca_stim/notes LOAD \
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
simundump kpool /kinetics/SPINE/CaM/CaM 1 5e-13 20 20 1080 1080 0 0 54 0 \
  /kinetics/geometry pink blue 37 4 0
simundump text /kinetics/SPINE/CaM/CaM/notes 0 \
  "There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)\nSay 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying\nit is comparable to CaMK levels. \n"
call /kinetics/SPINE/CaM/CaM/notes LOAD \
"There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)" \
"Say 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying" \
"it is comparable to CaMK levels. " \
""
simundump kreac /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca 1 0.066667 10 "" white \
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
simundump kreac /kinetics/SPINE/CaM/CaM_x2 0 1 100 "" white black 34 10 0
simundump text /kinetics/SPINE/CaM/CaM_x2/notes 0 ""
call /kinetics/SPINE/CaM/CaM_x2/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaM/CaM_xchange 0 2e-11 0 0 0 0 0 0 54 0 \
  /kinetics/geometry[2] blue black 29 7 0
simundump text /kinetics/SPINE/CaM/CaM_xchange/notes 0 ""
call /kinetics/SPINE/CaM/CaM_xchange/notes LOAD \
""
simundump group /kinetics/SPINE/CaMKII_BULK 0 33 5 x 0 0 "" CaMKII_BULK \
  defaultfile.g 0 0 0 41 -3 0
simundump text /kinetics/SPINE/CaMKII_BULK/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/tot-auto 0 0 2 0.5 27 108 0 0 54 \
  0 /kinetics/geometry 29 33 76 1 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-auto/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/tot-auto/notes LOAD \
""
simundump kenz /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-autoph 0 0 0 0 0 \
  69.588 0.0074074 8 2 0 0 "" red 29 "" 74 -12 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-autoph/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-autoph/notes LOAD \
""
simundump kenz /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-286 0 0 0 0 0 \
  69.588 0.0018519 2 0.5 0 0 "" red 29 "" 61 -12 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-286/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-286/notes LOAD \
""
simundump kenz /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-305 0 0 0 0 0 \
  486.01 0.022222 24 6 0 0 "" red 29 "" 80 -12 0
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
  0 0 0 69.588 0.011574 8 2 0 0 "" red 47 "" 74 -15 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph/notes 0 \
  ""
call /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph/notes LOAD \
""
simundump kenz /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 0 0 0 0 \
  0 486.01 0.034722 24 6 0 0 "" red 47 "" 80 -15 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305/notes 0 \
  ""
call /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305/notes LOAD \
""
simundump kenz /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 0 0 0 0 \
  0 69.588 0.0028935 2 0.5 0 0 "" red 47 "" 62 -15 0
simundump text /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286/notes 0 \
  ""
call /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/basal_CaMKII 0 0 0.5 0.5 27 27 0 \
  0 54 0 /kinetics/geometry blue 33 83 -2 0
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
simundump kpool /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM 0 1e-12 0 0 0 0 \
  0 0 54 0 /kinetics/geometry 47 33 58 -13 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286-CaM/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM 0 1e-12 0 0 0 0 0 0 54 \
  0 /kinetics/geometry 50 33 65 -13 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286 0 1e-12 0 0 0 0 0 0 \
  54 0 /kinetics/geometry 57 33 71 -13 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII-thr286/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/CaMKII 0 5e-13 20 20 1080 1080 0 \
  0 54 0 /kinetics/geometry 62 33 77 -13 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305 0 1e-12 0 0 0 0 0 0 \
  54 0 /kinetics/geometry 30 33 90 -13 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p 0 1e-12 0 0 0 0 0 0 \
  54 0 /kinetics/geometry 0 33 83 -13 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p/notes LOAD \
""
simundump kreac /kinetics/SPINE/CaMKII_BULK/CaMKII_x2 0 1 100 "" white black \
  92 -15 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII_x2/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII_x2/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaMKII_BULK/CaMKII_xchange 0 2e-11 0 0 0 0 0 \
  0 54 0 /kinetics/geometry[6] blue black 98 -13 0
simundump text /kinetics/SPINE/CaMKII_BULK/CaMKII_xchange/notes 0 ""
call /kinetics/SPINE/CaMKII_BULK/CaMKII_xchange/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaM_CaN 1 0 0 0 0 0 0 0 54 0 \
  /kinetics/geometry 1 5 32 -2 0
simundump text /kinetics/SPINE/CaM_CaN/notes 0 ""
call /kinetics/SPINE/CaM_CaN/notes LOAD \
""
simundump kpool /kinetics/SPINE/actCaMKII 0 0 2 2 108 108 0 0 54 0 \
  /kinetics/geometry 12 5 72 -2 0
simundump text /kinetics/SPINE/actCaMKII/notes 0 ""
call /kinetics/SPINE/actCaMKII/notes LOAD \
""
simundump kpool /kinetics/SPINE/PP2A 1 0 5 5 270 270 0 0 54 0 \
  /kinetics/geometry 62 5 46 -10 0
simundump text /kinetics/SPINE/PP2A/notes 0 \
  "Strack et al JBC 1997 show that PP2A is the primary\nphosphatase acting on CaMKII in the bulk."
call /kinetics/SPINE/PP2A/notes LOAD \
"Strack et al JBC 1997 show that PP2A is the primary" \
"phosphatase acting on CaMKII in the bulk."
simundump kenz /kinetics/SPINE/PP2A/Deph-thr286 0 0 0 0 0 486.01 0.037038 8 2 \
  0 0 "" red 62 "" 63 -10 0
simundump text /kinetics/SPINE/PP2A/Deph-thr286/notes 0 ""
call /kinetics/SPINE/PP2A/Deph-thr286/notes LOAD \
""
simundump kenz /kinetics/SPINE/PP2A/Deph-thr286b 0 0 0 0 0 486.01 0.037038 8 \
  2 0 0 "" red 62 "" 73 -10 0
simundump text /kinetics/SPINE/PP2A/Deph-thr286b/notes 0 ""
call /kinetics/SPINE/PP2A/Deph-thr286b/notes LOAD \
""
simundump kenz /kinetics/SPINE/PP2A/Deph-thr305 0 0 0 0 0 486.01 0.037038 8 2 \
  0 0 "" red 62 "" 80 -10 0
simundump text /kinetics/SPINE/PP2A/Deph-thr305/notes 0 ""
call /kinetics/SPINE/PP2A/Deph-thr305/notes LOAD \
""
simundump kenz /kinetics/SPINE/PP2A/Deph-thr286c 0 0 0 0 0 486.01 0.0092593 2 \
  0.5 0 0 "" red 62 "" 90 -10 0
simundump text /kinetics/SPINE/PP2A/Deph-thr286c/notes 0 ""
call /kinetics/SPINE/PP2A/Deph-thr286c/notes LOAD \
""
simundump kenz /kinetics/SPINE/PP2A/Deph-thr305a 0 0 0 0 0 486.01 0.037038 8 \
  2 0 0 "" red 62 "" 85 -10 0
simundump text /kinetics/SPINE/PP2A/Deph-thr305a/notes 0 ""
call /kinetics/SPINE/PP2A/Deph-thr305a/notes LOAD \
""
simundump group /kinetics/SPINE/CaN_BULK 1 yellow 5 x 0 0 "" CaN_BULK \
  /home2/bhalla/scripts/modules/PP1_PSD_0.g 0 \
  62a600ae10e53f567e47decb4f2b6488 0 14 -8 0
simundump text /kinetics/SPINE/CaN_BULK/notes 0 ""
call /kinetics/SPINE/CaN_BULK/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaN_BULK/CaN 0 0 1 1 54 54 0 0 54 0 \
  /kinetics/geometry 1 yellow 4 -2 0
simundump text /kinetics/SPINE/CaN_BULK/CaN/notes 0 ""
call /kinetics/SPINE/CaN_BULK/CaN/notes LOAD \
""
simundump kreac /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN 0 0.85185 0.5 "" white \
  yellow 23 5 0
simundump text /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN/notes 0 \
  "From Quintana et al 2005"
call /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN/notes LOAD \
"From Quintana et al 2005"
simundump kreac /kinetics/SPINE/CaN_BULK/Ca_bind_CaN 0 0.0085734 1 "" white \
  yellow 8 5 0
simundump text /kinetics/SPINE/CaN_BULK/Ca_bind_CaN/notes 0 ""
call /kinetics/SPINE/CaN_BULK/Ca_bind_CaN/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaN_BULK/Ca2_CaN 0 0 0 0 0 0 0 0 54 0 \
  /kinetics/geometry[1] blue yellow 15 -2 0
simundump text /kinetics/SPINE/CaN_BULK/Ca2_CaN/notes 0 ""
call /kinetics/SPINE/CaN_BULK/Ca2_CaN/notes LOAD \
""
simundump kpool /kinetics/Ca 1 1e-11 0.08 0.08 48 48 0 0 600 0 \
  /kinetics/geometry red 13 46 -36 0
simundump text /kinetics/Ca/notes 0 ""
call /kinetics/Ca/notes LOAD \
""
simundump xgraph /graphs/conc1 0 0 2000 0 4 0
simundump xgraph /graphs/conc2 0 0 2000 0 16.804 0
simundump xplot /graphs/conc1/Ca.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0 0 1
simundump xplot /graphs/conc1/actCaMKII.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 12 0 0 1
simundump xplot /graphs/conc1/tot_PSD_R.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc1/CaM-Ca4.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc2/Ca.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0 0 1
simundump xplot /graphs/conc2/actCaMKII.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 12 0 0 1
simundump xplot /graphs/conc2/CaM-Ca4.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xgraph /moregraphs/conc3 0 0 2000 0 4 0
simundump xgraph /moregraphs/conc4 0 0 2000 0 4 0
simundump xplot /moregraphs/conc3/Ca.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0 0 1
simundump xplot /moregraphs/conc3/actCaMKII.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 12 0 0 1
simundump xplot /moregraphs/conc3/CaM-Ca4.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xcoredraw /edit/draw 0 -10 101 -51 42
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
"" \
"22 Aug 2015. CaMKII_merged32.g: Duplicated spine head CaMKII and" \
"ancillary reactions, into Dend. Will need to figure out how to " \
"keep hihger CaMKII and CaM levels in spine head rather than" \
"dend." \
"23 Aug 2015. CaMKII_merged33.g: Added pools and reactions to " \
"sustain the gradients of CaM and CaMKII between DEND and SPINE." \
"Needed because we have diffusion of CaM, CaMKII and various " \
"converted forms of these molecules." \
"" \
"23 Aug 2015. CaMKII_merged34.g: Added Ca_input for PSD and DEND." \
"" \
"24 Aug 2015. CaMKII_merged35.g" \
"Increased CaDiffusion rate to 10 microns^2/sec. " \
"Increased Ca_stim Kf and Kb to 50 from 20." \
"" \
"13 Sep 2015. CaMKII_merged36.g" \
"Set PSD/PKA-active nInit to zero. It was a tiny fractional value" \
"leading to a possible discrepancy between stochastic and determ" \
"calculations." \
"" \
"14 Sep 2015. CaMKII_merged37.g" \
"PP2A in the PSD had some tiny nComplexInits in its enz sites." \
"Zeroed." \
"" \
"24 Sep 2015. CaMKII_merged38.g" \
"Fixed some discrepancies in rates in the Dend compartment. Seems" \
"that vol scaling had not happened properly." \
"Also raised affinity of final stage of CaM.Ca3 binding to Ca to" \
"the same value as for CaM.Ca2 binding." \
"" \
"25 Sep 2015. CaMKII_merged39.g" \
"Fixed CaM_xchange reactions, also similar for CaMKII. These set" \
"up the higher conc in the spine and PSD, but were not effective." \
"" \
"26 Sep 2015. CaMKII_merged42.g" \
"Moved PP1_1 and PP1_2 enzymes to CaM_CaN" \
"Some rates changes." \
"CaMKII_merged42a.g: moved /kinetics/DEND/Ca to /kinetics/Ca so" \
"that the system isn't confused during diffusions setup." \
"" \
"29 Sep 2015. CaMKII_merged43.g" \
"Added back the CaMKII exchange reaction, and CaMKII diffusion" \
"between dend and spine. Also moved the CaM_xchange reaction to" \
"the dend as its substrate is there." \
"CaMKII_merged44.g: Lower Keq rate of both xchangeReactions from" \
"10 to 2. kb now 0.5, kf still 1." \
"" \
"02 Oct 2015: CaMKII_merged50.g. Moved NMDAR to Spine rather than" \
"PSD, to keep both substrates in same compt. Also moved reac to" \
"same compt as substrates." \
"" \
"05 Nov 2015: CaMKII_merged57.g. Put all the translocation reactions" \
"in the PSD itself, now the CaMKII phospho forms just diffuse" \
"into the PSD if they want to translocate. Then they bind to" \
"NMDAR and form the scaffolded version." \
"CaMKII_merged58.g: There was too little CaM and CaMKII in the " \
"spine. Increased the rate of the reactions pumping them in." \
"" \
"09 Nov 2015" \
"CaMKII_merged59.g: 3 major changes. " \
"1. Lowered basal activity of CaMKII to 0.5 uM from 1 uM." \
"2. Fixed Km of intramolecular CaMKII. Divide Km by 20 for" \
"   each of the intramolecular reactions, this brings it into the" \
"   same range as the total amount of CaMKII" \
"3. CaN is now activated by Ca2.CaM, not by Ca4.CaM." \
"CaMKII_merged60.g. Fix to Km for PP2A on CaMKII in Dend: it had" \
"   been reduced to 0.45, restored to 5 so that it is the same as" \
"   in PSD. Also doubled kcat for PP2A action on CaMKII phospho-" \
"   forms, across the board." \
"" \
"CaMKII_merged61.g: Zero out diffusion constt for all CaN forms." \
"CaMKII_merged62.g: Double PP2A rates on CaMKII phosph forms in" \
"    dend and spine. Raise back-reac rate for Ca2.CaM binding to" \
"    CaN by factor of 10." \
"" \
"10 Nov 2015. CaMKII_merged63.g: Raised PP2A levels in dend and" \
"spine head by 25%. Doubled kcat for CaMKII on PSDR. Doubled" \
"back rate for CaM binding to CaN." \
"" \
"" \
"11 Nov 2015. CaMKII_merged65.g: (64 was a dead end). Removed" \
"xchange reacs between dend and spine, which were there for setting" \
"up the steady-state conc gradient of CaM and CaMKII. Replaced" \
"with a diffusion process and local reacs to achieve gradient." \
"CaMKII_merged66.g: Changed local rates to steepen gradient." \
"CaMKII_merged67.g: Further sped up diffusion of xchange molecules" \
"to steepen gradient. Also lowered CaN affinity for CaM." \
"CaMKII_merged68.g: Lowered CaN affinity for CaM. Lowered CaMKII" \
"activity on PSDR. Increased 10x rate for NMDAR binding to CaMKII." \
"CaMKII_merged69.g: further lowered CaN affinity for CaM to ~0.1." \
"Lowered CaMKII basal activity to 0.2 uM." \
"Increased 2x further NMDAR binding to CaMKII." \
"CaMKII_merged70.g: Double CaMKII rate on PSDR." \
"" \
"13 Nov 2015. CaMKII_merged71.g. Fine-tuning to restore bistability" \
"in the PSDR. Lowered CaN kcas on PSDR to 0.8. Raised basal" \
"CaMKII back to 0.5 uM, from 0.2, in PSD." \
"" \
"14 Nov 2015. CaMKII_merged74.g. Did various tests for the" \
"bistability of the PSDR system. This one has slower CaN kcat on" \
"PSDR, from 0.8 to 0.2. Based on psdr2.g" \
"CaMKII_merged75.g. The prev one didn't acheive turnoff. Doubled" \
"CaN kcat on psdr, now at 0.4." \
"15 Nov 2015." \
"CaMKII_merged76.g. Raised kcat and Km both for CaN on PSDR, to" \
"lessen saturation." \
"CaMKII_merged77.g. Raised kcat of basal_phosphatase for PSDR from" \
"0.11 to 0.15, in order to raise threshold and lessen spontaneous" \
"turnons." \
"" \
""
addmsg /kinetics/PSD/R_S2 /kinetics/PSD/tot_PSD_R SUMTOTAL n nInit 
addmsg /kinetics/PSD/R_SpS /kinetics/PSD/tot_PSD_R SUMTOTAL n nInit 
addmsg /kinetics/PSD/R_SpSp /kinetics/PSD/tot_PSD_R SUMTOTAL n nInit 
addmsg /kinetics/PSD/actCaMKII/CaMKII_1 /kinetics/PSD/actCaMKII REAC eA B 
addmsg /kinetics/PSD/actCaMKII/CaMKII_2 /kinetics/PSD/actCaMKII REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM /kinetics/PSD/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM /kinetics/PSD/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto /kinetics/PSD/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/actCaMKII /kinetics/PSD/actCaMKII/CaMKII_1 ENZYME n 
addmsg /kinetics/PSD/R_S2 /kinetics/PSD/actCaMKII/CaMKII_1 SUBSTRATE n 
addmsg /kinetics/PSD/actCaMKII /kinetics/PSD/actCaMKII/CaMKII_2 ENZYME n 
addmsg /kinetics/PSD/R_SpS /kinetics/PSD/actCaMKII/CaMKII_2 SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286 /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286b /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305 /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305a /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286c /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/dissoc-PP1-I1 /kinetics/PSD/PP1_PSD/PP1-active REAC B A 
addmsg /kinetics/PSD/PP1_PSD/Inact-PP1 /kinetics/PSD/PP1_PSD/PP1-active REAC A B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286 ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286 SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286b ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286b SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305 ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII_p_p_p /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305 SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286c ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII_p_p_p /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286c SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305a ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr305 /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305a SUBSTRATE n 
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
addmsg /kinetics/PSD/CaM_CaN /kinetics/PSD/PP1_PSD/CaM-Bind-CaN PRODUCT n 
addmsg /kinetics/PSD/PP1_PSD/Ca2_CaN /kinetics/PSD/PP1_PSD/CaM-Bind-CaN SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca2 /kinetics/PSD/PP1_PSD/CaM-Bind-CaN SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/Ca_bind_CaN /kinetics/PSD/PP1_PSD/Ca2_CaN REAC B A 
addmsg /kinetics/PSD/PP1_PSD/CaM-Bind-CaN /kinetics/PSD/PP1_PSD/Ca2_CaN REAC A B 
addmsg /kinetics/PSD/PP1_PSD/CaN /kinetics/PSD/PP1_PSD/Ca_bind_CaN SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/Ca2_CaN /kinetics/PSD/PP1_PSD/Ca_bind_CaN PRODUCT n 
addmsg /kinetics/PSD/CaM/Ca /kinetics/PSD/PP1_PSD/Ca_bind_CaN SUBSTRATE n 
addmsg /kinetics/PSD/CaM/Ca /kinetics/PSD/PP1_PSD/Ca_bind_CaN SUBSTRATE n 
addmsg /kinetics/SPINE/iR /kinetics/PSD/move_to_PSD SUBSTRATE n 
addmsg /kinetics/PSD/R_S2 /kinetics/PSD/move_to_PSD PRODUCT n 
addmsg /kinetics/PSD/basal_phosphatase/P1 /kinetics/PSD/R_S2 MM_PRD pA 
addmsg /kinetics/PSD/CaM_CaN/CaN_1 /kinetics/PSD/R_S2 MM_PRD pA 
addmsg /kinetics/PSD/move_to_PSD /kinetics/PSD/R_S2 REAC B A 
addmsg /kinetics/PSD/actCaMKII/CaMKII_1 /kinetics/PSD/R_S2 REAC sA B 
addmsg /kinetics/PSD/basal_phosphatase/P2 /kinetics/PSD/R_SpS MM_PRD pA 
addmsg /kinetics/PSD/basal_phosphatase/P1 /kinetics/PSD/R_SpS REAC sA B 
addmsg /kinetics/PSD/CaM_CaN/CaN_2 /kinetics/PSD/R_SpS MM_PRD pA 
addmsg /kinetics/PSD/CaM_CaN/CaN_1 /kinetics/PSD/R_SpS REAC sA B 
addmsg /kinetics/PSD/actCaMKII/CaMKII_1 /kinetics/PSD/R_SpS MM_PRD pA 
addmsg /kinetics/PSD/actCaMKII/CaMKII_2 /kinetics/PSD/R_SpS REAC sA B 
addmsg /kinetics/PSD/basal_phosphatase/P2 /kinetics/PSD/R_SpSp REAC sA B 
addmsg /kinetics/PSD/CaM_CaN/CaN_2 /kinetics/PSD/R_SpSp REAC sA B 
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
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 /kinetics/PSD/CaMKII_PSD/tot-auto SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII_p_p_p /kinetics/PSD/CaMKII_PSD/tot-auto SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305 /kinetics/PSD/CaMKII_PSD/tot-auto REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286 /kinetics/PSD/CaMKII_PSD/tot-auto REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/basal_CaMKII /kinetics/PSD/CaMKII_PSD/tot-auto SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph /kinetics/PSD/CaMKII_PSD/tot-auto REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286 ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305 ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305 SUBSTRATE n 
addmsg /kinetics/PSD/actCaMKII /kinetics/PSD/CaMKII_PSD/tot_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII /kinetics/PSD/CaMKII_PSD/tot_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr305 /kinetics/PSD/CaMKII_PSD/tot_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM /kinetics/PSD/CaMKII_PSD/286P SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 /kinetics/PSD/CaMKII_PSD/286P SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305 /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286 /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII REAC eA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305 ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286 ENZYME n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM PRODUCT n 
addmsg /kinetics/PSD/CaM/CaM-Ca4 /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca4 /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM PRODUCT n 
addmsg /kinetics/PSD/CaMKII_PSD/transloc_2 /kinetics/PSD/CaMKII_PSD/NMDAR REAC A B 
addmsg /kinetics/PSD/CaMKII_PSD/transloc_1 /kinetics/PSD/CaMKII_PSD/NMDAR REAC A B 
addmsg /kinetics/PSD/CaMKII_PSD/back_1 /kinetics/PSD/CaMKII_PSD/NMDAR REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/back_2 /kinetics/PSD/CaMKII_PSD/NMDAR REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM /kinetics/PSD/CaMKII_PSD/transloc_2 PRODUCT n 
addmsg /kinetics/PSD/CaMKII_PSD/NMDAR /kinetics/PSD/CaMKII_PSD/transloc_2 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM /kinetics/PSD/CaMKII_PSD/transloc_2 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM /kinetics/PSD/CaMKII_PSD/transloc_1 PRODUCT n 
addmsg /kinetics/PSD/CaMKII_PSD/NMDAR /kinetics/PSD/CaMKII_PSD/transloc_1 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-CaM /kinetics/PSD/CaMKII_PSD/transloc_1 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII /kinetics/PSD/CaMKII_PSD/back_1 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/NMDAR /kinetics/PSD/CaMKII_PSD/back_1 PRODUCT n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII /kinetics/PSD/CaMKII_PSD/back_1 PRODUCT n 
addmsg /kinetics/PSD/CaMKII_PSD/sCaMKII-thr305 /kinetics/PSD/CaMKII_PSD/back_2 SUBSTRATE n 
addmsg /kinetics/PSD/CaMKII_PSD/NMDAR /kinetics/PSD/CaMKII_PSD/back_2 PRODUCT n 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr305 /kinetics/PSD/CaMKII_PSD/back_2 PRODUCT n 
addmsg /kinetics/PSD/CaMKII_PSD/transloc_2 /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286 /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286 /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM MM_PRD pA 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286 /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286-CaM REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/transloc_1 /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-286 /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-286 /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM REAC sA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286 /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM /kinetics/PSD/CaMKII_PSD/sCaMKII-CaM REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/back_1 /kinetics/PSD/CaMKII_PSD/sCaMKII REAC A B 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM /kinetics/PSD/CaMKII_PSD/sCaMKII REAC A B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305a /kinetics/PSD/CaMKII_PSD/sCaMKII MM_PRD pA 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286b /kinetics/PSD/CaMKII_PSD/sCaMKII MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph /kinetics/PSD/CaMKII_PSD/sCaMKII REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph /kinetics/PSD/CaMKII_PSD/sCaMKII REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305 /kinetics/PSD/CaMKII_PSD/sCaMKII_p_p_p MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305 /kinetics/PSD/CaMKII_PSD/sCaMKII_p_p_p MM_PRD pA 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305 /kinetics/PSD/CaMKII_PSD/sCaMKII_p_p_p REAC sA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286c /kinetics/PSD/CaMKII_PSD/sCaMKII_p_p_p REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/back_2 /kinetics/PSD/CaMKII_PSD/sCaMKII-thr305 REAC A B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305a /kinetics/PSD/CaMKII_PSD/sCaMKII-thr305 REAC sA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286c /kinetics/PSD/CaMKII_PSD/sCaMKII-thr305 MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/transloc_1 /kinetics/PSD/CaMKII_PSD/CaMKII-CaM REAC A B 
addmsg /kinetics/PSD/CaMKII_PSD/transloc_2 /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-CaM REAC A B 
addmsg /kinetics/PSD/CaMKII_PSD/back_1 /kinetics/PSD/CaMKII_PSD/CaMKII REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/back_2 /kinetics/PSD/CaMKII_PSD/CaMKII-thr305 REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM-act-305 /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-305 /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 REAC sA B 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 REAC A B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr286b /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 REAC sA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/Deph-thr305 /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/tot-CaM-CaMKII/CaM_act_autoph /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 MM_PRD pA 
addmsg /kinetics/PSD/CaMKII_PSD/tot-auto/auton-autoph /kinetics/PSD/CaMKII_PSD/sCaMKII-thr286 MM_PRD pA 
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
addmsg /kinetics/PSD/PP1_PSD/CaM-Bind-CaN /kinetics/PSD/CaM/CaM-Ca2 REAC A B 
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
addmsg /kinetics/PSD/CaM/Ca_stim /kinetics/PSD/CaM/Ca REAC B A 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM /kinetics/PSD/CaM/CaM-Ca4 REAC A B 
addmsg /kinetics/PSD/CaMKII_PSD/CaMKII-bind-CaM /kinetics/PSD/CaM/CaM-Ca4 REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca3-bind-Ca /kinetics/PSD/CaM/CaM-Ca4 REAC B A 
addmsg /kinetics/PSD/CaM/Ca_stim /kinetics/PSD/CaM/Ca_input REAC A B 
addmsg /kinetics/PSD/CaM/Ca_input /kinetics/PSD/CaM/Ca_stim SUBSTRATE n 
addmsg /kinetics/PSD/CaM/Ca /kinetics/PSD/CaM/Ca_stim PRODUCT n 
addmsg /kinetics/PSD/CaM_CaN/dephosph_inhib1 /kinetics/PSD/CaM_CaN REAC eA B 
addmsg /kinetics/PSD/CaM_CaN/dephosph-PP1-I_p /kinetics/PSD/CaM_CaN REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/CaM-Bind-CaN /kinetics/PSD/CaM_CaN REAC B A 
addmsg /kinetics/PSD/CaM_CaN/CaN_1 /kinetics/PSD/CaM_CaN REAC eA B 
addmsg /kinetics/PSD/CaM_CaN/CaN_2 /kinetics/PSD/CaM_CaN REAC eA B 
addmsg /kinetics/PSD/CaM_CaN /kinetics/PSD/CaM_CaN/dephosph-PP1-I_p ENZYME n 
addmsg /kinetics/PSD/PP1_PSD/PP1-I1_p /kinetics/PSD/CaM_CaN/dephosph-PP1-I_p SUBSTRATE n 
addmsg /kinetics/PSD/CaM_CaN /kinetics/PSD/CaM_CaN/dephosph_inhib1 ENZYME n 
addmsg /kinetics/PSD/PP1_PSD/I1_p /kinetics/PSD/CaM_CaN/dephosph_inhib1 SUBSTRATE n 
addmsg /kinetics/PSD/R_SpS /kinetics/PSD/CaM_CaN/CaN_1 SUBSTRATE n 
addmsg /kinetics/PSD/CaM_CaN /kinetics/PSD/CaM_CaN/CaN_1 ENZYME n 
addmsg /kinetics/PSD/R_SpSp /kinetics/PSD/CaM_CaN/CaN_2 SUBSTRATE n 
addmsg /kinetics/PSD/CaM_CaN /kinetics/PSD/CaM_CaN/CaN_2 ENZYME n 
addmsg /kinetics/DEND/CaM/CaM-bind-Ca /kinetics/DEND/CaM/CaM REAC A B 
addmsg /kinetics/DEND/CaM/CaM_x2 /kinetics/DEND/CaM/CaM REAC A B 
addmsg /kinetics/DEND/CaM/CaM-Ca3 /kinetics/DEND/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/DEND/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM-Ca4 /kinetics/DEND/CaM/CaM-Ca3-bind-Ca PRODUCT n 
addmsg /kinetics/DEND/CaM/CaM-Ca2-bind-Ca /kinetics/DEND/CaM/CaM-Ca3 REAC B A 
addmsg /kinetics/DEND/CaM/CaM-Ca3-bind-Ca /kinetics/DEND/CaM/CaM-Ca3 REAC A B 
addmsg /kinetics/DEND/CaM/CaM /kinetics/DEND/CaM/CaM-bind-Ca SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/DEND/CaM/CaM-bind-Ca SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM-Ca /kinetics/DEND/CaM/CaM-bind-Ca PRODUCT n 
addmsg /kinetics/DEND/CaM/CaM-Ca2-bind-Ca /kinetics/DEND/CaM/CaM-Ca2 REAC A B 
addmsg /kinetics/DEND/CaM/CaM-Ca-bind-Ca /kinetics/DEND/CaM/CaM-Ca2 REAC B A 
addmsg /kinetics/DEND/CaM/CaM-Ca2 /kinetics/DEND/CaM/CaM-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/DEND/CaM/CaM-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM-Ca3 /kinetics/DEND/CaM/CaM-Ca2-bind-Ca PRODUCT n 
addmsg /kinetics/DEND/CaM/CaM-Ca /kinetics/DEND/CaM/CaM-Ca-bind-Ca SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/DEND/CaM/CaM-Ca-bind-Ca SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM-Ca2 /kinetics/DEND/CaM/CaM-Ca-bind-Ca PRODUCT n 
addmsg /kinetics/DEND/CaM/CaM-bind-Ca /kinetics/DEND/CaM/CaM-Ca REAC B A 
addmsg /kinetics/DEND/CaM/CaM-Ca-bind-Ca /kinetics/DEND/CaM/CaM-Ca REAC A B 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-bind-CaM /kinetics/DEND/CaM/CaM-Ca4 REAC A B 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-bind-CaM /kinetics/DEND/CaM/CaM-Ca4 REAC A B 
addmsg /kinetics/DEND/CaM/CaM-Ca3-bind-Ca /kinetics/DEND/CaM/CaM-Ca4 REAC B A 
addmsg /kinetics/DEND/CaM/CaM /kinetics/DEND/CaM/CaM_x2 SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM_xchange /kinetics/DEND/CaM/CaM_x2 PRODUCT n 
addmsg /kinetics/DEND/CaM/CaM_x2 /kinetics/DEND/CaM/CaM_xchange REAC B A 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr286 /kinetics/DEND/CaMKII_BULK/tot-auto SUMTOTAL n nInit 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII_p_p_p /kinetics/DEND/CaMKII_BULK/tot-auto SUMTOTAL n nInit 
addmsg /kinetics/DEND/CaMKII_BULK/tot-auto/auton-305 /kinetics/DEND/CaMKII_BULK/tot-auto REAC eA B 
addmsg /kinetics/DEND/CaMKII_BULK/tot-auto/auton-286 /kinetics/DEND/CaMKII_BULK/tot-auto REAC eA B 
addmsg /kinetics/DEND/CaMKII_BULK/basal_CaMKII /kinetics/DEND/CaMKII_BULK/tot-auto SUMTOTAL n nInit 
addmsg /kinetics/DEND/CaMKII_BULK/tot-auto/auton-autoph /kinetics/DEND/CaMKII_BULK/tot-auto REAC eA B 
addmsg /kinetics/DEND/CaMKII_BULK/tot-auto /kinetics/DEND/CaMKII_BULK/tot-auto/auton-autoph ENZYME n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII /kinetics/DEND/CaMKII_BULK/tot-auto/auton-autoph SUBSTRATE n 
addmsg /kinetics/DEND/CaMKII_BULK/tot-auto /kinetics/DEND/CaMKII_BULK/tot-auto/auton-286 ENZYME n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-CaM /kinetics/DEND/CaMKII_BULK/tot-auto/auton-286 SUBSTRATE n 
addmsg /kinetics/DEND/CaMKII_BULK/tot-auto /kinetics/DEND/CaMKII_BULK/tot-auto/auton-305 ENZYME n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr286 /kinetics/DEND/CaMKII_BULK/tot-auto/auton-305 SUBSTRATE n 
addmsg /kinetics/DEND/actCaMKII /kinetics/DEND/CaMKII_BULK/tot_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII /kinetics/DEND/CaMKII_BULK/tot_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr305 /kinetics/DEND/CaMKII_BULK/tot_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-CaM /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII SUMTOTAL n nInit 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-CaM /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII SUMTOTAL n nInit 
addmsg /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII REAC eA B 
addmsg /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII REAC eA B 
addmsg /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII REAC eA B 
addmsg /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph ENZYME n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph SUBSTRATE n 
addmsg /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 ENZYME n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr286 /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 SUBSTRATE n 
addmsg /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 ENZYME n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-CaM /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 SUBSTRATE n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr286 /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-bind-CaM SUBSTRATE n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-CaM /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-bind-CaM PRODUCT n 
addmsg /kinetics/DEND/CaM/CaM-Ca4 /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-bind-CaM SUBSTRATE n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII /kinetics/DEND/CaMKII_BULK/CaMKII-bind-CaM SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM-Ca4 /kinetics/DEND/CaMKII_BULK/CaMKII-bind-CaM SUBSTRATE n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-CaM /kinetics/DEND/CaMKII_BULK/CaMKII-bind-CaM PRODUCT n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-bind-CaM /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-CaM REAC B A 
addmsg /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-CaM MM_PRD pA 
addmsg /kinetics/DEND/CaMKII_BULK/tot-auto/auton-286 /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-CaM MM_PRD pA 
addmsg /kinetics/DEND/PP2A/Deph-thr286 /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-CaM REAC sA B 
addmsg /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 /kinetics/DEND/CaMKII_BULK/CaMKII-CaM REAC sA B 
addmsg /kinetics/DEND/CaMKII_BULK/tot-auto/auton-286 /kinetics/DEND/CaMKII_BULK/CaMKII-CaM REAC sA B 
addmsg /kinetics/DEND/PP2A/Deph-thr286 /kinetics/DEND/CaMKII_BULK/CaMKII-CaM MM_PRD pA 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-bind-CaM /kinetics/DEND/CaMKII_BULK/CaMKII-CaM REAC B A 
addmsg /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 /kinetics/DEND/CaMKII_BULK/CaMKII-thr286 REAC sA B 
addmsg /kinetics/DEND/CaMKII_BULK/tot-auto/auton-305 /kinetics/DEND/CaMKII_BULK/CaMKII-thr286 REAC sA B 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-bind-CaM /kinetics/DEND/CaMKII_BULK/CaMKII-thr286 REAC A B 
addmsg /kinetics/DEND/PP2A/Deph-thr286b /kinetics/DEND/CaMKII_BULK/CaMKII-thr286 REAC sA B 
addmsg /kinetics/DEND/PP2A/Deph-thr305 /kinetics/DEND/CaMKII_BULK/CaMKII-thr286 MM_PRD pA 
addmsg /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph /kinetics/DEND/CaMKII_BULK/CaMKII-thr286 MM_PRD pA 
addmsg /kinetics/DEND/CaMKII_BULK/tot-auto/auton-autoph /kinetics/DEND/CaMKII_BULK/CaMKII-thr286 MM_PRD pA 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-bind-CaM /kinetics/DEND/CaMKII_BULK/CaMKII REAC A B 
addmsg /kinetics/DEND/PP2A/Deph-thr305a /kinetics/DEND/CaMKII_BULK/CaMKII MM_PRD pA 
addmsg /kinetics/DEND/PP2A/Deph-thr286b /kinetics/DEND/CaMKII_BULK/CaMKII MM_PRD pA 
addmsg /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM_act_autoph /kinetics/DEND/CaMKII_BULK/CaMKII REAC sA B 
addmsg /kinetics/DEND/CaMKII_BULK/tot-auto/auton-autoph /kinetics/DEND/CaMKII_BULK/CaMKII REAC sA B 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII_x2 /kinetics/DEND/CaMKII_BULK/CaMKII REAC A B 
addmsg /kinetics/DEND/PP2A/Deph-thr305a /kinetics/DEND/CaMKII_BULK/CaMKII-thr305 REAC sA B 
addmsg /kinetics/DEND/PP2A/Deph-thr286c /kinetics/DEND/CaMKII_BULK/CaMKII-thr305 MM_PRD pA 
addmsg /kinetics/DEND/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 /kinetics/DEND/CaMKII_BULK/CaMKII_p_p_p MM_PRD pA 
addmsg /kinetics/DEND/CaMKII_BULK/tot-auto/auton-305 /kinetics/DEND/CaMKII_BULK/CaMKII_p_p_p MM_PRD pA 
addmsg /kinetics/DEND/PP2A/Deph-thr305 /kinetics/DEND/CaMKII_BULK/CaMKII_p_p_p REAC sA B 
addmsg /kinetics/DEND/PP2A/Deph-thr286c /kinetics/DEND/CaMKII_BULK/CaMKII_p_p_p REAC sA B 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII /kinetics/DEND/CaMKII_BULK/CaMKII_x2 SUBSTRATE n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII_xchange /kinetics/DEND/CaMKII_BULK/CaMKII_x2 PRODUCT n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII_x2 /kinetics/DEND/CaMKII_BULK/CaMKII_xchange REAC B A 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-CaM /kinetics/DEND/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-CaM /kinetics/DEND/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/DEND/CaMKII_BULK/tot-auto /kinetics/DEND/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/DEND/PP2A/Deph-thr286 /kinetics/DEND/PP2A REAC eA B 
addmsg /kinetics/DEND/PP2A/Deph-thr286b /kinetics/DEND/PP2A REAC eA B 
addmsg /kinetics/DEND/PP2A/Deph-thr305 /kinetics/DEND/PP2A REAC eA B 
addmsg /kinetics/DEND/PP2A/Deph-thr305a /kinetics/DEND/PP2A REAC eA B 
addmsg /kinetics/DEND/PP2A/Deph-thr286c /kinetics/DEND/PP2A REAC eA B 
addmsg /kinetics/DEND/PP2A /kinetics/DEND/PP2A/Deph-thr286 ENZYME n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-CaM /kinetics/DEND/PP2A/Deph-thr286 SUBSTRATE n 
addmsg /kinetics/DEND/PP2A /kinetics/DEND/PP2A/Deph-thr286b ENZYME n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr286 /kinetics/DEND/PP2A/Deph-thr286b SUBSTRATE n 
addmsg /kinetics/DEND/PP2A /kinetics/DEND/PP2A/Deph-thr305 ENZYME n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII_p_p_p /kinetics/DEND/PP2A/Deph-thr305 SUBSTRATE n 
addmsg /kinetics/DEND/PP2A /kinetics/DEND/PP2A/Deph-thr286c ENZYME n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII_p_p_p /kinetics/DEND/PP2A/Deph-thr286c SUBSTRATE n 
addmsg /kinetics/DEND/PP2A /kinetics/DEND/PP2A/Deph-thr305a ENZYME n 
addmsg /kinetics/DEND/CaMKII_BULK/CaMKII-thr305 /kinetics/DEND/PP2A/Deph-thr305a SUBSTRATE n 
addmsg /kinetics/DEND/Ca_stim /kinetics/DEND/Ca_input REAC A B 
addmsg /kinetics/DEND/Ca_input /kinetics/DEND/Ca_stim SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/DEND/Ca_stim PRODUCT n 
addmsg /kinetics/PSD/move_to_PSD /kinetics/SPINE/iR REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-bind-Ca /kinetics/SPINE/CaM/CaM REAC A B 
addmsg /kinetics/SPINE/CaM/CaM_x2 /kinetics/SPINE/CaM/CaM REAC A B 
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
addmsg /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN /kinetics/SPINE/CaM/CaM-Ca2 REAC A B 
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
addmsg /kinetics/SPINE/CaM/CaM /kinetics/SPINE/CaM/CaM_x2 SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM_xchange /kinetics/SPINE/CaM/CaM_x2 PRODUCT n 
addmsg /kinetics/SPINE/CaM/CaM_x2 /kinetics/SPINE/CaM/CaM_xchange REAC B A 
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
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-286 /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM REAC sA B 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-286 /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM REAC sA B 
addmsg /kinetics/SPINE/PP2A/Deph-thr286 /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM MM_PRD pA 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII-bind-CaM /kinetics/SPINE/CaMKII_BULK/CaMKII-CaM REAC B A 
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
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII_x2 /kinetics/SPINE/CaMKII_BULK/CaMKII REAC A B 
addmsg /kinetics/SPINE/PP2A/Deph-thr305a /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305 REAC sA B 
addmsg /kinetics/SPINE/PP2A/Deph-thr286c /kinetics/SPINE/CaMKII_BULK/CaMKII-thr305 MM_PRD pA 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-CaM-CaMKII/CaM-act-305 /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p MM_PRD pA 
addmsg /kinetics/SPINE/CaMKII_BULK/tot-auto/auton-305 /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p MM_PRD pA 
addmsg /kinetics/SPINE/PP2A/Deph-thr305 /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p REAC sA B 
addmsg /kinetics/SPINE/PP2A/Deph-thr286c /kinetics/SPINE/CaMKII_BULK/CaMKII_p_p_p REAC sA B 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII /kinetics/SPINE/CaMKII_BULK/CaMKII_x2 SUBSTRATE n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII_xchange /kinetics/SPINE/CaMKII_BULK/CaMKII_x2 PRODUCT n 
addmsg /kinetics/SPINE/CaMKII_BULK/CaMKII_x2 /kinetics/SPINE/CaMKII_BULK/CaMKII_xchange REAC B A 
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
addmsg /kinetics/SPINE/CaM_CaN /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN PRODUCT n 
addmsg /kinetics/SPINE/CaN_BULK/Ca2_CaN /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-Ca2 /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN SUBSTRATE n 
addmsg /kinetics/SPINE/CaN_BULK/CaN /kinetics/SPINE/CaN_BULK/Ca_bind_CaN SUBSTRATE n 
addmsg /kinetics/SPINE/CaN_BULK/Ca2_CaN /kinetics/SPINE/CaN_BULK/Ca_bind_CaN PRODUCT n 
addmsg /kinetics/SPINE/CaM/Ca /kinetics/SPINE/CaN_BULK/Ca_bind_CaN SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/Ca /kinetics/SPINE/CaN_BULK/Ca_bind_CaN SUBSTRATE n 
addmsg /kinetics/SPINE/CaN_BULK/Ca_bind_CaN /kinetics/SPINE/CaN_BULK/Ca2_CaN REAC B A 
addmsg /kinetics/SPINE/CaN_BULK/CaM-Bind-CaN /kinetics/SPINE/CaN_BULK/Ca2_CaN REAC A B 
addmsg /kinetics/DEND/CaM/CaM-bind-Ca /kinetics/Ca REAC A B 
addmsg /kinetics/DEND/CaM/CaM-Ca2-bind-Ca /kinetics/Ca REAC A B 
addmsg /kinetics/DEND/CaM/CaM-Ca3-bind-Ca /kinetics/Ca REAC A B 
addmsg /kinetics/DEND/CaM/CaM-Ca-bind-Ca /kinetics/Ca REAC A B 
addmsg /kinetics/DEND/Ca_stim /kinetics/Ca REAC B A 
addmsg /kinetics/PSD/CaM/Ca /graphs/conc1/Ca.Co PLOT Co *Ca.Co *red 
addmsg /kinetics/PSD/actCaMKII /graphs/conc1/actCaMKII.Co PLOT Co *actCaMKII.Co *12 
addmsg /kinetics/PSD/tot_PSD_R /graphs/conc1/tot_PSD_R.Co PLOT Co *tot_PSD_R.Co *blue 
addmsg /kinetics/PSD/CaM/CaM-Ca4 /graphs/conc1/CaM-Ca4.Co PLOT Co *CaM-Ca4.Co *blue 
addmsg /kinetics/SPINE/CaM/Ca /graphs/conc2/Ca.Co PLOT Co *Ca.Co *red 
addmsg /kinetics/SPINE/actCaMKII /graphs/conc2/actCaMKII.Co PLOT Co *actCaMKII.Co *12 
addmsg /kinetics/SPINE/CaM/CaM-Ca4 /graphs/conc2/CaM-Ca4.Co PLOT Co *CaM-Ca4.Co *blue 
addmsg /kinetics/Ca /moregraphs/conc3/Ca.Co PLOT Co *Ca.Co *red 
addmsg /kinetics/DEND/actCaMKII /moregraphs/conc3/actCaMKII.Co PLOT Co *actCaMKII.Co *12 
addmsg /kinetics/DEND/CaM/CaM-Ca4 /moregraphs/conc3/CaM-Ca4.Co PLOT Co *CaM-Ca4.Co *blue 
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
call /kinetics/PSD/CaMKII_PSD/CaMKII-thr286-bind-CaM/notes LOAD \
"Same values as for the main compartment" \
"Can the main compartment pool of Ca/CaM be used?"
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
call /kinetics/PSD/CaMKII_PSD/transloc_2/notes LOAD \
"Same as for transloc_1" \
"" \
"" \
""
call /kinetics/PSD/CaMKII_PSD/back_1/notes LOAD \
"Rates set by the translocation experiments of " \
"Shen and Meyer, Science 1999."
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
"kb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10" \
"24 Sep 2015" \
"This is a bit too low affinity. Changing to match K2 at " \
"Kd = 2.8."
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
call /kinetics/DEND/CaM/CaM/notes LOAD \
"There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)" \
"Say 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying" \
"it is comparable to CaMK levels. " \
""
call /kinetics/DEND/CaM/CaM-Ca3-bind-Ca/notes LOAD \
"Use K3 = 21.5 uM here from Stemmer and Klee table 3." \
"kb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10"
call /kinetics/DEND/CaM/CaM-bind-Ca/notes LOAD \
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
call /kinetics/DEND/CaM/CaM-Ca2/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
call /kinetics/DEND/CaM/CaM-Ca2-bind-Ca/notes LOAD \
"K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get" \
"kb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10" \
""
call /kinetics/DEND/CaM/CaM-Ca-bind-Ca/notes LOAD \
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
call /kinetics/DEND/CaM/CaM-Ca/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
call /kinetics/DEND/CaMKII_BULK/CaMKII-thr286-bind-CaM/notes LOAD \
"Same values as for the main compartment" \
"Can the main compartment pool of Ca/CaM be used?"
call /kinetics/DEND/PP2A/notes LOAD \
"Strack et al JBC 1997 show that PP2A is the primary" \
"phosphatase acting on CaMKII in the bulk."
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
