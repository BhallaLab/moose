//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Sun Aug 21 08:50:42 2016
 
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
simundump group /kinetics/PSD/CaM 1 blue 20 x 0 0 "" defaultfile \
  defaultfile.g 0 fbb0ff81553508bc01f3dd51428742fb 0 -28 11 0
simundump text /kinetics/PSD/CaM/notes 0 ""
call /kinetics/PSD/CaM/notes LOAD \
""
simundump kpool /kinetics/PSD/CaM/CaM 1 5e-13 40 40 240 240 0 0 6 0 \
  /kinetics/geometry pink blue -24 13 0
simundump text /kinetics/PSD/CaM/CaM/notes 0 \
  "There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)\nSay 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying\nit is comparable to CaMK levels. \n"
call /kinetics/PSD/CaM/CaM/notes LOAD \
"There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)" \
"Say 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying" \
"it is comparable to CaMK levels. " \
""
simundump kreac /kinetics/PSD/CaM/CaM-Ca3-bind-Ca 1 0.3 10 "" white blue -10 \
  10 0
simundump text /kinetics/PSD/CaM/CaM-Ca3-bind-Ca/notes 0 \
  "Use K3 = 21.5 uM here from Stemmer and Klee table 3.\nkb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10\n24 Sep 2015\nThis is a bit too low affinity. Changing to match K2 at \nKd = 2.8."
call /kinetics/PSD/CaM/CaM-Ca3-bind-Ca/notes LOAD \
"Use K3 = 21.5 uM here from Stemmer and Klee table 3." \
"kb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10" \
"24 Sep 2015" \
"This is a bit too low affinity. Changing to match K2 at " \
"Kd = 2.8."
simundump kpool /kinetics/PSD/CaM/CaM-Ca3 1 1e-12 0 0 0 0 0 0 6 0 \
  /kinetics/geometry hotpink blue -12 13 0
simundump text /kinetics/PSD/CaM/CaM-Ca3/notes 0 ""
call /kinetics/PSD/CaM/CaM-Ca3/notes LOAD \
""
simundump kreac /kinetics/PSD/CaM/CaM-bind-Ca 1 1.4141 8.4853 "" white blue \
  -22 10 0
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
  /kinetics/geometry pink blue -16 13 0
simundump text /kinetics/PSD/CaM/CaM-Ca2/notes 0 \
  "This is the intermediate where the TR2 end (the high-affinity end) has\nbound the Ca but the TR1 end has not."
call /kinetics/PSD/CaM/CaM-Ca2/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
simundump kreac /kinetics/PSD/CaM/CaM-Ca2-bind-Ca 1 0.60001 10 "" white blue \
  -14 10 0
simundump text /kinetics/PSD/CaM/CaM-Ca2-bind-Ca/notes 0 \
  "K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get\nkb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10\n"
call /kinetics/PSD/CaM/CaM-Ca2-bind-Ca/notes LOAD \
"K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get" \
"kb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10" \
""
simundump kreac /kinetics/PSD/CaM/CaM-Ca-bind-Ca 1 1.4141 8.4853 "" white \
  blue -18 10 0
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
  /kinetics/geometry pink blue -20 13 0
simundump text /kinetics/PSD/CaM/CaM-Ca/notes 0 \
  "This is the intermediate where the TR2 end (the high-affinity end) has\nbound the Ca but the TR1 end has not."
call /kinetics/PSD/CaM/CaM-Ca/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
simundump kpool /kinetics/PSD/CaM/CaM-Ca4 1 1e-12 0 0 0 0 0 0 6 0 \
  /kinetics/geometry blue blue -7 13 0
simundump text /kinetics/PSD/CaM/CaM-Ca4/notes 0 ""
call /kinetics/PSD/CaM/CaM-Ca4/notes LOAD \
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
simundump group /kinetics/DEND/CaM 1 blue 13 x 0 0 "" defaultfile \
  defaultfile.g 0 fbb0ff81553508bc01f3dd51428742fb 0 -21 -15 0
simundump text /kinetics/DEND/CaM/notes 0 ""
call /kinetics/DEND/CaM/notes LOAD \
""
simundump kpool /kinetics/DEND/CaM/CaM 1 5e-13 2 2 1200 1200 0 0 600 0 \
  /kinetics/geometry pink blue -17 -13 0
simundump text /kinetics/DEND/CaM/CaM/notes 0 \
  "There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)\nSay 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying\nit is comparable to CaMK levels. \n"
call /kinetics/DEND/CaM/CaM/notes LOAD \
"There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)" \
"Say 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying" \
"it is comparable to CaMK levels. " \
""
simundump kreac /kinetics/DEND/CaM/CaM-Ca3-bind-Ca 1 0.003 10 "" white blue \
  -3 -16 0
simundump text /kinetics/DEND/CaM/CaM-Ca3-bind-Ca/notes 0 \
  "Use K3 = 21.5 uM here from Stemmer and Klee table 3.\nkb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10"
call /kinetics/DEND/CaM/CaM-Ca3-bind-Ca/notes LOAD \
"Use K3 = 21.5 uM here from Stemmer and Klee table 3." \
"kb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10"
simundump kpool /kinetics/DEND/CaM/CaM-Ca3 1 1e-12 0 0 0 0 0 0 600 0 \
  /kinetics/geometry hotpink blue -5 -13 0
simundump text /kinetics/DEND/CaM/CaM-Ca3/notes 0 ""
call /kinetics/DEND/CaM/CaM-Ca3/notes LOAD \
""
simundump kreac /kinetics/DEND/CaM/CaM-bind-Ca 1 0.014141 8.4853 "" white \
  blue -15 -16 0
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
  /kinetics/geometry pink blue -9 -13 0
simundump text /kinetics/DEND/CaM/CaM-Ca2/notes 0 \
  "This is the intermediate where the TR2 end (the high-affinity end) has\nbound the Ca but the TR1 end has not."
call /kinetics/DEND/CaM/CaM-Ca2/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
simundump kreac /kinetics/DEND/CaM/CaM-Ca2-bind-Ca 1 0.006 10 "" white blue \
  -7 -16 0
simundump text /kinetics/DEND/CaM/CaM-Ca2-bind-Ca/notes 0 \
  "K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get\nkb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10\n"
call /kinetics/DEND/CaM/CaM-Ca2-bind-Ca/notes LOAD \
"K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get" \
"kb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10" \
""
simundump kreac /kinetics/DEND/CaM/CaM-Ca-bind-Ca 1 0.014141 8.4853 "" white \
  blue -11 -16 0
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
  /kinetics/geometry pink blue -13 -13 0
simundump text /kinetics/DEND/CaM/CaM-Ca/notes 0 \
  "This is the intermediate where the TR2 end (the high-affinity end) has\nbound the Ca but the TR1 end has not."
call /kinetics/DEND/CaM/CaM-Ca/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
simundump kpool /kinetics/DEND/CaM/CaM-Ca4 1 1e-12 0 0 0 0 0 0 600 0 \
  /kinetics/geometry blue blue 0 -13 0
simundump text /kinetics/DEND/CaM/CaM-Ca4/notes 0 ""
call /kinetics/DEND/CaM/CaM-Ca4/notes LOAD \
""
simundump kreac /kinetics/DEND/CaM/CaM_x2 0 10 10 "" white black -27 -13 0
simundump text /kinetics/DEND/CaM/CaM_x2/notes 0 ""
call /kinetics/DEND/CaM/CaM_x2/notes LOAD \
""
simundump kpool /kinetics/DEND/CaM/CaM_xchange 0 2e-11 0 0 0 0 0 0 600 0 \
  /kinetics/geometry[1] blue blue -36 -15 0
simundump text /kinetics/DEND/CaM/CaM_xchange/notes 0 ""
call /kinetics/DEND/CaM/CaM_xchange/notes LOAD \
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
simundump group /kinetics/SPINE/CaM 1 blue 5 x 0 0 "" defaultfile \
  defaultfile.g 0 fbb0ff81553508bc01f3dd51428742fb 0 -24 -7 0
simundump text /kinetics/SPINE/CaM/notes 0 ""
call /kinetics/SPINE/CaM/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaM/CaM 1 5e-13 40 40 2160 2160 0 0 54 0 \
  /kinetics/geometry pink blue -20 -5 0
simundump text /kinetics/SPINE/CaM/CaM/notes 0 \
  "There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)\nSay 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying\nit is comparable to CaMK levels. \n"
call /kinetics/SPINE/CaM/CaM/notes LOAD \
"There is a LOT of this in the cell: upto 1% of total protein mass. (Alberts et al)" \
"Say 25 uM. Meyer et al Science 256 1199-1202 1992 refer to studies saying" \
"it is comparable to CaMK levels. " \
""
simundump kreac /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca 1 0.033333 10 "" white \
  blue -6 -8 0
simundump text /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca/notes 0 \
  "Use K3 = 21.5 uM here from Stemmer and Klee table 3.\nkb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10"
call /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca/notes LOAD \
"Use K3 = 21.5 uM here from Stemmer and Klee table 3." \
"kb/kf =21.5 * 6e5 so kf = 7.75e-7, kb = 10"
simundump kpool /kinetics/SPINE/CaM/CaM-Ca3 1 1e-12 0 0 0 0 0 0 54 0 \
  /kinetics/geometry hotpink blue -8 -5 0
simundump text /kinetics/SPINE/CaM/CaM-Ca3/notes 0 ""
call /kinetics/SPINE/CaM/CaM-Ca3/notes LOAD \
""
simundump kreac /kinetics/SPINE/CaM/CaM-bind-Ca 1 0.15712 8.4853 "" white \
  blue -18 -8 0
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
  /kinetics/geometry pink blue -12 -5 0
simundump text /kinetics/SPINE/CaM/CaM-Ca2/notes 0 \
  "This is the intermediate where the TR2 end (the high-affinity end) has\nbound the Ca but the TR1 end has not."
call /kinetics/SPINE/CaM/CaM-Ca2/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
simundump kreac /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca 1 0.066668 10 "" white \
  blue -10 -8 0
simundump text /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca/notes 0 \
  "K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get\nkb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10\n"
call /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca/notes LOAD \
"K3 = 21.5, K4 = 2.8. Assuming that the K4 step happens first, we get" \
"kb/kf = 2.8 uM = 1.68e6 so kf =6e-6 assuming kb = 10" \
""
simundump kreac /kinetics/SPINE/CaM/CaM-Ca-bind-Ca 1 0.15712 8.4853 "" white \
  blue -14 -8 0
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
  /kinetics/geometry pink blue -16 -5 0
simundump text /kinetics/SPINE/CaM/CaM-Ca/notes 0 \
  "This is the intermediate where the TR2 end (the high-affinity end) has\nbound the Ca but the TR1 end has not."
call /kinetics/SPINE/CaM/CaM-Ca/notes LOAD \
"This is the intermediate where the TR2 end (the high-affinity end) has" \
"bound the Ca but the TR1 end has not."
simundump kpool /kinetics/SPINE/CaM/CaM-Ca4 1 1e-12 0 0 0 0 0 0 54 0 \
  /kinetics/geometry blue blue -3 -5 0
simundump text /kinetics/SPINE/CaM/CaM-Ca4/notes 0 ""
call /kinetics/SPINE/CaM/CaM-Ca4/notes LOAD \
""
simundump kreac /kinetics/SPINE/CaM/CaM_x2 0 1 100 "" white black -23 1 0
simundump text /kinetics/SPINE/CaM/CaM_x2/notes 0 ""
call /kinetics/SPINE/CaM/CaM_x2/notes LOAD \
""
simundump kpool /kinetics/SPINE/CaM/CaM_xchange 0 2e-11 0 0 0 0 0 0 54 0 \
  /kinetics/geometry[2] blue black -28 -2 0
simundump text /kinetics/SPINE/CaM/CaM_xchange/notes 0 ""
call /kinetics/SPINE/CaM/CaM_xchange/notes LOAD \
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
simundump xcoredraw /edit/draw 0 -38 31 -32 15
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
"21 Aug 2016: Ca and CaM in spine and dendrite for diffusive" \
"calculations with CaM acting as buffer." \
""
addmsg /kinetics/PSD/Ca_stim /kinetics/PSD/Ca REAC B A 
addmsg /kinetics/PSD/CaM/CaM-bind-Ca /kinetics/PSD/Ca REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca-bind-Ca /kinetics/PSD/Ca REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca2-bind-Ca /kinetics/PSD/Ca REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca3-bind-Ca /kinetics/PSD/Ca REAC A B 
addmsg /kinetics/PSD/Ca_stim /kinetics/PSD/Ca_input REAC A B 
addmsg /kinetics/PSD/Ca_input /kinetics/PSD/Ca_stim SUBSTRATE n 
addmsg /kinetics/PSD/Ca /kinetics/PSD/Ca_stim PRODUCT n 
addmsg /kinetics/PSD/CaM/CaM-bind-Ca /kinetics/PSD/CaM/CaM REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca3 /kinetics/PSD/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca4 /kinetics/PSD/CaM/CaM-Ca3-bind-Ca PRODUCT n 
addmsg /kinetics/PSD/Ca /kinetics/PSD/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca2-bind-Ca /kinetics/PSD/CaM/CaM-Ca3 REAC B A 
addmsg /kinetics/PSD/CaM/CaM-Ca3-bind-Ca /kinetics/PSD/CaM/CaM-Ca3 REAC A B 
addmsg /kinetics/PSD/CaM/CaM /kinetics/PSD/CaM/CaM-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca /kinetics/PSD/CaM/CaM-bind-Ca PRODUCT n 
addmsg /kinetics/PSD/Ca /kinetics/PSD/CaM/CaM-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca2-bind-Ca /kinetics/PSD/CaM/CaM-Ca2 REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca-bind-Ca /kinetics/PSD/CaM/CaM-Ca2 REAC B A 
addmsg /kinetics/PSD/CaM/CaM-Ca2 /kinetics/PSD/CaM/CaM-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca3 /kinetics/PSD/CaM/CaM-Ca2-bind-Ca PRODUCT n 
addmsg /kinetics/PSD/Ca /kinetics/PSD/CaM/CaM-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca /kinetics/PSD/CaM/CaM-Ca-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-Ca2 /kinetics/PSD/CaM/CaM-Ca-bind-Ca PRODUCT n 
addmsg /kinetics/PSD/Ca /kinetics/PSD/CaM/CaM-Ca-bind-Ca SUBSTRATE n 
addmsg /kinetics/PSD/CaM/CaM-bind-Ca /kinetics/PSD/CaM/CaM-Ca REAC B A 
addmsg /kinetics/PSD/CaM/CaM-Ca-bind-Ca /kinetics/PSD/CaM/CaM-Ca REAC A B 
addmsg /kinetics/PSD/CaM/CaM-Ca3-bind-Ca /kinetics/PSD/CaM/CaM-Ca4 REAC B A 
addmsg /kinetics/DEND/Ca_stim /kinetics/DEND/Ca REAC B A 
addmsg /kinetics/DEND/CaM/CaM-bind-Ca /kinetics/DEND/Ca REAC A B 
addmsg /kinetics/DEND/CaM/CaM-Ca-bind-Ca /kinetics/DEND/Ca REAC A B 
addmsg /kinetics/DEND/CaM/CaM-Ca2-bind-Ca /kinetics/DEND/Ca REAC A B 
addmsg /kinetics/DEND/CaM/CaM-Ca3-bind-Ca /kinetics/DEND/Ca REAC A B 
addmsg /kinetics/DEND/Ca_stim /kinetics/DEND/Ca_input REAC A B 
addmsg /kinetics/DEND/Ca_input /kinetics/DEND/Ca_stim SUBSTRATE n 
addmsg /kinetics/DEND/Ca /kinetics/DEND/Ca_stim PRODUCT n 
addmsg /kinetics/DEND/CaM/CaM-bind-Ca /kinetics/DEND/CaM/CaM REAC A B 
addmsg /kinetics/DEND/CaM/CaM_x2 /kinetics/DEND/CaM/CaM REAC A B 
addmsg /kinetics/DEND/CaM/CaM-Ca3 /kinetics/DEND/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM-Ca4 /kinetics/DEND/CaM/CaM-Ca3-bind-Ca PRODUCT n 
addmsg /kinetics/DEND/Ca /kinetics/DEND/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM-Ca2-bind-Ca /kinetics/DEND/CaM/CaM-Ca3 REAC B A 
addmsg /kinetics/DEND/CaM/CaM-Ca3-bind-Ca /kinetics/DEND/CaM/CaM-Ca3 REAC A B 
addmsg /kinetics/DEND/CaM/CaM /kinetics/DEND/CaM/CaM-bind-Ca SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM-Ca /kinetics/DEND/CaM/CaM-bind-Ca PRODUCT n 
addmsg /kinetics/DEND/Ca /kinetics/DEND/CaM/CaM-bind-Ca SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM-Ca2-bind-Ca /kinetics/DEND/CaM/CaM-Ca2 REAC A B 
addmsg /kinetics/DEND/CaM/CaM-Ca-bind-Ca /kinetics/DEND/CaM/CaM-Ca2 REAC B A 
addmsg /kinetics/DEND/CaM/CaM-Ca2 /kinetics/DEND/CaM/CaM-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM-Ca3 /kinetics/DEND/CaM/CaM-Ca2-bind-Ca PRODUCT n 
addmsg /kinetics/DEND/Ca /kinetics/DEND/CaM/CaM-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM-Ca /kinetics/DEND/CaM/CaM-Ca-bind-Ca SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM-Ca2 /kinetics/DEND/CaM/CaM-Ca-bind-Ca PRODUCT n 
addmsg /kinetics/DEND/Ca /kinetics/DEND/CaM/CaM-Ca-bind-Ca SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM-bind-Ca /kinetics/DEND/CaM/CaM-Ca REAC B A 
addmsg /kinetics/DEND/CaM/CaM-Ca-bind-Ca /kinetics/DEND/CaM/CaM-Ca REAC A B 
addmsg /kinetics/DEND/CaM/CaM-Ca3-bind-Ca /kinetics/DEND/CaM/CaM-Ca4 REAC B A 
addmsg /kinetics/DEND/CaM/CaM /kinetics/DEND/CaM/CaM_x2 SUBSTRATE n 
addmsg /kinetics/DEND/CaM/CaM_xchange /kinetics/DEND/CaM/CaM_x2 PRODUCT n 
addmsg /kinetics/DEND/CaM/CaM_x2 /kinetics/DEND/CaM/CaM_xchange REAC B A 
addmsg /kinetics/SPINE/CaM/CaM-bind-Ca /kinetics/SPINE/Ca REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-Ca-bind-Ca /kinetics/SPINE/Ca REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca /kinetics/SPINE/Ca REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca /kinetics/SPINE/Ca REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-bind-Ca /kinetics/SPINE/CaM/CaM REAC A B 
addmsg /kinetics/SPINE/CaM/CaM_x2 /kinetics/SPINE/CaM/CaM REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-Ca3 /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-Ca4 /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca PRODUCT n 
addmsg /kinetics/SPINE/Ca /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca /kinetics/SPINE/CaM/CaM-Ca3 REAC B A 
addmsg /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca /kinetics/SPINE/CaM/CaM-Ca3 REAC A B 
addmsg /kinetics/SPINE/CaM/CaM /kinetics/SPINE/CaM/CaM-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-Ca /kinetics/SPINE/CaM/CaM-bind-Ca PRODUCT n 
addmsg /kinetics/SPINE/Ca /kinetics/SPINE/CaM/CaM-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca /kinetics/SPINE/CaM/CaM-Ca2 REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-Ca-bind-Ca /kinetics/SPINE/CaM/CaM-Ca2 REAC B A 
addmsg /kinetics/SPINE/CaM/CaM-Ca2 /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-Ca3 /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca PRODUCT n 
addmsg /kinetics/SPINE/Ca /kinetics/SPINE/CaM/CaM-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-Ca /kinetics/SPINE/CaM/CaM-Ca-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-Ca2 /kinetics/SPINE/CaM/CaM-Ca-bind-Ca PRODUCT n 
addmsg /kinetics/SPINE/Ca /kinetics/SPINE/CaM/CaM-Ca-bind-Ca SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM-bind-Ca /kinetics/SPINE/CaM/CaM-Ca REAC B A 
addmsg /kinetics/SPINE/CaM/CaM-Ca-bind-Ca /kinetics/SPINE/CaM/CaM-Ca REAC A B 
addmsg /kinetics/SPINE/CaM/CaM-Ca3-bind-Ca /kinetics/SPINE/CaM/CaM-Ca4 REAC B A 
addmsg /kinetics/SPINE/CaM/CaM /kinetics/SPINE/CaM/CaM_x2 SUBSTRATE n 
addmsg /kinetics/SPINE/CaM/CaM_xchange /kinetics/SPINE/CaM/CaM_x2 PRODUCT n 
addmsg /kinetics/SPINE/CaM/CaM_x2 /kinetics/SPINE/CaM/CaM_xchange REAC B A 
enddump
// End of dump

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
complete_loading
