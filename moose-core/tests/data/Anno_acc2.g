//  DOQCS : http://doqcs.ncbs.res.in/ 
//  Accession Name = fig3_CaMKII 
//  Accession Number = 2 
//  Transcriber = Upinder S. Bhalla, NCBS 
//  Developer = Upinder S. Bhalla, NCBS 
//  Species = Generic mammalian 
//  Tissue = Neuronal 
//  Cell Compartment = Synapse 
//  Notes =  This is the model file for figure 3 from <a href=http://www.ncbi.nlm.nih.gov:80/entrez/query.fcgi?cmd=Retrieve&db=PubMed&list_uids=9888852&dopt=Abstract>Bhalla US and Iyengar R. Science (1999) 283(5400):381-7</a>. It is a model of the Ca activation of CaMKII and other CaM-activated enzymes. It includes the regulatory phosphatases PP1 and PP2B (Calcineurin) acting on CaMKII and also includes CaM-activated adenylyl cyclase and PKA in the synapse.<br>Demonstration script files for generating the figures in the paper, including figure 3, are available <a href=http://www.ncbs.res.in/~bhalla/ltploop/ltploop/demos/>here</a>. 
 
 //genesis
// kkit Version 11 flat dumpfile
 
// Saved on Mon Aug  7 15:49:53 2006
 
include kkit {argv 1}
 
FASTDT = 0.1
SIMDT = 0.1
CONTROLDT = 1
PLOTDT = 1
MAXTIME = 300
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
simundump geometry /kinetics/geometry 0 1e-15 3 sphere "" white black 12 2 0
simundump geometry /kinetics/geometry[1] 0 1.6667e-21 3 sphere "" white black \
  0 0 0
simundump geometry /kinetics/geometry[2] 0 1e-15 3 sphere "" white black 0 0 \
  0
simundump group /kinetics/CaMKII 1 purple black x 0 0 "" defaultfile \
  defaultfile.g 0 0 0 2.6875 3.7292 0
simundump kpool /kinetics/CaMKII/CaMKII 1 0 70 70 4.2e+07 4.2e+07 0 0 6e+05 0 \
  /kinetics/geometry[2] palegreen purple 1.2692 3.0259 0
simundump kpool /kinetics/CaMKII/CaMKII-CaM 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] palegreen purple -2.3548 2.7925 0
simundump kpool /kinetics/CaMKII/CaMKII-thr286*-CaM 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] palegreen purple -2.6144 1.3705 0
simundump kpool /kinetics/CaMKII/CaMKII*** 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] cyan purple -2.5276 -1.8689 0
simundump kreac /kinetics/CaMKII/CaMKII-bind-CaM 1 8.3333e-05 5 "" white \
  purple 1.7899 1.2816 0
simundump kreac /kinetics/CaMKII/CaMK-thr286-bind-CaM 1 0.001667 0.1 "" white \
  purple 1.8116 0.67637 0
simundump kpool /kinetics/CaMKII/CaMKII-thr286 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] red purple -2.4625 -0.33819 0
simundump kpool /kinetics/CaMKII/CaMK-thr306 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] palegreen purple -2.4506 -3.5103 0
simundump kpool /kinetics/CaMKII/total-CaMKII 1 0 70 70 4.2e+07 4.2e+07 0 0 \
  6e+05 4 /kinetics/geometry[2] cyan purple -2.7513 3.8942 0
simundump kreac /kinetics/CaMKII/basal-activity 1 0.003 0 "" white purple \
  -0.28069 -0.40738 0
simundump kpool /kinetics/CaMKII/tot_CaM_CaMKII 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] green purple -6.627 3.0522 0
simundump kenz /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_305 1 0 0 0 0 6e+05 \
  18.48 24 6 0 0 "" red green "" -4.4631 0.3694 0
simundump kenz /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_286 1 0 0 0 0 6e+05 \
  1.54 2 0.5 0 0 "" red green "" -0.5082 2.5709 0
simundump kpool /kinetics/CaMKII/tot_autonomous_CaMKII 1 0 0 0 0 0 0 0 6e+05 \
  0 /kinetics/geometry[2] green purple -6.9754 2.0821 0
simundump kenz /kinetics/CaMKII/tot_autonomous_CaMKII/auton_305 1 0 0 0 0 \
  6e+05 12 24 6 0 0 "" red green "" -4.6475 -0.65672 0
simundump kenz /kinetics/CaMKII/tot_autonomous_CaMKII/auton_286 1 0 0 0 0 \
  6e+05 1 2 0.5 0 0 "" red green "" -0.38525 1.75 0
simundump kpool /kinetics/CaM-Ca4 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] blue yellow 3.0133 -3.112 0
simundump kpool /kinetics/PP1-active 1 0 1.8 1.8 1.08e+06 1.08e+06 0 0 6e+05 \
  0 /kinetics/geometry[2] cyan yellow -6.3601 -0.10535 0
simundump kenz /kinetics/PP1-active/Deph-thr286 1 0 0 0 0 6e+05 5.72e-07 1.4 \
  0.35 0 0 "" red cyan "" -6.0085 1.5362 0
simundump kenz /kinetics/PP1-active/Deph-thr305 1 0 0 0 0 6e+05 5.72e-07 1.4 \
  0.35 0 0 "" red cyan "" -5.225 -1.3503 0
simundump kenz /kinetics/PP1-active/Deph-thr306 1 0 0 0 0 6e+05 5.72e-07 1.4 \
  0.35 0 0 "" red cyan "" -0.44927 3.4772 0
simundump kenz /kinetics/PP1-active/Deph-thr286c 1 0 0 0 0 6e+05 5.72e-07 1.4 \
  0.35 0 0 "" red cyan "" -5.2457 -3.0616 0
simundump kenz /kinetics/PP1-active/Deph_thr286b 1 0 0 0 0 6e+05 5.72e-07 1.4 \
  0.35 0 0 "" red cyan "" 0.33058 -1.3636 0
simundump group /kinetics/CaM 1 blue black x 0 0 "" defaultfile defaultfile.g \
  0 0 0 -20.239 -3.8552 0
simundump kpool /kinetics/CaM/CaM 1 0 20 20 1.2e+07 1.2e+07 0 0 6e+05 0 \
  /kinetics/geometry[2] pink blue -20.256 3.8645 0
simundump kreac /kinetics/CaM/CaM-TR2-bind-Ca 1 2e-10 72 "" white blue \
  -18.077 3.2237 0
simundump kreac /kinetics/CaM/CaM-TR2-Ca2-bind-Ca 1 6e-06 10 "" white blue \
  -19.081 1.3701 0
simundump kreac /kinetics/CaM/CaM-Ca3-bind-Ca 1 7.75e-07 10 "" white blue \
  -20.639 -1.5956 0
simundump kpool /kinetics/CaM/neurogranin-CaM 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] red blue -29.85 -4.3835 0
simundump kreac /kinetics/CaM/neurogranin-bind-CaM 1 5e-07 1 "" white blue \
  -25.869 -3.9464 0
simundump kpool /kinetics/CaM/neurogranin* 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] red blue -22.782 -7.0509 0
simundump kpool /kinetics/CaM/neurogranin 1 0 10 10 6e+06 6e+06 0 0 6e+05 0 \
  /kinetics/geometry[2] red blue -27.48 -7.0612 0
simundump kreac /kinetics/CaM/dephosph-neurogranin 1 0.005 0 "" white blue \
  -20.667 -5.3862 0
simundump kpool /kinetics/Ca 1 0 0.08 0.08 48000 48000 0 0 6e+05 2 \
  /kinetics/geometry[2] red black -12.573 -0.45824 0
simundump kpool /kinetics/PKC-active 1 0 0.09 0.09 54000 54000 0 0 6e+05 4 \
  /kinetics/geometry[2] red black 14.665 -0.98951 0
simundump kenz /kinetics/PKC-active/PKC-phosph-neurogranin 1 0 0 0 0 6e+05 \
  1.7e-07 2.34 0.58 0 0 "" red red "" -23.917 -4.6577 0
simundump kenz /kinetics/PKC-active/PKC-phosph-ng-CaM 1 0 0 0 0 6e+05 \
  1.02e-07 1.4 0.35 0 0 "" red red "" -27.353 -1.8725 0
simundump kenz /kinetics/PKC-active/phosph-AC2 1 0 0 0 0 6e+05 1e-06 16 4 0 0 \
  "" red red "" 8.2737 -21.525 0
simundump kpool /kinetics/CaM-Ca3 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] hotpink yellow -16.75 -0.45824 0
simundump kpool /kinetics/CaM-TR2-Ca2 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] pink yellow -15.24 2.4025 0
simundump kpool /kinetics/CaM(Ca)n-CaNAB 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] darkblue yellow -5.4358 -6.8994 0
simundump kenz /kinetics/CaM(Ca)n-CaNAB/dephosph_neurogranin 1 0 0 0 0 6e+05 \
  5.56e-07 2.67 0.67 0 0 "" red darkblue "" -25.959 -9.4632 0
simundump kenz /kinetics/CaM(Ca)n-CaNAB/dephosph_inhib1 1 0 0 0 0 6e+05 \
  5.7e-07 1.36 0.34 0 0 "" red darkblue "" -17.654 -17.602 0
simundump kenz /kinetics/CaM(Ca)n-CaNAB/dephosph-PP1-I* 1 0 0 0 0 6e+05 \
  5.7e-07 1.36 0.34 0 0 "" white darkblue "" -16.152 -6.6886 0
simundump group /kinetics/PP1 1 yellow black x 0 0 "" defaultfile \
  defaultfile.g 0 0 0 -21.274 -10.48 0
simundump kpool /kinetics/PP1/I1 1 0 1.8 1.8 1.08e+06 1.08e+06 0 0 6e+05 0 \
  /kinetics/geometry[2] orange yellow -12.925 -14.596 0
simundump kpool /kinetics/PP1/I1* 1 0 0.001 0.001 600 600 0 0 6e+05 0 \
  /kinetics/geometry[2] orange yellow -17.069 -14.564 0
simundump kreac /kinetics/PP1/Inact-PP1 1 0.0008333 0.1 "" white yellow \
  -20.315 -12.662 0
simundump kpool /kinetics/PP1/PP1-I1* 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] brown yellow -18.659 -9.2092 0
simundump kpool /kinetics/PP1/PP1-I1 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] brown yellow -11.251 -8.933 0
simundump kreac /kinetics/PP1/dissoc-PP1-I1 1 1 0 "" white yellow -17.41 \
  -12.386 0
simundump kpool /kinetics/PKA-active 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] yellow black -8.4971 -13.103 0
simundump kenz /kinetics/PKA-active/PKA-phosph-I1 1 0 0 0 0 6e+05 1e-05 36 9 \
  0 0 "" red yellow "" -11.806 -17.359 0
simundump kenz /kinetics/PKA-active/PKA-phosph-GAP 1 0 0 0 0 6e+05 9e-05 108 \
  27 0 0 "" red yellow "" -8.8035 -19.992 0
simundump kenz /kinetics/PKA-active/phosph-PDE 1 0 0 0 0 6e+05 1e-05 36 9 0 0 \
  "" red yellow "" -5.8456 -13.562 0
simundump kpool /kinetics/PP2A 1 0 0.12 0.12 72000 72000 0 0 6e+05 0 \
  /kinetics/geometry[2] red black -11.432 -3.8776 0
simundump kenz /kinetics/PP2A/PP2A-dephosph-I1 1 0 0 0 0 6e+05 6.6e-06 25 6 0 \
  0 "" red red "" -13.866 -10.908 0
simundump kenz /kinetics/PP2A/PP2A-dephosph-PP1-I* 1 0 0 0 0 6e+05 6.6e-06 25 \
  6 0 0 "" red red "" -11.433 -5.0184 0
simundump kpool /kinetics/CaNAB-Ca4 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] tan yellow 0.16478 -8.8168 0
simundump kenz /kinetics/CaNAB-Ca4/dephosph_inhib1_noCaM 1 0 0 0 0 6e+05 \
  5.7e-08 0.136 0.034 0 0 "" red tan "" -10.451 -10.741 0
simundump group /kinetics/PP2B 1 red3 black x 0 0 "" defaultfile \
  defaultfile.g 0 0 0 5.0358 -6.0997 0
simundump kpool /kinetics/PP2B/CaNAB 1 0 1 1 6e+05 6e+05 0 0 6e+05 0 \
  /kinetics/geometry[2] tan red3 6.3861 -8.6907 0
simundump kpool /kinetics/PP2B/CaNAB-Ca2 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] tan red3 3.8301 -8.7824 0
simundump kreac /kinetics/PP2B/Ca-bind-CaNAB-Ca2 1 1e-11 1 "" white red3 \
  2.2627 -9.9976 0
simundump kreac /kinetics/PP2B/Ca-bind-CaNAB 1 2.78e-08 1 "" white red3 \
  4.9634 -10.135 0
simundump kreac /kinetics/PP2B/CaM-Ca2-bind-CaNAB 1 4e-07 1 "" white red3 \
  -1.8742 -9.5607 0
simundump kpool /kinetics/PP2B/CaMCa3-CaNAB 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] blue red3 -4.5792 -10.999 0
simundump kpool /kinetics/PP2B/CaMCa2-CANAB 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] blue red3 -2.4377 -10.934 0
simundump kpool /kinetics/PP2B/CaMCa4-CaNAB 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] blue red3 -5.9091 -9.8612 0
simundump kreac /kinetics/PP2B/CaMCa3-bind-CaNAB 1 3.73e-06 1 "" white red3 \
  -2.5053 -8.8524 0
simundump kreac /kinetics/PP2B/CaMCa4-bind-CaNAB 1 0.001 1 "" white red3 \
  -2.5504 -7.8866 0
simundump group /kinetics/PKA 0 blue blue x 0 0 "" defaultfile defaultfile.g \
  0 0 0 -16.855 -20.912 0
simundump kpool /kinetics/PKA/R2C2 0 0 0.5 0.5 3e+05 3e+05 0 0 6e+05 0 \
  /kinetics/geometry[2] white blue -21.567 -27.985 0
simundump kpool /kinetics/PKA/R2C2-cAMP 0 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] white blue -18.605 -27.517 0
simundump kreac /kinetics/PKA/cAMP-bind-site-B1 0 9e-05 33 "" white blue \
  -19.191 -31.26 0
simundump kreac /kinetics/PKA/cAMP-bind-site-B2 1 9e-05 33 "" white blue \
  -16.849 -30.012 0
simundump kreac /kinetics/PKA/cAMP-bind-site-A1 1 0.000125 110 "" white blue \
  -14.163 -31.197 0
simundump kreac /kinetics/PKA/cAMP-bind-site-A2 1 0.000125 32.5 "" white blue \
  -10.875 -29.766 0
simundump kpool /kinetics/PKA/R2C2-cAMP2 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] white blue -15.058 -26.675 0
simundump kpool /kinetics/PKA/R2C2-cAMP3 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] white blue -12.372 -27.735 0
simundump kpool /kinetics/PKA/R2C2-cAMP4 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] white blue -9.9853 -26.124 0
simundump kpool /kinetics/PKA/R2C-cAMP4 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] white blue -12.269 -24.99 0
simundump kpool /kinetics/PKA/R2-cAMP4 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] white blue -18.605 -25.427 0
simundump kreac /kinetics/PKA/Release-C1 1 60 3e-05 "" white blue -10.273 \
  -23.122 0
simundump kreac /kinetics/PKA/Release-C2 1 60 3e-05 "" white blue -15.144 \
  -24.4 0
simundump kpool /kinetics/PKA/PKA-inhibitor 1 0 0.25 0.25 1.5e+05 1.5e+05 0 0 \
  6e+05 0 /kinetics/geometry[2] cyan blue -19.626 -23.533 0
simundump kreac /kinetics/PKA/inhib-PKA 1 1e-04 1 "" white blue -16.833 \
  -22.91 0
simundump kpool /kinetics/PKA/inhibited-PKA 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] cyan blue -20.253 -21.934 0
simundump kpool /kinetics/cAMP 1 0 0 0 0 0 0 0 6e+05 2 /kinetics/geometry[2] \
  green black -5.0683 -32.836 0
simundump group /kinetics/AC 1 blue blue x 0 0 "" defaultfile defaultfile.g 0 \
  0 0 7.5593 -17.715 0
simundump kpool /kinetics/AC/ATP 1 0 5000 5000 3e+09 3e+09 0 0 6e+05 4 \
  /kinetics/geometry[2] red blue 7.0461 -19.113 0
simundump kpool /kinetics/AC/AC1-CaM 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] orange blue 4.6056 -17.504 0
simundump kenz /kinetics/AC/AC1-CaM/kenz 1 0 0 0 0 6e+05 7.5e-06 72 18 0 1 "" \
  red orange "" 4.5678 -18.639 0
simundump kpool /kinetics/AC/AC1 1 0 0.02 0.02 12000 12000 0 0 6e+05 0 \
  /kinetics/geometry[2] orange blue 0.8408 -15.639 0
simundump kreac /kinetics/AC/CaM-bind-AC1 1 8.3333e-05 1 "" white blue 2.326 \
  -15.835 0
simundump kpool /kinetics/AC/AC2* 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] yellow blue 6.441 -22.765 0
simundump kenz /kinetics/AC/AC2*/kenz 1 0 0 0 0 6e+05 2.9e-06 28 7 0 1 "" red \
  yellow "" 6.3147 -21.908 0
simundump kpool /kinetics/AC/AC2-Gs 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] yellow blue 3.6027 -21.954 0
simundump kenz /kinetics/AC/AC2-Gs/kenz 1 0 0 0 0 6e+05 7.5e-06 72 18 0 1 "" \
  red yellow "" 3.5238 -20.946 0
simundump kpool /kinetics/AC/AC2 1 0 0.015 0.015 9000 9000 0 0 6e+05 0 \
  /kinetics/geometry[2] yellow blue 7.4818 -24.548 0
simundump kreac /kinetics/AC/dephosph-AC2 1 0.1 0 "" white blue 5.3295 \
  -25.353 0
simundump kpool /kinetics/AC/AC1-Gs 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] orange blue 2.168 -16.752 0
simundump kenz /kinetics/AC/AC1-Gs/kenz 1 0 0 0 0 1 7.5e-06 72 18 0 1 "" red \
  orange "" 3.1436 -17.9 0
simundump kreac /kinetics/AC/Gs-bind-AC2 1 0.00083333 1 "" white blue 4.9182 \
  -27.387 0
simundump kreac /kinetics/AC/Gs-bind-AC1 1 0.00021 1 "" white blue 0.2088 \
  -17.128 0
simundump kpool /kinetics/AC/AMP 1 0 3.2549e+05 3.2549e+05 3.2549e+05 \
  3.2549e+05 0 0 1 0 /kinetics/geometry pink blue 1.4388 -17.715 0
simundump kpool /kinetics/AC/AC2*-Gs 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] green blue 4.9462 -22.386 0
simundump kenz /kinetics/AC/AC2*-Gs/kenz 1 0 0 0 0 1 7.5e-06 216 54 0 1 "" \
  red green "" 5.0219 -21.332 0
simundump kreac /kinetics/AC/Gs-bind-AC2* 1 0.0013888 1 "" white blue 4.745 \
  -24.236 0
simundump kpool /kinetics/AC/cAMP-PDE 1 0 0.45 0.45 2.7e+05 2.7e+05 0 0 6e+05 \
  0 /kinetics/geometry[2] green blue -1.6237 -15.941 0
simundump kenz /kinetics/AC/cAMP-PDE/PDE 1 0 0 0 0 6e+05 4.2e-06 40 10 0 0 "" \
  red green "" -1.7324 -23.376 0
simundump kpool /kinetics/AC/cAMP-PDE* 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] green blue -1.5969 -18.025 0
simundump kenz /kinetics/AC/cAMP-PDE*/PDE* 1 0 0 0 0 6e+05 8.4e-06 80 20 0 0 \
  "" red green "" -0.3498 -22.55 0
simundump kreac /kinetics/AC/dephosph-PDE 1 0.1 0 "" white blue -3.4985 \
  -19.087 0
simundump kpool /kinetics/AC/PDE1 1 0 2 2 1.2e+06 1.2e+06 0 0 6e+05 0 \
  /kinetics/geometry[2] green blue -5.4043 -12.36 0
simundump kenz /kinetics/AC/PDE1/PDE1 1 0 0 0 0 6e+05 3.5e-07 6.67 1.667 0 0 \
  "" red green "" -2.3373 -22.314 0
simundump kpool /kinetics/AC/CaM.PDE1 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] green blue -5.4043 -15.095 0
simundump kenz /kinetics/AC/CaM.PDE1/CaM.PDE1 1 0 0 0 0 6e+05 2.1e-06 40 10 0 \
  0 "" red green "" -3.2447 -21.527 0
simundump kreac /kinetics/AC/CaM_bind_PDE1 1 0.0012 5 "" white blue -2.1917 \
  -13.538 0
simundump kpool /kinetics/Gs-alpha 1 0 0 0 0 0 0 0 6e+05 2 \
  /kinetics/geometry[2] red black 1.4111 -28.275 0
simundump kpool /kinetics/tot_PP1 1 0 0 1.8 1.08e+06 0 0 0 6e+05 0 \
  /kinetics/geometry[2] blue black -4 5 0
simundump xtab /kinetics/Ca_tab 1 0 0 1 2 0 "" edit_xtab white red 0 0 0 3 \
  -12.713 2.8136 0
loadtab /kinetics/Ca_tab table 1 100 0 100 \
 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 \
 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 \
 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
simundump kpool /kinetics/tot_CaMKII-active 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] blue black -11.365 4.2863 0
simundump kpool /kinetics/tot_CaN-active 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] red black -8.861 -8.5101 0
simundump doqcsinfo /kinetics/doqcsinfo 0 db2.g \
  fig3_CaMKII network "Upinder S. Bhalla, NCBS" "Upinder S. Bhalla, NCBS" \
  "citation here" "Generic Mammalian" Neuronal Synapse \
  "Quantitative match to experiments, Qualitative" \
  "<a href = http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=PubMed&list_uids=9888852&dopt=Abstract>Bhalla US and Iyengar R. Science (1999) 283(5400):381-7</a> ( Peer-reviewed publication )" \
  "Exact GENESIS implementation" \
  "Approximates original data , Quantitatively predicts new data" 12 4 0
simundump xgraph /graphs/conc1 0 0 10000 0 35 0
simundump xgraph /graphs/conc2 0 0 10000 0 1.2 0
simundump xplot /graphs/conc1/Ca.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0 0 1
simundump xplot /graphs/conc1/tot_CaMKII-active.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc2/tot_CaN-active.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0 0 1
simundump xgraph /moregraphs/conc3 0 0 10000 0 0.3 0
simundump xgraph /moregraphs/conc4 0 0 10000 0 1.6 0
simundump xplot /moregraphs/conc3/PKA-active.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" yellow 0 0 1
simundump xplot /moregraphs/conc4/tot_PP1.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xcoredraw /edit/draw 0 -12.24 2.2387 -6.9655 5.3066
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"Merged in ac19.g on top of" \
"allcam35.g to add in" \
"CaM-dependent PDE" \
"Added in  a few plots and a stimulus" \
"table to allcam36.g" \
"Eliminated notes for portability."
addmsg /kinetics/CaMKII/CaMKII-bind-CaM /kinetics/CaMKII/CaMKII REAC A B 
addmsg /kinetics/PP1-active/Deph-thr306 /kinetics/CaMKII/CaMKII MM_PRD pA 
addmsg /kinetics/PP1-active/Deph_thr286b /kinetics/CaMKII/CaMKII MM_PRD pA 
addmsg /kinetics/CaMKII/basal-activity /kinetics/CaMKII/CaMKII REAC A B 
addmsg /kinetics/CaMKII/CaMKII-bind-CaM /kinetics/CaMKII/CaMKII-CaM REAC B A 
addmsg /kinetics/PP1-active/Deph-thr286 /kinetics/CaMKII/CaMKII-CaM MM_PRD pA 
addmsg /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_286 /kinetics/CaMKII/CaMKII-CaM REAC sA B 
addmsg /kinetics/CaMKII/tot_autonomous_CaMKII/auton_286 /kinetics/CaMKII/CaMKII-CaM REAC sA B 
addmsg /kinetics/CaMKII/CaMK-thr286-bind-CaM /kinetics/CaMKII/CaMKII-thr286*-CaM REAC B A 
addmsg /kinetics/PP1-active/Deph-thr286 /kinetics/CaMKII/CaMKII-thr286*-CaM REAC sA B 
addmsg /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_286 /kinetics/CaMKII/CaMKII-thr286*-CaM MM_PRD pA 
addmsg /kinetics/CaMKII/tot_autonomous_CaMKII/auton_286 /kinetics/CaMKII/CaMKII-thr286*-CaM MM_PRD pA 
addmsg /kinetics/PP1-active/Deph-thr305 /kinetics/CaMKII/CaMKII*** REAC sA B 
addmsg /kinetics/PP1-active/Deph-thr286c /kinetics/CaMKII/CaMKII*** REAC sA B 
addmsg /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_305 /kinetics/CaMKII/CaMKII*** MM_PRD pA 
addmsg /kinetics/CaMKII/tot_autonomous_CaMKII/auton_305 /kinetics/CaMKII/CaMKII*** MM_PRD pA 
addmsg /kinetics/CaM-Ca4 /kinetics/CaMKII/CaMKII-bind-CaM SUBSTRATE n 
addmsg /kinetics/CaMKII/CaMKII /kinetics/CaMKII/CaMKII-bind-CaM SUBSTRATE n 
addmsg /kinetics/CaMKII/CaMKII-CaM /kinetics/CaMKII/CaMKII-bind-CaM PRODUCT n 
addmsg /kinetics/CaMKII/CaMKII-thr286 /kinetics/CaMKII/CaMK-thr286-bind-CaM SUBSTRATE n 
addmsg /kinetics/CaM-Ca4 /kinetics/CaMKII/CaMK-thr286-bind-CaM SUBSTRATE n 
addmsg /kinetics/CaMKII/CaMKII-thr286*-CaM /kinetics/CaMKII/CaMK-thr286-bind-CaM PRODUCT n 
addmsg /kinetics/CaMKII/CaMK-thr286-bind-CaM /kinetics/CaMKII/CaMKII-thr286 REAC A B 
addmsg /kinetics/PP1-active/Deph-thr305 /kinetics/CaMKII/CaMKII-thr286 MM_PRD pA 
addmsg /kinetics/PP1-active/Deph_thr286b /kinetics/CaMKII/CaMKII-thr286 REAC sA B 
addmsg /kinetics/CaMKII/basal-activity /kinetics/CaMKII/CaMKII-thr286 REAC B A 
addmsg /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_305 /kinetics/CaMKII/CaMKII-thr286 REAC sA B 
addmsg /kinetics/CaMKII/tot_autonomous_CaMKII/auton_305 /kinetics/CaMKII/CaMKII-thr286 REAC sA B 
addmsg /kinetics/PP1-active/Deph-thr306 /kinetics/CaMKII/CaMK-thr306 REAC sA B 
addmsg /kinetics/PP1-active/Deph-thr286c /kinetics/CaMKII/CaMK-thr306 MM_PRD pA 
addmsg /kinetics/CaMKII/CaMKII /kinetics/CaMKII/basal-activity SUBSTRATE n 
addmsg /kinetics/CaMKII/CaMKII-thr286 /kinetics/CaMKII/basal-activity PRODUCT n 
addmsg /kinetics/CaMKII/CaMKII-CaM /kinetics/CaMKII/tot_CaM_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/CaMKII/CaMKII-thr286*-CaM /kinetics/CaMKII/tot_CaM_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_305 /kinetics/CaMKII/tot_CaM_CaMKII REAC eA B 
addmsg /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_286 /kinetics/CaMKII/tot_CaM_CaMKII REAC eA B 
addmsg /kinetics/CaMKII/tot_CaM_CaMKII /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_305 ENZYME n 
addmsg /kinetics/CaMKII/CaMKII-thr286 /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_305 SUBSTRATE n 
addmsg /kinetics/CaMKII/total-CaMKII /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_305 INTRAMOL n 
addmsg /kinetics/CaMKII/tot_CaM_CaMKII /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_286 ENZYME n 
addmsg /kinetics/CaMKII/CaMKII-CaM /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_286 SUBSTRATE n 
addmsg /kinetics/CaMKII/total-CaMKII /kinetics/CaMKII/tot_CaM_CaMKII/CaM_act_286 INTRAMOL n 
addmsg /kinetics/CaMKII/CaMKII-thr286 /kinetics/CaMKII/tot_autonomous_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/CaMKII/CaMKII*** /kinetics/CaMKII/tot_autonomous_CaMKII SUMTOTAL n nInit 
addmsg /kinetics/CaMKII/tot_autonomous_CaMKII/auton_305 /kinetics/CaMKII/tot_autonomous_CaMKII REAC eA B 
addmsg /kinetics/CaMKII/tot_autonomous_CaMKII/auton_286 /kinetics/CaMKII/tot_autonomous_CaMKII REAC eA B 
addmsg /kinetics/CaMKII/tot_autonomous_CaMKII /kinetics/CaMKII/tot_autonomous_CaMKII/auton_305 ENZYME n 
addmsg /kinetics/CaMKII/CaMKII-thr286 /kinetics/CaMKII/tot_autonomous_CaMKII/auton_305 SUBSTRATE n 
addmsg /kinetics/CaMKII/total-CaMKII /kinetics/CaMKII/tot_autonomous_CaMKII/auton_305 INTRAMOL n 
addmsg /kinetics/CaMKII/tot_autonomous_CaMKII /kinetics/CaMKII/tot_autonomous_CaMKII/auton_286 ENZYME n 
addmsg /kinetics/CaMKII/CaMKII-CaM /kinetics/CaMKII/tot_autonomous_CaMKII/auton_286 SUBSTRATE n 
addmsg /kinetics/CaMKII/total-CaMKII /kinetics/CaMKII/tot_autonomous_CaMKII/auton_286 INTRAMOL n 
addmsg /kinetics/CaMKII/CaMKII-bind-CaM /kinetics/CaM-Ca4 REAC A B 
addmsg /kinetics/CaMKII/CaMK-thr286-bind-CaM /kinetics/CaM-Ca4 REAC A B 
addmsg /kinetics/CaM/CaM-Ca3-bind-Ca /kinetics/CaM-Ca4 REAC B A 
addmsg /kinetics/PP2B/CaMCa4-bind-CaNAB /kinetics/CaM-Ca4 REAC A B 
addmsg /kinetics/AC/CaM-bind-AC1 /kinetics/CaM-Ca4 REAC A B 
addmsg /kinetics/AC/CaM_bind_PDE1 /kinetics/CaM-Ca4 REAC A B 
addmsg /kinetics/PP1-active/Deph-thr286 /kinetics/PP1-active REAC eA B 
addmsg /kinetics/PP1-active/Deph-thr305 /kinetics/PP1-active REAC eA B 
addmsg /kinetics/PP1-active/Deph-thr306 /kinetics/PP1-active REAC eA B 
addmsg /kinetics/PP1-active/Deph_thr286b /kinetics/PP1-active REAC eA B 
addmsg /kinetics/PP1-active/Deph-thr286c /kinetics/PP1-active REAC eA B 
addmsg /kinetics/PP1/Inact-PP1 /kinetics/PP1-active REAC A B 
addmsg /kinetics/PP1/dissoc-PP1-I1 /kinetics/PP1-active REAC B A 
addmsg /kinetics/PP1-active /kinetics/PP1-active/Deph-thr286 ENZYME n 
addmsg /kinetics/CaMKII/CaMKII-thr286*-CaM /kinetics/PP1-active/Deph-thr286 SUBSTRATE n 
addmsg /kinetics/PP1-active /kinetics/PP1-active/Deph-thr305 ENZYME n 
addmsg /kinetics/CaMKII/CaMKII*** /kinetics/PP1-active/Deph-thr305 SUBSTRATE n 
addmsg /kinetics/PP1-active /kinetics/PP1-active/Deph-thr306 ENZYME n 
addmsg /kinetics/CaMKII/CaMK-thr306 /kinetics/PP1-active/Deph-thr306 SUBSTRATE n 
addmsg /kinetics/PP1-active /kinetics/PP1-active/Deph-thr286c ENZYME n 
addmsg /kinetics/CaMKII/CaMKII*** /kinetics/PP1-active/Deph-thr286c SUBSTRATE n 
addmsg /kinetics/PP1-active /kinetics/PP1-active/Deph_thr286b ENZYME n 
addmsg /kinetics/CaMKII/CaMKII-thr286 /kinetics/PP1-active/Deph_thr286b SUBSTRATE n 
addmsg /kinetics/CaM/CaM-TR2-bind-Ca /kinetics/CaM/CaM REAC A B 
addmsg /kinetics/CaM/neurogranin-bind-CaM /kinetics/CaM/CaM REAC A B 
addmsg /kinetics/PKC-active/PKC-phosph-ng-CaM /kinetics/CaM/CaM MM_PRD pA 
addmsg /kinetics/CaM/CaM /kinetics/CaM/CaM-TR2-bind-Ca SUBSTRATE n 
addmsg /kinetics/CaM-TR2-Ca2 /kinetics/CaM/CaM-TR2-bind-Ca PRODUCT n 
addmsg /kinetics/Ca /kinetics/CaM/CaM-TR2-bind-Ca SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/CaM/CaM-TR2-bind-Ca SUBSTRATE n 
addmsg /kinetics/CaM-TR2-Ca2 /kinetics/CaM/CaM-TR2-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/CaM/CaM-TR2-Ca2-bind-Ca SUBSTRATE n 
addmsg /kinetics/CaM-Ca3 /kinetics/CaM/CaM-TR2-Ca2-bind-Ca PRODUCT n 
addmsg /kinetics/CaM-Ca3 /kinetics/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/CaM/CaM-Ca3-bind-Ca SUBSTRATE n 
addmsg /kinetics/CaM-Ca4 /kinetics/CaM/CaM-Ca3-bind-Ca PRODUCT n 
addmsg /kinetics/CaM/neurogranin-bind-CaM /kinetics/CaM/neurogranin-CaM REAC B A 
addmsg /kinetics/PKC-active/PKC-phosph-ng-CaM /kinetics/CaM/neurogranin-CaM REAC sA B 
addmsg /kinetics/CaM/neurogranin /kinetics/CaM/neurogranin-bind-CaM SUBSTRATE n 
addmsg /kinetics/CaM/neurogranin-CaM /kinetics/CaM/neurogranin-bind-CaM PRODUCT n 
addmsg /kinetics/CaM/CaM /kinetics/CaM/neurogranin-bind-CaM SUBSTRATE n 
addmsg /kinetics/CaM(Ca)n-CaNAB/dephosph_neurogranin /kinetics/CaM/neurogranin* REAC sA B 
addmsg /kinetics/PKC-active/PKC-phosph-neurogranin /kinetics/CaM/neurogranin* MM_PRD pA 
addmsg /kinetics/PKC-active/PKC-phosph-ng-CaM /kinetics/CaM/neurogranin* MM_PRD pA 
addmsg /kinetics/CaM/dephosph-neurogranin /kinetics/CaM/neurogranin* REAC A B 
addmsg /kinetics/CaM(Ca)n-CaNAB/dephosph_neurogranin /kinetics/CaM/neurogranin MM_PRD pA 
addmsg /kinetics/CaM/neurogranin-bind-CaM /kinetics/CaM/neurogranin REAC A B 
addmsg /kinetics/PKC-active/PKC-phosph-neurogranin /kinetics/CaM/neurogranin REAC sA B 
addmsg /kinetics/CaM/dephosph-neurogranin /kinetics/CaM/neurogranin REAC B A 
addmsg /kinetics/CaM/neurogranin* /kinetics/CaM/dephosph-neurogranin SUBSTRATE n 
addmsg /kinetics/CaM/neurogranin /kinetics/CaM/dephosph-neurogranin PRODUCT n 
addmsg /kinetics/CaM/CaM-TR2-bind-Ca /kinetics/Ca REAC A B 
addmsg /kinetics/CaM/CaM-TR2-bind-Ca /kinetics/Ca REAC A B 
addmsg /kinetics/CaM/CaM-TR2-Ca2-bind-Ca /kinetics/Ca REAC A B 
addmsg /kinetics/CaM/CaM-Ca3-bind-Ca /kinetics/Ca REAC A B 
addmsg /kinetics/PP2B/Ca-bind-CaNAB-Ca2 /kinetics/Ca REAC A B 
addmsg /kinetics/PP2B/Ca-bind-CaNAB-Ca2 /kinetics/Ca REAC A B 
addmsg /kinetics/PP2B/Ca-bind-CaNAB /kinetics/Ca REAC A B 
addmsg /kinetics/PP2B/Ca-bind-CaNAB /kinetics/Ca REAC A B 
addmsg /kinetics/Ca_tab /kinetics/Ca SLAVE output 
addmsg /kinetics/PKC-active/PKC-phosph-neurogranin /kinetics/PKC-active REAC eA B 
addmsg /kinetics/PKC-active/PKC-phosph-ng-CaM /kinetics/PKC-active REAC eA B 
addmsg /kinetics/PKC-active/phosph-AC2 /kinetics/PKC-active REAC eA B 
addmsg /kinetics/PKC-active /kinetics/PKC-active/PKC-phosph-neurogranin ENZYME n 
addmsg /kinetics/CaM/neurogranin /kinetics/PKC-active/PKC-phosph-neurogranin SUBSTRATE n 
addmsg /kinetics/PKC-active /kinetics/PKC-active/PKC-phosph-ng-CaM ENZYME n 
addmsg /kinetics/CaM/neurogranin-CaM /kinetics/PKC-active/PKC-phosph-ng-CaM SUBSTRATE n 
addmsg /kinetics/PKC-active /kinetics/PKC-active/phosph-AC2 ENZYME n 
addmsg /kinetics/AC/AC2 /kinetics/PKC-active/phosph-AC2 SUBSTRATE n 
addmsg /kinetics/CaM/CaM-TR2-Ca2-bind-Ca /kinetics/CaM-Ca3 REAC B A 
addmsg /kinetics/CaM/CaM-Ca3-bind-Ca /kinetics/CaM-Ca3 REAC A B 
addmsg /kinetics/PP2B/CaMCa3-bind-CaNAB /kinetics/CaM-Ca3 REAC A B 
addmsg /kinetics/CaM/CaM-TR2-bind-Ca /kinetics/CaM-TR2-Ca2 REAC B A 
addmsg /kinetics/CaM/CaM-TR2-Ca2-bind-Ca /kinetics/CaM-TR2-Ca2 REAC A B 
addmsg /kinetics/PP2B/CaM-Ca2-bind-CaNAB /kinetics/CaM-TR2-Ca2 REAC A B 
addmsg /kinetics/CaM(Ca)n-CaNAB/dephosph_neurogranin /kinetics/CaM(Ca)n-CaNAB REAC eA B 
addmsg /kinetics/CaM(Ca)n-CaNAB/dephosph_inhib1 /kinetics/CaM(Ca)n-CaNAB REAC eA B 
addmsg /kinetics/CaM(Ca)n-CaNAB/dephosph-PP1-I* /kinetics/CaM(Ca)n-CaNAB REAC eA B 
addmsg /kinetics/PP2B/CaMCa4-CaNAB /kinetics/CaM(Ca)n-CaNAB SUMTOTAL n nInit 
addmsg /kinetics/PP2B/CaMCa3-CaNAB /kinetics/CaM(Ca)n-CaNAB SUMTOTAL n nInit 
addmsg /kinetics/PP2B/CaMCa2-CANAB /kinetics/CaM(Ca)n-CaNAB SUMTOTAL n nInit 
addmsg /kinetics/CaM(Ca)n-CaNAB /kinetics/CaM(Ca)n-CaNAB/dephosph_neurogranin ENZYME n 
addmsg /kinetics/CaM/neurogranin* /kinetics/CaM(Ca)n-CaNAB/dephosph_neurogranin SUBSTRATE n 
addmsg /kinetics/CaM(Ca)n-CaNAB /kinetics/CaM(Ca)n-CaNAB/dephosph_inhib1 ENZYME n 
addmsg /kinetics/PP1/I1* /kinetics/CaM(Ca)n-CaNAB/dephosph_inhib1 SUBSTRATE n 
addmsg /kinetics/CaM(Ca)n-CaNAB /kinetics/CaM(Ca)n-CaNAB/dephosph-PP1-I* ENZYME n 
addmsg /kinetics/PP1/PP1-I1* /kinetics/CaM(Ca)n-CaNAB/dephosph-PP1-I* SUBSTRATE n 
addmsg /kinetics/PKA-active/PKA-phosph-I1 /kinetics/PP1/I1 REAC sA B 
addmsg /kinetics/CaNAB-Ca4/dephosph_inhib1_noCaM /kinetics/PP1/I1 MM_PRD pA 
addmsg /kinetics/CaM(Ca)n-CaNAB/dephosph_inhib1 /kinetics/PP1/I1 MM_PRD pA 
addmsg /kinetics/PP2A/PP2A-dephosph-I1 /kinetics/PP1/I1 MM_PRD pA 
addmsg /kinetics/PP1/dissoc-PP1-I1 /kinetics/PP1/I1 REAC B A 
addmsg /kinetics/PP1/Inact-PP1 /kinetics/PP1/I1* REAC A B 
addmsg /kinetics/PKA-active/PKA-phosph-I1 /kinetics/PP1/I1* MM_PRD pA 
addmsg /kinetics/CaNAB-Ca4/dephosph_inhib1_noCaM /kinetics/PP1/I1* REAC sA B 
addmsg /kinetics/CaM(Ca)n-CaNAB/dephosph_inhib1 /kinetics/PP1/I1* REAC sA B 
addmsg /kinetics/PP2A/PP2A-dephosph-I1 /kinetics/PP1/I1* REAC sA B 
addmsg /kinetics/PP1/PP1-I1* /kinetics/PP1/Inact-PP1 PRODUCT n 
addmsg /kinetics/PP1/I1* /kinetics/PP1/Inact-PP1 SUBSTRATE n 
addmsg /kinetics/PP1-active /kinetics/PP1/Inact-PP1 SUBSTRATE n 
addmsg /kinetics/PP1/Inact-PP1 /kinetics/PP1/PP1-I1* REAC B A 
addmsg /kinetics/PP2A/PP2A-dephosph-PP1-I* /kinetics/PP1/PP1-I1* REAC sA B 
addmsg /kinetics/CaM(Ca)n-CaNAB/dephosph-PP1-I* /kinetics/PP1/PP1-I1* REAC sA B 
addmsg /kinetics/PP1/dissoc-PP1-I1 /kinetics/PP1/PP1-I1 REAC A B 
addmsg /kinetics/PP2A/PP2A-dephosph-PP1-I* /kinetics/PP1/PP1-I1 MM_PRD pA 
addmsg /kinetics/CaM(Ca)n-CaNAB/dephosph-PP1-I* /kinetics/PP1/PP1-I1 MM_PRD pA 
addmsg /kinetics/PP1/PP1-I1 /kinetics/PP1/dissoc-PP1-I1 SUBSTRATE n 
addmsg /kinetics/PP1-active /kinetics/PP1/dissoc-PP1-I1 PRODUCT n 
addmsg /kinetics/PP1/I1 /kinetics/PP1/dissoc-PP1-I1 PRODUCT n 
addmsg /kinetics/PKA-active/PKA-phosph-I1 /kinetics/PKA-active REAC eA B 
addmsg /kinetics/PKA/Release-C1 /kinetics/PKA-active REAC B A 
addmsg /kinetics/PKA/Release-C2 /kinetics/PKA-active REAC B A 
addmsg /kinetics/PKA/inhib-PKA /kinetics/PKA-active REAC A B 
addmsg /kinetics/PKA-active/PKA-phosph-GAP /kinetics/PKA-active REAC eA B 
addmsg /kinetics/PKA-active/phosph-PDE /kinetics/PKA-active REAC eA B 
addmsg /kinetics/PKA-active /kinetics/PKA-active/PKA-phosph-I1 ENZYME n 
addmsg /kinetics/PP1/I1 /kinetics/PKA-active/PKA-phosph-I1 SUBSTRATE n 
addmsg /kinetics/PKA-active /kinetics/PKA-active/PKA-phosph-GAP ENZYME n 
addmsg /kinetics/PKA-active /kinetics/PKA-active/phosph-PDE ENZYME n 
addmsg /kinetics/AC/cAMP-PDE /kinetics/PKA-active/phosph-PDE SUBSTRATE n 
addmsg /kinetics/PP2A/PP2A-dephosph-I1 /kinetics/PP2A REAC eA B 
addmsg /kinetics/PP2A/PP2A-dephosph-PP1-I* /kinetics/PP2A REAC eA B 
addmsg /kinetics/PP2A /kinetics/PP2A/PP2A-dephosph-I1 ENZYME n 
addmsg /kinetics/PP1/I1* /kinetics/PP2A/PP2A-dephosph-I1 SUBSTRATE n 
addmsg /kinetics/PP2A /kinetics/PP2A/PP2A-dephosph-PP1-I* ENZYME n 
addmsg /kinetics/PP1/PP1-I1* /kinetics/PP2A/PP2A-dephosph-PP1-I* SUBSTRATE n 
addmsg /kinetics/CaNAB-Ca4/dephosph_inhib1_noCaM /kinetics/CaNAB-Ca4 REAC eA B 
addmsg /kinetics/PP2B/Ca-bind-CaNAB-Ca2 /kinetics/CaNAB-Ca4 REAC B A 
addmsg /kinetics/PP2B/CaM-Ca2-bind-CaNAB /kinetics/CaNAB-Ca4 REAC A B 
addmsg /kinetics/PP2B/CaMCa4-bind-CaNAB /kinetics/CaNAB-Ca4 REAC A B 
addmsg /kinetics/PP2B/CaMCa3-bind-CaNAB /kinetics/CaNAB-Ca4 REAC A B 
addmsg /kinetics/CaNAB-Ca4 /kinetics/CaNAB-Ca4/dephosph_inhib1_noCaM ENZYME n 
addmsg /kinetics/PP1/I1* /kinetics/CaNAB-Ca4/dephosph_inhib1_noCaM SUBSTRATE n 
addmsg /kinetics/PP2B/Ca-bind-CaNAB /kinetics/PP2B/CaNAB REAC A B 
addmsg /kinetics/PP2B/Ca-bind-CaNAB /kinetics/PP2B/CaNAB-Ca2 REAC B A 
addmsg /kinetics/PP2B/Ca-bind-CaNAB-Ca2 /kinetics/PP2B/CaNAB-Ca2 REAC A B 
addmsg /kinetics/CaNAB-Ca4 /kinetics/PP2B/Ca-bind-CaNAB-Ca2 PRODUCT n 
addmsg /kinetics/Ca /kinetics/PP2B/Ca-bind-CaNAB-Ca2 SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/PP2B/Ca-bind-CaNAB-Ca2 SUBSTRATE n 
addmsg /kinetics/PP2B/CaNAB-Ca2 /kinetics/PP2B/Ca-bind-CaNAB-Ca2 SUBSTRATE n 
addmsg /kinetics/PP2B/CaNAB /kinetics/PP2B/Ca-bind-CaNAB SUBSTRATE n 
addmsg /kinetics/PP2B/CaNAB-Ca2 /kinetics/PP2B/Ca-bind-CaNAB PRODUCT n 
addmsg /kinetics/Ca /kinetics/PP2B/Ca-bind-CaNAB SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/PP2B/Ca-bind-CaNAB SUBSTRATE n 
addmsg /kinetics/CaNAB-Ca4 /kinetics/PP2B/CaM-Ca2-bind-CaNAB SUBSTRATE n 
addmsg /kinetics/CaM-TR2-Ca2 /kinetics/PP2B/CaM-Ca2-bind-CaNAB SUBSTRATE n 
addmsg /kinetics/PP2B/CaMCa2-CANAB /kinetics/PP2B/CaM-Ca2-bind-CaNAB PRODUCT n 
addmsg /kinetics/PP2B/CaMCa3-bind-CaNAB /kinetics/PP2B/CaMCa3-CaNAB REAC B A 
addmsg /kinetics/PP2B/CaM-Ca2-bind-CaNAB /kinetics/PP2B/CaMCa2-CANAB REAC B A 
addmsg /kinetics/PP2B/CaMCa4-bind-CaNAB /kinetics/PP2B/CaMCa4-CaNAB REAC B A 
addmsg /kinetics/PP2B/CaMCa3-CaNAB /kinetics/PP2B/CaMCa3-bind-CaNAB PRODUCT n 
addmsg /kinetics/CaM-Ca3 /kinetics/PP2B/CaMCa3-bind-CaNAB SUBSTRATE n 
addmsg /kinetics/CaNAB-Ca4 /kinetics/PP2B/CaMCa3-bind-CaNAB SUBSTRATE n 
addmsg /kinetics/PP2B/CaMCa4-CaNAB /kinetics/PP2B/CaMCa4-bind-CaNAB PRODUCT n 
addmsg /kinetics/CaM-Ca4 /kinetics/PP2B/CaMCa4-bind-CaNAB SUBSTRATE n 
addmsg /kinetics/CaNAB-Ca4 /kinetics/PP2B/CaMCa4-bind-CaNAB SUBSTRATE n 
addmsg /kinetics/PKA/cAMP-bind-site-B1 /kinetics/PKA/R2C2 REAC A B 
addmsg /kinetics/PKA/cAMP-bind-site-B1 /kinetics/PKA/R2C2-cAMP REAC B A 
addmsg /kinetics/PKA/cAMP-bind-site-B2 /kinetics/PKA/R2C2-cAMP REAC A B 
addmsg /kinetics/PKA/R2C2 /kinetics/PKA/cAMP-bind-site-B1 SUBSTRATE n 
addmsg /kinetics/PKA/R2C2-cAMP /kinetics/PKA/cAMP-bind-site-B1 PRODUCT n 
addmsg /kinetics/cAMP /kinetics/PKA/cAMP-bind-site-B1 SUBSTRATE n 
addmsg /kinetics/PKA/R2C2-cAMP /kinetics/PKA/cAMP-bind-site-B2 SUBSTRATE n 
addmsg /kinetics/cAMP /kinetics/PKA/cAMP-bind-site-B2 SUBSTRATE n 
addmsg /kinetics/PKA/R2C2-cAMP2 /kinetics/PKA/cAMP-bind-site-B2 PRODUCT n 
addmsg /kinetics/PKA/R2C2-cAMP2 /kinetics/PKA/cAMP-bind-site-A1 SUBSTRATE n 
addmsg /kinetics/cAMP /kinetics/PKA/cAMP-bind-site-A1 SUBSTRATE n 
addmsg /kinetics/PKA/R2C2-cAMP3 /kinetics/PKA/cAMP-bind-site-A1 PRODUCT n 
addmsg /kinetics/cAMP /kinetics/PKA/cAMP-bind-site-A2 SUBSTRATE n 
addmsg /kinetics/PKA/R2C2-cAMP3 /kinetics/PKA/cAMP-bind-site-A2 SUBSTRATE n 
addmsg /kinetics/PKA/R2C2-cAMP4 /kinetics/PKA/cAMP-bind-site-A2 PRODUCT n 
addmsg /kinetics/PKA/cAMP-bind-site-B2 /kinetics/PKA/R2C2-cAMP2 REAC B A 
addmsg /kinetics/PKA/cAMP-bind-site-A1 /kinetics/PKA/R2C2-cAMP2 REAC A B 
addmsg /kinetics/PKA/cAMP-bind-site-A1 /kinetics/PKA/R2C2-cAMP3 REAC B A 
addmsg /kinetics/PKA/cAMP-bind-site-A2 /kinetics/PKA/R2C2-cAMP3 REAC A B 
addmsg /kinetics/PKA/cAMP-bind-site-A2 /kinetics/PKA/R2C2-cAMP4 REAC B A 
addmsg /kinetics/PKA/Release-C1 /kinetics/PKA/R2C2-cAMP4 REAC A B 
addmsg /kinetics/PKA/Release-C1 /kinetics/PKA/R2C-cAMP4 REAC B A 
addmsg /kinetics/PKA/Release-C2 /kinetics/PKA/R2C-cAMP4 REAC A B 
addmsg /kinetics/PKA/Release-C2 /kinetics/PKA/R2-cAMP4 REAC B A 
addmsg /kinetics/PKA/R2C2-cAMP4 /kinetics/PKA/Release-C1 SUBSTRATE n 
addmsg /kinetics/PKA-active /kinetics/PKA/Release-C1 PRODUCT n 
addmsg /kinetics/PKA/R2C-cAMP4 /kinetics/PKA/Release-C1 PRODUCT n 
addmsg /kinetics/PKA/R2C-cAMP4 /kinetics/PKA/Release-C2 SUBSTRATE n 
addmsg /kinetics/PKA-active /kinetics/PKA/Release-C2 PRODUCT n 
addmsg /kinetics/PKA/R2-cAMP4 /kinetics/PKA/Release-C2 PRODUCT n 
addmsg /kinetics/PKA/inhib-PKA /kinetics/PKA/PKA-inhibitor REAC A B 
addmsg /kinetics/PKA-active /kinetics/PKA/inhib-PKA SUBSTRATE n 
addmsg /kinetics/PKA/PKA-inhibitor /kinetics/PKA/inhib-PKA SUBSTRATE n 
addmsg /kinetics/PKA/inhibited-PKA /kinetics/PKA/inhib-PKA PRODUCT n 
addmsg /kinetics/PKA/inhib-PKA /kinetics/PKA/inhibited-PKA REAC B A 
addmsg /kinetics/PKA/cAMP-bind-site-B1 /kinetics/cAMP REAC A B 
addmsg /kinetics/PKA/cAMP-bind-site-B2 /kinetics/cAMP REAC A B 
addmsg /kinetics/PKA/cAMP-bind-site-A1 /kinetics/cAMP REAC A B 
addmsg /kinetics/PKA/cAMP-bind-site-A2 /kinetics/cAMP REAC A B 
addmsg /kinetics/AC/AC2*/kenz /kinetics/cAMP MM_PRD pA 
addmsg /kinetics/AC/AC2-Gs/kenz /kinetics/cAMP MM_PRD pA 
addmsg /kinetics/AC/AC1-CaM/kenz /kinetics/cAMP MM_PRD pA 
addmsg /kinetics/AC/AC1-Gs/kenz /kinetics/cAMP MM_PRD pA 
addmsg /kinetics/AC/AC2*-Gs/kenz /kinetics/cAMP MM_PRD pA 
addmsg /kinetics/AC/cAMP-PDE*/PDE* /kinetics/cAMP REAC sA B 
addmsg /kinetics/AC/cAMP-PDE/PDE /kinetics/cAMP REAC sA B 
addmsg /kinetics/AC/PDE1/PDE1 /kinetics/cAMP REAC sA B 
addmsg /kinetics/AC/CaM.PDE1/CaM.PDE1 /kinetics/cAMP REAC sA B 
addmsg /kinetics/AC/AC2*/kenz /kinetics/AC/ATP REAC sA B 
addmsg /kinetics/AC/AC2-Gs/kenz /kinetics/AC/ATP REAC sA B 
addmsg /kinetics/AC/AC1-CaM/kenz /kinetics/AC/ATP REAC sA B 
addmsg /kinetics/AC/AC1-Gs/kenz /kinetics/AC/ATP REAC sA B 
addmsg /kinetics/AC/AC2*-Gs/kenz /kinetics/AC/ATP REAC sA B 
addmsg /kinetics/AC/CaM-bind-AC1 /kinetics/AC/AC1-CaM REAC B A 
addmsg /kinetics/AC/AC1-CaM /kinetics/AC/AC1-CaM/kenz ENZYME n 
addmsg /kinetics/AC/ATP /kinetics/AC/AC1-CaM/kenz SUBSTRATE n 
addmsg /kinetics/AC/CaM-bind-AC1 /kinetics/AC/AC1 REAC A B 
addmsg /kinetics/AC/Gs-bind-AC1 /kinetics/AC/AC1 REAC A B 
addmsg /kinetics/CaM-Ca4 /kinetics/AC/CaM-bind-AC1 SUBSTRATE n 
addmsg /kinetics/AC/AC1-CaM /kinetics/AC/CaM-bind-AC1 PRODUCT n 
addmsg /kinetics/AC/AC1 /kinetics/AC/CaM-bind-AC1 SUBSTRATE n 
addmsg /kinetics/PKC-active/phosph-AC2 /kinetics/AC/AC2* MM_PRD pA 
addmsg /kinetics/AC/dephosph-AC2 /kinetics/AC/AC2* REAC A B 
addmsg /kinetics/AC/Gs-bind-AC2* /kinetics/AC/AC2* REAC A B 
addmsg /kinetics/AC/AC2* /kinetics/AC/AC2*/kenz ENZYME n 
addmsg /kinetics/AC/ATP /kinetics/AC/AC2*/kenz SUBSTRATE n 
addmsg /kinetics/AC/Gs-bind-AC2 /kinetics/AC/AC2-Gs REAC B A 
addmsg /kinetics/AC/AC2-Gs /kinetics/AC/AC2-Gs/kenz ENZYME n 
addmsg /kinetics/AC/ATP /kinetics/AC/AC2-Gs/kenz SUBSTRATE n 
addmsg /kinetics/AC/Gs-bind-AC2 /kinetics/AC/AC2 REAC A B 
addmsg /kinetics/PKC-active/phosph-AC2 /kinetics/AC/AC2 REAC sA B 
addmsg /kinetics/AC/dephosph-AC2 /kinetics/AC/AC2 REAC B A 
addmsg /kinetics/AC/AC2* /kinetics/AC/dephosph-AC2 SUBSTRATE n 
addmsg /kinetics/AC/AC2 /kinetics/AC/dephosph-AC2 PRODUCT n 
addmsg /kinetics/AC/Gs-bind-AC1 /kinetics/AC/AC1-Gs REAC B A 
addmsg /kinetics/AC/AC1-Gs /kinetics/AC/AC1-Gs/kenz ENZYME n 
addmsg /kinetics/AC/ATP /kinetics/AC/AC1-Gs/kenz SUBSTRATE n 
addmsg /kinetics/AC/AC2 /kinetics/AC/Gs-bind-AC2 SUBSTRATE n 
addmsg /kinetics/AC/AC2-Gs /kinetics/AC/Gs-bind-AC2 PRODUCT n 
addmsg /kinetics/Gs-alpha /kinetics/AC/Gs-bind-AC2 SUBSTRATE n 
addmsg /kinetics/Gs-alpha /kinetics/AC/Gs-bind-AC1 SUBSTRATE n 
addmsg /kinetics/AC/AC1 /kinetics/AC/Gs-bind-AC1 SUBSTRATE n 
addmsg /kinetics/AC/AC1-Gs /kinetics/AC/Gs-bind-AC1 PRODUCT n 
addmsg /kinetics/AC/cAMP-PDE*/PDE* /kinetics/AC/AMP MM_PRD pA 
addmsg /kinetics/AC/cAMP-PDE/PDE /kinetics/AC/AMP MM_PRD pA 
addmsg /kinetics/AC/CaM.PDE1/CaM.PDE1 /kinetics/AC/AMP MM_PRD pA 
addmsg /kinetics/AC/PDE1/PDE1 /kinetics/AC/AMP MM_PRD pA 
addmsg /kinetics/AC/Gs-bind-AC2* /kinetics/AC/AC2*-Gs REAC B A 
addmsg /kinetics/AC/AC2*-Gs /kinetics/AC/AC2*-Gs/kenz ENZYME n 
addmsg /kinetics/AC/ATP /kinetics/AC/AC2*-Gs/kenz SUBSTRATE n 
addmsg /kinetics/Gs-alpha /kinetics/AC/Gs-bind-AC2* SUBSTRATE n 
addmsg /kinetics/AC/AC2*-Gs /kinetics/AC/Gs-bind-AC2* PRODUCT n 
addmsg /kinetics/AC/AC2* /kinetics/AC/Gs-bind-AC2* SUBSTRATE n 
addmsg /kinetics/AC/cAMP-PDE/PDE /kinetics/AC/cAMP-PDE REAC eA B 
addmsg /kinetics/AC/dephosph-PDE /kinetics/AC/cAMP-PDE REAC B A 
addmsg /kinetics/PKA-active/phosph-PDE /kinetics/AC/cAMP-PDE REAC sA B 
addmsg /kinetics/AC/cAMP-PDE /kinetics/AC/cAMP-PDE/PDE ENZYME n 
addmsg /kinetics/cAMP /kinetics/AC/cAMP-PDE/PDE SUBSTRATE n 
addmsg /kinetics/AC/cAMP-PDE*/PDE* /kinetics/AC/cAMP-PDE* REAC eA B 
addmsg /kinetics/AC/dephosph-PDE /kinetics/AC/cAMP-PDE* REAC A B 
addmsg /kinetics/PKA-active/phosph-PDE /kinetics/AC/cAMP-PDE* MM_PRD pA 
addmsg /kinetics/AC/cAMP-PDE* /kinetics/AC/cAMP-PDE*/PDE* ENZYME n 
addmsg /kinetics/cAMP /kinetics/AC/cAMP-PDE*/PDE* SUBSTRATE n 
addmsg /kinetics/AC/cAMP-PDE* /kinetics/AC/dephosph-PDE SUBSTRATE n 
addmsg /kinetics/AC/cAMP-PDE /kinetics/AC/dephosph-PDE PRODUCT n 
addmsg /kinetics/AC/PDE1/PDE1 /kinetics/AC/PDE1 REAC eA B 
addmsg /kinetics/AC/CaM_bind_PDE1 /kinetics/AC/PDE1 REAC A B 
addmsg /kinetics/AC/PDE1 /kinetics/AC/PDE1/PDE1 ENZYME n 
addmsg /kinetics/cAMP /kinetics/AC/PDE1/PDE1 SUBSTRATE n 
addmsg /kinetics/AC/CaM.PDE1/CaM.PDE1 /kinetics/AC/CaM.PDE1 REAC eA B 
addmsg /kinetics/AC/CaM_bind_PDE1 /kinetics/AC/CaM.PDE1 REAC B A 
addmsg /kinetics/AC/CaM.PDE1 /kinetics/AC/CaM.PDE1/CaM.PDE1 ENZYME n 
addmsg /kinetics/cAMP /kinetics/AC/CaM.PDE1/CaM.PDE1 SUBSTRATE n 
addmsg /kinetics/AC/PDE1 /kinetics/AC/CaM_bind_PDE1 SUBSTRATE n 
addmsg /kinetics/AC/CaM.PDE1 /kinetics/AC/CaM_bind_PDE1 PRODUCT n 
addmsg /kinetics/CaM-Ca4 /kinetics/AC/CaM_bind_PDE1 SUBSTRATE n 
addmsg /kinetics/AC/Gs-bind-AC2 /kinetics/Gs-alpha REAC A B 
addmsg /kinetics/AC/Gs-bind-AC1 /kinetics/Gs-alpha REAC A B 
addmsg /kinetics/AC/Gs-bind-AC2* /kinetics/Gs-alpha REAC A B 
addmsg /kinetics/PP1-active /kinetics/tot_PP1 SUMTOTAL n nInit 
addmsg /kinetics/PP1-active/Deph-thr286 /kinetics/tot_PP1 SUMTOTAL nComplex nComplexInit 
addmsg /kinetics/PP1-active/Deph-thr305 /kinetics/tot_PP1 SUMTOTAL nComplex nComplexInit 
addmsg /kinetics/PP1-active/Deph-thr286c /kinetics/tot_PP1 SUMTOTAL nComplex nComplexInit 
addmsg /kinetics/PP1-active/Deph_thr286b /kinetics/tot_PP1 SUMTOTAL nComplex nComplexInit 
addmsg /kinetics/PP1-active/Deph-thr306 /kinetics/tot_PP1 SUMTOTAL nComplex nComplexInit 
addmsg /kinetics/CaMKII/tot_CaM_CaMKII /kinetics/tot_CaMKII-active SUMTOTAL n nInit 
addmsg /kinetics/CaMKII/tot_autonomous_CaMKII /kinetics/tot_CaMKII-active SUMTOTAL n nInit 
addmsg /kinetics/CaM(Ca)n-CaNAB /kinetics/tot_CaN-active SUMTOTAL n nInit 
addmsg /kinetics/CaNAB-Ca4 /kinetics/tot_CaN-active SUMTOTAL n nInit 
addmsg /kinetics/Ca /graphs/conc1/Ca.Co PLOT Co *Ca.Co *red 
addmsg /kinetics/tot_CaMKII-active /graphs/conc1/tot_CaMKII-active.Co PLOT Co *tot_CaMKII-active.Co *blue 
addmsg /kinetics/tot_CaN-active /graphs/conc2/tot_CaN-active.Co PLOT Co *tot_CaN-active.Co *red 
addmsg /kinetics/PKA-active /moregraphs/conc3/PKA-active.Co PLOT Co *PKA-active.Co *yellow 
addmsg /kinetics/tot_PP1 /moregraphs/conc4/tot_PP1.Co PLOT Co *tot_PP1.Co *blue 
enddump
// End of dump

setfield /kinetics/Ca_tab table->dx 1
setfield /kinetics/Ca_tab table->invdx 1
call /kinetics/tot_PP1/notes LOAD \
" tot_PP1 has inputs from Deph-thr286, PP1-active, Deph-thr305, Deph-thr286c, Deph-thr286b,Deph-thr305. except PP1-active all other inputs are enzyme-substrate complex which is not currently supported by doqcs." \
"But can be accessed from GENESIS file."
call /kinetics/doqcsinfo/notes LOAD \
"This is the model file for figure 3 from <a href=&#34http://www.ncbi.nlm.nih.gov:80/entrez/query.fcgi?cmd=Retrieve&db=PubMed&list_uids=9888852&dopt=Abstract&#34>Bhalla US and Iyengar R. Science (1999) 283(5400):381-7</a>. It is a model of the Ca activation of CaMKII and other CaM-activated enzymes. It includes the regulatory phosphatases PP1 and PP2B (Calcineurin) acting on CaMKII and also includes CaM-activated adenylyl cyclase and PKA in the synapse.<br>Demonstration script files for generating the figures in the paper, including figure 3, are available <a href=http://www.ncbs.res.in/~bhalla/ltploop/ltploop/demos/>here</a>."
complete_loading
