//  DOQCS : http://doqcs.ncbs.res.in/ 
//  Accession Name = mkp1_feedback_effects 
//  Accession Number = 4 
//  Transcriber = Upinder S. Bhalla, NCBS 
//  Developer = Upinder S. Bhalla, NCBS 
//  Species = Generic mammalian 
//  Tissue = NIH 3T3 Expression 
//  Cell Compartment = Surface - nucleus 
//  Notes =  This is a network involving the MAPK-PKC feedback loop with input from the PDGFR in the synapse. The distinctive feature of this model is that it includes MKP-1 induction by MAPK, and the consequent inhibitory regulation of MAPK and the feedback loop. Lots of interesting dynamics arise from this. This <a href= http://www.ncbs.res.in/~bhalla/ltploop/mkpfeedback/index.html >link</a> provides supplementary material for the paper <a href= http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=pubmed&dopt=Abstract&list_uids=12169734 >Bhalla US et al. Science (2002) 297(5583):1018-23</a>. In the form of several example simulations and demos for the figures in the paper. 
 
  //genesis
// kkit Version 11 flat dumpfile
 
// Saved on Thu Dec  8 13:03:54 2005
 
include kkit {argv 1}
 
FASTDT = 0.0001
SIMDT = 0.005
CONTROLDT = 10
PLOTDT = 10
MAXTIME = 2000
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 1
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
simundump geometry /kinetics/geometry 0 1e-15 3 sphere "" white black 90 103 \
  0
simundump geometry /kinetics/geometry[1] 0 2e-16 3 sphere "" white black 90 \
  98 0
simundump geometry /kinetics/geometry[2] 0 1e-15 3 sphere "" white black 90 \
  93 0
simundump geometry /kinetics/geometry[3] 0 2e-16 3 sphere "" white black 90 \
  88 0
simundump kpool /kinetics/Shc*.Sos.Grb2 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] brown yellow 68 105 0
simundump kenz /kinetics/Shc*.Sos.Grb2/Sos.Ras_GEF 1 0 0 0 0 6e+05 3.3e-07 \
  0.08 0.02 0 0 "" red brown "" 65 94 0
simundump group /kinetics/Sos 1 blue black x 0 1 "" defaultfile \
  /home2/bhalla/scripts/modules/defaultfile_0.g 0 0 0 74 113 0
simundump kreac /kinetics/Sos/Shc_bind_Sos.Grb2 1 8.333e-07 0.1 "" white blue \
  69 107 0
simundump kpool /kinetics/Sos/Sos*.Grb2 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] orange blue 76 109 0
simundump kreac /kinetics/Sos/Grb2_bind_Sos* 1 4.1667e-08 0.0168 "" white \
  blue 75 107 0
simundump kpool /kinetics/Sos/Grb2 1 0 1 1 6e+05 6e+05 0 0 6e+05 0 \
  /kinetics/geometry[2] orange blue 73 105 0
simundump kpool /kinetics/Sos/Sos.Grb2 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] orange blue 70 105 0
simundump kpool /kinetics/Sos/Sos* 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] red blue 74 109 0
simundump kreac /kinetics/Sos/dephosph_Sos 1 0.001 0 "" white blue 73 107 0
simundump kreac /kinetics/Sos/Grb2_bind_Sos 1 4.1667e-08 0.0168 "" white blue \
  71 107 0
simundump kpool /kinetics/Sos/Sos 1 0 0.1 0.1 60000 60000 0 0 6e+05 0 \
  /kinetics/geometry[2] red blue 72 109 0
simundump group /kinetics/PKC 0 blue black x 0 0 "" defaultfile \
  /home2/bhalla/scripts/modules/defaultfile_0.g 0 0 0 47 80 0
simundump kpool /kinetics/PKC/PKC-Ca 0 0 3.7208e-17 3.7208e-17 2.2325e-11 \
  2.2325e-11 0 0 6e+05 0 /kinetics/geometry[2] red black 48 74 0
simundump kreac /kinetics/PKC/PKC-act-by-Ca 0 1e-06 0.5 "" white blue 48 72 0
simundump kreac /kinetics/PKC/PKC-act-by-DAG 0 1.3333e-08 8.6348 "" white \
  blue 56 72 0
simundump kreac /kinetics/PKC/PKC-Ca-to-memb 0 1.2705 3.5026 "" white blue 48 \
  76 0
simundump kreac /kinetics/PKC/PKC-DAG-to-memb 0 1 0.1 "" white blue 56 76 0
simundump kreac /kinetics/PKC/PKC-act-by-Ca-AA 0 2e-09 0.1 "" white blue 50 \
  76 0
simundump kreac /kinetics/PKC/PKC-act-by-DAG-AA 0 2 0.2 "" white blue 54 76 0
simundump kpool /kinetics/PKC/PKC-DAG-AA* 0 0 4.9137e-18 4.9137e-18 \
  2.9482e-12 2.9482e-12 0 0 6e+05 0 /kinetics/geometry[2] cyan blue 54 78 0
simundump kpool /kinetics/PKC/PKC-Ca-AA* 0 0 1.75e-16 1.75e-16 1.05e-10 \
  1.05e-10 0 0 6e+05 0 /kinetics/geometry[2] orange blue 50 78 0
simundump kpool /kinetics/PKC/PKC-Ca-memb* 0 0 1.3896e-17 1.3896e-17 \
  8.3376e-12 8.3376e-12 0 0 6e+05 0 /kinetics/geometry[2] pink blue 48 78 0
simundump kpool /kinetics/PKC/PKC-DAG-memb* 0 0 9.4352e-21 9.4352e-21 \
  5.6611e-15 5.6611e-15 0 0 6e+05 0 /kinetics/geometry[2] yellow blue 56 78 0
simundump kpool /kinetics/PKC/PKC-basal* 0 0 0.02 0.02 12000 12000 0 0 6e+05 \
  0 /kinetics/geometry[2] pink blue 46 78 0
simundump kreac /kinetics/PKC/PKC-basal-act 0 1 50 "" white blue 46 72 0
simundump kpool /kinetics/PKC/PKC-AA* 0 0 1.8133e-17 1.8133e-17 1.088e-11 \
  1.088e-11 0 0 6e+05 0 /kinetics/geometry[2] cyan blue 52 78 0
simundump kreac /kinetics/PKC/PKC-act-by-AA 0 2e-10 0.1 "" white blue 50 72 0
simundump kpool /kinetics/PKC/PKC-Ca-DAG 0 0 8.4632e-23 8.4632e-23 5.0779e-17 \
  5.0779e-17 0 0 6e+05 0 /kinetics/geometry[2] white blue 56 74 0
simundump kreac /kinetics/PKC/PKC-n-DAG 0 1e-09 0.1 "" white blue 52 72 0
simundump kpool /kinetics/PKC/PKC-DAG 0 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] white blue 52 74 0
simundump kreac /kinetics/PKC/PKC-n-DAG-AA 0 3e-08 2 "" white blue 54 72 0
simundump kpool /kinetics/PKC/PKC-DAG-AA 0 0 2.5188e-19 2.5188e-19 1.5113e-13 \
  1.5113e-13 0 0 6e+05 0 /kinetics/geometry[2] white blue 54 74 0
simundump kpool /kinetics/PKC/PKC-cytosolic 0 0 1 1 6e+05 6e+05 0 0 6e+05 0 \
  /kinetics/geometry[2] white blue 50 70 0
simundump kpool /kinetics/DAG 1 0 11.661 11.661 6.9966e+06 6.9966e+06 0 0 \
  6e+05 6 /kinetics/geometry[2] green black 54 60 0
simundump kpool /kinetics/Ca 1 0 0.08 0.08 48000 48000 0 0 6e+05 6 \
  /kinetics/geometry[2] red black 52 60 0
simundump kpool /kinetics/AA 0 0 6.12 6.12 3.672e+06 3.672e+06 0 0 6e+05 0 \
  /kinetics/geometry[2] darkgreen black 50 68 0
simundump kpool /kinetics/PKC-active 0 0 2.1222e-16 0.02 12000 12000 0 0 \
  6e+05 2 /kinetics/geometry[2] yellow black 52 80 0
simundump kenz /kinetics/PKC-active/PKC-act-raf 1 0 0 0 0 6e+05 5e-07 16 4 0 \
  0 "" red yellow "" 64 88 0
simundump kenz /kinetics/PKC-active/PKC-inact-GAP 1 0 0 0 0 6e+05 3.125e-06 \
  100 25 0 0 "" red yellow "" 60 98 0
simundump kenz /kinetics/PKC-active/PKC-act-GEF 1 0 0 0 0 6e+05 5e-07 16 4 0 \
  0 "" red yellow "" 68 98 0
simundump group /kinetics/MAPK 0 brown black x 0 0 "" defaultfile \
  /home2/bhalla/scripts/modules/defaultfile_0.g 0 0 0 69 88 0
simundump kpool /kinetics/MAPK/craf-1 0 0 0.2 0.2 1.2e+05 1.2e+05 0 0 6e+05 0 \
  /kinetics/geometry[2] pink brown 63 86 0
simundump kpool /kinetics/MAPK/craf-1* 0 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] pink brown 65 86 0
simundump kpool /kinetics/MAPK/MAPKK 0 0 0.18 0.18 1.08e+05 1.08e+05 0 0 \
  6e+05 0 /kinetics/geometry[2] pink brown 64 82 0
simundump kpool /kinetics/MAPK/MAPK 0 0 0.36 0.36 2.16e+05 2.16e+05 0 0 6e+05 \
  0 /kinetics/geometry[2] pink brown 67 78 0
simundump kpool /kinetics/MAPK/craf-1** 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] hotpink brown 67 86 0
simundump kpool /kinetics/MAPK/MAPK-tyr 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] orange brown 67 76 0
simundump kpool /kinetics/MAPK/MAPKK* 0 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] pink brown 64 78 0
simundump kenz /kinetics/MAPK/MAPKK*/MAPKKtyr 0 0 0 0 0 6e+05 2.7e-05 0.6 \
  0.15 0 0 "" red pink "" 64 77 0
simundump kenz /kinetics/MAPK/MAPKK*/MAPKKthr 1 0 0 0 0 6e+05 2.7e-05 0.6 \
  0.15 0 0 "" red pink "" 64 75 0
simundump kpool /kinetics/MAPK/MAPKK-ser 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] pink brown 64 80 0
simundump kpool /kinetics/MAPK/RGR 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] red brown 62 82 0
simundump kenz /kinetics/MAPK/RGR/RGR.1 1 0 0 0 0 6e+05 5.5e-06 0.42 0.105 0 \
  0 "" red red "" 62 81 0
simundump kenz /kinetics/MAPK/RGR/RGR.2 1 0 0 0 0 6e+05 5.5e-06 0.42 0.105 0 \
  0 "" red red "" 62 79 0
simundump kpool /kinetics/MAPK/Ubiquitination 1 0 0 0 0 0 0 0 6e+05 4 \
  /kinetics/geometry[2] magenta brown 59 72 0
simundump kreac /kinetics/MAPK/turnover_MKP1 1 0.00037 0 "" white brown 61 75 \
  0
simundump kpool /kinetics/MAPK/MKP-1-ser359* 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] pink brown 64 72 0
simundump kpool /kinetics/MAPK/MKP-1** 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] hotpink brown 64 70 0
simundump kenz /kinetics/MAPK/MKP-1**/MKP1*-tyr-deph 1 0 0 0 0 6e+05 0.000125 \
  4 1 0 1 "" red hotpink "" 68 77 0
simundump kenz /kinetics/MAPK/MKP-1**/MKP1*-thr-deph 1 0 0 0 0 6e+05 0.000125 \
  4 1 0 1 "" red hotpink "" 68 75 0
simundump kreac /kinetics/MAPK/turnover_MKP1** 1 1e-04 0 "" white brown 61 69 \
  0
simundump kreac /kinetics/MAPK/turnover_MKP1* 1 1e-04 0 "" white brown 61 72 \
  0
simundump kreac /kinetics/MAPK/MKP-1*dephosph 1 1e-04 0 "" white brown 61 73 \
  0
simundump kreac /kinetics/MAPK/MKP-1**dephosph 1 1e-04 0 "" white brown 61 71 \
  0
simundump kreac /kinetics/MAPK/translocation 1 1.6667e-08 0.005 "" white \
  brown 68 73 0
simundump kpool /kinetics/MAPK/nuc_MAPK* 0 0 0 0 0 0 0 0 1.2e+05 0 \
  /kinetics/geometry[3] 47 brown 70 72 0
simundump kenz /kinetics/MAPK/nuc_MAPK*/act_transcription 0 0 0 0 0 1.2e+05 \
  8.3333e-09 0.0032 0.0008 0 0 "" red 47 "" 69 71 0
simundump kpool /kinetics/MAPK/MKP1-RNA 0 0 0.0001 0.0001 12 12 0 0 1.2e+05 0 \
  /kinetics/geometry[3] 1 brown 68 72 0
simundump kreac /kinetics/MAPK/MKP1_synth 0 0.002 0 "" white brown 66 73 0
simundump kpool /kinetics/MAPK/Nucleotides 0 0 0.1 0.1 12000 12000 0 0 \
  1.2e+05 4 /kinetics/geometry[3] blue brown 68 70 0
simundump kreac /kinetics/MAPK/Basal_transcription 0 5e-06 0 "" white brown \
  67 71 0
simundump kpool /kinetics/MAPK/Raf*-GTP-Ras 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] red brown 66 82 0
simundump kenz /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.1 1 0 0 0 0 6e+05 \
  5.5e-06 0.42 0.105 0 0 "" red red "" 65 81 0
simundump kenz /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.2 1 0 0 0 0 6e+05 \
  5.5e-06 0.42 0.105 0 0 "" red red "" 65 79 0
simundump kpool /kinetics/MAPK* 0 0 0 0 0 0 0 0 6e+05 0 /kinetics/geometry[2] \
  orange yellow 67 74 0
simundump kenz /kinetics/MAPK*/MAPK*-feedback 1 0 0 0 0 6e+05 3.25e-06 40 10 \
  0 0 "" red orange "" 66 84 0
simundump kenz /kinetics/MAPK*/MAPK* 0 0 0 0 0 6e+05 6.5e-06 80 20 0 0 "" red \
  orange "" 52 57 0
simundump kenz /kinetics/MAPK*/MKP-1-phosph 1 0 0 0 0 6e+05 3.25e-07 4 1 0 0 \
  "" red orange "" 63 73 0
simundump kenz /kinetics/MAPK*/MKP-1-phosph2 1 0 0 0 0 6e+05 3.25e-07 4 1 0 0 \
  "" red orange "" 63 71 0
simundump kpool /kinetics/MKP-1 1 0 0.0004 0.0004 240 240 0 0 6e+05 0 \
  /kinetics/geometry[2] hotpink black 64 74 0
simundump kenz /kinetics/MKP-1/MKP1-tyr-deph 1 0 0 0 0 6e+05 0.000125 4 1 0 1 \
  "" red hotpink "" 66 77 0
simundump kenz /kinetics/MKP-1/MKP1-thr-deph 1 0 0 0 0 6e+05 0.000125 4 1 0 1 \
  "" red hotpink "" 66 75 0
simundump kreac /kinetics/Ras-act-craf 1 1e-04 0.5 "" white black 68 84 0
simundump kpool /kinetics/PPhosphatase2A 1 0 0.224 0.224 1.344e+05 1.344e+05 \
  0 0 6e+05 0 /kinetics/geometry[2] hotpink yellow 68 80 0
simundump kenz /kinetics/PPhosphatase2A/craf-deph 1 0 0 0 0 6e+05 3.3e-06 25 \
  6 0 0 "" red hotpink "" 64 84 0
simundump kenz /kinetics/PPhosphatase2A/MAPKK-deph 1 0 0 0 0 6e+05 3.3e-06 25 \
  6 0 0 "" red hotpink "" 68 79 0
simundump kenz /kinetics/PPhosphatase2A/MAPKK-deph-ser 1 0 0 0 0 6e+05 \
  3.3e-06 25 6 0 0 "" red hotpink "" 68 81 0
simundump kenz /kinetics/PPhosphatase2A/craf**-deph 1 0 0 0 0 1 3.3e-06 25 6 \
  0 0 "" red hotpink "" 66 88 0
simundump group /kinetics/PLA2 0 darkgreen black x 0 1 "" defaultfile \
  /home2/bhalla/scripts/modules/defaultfile_0.g 0 0 0 56 58 0
simundump kpool /kinetics/PLA2/PLA2-cytosolic 0 0 0.4 0.4 2.4e+05 2.4e+05 0 0 \
  6e+05 0 /kinetics/geometry[2] yellow darkgreen 48 57 0
simundump kreac /kinetics/PLA2/PLA2-Ca-act 0 1.6667e-06 0.1 "" white \
  darkgreen 51 62 0
simundump kpool /kinetics/PLA2/PLA2-Ca* 0 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] yellow darkgreen 52 64 0
simundump kenz /kinetics/PLA2/PLA2-Ca*/kenz 0 0 0 0 0 6e+05 2.25e-06 21.6 5.4 \
  0 0 "" red yellow "" 52 66 0
simundump kreac /kinetics/PLA2/PIP2-PLA2-act 0 2e-09 0.5 "" white darkgreen \
  47 62 0
simundump kpool /kinetics/PLA2/PIP2-PLA2* 0 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] cyan darkgreen 46 64 0
simundump kenz /kinetics/PLA2/PIP2-PLA2*/kenz 0 0 0 0 0 6e+05 4.6e-06 44.16 \
  11.04 0 0 "" red cyan "" 46 66 0
simundump kreac /kinetics/PLA2/PIP2-Ca-PLA2-act 0 2e-08 0.1 "" white \
  darkgreen 49 62 0
simundump kpool /kinetics/PLA2/PIP2-Ca-PLA2* 0 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] cyan darkgreen 48 64 0
simundump kenz /kinetics/PLA2/PIP2-Ca-PLA2*/kenz 0 0 0 0 0 6e+05 1.5e-05 144 \
  36 0 0 "" red cyan "" 48 66 0
simundump kreac /kinetics/PLA2/DAG-Ca-PLA2-act 0 5e-09 4 "" white darkgreen \
  53 62 0
simundump kpool /kinetics/PLA2/DAG-Ca-PLA2* 0 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] pink darkgreen 54 64 0
simundump kenz /kinetics/PLA2/DAG-Ca-PLA2*/kenz 0 0 0 0 0 6e+05 2.5e-05 240 \
  60 0 0 "" red pink "" 54 66 0
simundump kpool /kinetics/PLA2/APC 0 0 30 30 1.8e+07 1.8e+07 0 0 6e+05 5 \
  /kinetics/geometry[2] yellow darkgreen 50 64 0
simundump kreac /kinetics/PLA2/Degrade-AA 1 0.4 0 "" white darkgreen 50 66 0
simundump kpool /kinetics/PLA2/PLA2*-Ca 0 0 0 0 0 0 0 0 6e+05 1 \
  /kinetics/geometry[2] orange darkgreen 56 64 0
simundump kenz /kinetics/PLA2/PLA2*-Ca/kenz 0 0 0 0 0 6e+05 5e-05 480 120 0 0 \
  "" red orange "" 56 66 0
simundump kpool /kinetics/PLA2/PLA2* 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] orange darkgreen 56 60 0
simundump kreac /kinetics/PLA2/PLA2*-Ca-act 1 1e-05 0.1 "" white darkgreen 55 \
  62 0
simundump kreac /kinetics/PLA2/dephosphorylate-PLA2* 1 0.17 0 "" white \
  darkgreen 52 59 0
simundump kpool /kinetics/temp-PIP2 1 0 2.5 2.5 1.5e+06 1.5e+06 0 0 6e+05 6 \
  /kinetics/geometry[2] green black 48 60 0
simundump group /kinetics/Ras 1 blue black x 0 0 "" defaultfile \
  /home2/bhalla/scripts/modules/defaultfile_0.g 0 0 0 64 100 0
simundump kreac /kinetics/Ras/dephosph-GEF 1 0.1 0 "" white blue 66 98 0
simundump kpool /kinetics/Ras/inact-GEF 1 0 0.1 0.1 60000 60000 0 0 6e+05 0 \
  /kinetics/geometry[2] hotpink blue 67 100 0
simundump kpool /kinetics/Ras/GEF* 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] hotpink blue 67 96 0
simundump kenz /kinetics/Ras/GEF*/GEF*-act-ras 1 0 0 0 0 6e+05 3.3e-07 0.08 \
  0.02 0 0 "" red hotpink "" 67 94 0
simundump kpool /kinetics/Ras/GTP-Ras 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] orange blue 64 92 0
simundump kpool /kinetics/Ras/GDP-Ras 1 0 0.2 0.2 1.2e+05 1.2e+05 0 0 6e+05 0 \
  /kinetics/geometry[2] pink blue 64 96 0
simundump kreac /kinetics/Ras/Ras-intrinsic-GTPase 1 1e-04 0 "" white blue 63 \
  94 0
simundump kreac /kinetics/Ras/dephosph-GAP 1 0.1 0 "" white blue 62 98 0
simundump kpool /kinetics/Ras/GAP* 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] red blue 61 100 0
simundump kpool /kinetics/Ras/GAP 1 0 0.002 0.002 1200 1200 0 0 6e+05 0 \
  /kinetics/geometry[2] red blue 61 96 0
simundump kenz /kinetics/Ras/GAP/GAP-inact-ras 1 0 0 0 0 6e+05 0.001666 1000 \
  10 0 0 "" red red "" 61 94 0
simundump kreac /kinetics/Ras-act-unphosph-raf 1 1e-05 1 "" white black 62 84 \
  0
simundump group /kinetics/PDGFR 1 yellow black x 0 1 "" defaultfile \
  /home2/bhalla/scripts/modules/defaultfile_0.g 0 0 0 64 113 0
simundump kpool /kinetics/PDGFR/PDGFR 1 0 0.10833 0.10833 65000 65000 0 0 \
  6e+05 0 /kinetics/geometry[2] red yellow 59 113 0
simundump kreac /kinetics/PDGFR/act_PDGFR 1 0.00033333 0.1 "" white yellow 60 \
  111 0
simundump kpool /kinetics/PDGFR/L.PDGFR 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] red yellow 61 109 0
simundump kenz /kinetics/PDGFR/L.PDGFR/phosph_Shc 1 0 0 0 0 6e+05 5e-07 0.2 \
  0.05 0 0 "" red red "" 64 111 0
simundump kenz /kinetics/PDGFR/L.PDGFR/phosph_PLC_g 1 0 0 0 0 6e+05 5e-06 0.8 \
  0.2 0 0 "" red red "" 61 107 0
simundump kpool /kinetics/PDGFR/PDGF 1 0 0 0 0 0 0 0 6e+05 4 \
  /kinetics/geometry[2] red yellow 59 109 0
simundump kpool /kinetics/PDGFR/SHC 1 0 0.5 0.5 3e+05 3e+05 0 0 6e+05 0 \
  /kinetics/geometry[2] orange yellow 63 109 0
simundump kpool /kinetics/PDGFR/SHC* 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] orange yellow 65 109 0
simundump kreac /kinetics/PDGFR/dephosph_Shc 1 0.01 0 "" white yellow 64 107 \
  0
simundump kpool /kinetics/PDGFR/Internal_L.PDGFR 1 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] red yellow 61 113 0
simundump kreac /kinetics/PDGFR/Internalize 1 0.001 0.00066 "" white yellow \
  62 111 0
simundump kpool /kinetics/tot_MKP1 0 0 0 0.0004 240 0 0 0 6e+05 0 \
  /kinetics/geometry[2] 30 black 66 72 0
simundump kpool /kinetics/MKP-2 0 0 0.002 0.002 1200 1200 0 0 6e+05 0 \
  /kinetics/geometry[2] 4 black 70 76 0
simundump kenz /kinetics/MKP-2/MKP2-tyr-deph 0 0 0 0 0 6e+05 0.000125 4 1 0 0 \
  "" red 4 "" 70 77 0
simundump kenz /kinetics/MKP-2/MKP2-thr-deph 0 0 0 0 0 6e+05 0.000125 4 1 0 0 \
  "" red 4 "" 70 75 0
simundump kpool /kinetics/tot_MAPK 0 0 0 0 0 0 0 0 6e+05 0 \
  /kinetics/geometry[2] blue black 70 74 0
simundump doqcsinfo /kinetics/doqcsinfo 0 db4.g mkp1_feedback_effects network \
  "Upinder S. Bhalla, NCBS" "Upinder S. Bhalla, NCBS" "citation here" \
  "Generic Mammalian" "NIH 3T3 Expression" "Surface - nucleus" \
  "Quantitative match to experiments" \
  "<a href = http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=pubmed&dopt=Abstract&list_uids=12169734Bhalla US et al. Science (2002) 297(5583):1018-23</a>( Peer-reviewed publication )" \
  "Exact GENESIS implementation" \
  "Replicates original data , Quantitatively predicts new data" 90 108 0
simundump xgraph /graphs/conc1 0 0 26000 0 0.12896 0
simundump xgraph /graphs/conc2 0 0 26000 0 0.00053347 0
simundump xplot /graphs/conc1/MAPK*.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" orange 0 0 1
simundump xplot /graphs/conc1/PKC-active.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" yellow 0 0 1
simundump xplot /graphs/conc1/nuc_MAPK*.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 47 0 0 1
simundump xplot /graphs/conc1/tot_MAPK.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc2/MKP-1.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" hotpink 0 0 1
simundump xplot /graphs/conc2/PDGFR.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0 0 1
simundump xplot /graphs/conc2/PDGF.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" red 0 0 1
simundump xplot /graphs/conc2/tot_MKP1.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 30 0 0 1
simundump xgraph /moregraphs/conc3 0 0 26000 0 1 0
simundump xgraph /moregraphs/conc4 0 0 26000 0 1 0
simundump xcoredraw /edit/draw 0 13 113 -2 117
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"Based on fb28c.g, cleaning up notes and in a couple of cases" \
"the naming of pools. No parameter changes." \
"This version has notes for all parameters."
addmsg /kinetics/Sos/Shc_bind_Sos.Grb2 /kinetics/Shc*.Sos.Grb2 REAC B A 
addmsg /kinetics/Shc*.Sos.Grb2/Sos.Ras_GEF /kinetics/Shc*.Sos.Grb2 REAC eA B 
addmsg /kinetics/Shc*.Sos.Grb2 /kinetics/Shc*.Sos.Grb2/Sos.Ras_GEF ENZYME n 
addmsg /kinetics/Ras/GDP-Ras /kinetics/Shc*.Sos.Grb2/Sos.Ras_GEF SUBSTRATE n 
addmsg /kinetics/Sos/Sos.Grb2 /kinetics/Sos/Shc_bind_Sos.Grb2 SUBSTRATE n 
addmsg /kinetics/Shc*.Sos.Grb2 /kinetics/Sos/Shc_bind_Sos.Grb2 PRODUCT n 
addmsg /kinetics/PDGFR/SHC* /kinetics/Sos/Shc_bind_Sos.Grb2 SUBSTRATE n 
addmsg /kinetics/Sos/Grb2_bind_Sos* /kinetics/Sos/Sos*.Grb2 REAC B A 
addmsg /kinetics/Sos/Sos* /kinetics/Sos/Grb2_bind_Sos* SUBSTRATE n 
addmsg /kinetics/Sos/Grb2 /kinetics/Sos/Grb2_bind_Sos* SUBSTRATE n 
addmsg /kinetics/Sos/Sos*.Grb2 /kinetics/Sos/Grb2_bind_Sos* PRODUCT n 
addmsg /kinetics/Sos/Grb2_bind_Sos /kinetics/Sos/Grb2 REAC A B 
addmsg /kinetics/Sos/Grb2_bind_Sos* /kinetics/Sos/Grb2 REAC A B 
addmsg /kinetics/Sos/Grb2_bind_Sos /kinetics/Sos/Sos.Grb2 REAC B A 
addmsg /kinetics/Sos/Shc_bind_Sos.Grb2 /kinetics/Sos/Sos.Grb2 REAC A B 
addmsg /kinetics/Sos/Grb2_bind_Sos* /kinetics/Sos/Sos* REAC A B 
addmsg /kinetics/Sos/dephosph_Sos /kinetics/Sos/Sos* REAC A B 
addmsg /kinetics/Sos/Sos* /kinetics/Sos/dephosph_Sos SUBSTRATE n 
addmsg /kinetics/Sos/Sos /kinetics/Sos/dephosph_Sos PRODUCT n 
addmsg /kinetics/Sos/Grb2 /kinetics/Sos/Grb2_bind_Sos SUBSTRATE n 
addmsg /kinetics/Sos/Sos.Grb2 /kinetics/Sos/Grb2_bind_Sos PRODUCT n 
addmsg /kinetics/Sos/Sos /kinetics/Sos/Grb2_bind_Sos SUBSTRATE n 
addmsg /kinetics/Sos/Grb2_bind_Sos /kinetics/Sos/Sos REAC A B 
addmsg /kinetics/Sos/dephosph_Sos /kinetics/Sos/Sos REAC B A 
addmsg /kinetics/PKC/PKC-act-by-Ca /kinetics/PKC/PKC-Ca REAC B A 
addmsg /kinetics/PKC/PKC-act-by-DAG /kinetics/PKC/PKC-Ca REAC A B 
addmsg /kinetics/PKC/PKC-Ca-to-memb /kinetics/PKC/PKC-Ca REAC A B 
addmsg /kinetics/PKC/PKC-act-by-Ca-AA /kinetics/PKC/PKC-Ca REAC A B 
addmsg /kinetics/PKC/PKC-cytosolic /kinetics/PKC/PKC-act-by-Ca SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/PKC/PKC-act-by-Ca SUBSTRATE n 
addmsg /kinetics/PKC/PKC-Ca /kinetics/PKC/PKC-act-by-Ca PRODUCT n 
addmsg /kinetics/DAG /kinetics/PKC/PKC-act-by-DAG SUBSTRATE n 
addmsg /kinetics/PKC/PKC-Ca /kinetics/PKC/PKC-act-by-DAG SUBSTRATE n 
addmsg /kinetics/PKC/PKC-Ca-DAG /kinetics/PKC/PKC-act-by-DAG PRODUCT n 
addmsg /kinetics/PKC/PKC-Ca /kinetics/PKC/PKC-Ca-to-memb SUBSTRATE n 
addmsg /kinetics/PKC/PKC-Ca-memb* /kinetics/PKC/PKC-Ca-to-memb PRODUCT n 
addmsg /kinetics/PKC/PKC-Ca-DAG /kinetics/PKC/PKC-DAG-to-memb SUBSTRATE n 
addmsg /kinetics/PKC/PKC-DAG-memb* /kinetics/PKC/PKC-DAG-to-memb PRODUCT n 
addmsg /kinetics/PKC/PKC-Ca /kinetics/PKC/PKC-act-by-Ca-AA SUBSTRATE n 
addmsg /kinetics/AA /kinetics/PKC/PKC-act-by-Ca-AA SUBSTRATE n 
addmsg /kinetics/PKC/PKC-Ca-AA* /kinetics/PKC/PKC-act-by-Ca-AA PRODUCT n 
addmsg /kinetics/PKC/PKC-DAG-AA* /kinetics/PKC/PKC-act-by-DAG-AA PRODUCT n 
addmsg /kinetics/PKC/PKC-DAG-AA /kinetics/PKC/PKC-act-by-DAG-AA SUBSTRATE n 
addmsg /kinetics/PKC/PKC-act-by-DAG-AA /kinetics/PKC/PKC-DAG-AA* REAC B A 
addmsg /kinetics/PKC/PKC-act-by-Ca-AA /kinetics/PKC/PKC-Ca-AA* REAC B A 
addmsg /kinetics/PKC/PKC-Ca-to-memb /kinetics/PKC/PKC-Ca-memb* REAC B A 
addmsg /kinetics/PKC/PKC-DAG-to-memb /kinetics/PKC/PKC-DAG-memb* REAC B A 
addmsg /kinetics/PKC/PKC-basal-act /kinetics/PKC/PKC-basal* REAC B A 
addmsg /kinetics/PKC/PKC-cytosolic /kinetics/PKC/PKC-basal-act SUBSTRATE n 
addmsg /kinetics/PKC/PKC-basal* /kinetics/PKC/PKC-basal-act PRODUCT n 
addmsg /kinetics/PKC/PKC-act-by-AA /kinetics/PKC/PKC-AA* REAC B A 
addmsg /kinetics/AA /kinetics/PKC/PKC-act-by-AA SUBSTRATE n 
addmsg /kinetics/PKC/PKC-AA* /kinetics/PKC/PKC-act-by-AA PRODUCT n 
addmsg /kinetics/PKC/PKC-cytosolic /kinetics/PKC/PKC-act-by-AA SUBSTRATE n 
addmsg /kinetics/PKC/PKC-act-by-DAG /kinetics/PKC/PKC-Ca-DAG REAC B A 
addmsg /kinetics/PKC/PKC-DAG-to-memb /kinetics/PKC/PKC-Ca-DAG REAC A B 
addmsg /kinetics/PKC/PKC-cytosolic /kinetics/PKC/PKC-n-DAG SUBSTRATE n 
addmsg /kinetics/DAG /kinetics/PKC/PKC-n-DAG SUBSTRATE n 
addmsg /kinetics/PKC/PKC-DAG /kinetics/PKC/PKC-n-DAG PRODUCT n 
addmsg /kinetics/PKC/PKC-n-DAG /kinetics/PKC/PKC-DAG REAC B A 
addmsg /kinetics/PKC/PKC-n-DAG-AA /kinetics/PKC/PKC-DAG REAC A B 
addmsg /kinetics/PKC/PKC-DAG /kinetics/PKC/PKC-n-DAG-AA SUBSTRATE n 
addmsg /kinetics/AA /kinetics/PKC/PKC-n-DAG-AA SUBSTRATE n 
addmsg /kinetics/PKC/PKC-DAG-AA /kinetics/PKC/PKC-n-DAG-AA PRODUCT n 
addmsg /kinetics/PKC/PKC-n-DAG-AA /kinetics/PKC/PKC-DAG-AA REAC B A 
addmsg /kinetics/PKC/PKC-act-by-DAG-AA /kinetics/PKC/PKC-DAG-AA REAC A B 
addmsg /kinetics/PKC/PKC-act-by-Ca /kinetics/PKC/PKC-cytosolic REAC A B 
addmsg /kinetics/PKC/PKC-basal-act /kinetics/PKC/PKC-cytosolic REAC A B 
addmsg /kinetics/PKC/PKC-act-by-AA /kinetics/PKC/PKC-cytosolic REAC A B 
addmsg /kinetics/PKC/PKC-n-DAG /kinetics/PKC/PKC-cytosolic REAC A B 
addmsg /kinetics/PKC/PKC-act-by-DAG /kinetics/DAG REAC A B 
addmsg /kinetics/PKC/PKC-n-DAG /kinetics/DAG REAC A B 
addmsg /kinetics/PLA2/DAG-Ca-PLA2-act /kinetics/DAG REAC A B 
addmsg /kinetics/PKC/PKC-act-by-Ca /kinetics/Ca REAC A B 
addmsg /kinetics/PLA2/PLA2-Ca-act /kinetics/Ca REAC A B 
addmsg /kinetics/PLA2/PLA2*-Ca-act /kinetics/Ca REAC A B 
addmsg /kinetics/PKC/PKC-act-by-Ca-AA /kinetics/AA REAC A B 
addmsg /kinetics/PKC/PKC-act-by-AA /kinetics/AA REAC A B 
addmsg /kinetics/PKC/PKC-n-DAG-AA /kinetics/AA REAC A B 
addmsg /kinetics/PLA2/PLA2-Ca*/kenz /kinetics/AA MM_PRD pA 
addmsg /kinetics/PLA2/PIP2-PLA2*/kenz /kinetics/AA MM_PRD pA 
addmsg /kinetics/PLA2/PIP2-Ca-PLA2*/kenz /kinetics/AA MM_PRD pA 
addmsg /kinetics/PLA2/DAG-Ca-PLA2*/kenz /kinetics/AA MM_PRD pA 
addmsg /kinetics/PLA2/PLA2*-Ca/kenz /kinetics/AA MM_PRD pA 
addmsg /kinetics/PLA2/Degrade-AA /kinetics/AA REAC A B 
addmsg /kinetics/PKC/PKC-DAG-AA* /kinetics/PKC-active SUMTOTAL n nInit 
addmsg /kinetics/PKC/PKC-Ca-memb* /kinetics/PKC-active SUMTOTAL n nInit 
addmsg /kinetics/PKC/PKC-Ca-AA* /kinetics/PKC-active SUMTOTAL n nInit 
addmsg /kinetics/PKC/PKC-DAG-memb* /kinetics/PKC-active SUMTOTAL n nInit 
addmsg /kinetics/PKC/PKC-basal* /kinetics/PKC-active SUMTOTAL n nInit 
addmsg /kinetics/PKC/PKC-AA* /kinetics/PKC-active SUMTOTAL n nInit 
addmsg /kinetics/PKC-active/PKC-act-raf /kinetics/PKC-active CONSERVE nComplex nComplexInit 
addmsg /kinetics/PKC-active/PKC-inact-GAP /kinetics/PKC-active REAC eA B 
addmsg /kinetics/PKC-active/PKC-inact-GAP /kinetics/PKC-active CONSERVE nComplex nComplexInit 
addmsg /kinetics/PKC-active/PKC-act-GEF /kinetics/PKC-active REAC eA B 
addmsg /kinetics/PKC-active/PKC-act-GEF /kinetics/PKC-active CONSERVE nComplex nComplexInit 
addmsg /kinetics/PKC-active/PKC-act-raf /kinetics/PKC-active REAC eA B 
addmsg /kinetics/PKC-active /kinetics/PKC-active/PKC-act-raf ENZYME n 
addmsg /kinetics/MAPK/craf-1 /kinetics/PKC-active/PKC-act-raf SUBSTRATE n 
addmsg /kinetics/PKC-active /kinetics/PKC-active/PKC-inact-GAP ENZYME n 
addmsg /kinetics/Ras/GAP /kinetics/PKC-active/PKC-inact-GAP SUBSTRATE n 
addmsg /kinetics/PKC-active /kinetics/PKC-active/PKC-act-GEF ENZYME n 
addmsg /kinetics/Ras/inact-GEF /kinetics/PKC-active/PKC-act-GEF SUBSTRATE n 
addmsg /kinetics/PKC-active/PKC-act-raf /kinetics/MAPK/craf-1 REAC sA B 
addmsg /kinetics/PPhosphatase2A/craf-deph /kinetics/MAPK/craf-1 MM_PRD pA 
addmsg /kinetics/Ras-act-unphosph-raf /kinetics/MAPK/craf-1 REAC A B 
addmsg /kinetics/PKC-active/PKC-act-raf /kinetics/MAPK/craf-1* MM_PRD pA 
addmsg /kinetics/MAPK*/MAPK*-feedback /kinetics/MAPK/craf-1* REAC sA B 
addmsg /kinetics/PPhosphatase2A/craf-deph /kinetics/MAPK/craf-1* REAC sA B 
addmsg /kinetics/PPhosphatase2A/craf**-deph /kinetics/MAPK/craf-1* MM_PRD pA 
addmsg /kinetics/Ras-act-craf /kinetics/MAPK/craf-1* REAC A B 
addmsg /kinetics/PPhosphatase2A/MAPKK-deph-ser /kinetics/MAPK/MAPKK MM_PRD pA 
addmsg /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.1 /kinetics/MAPK/MAPKK REAC sA B 
addmsg /kinetics/MAPK/RGR/RGR.1 /kinetics/MAPK/MAPKK REAC sA B 
addmsg /kinetics/MAPK/MAPKK*/MAPKKtyr /kinetics/MAPK/MAPK REAC sA B 
addmsg /kinetics/MKP-1/MKP1-tyr-deph /kinetics/MAPK/MAPK MM_PRD pA 
addmsg /kinetics/MAPK/MKP-1**/MKP1*-tyr-deph /kinetics/MAPK/MAPK MM_PRD pA 
addmsg /kinetics/MKP-2/MKP2-tyr-deph /kinetics/MAPK/MAPK MM_PRD pA 
addmsg /kinetics/MAPK*/MAPK*-feedback /kinetics/MAPK/craf-1** MM_PRD pA 
addmsg /kinetics/PPhosphatase2A/craf**-deph /kinetics/MAPK/craf-1** REAC sA B 
addmsg /kinetics/MAPK/MAPKK*/MAPKKtyr /kinetics/MAPK/MAPK-tyr MM_PRD pA 
addmsg /kinetics/MAPK/MAPKK*/MAPKKthr /kinetics/MAPK/MAPK-tyr REAC sA B 
addmsg /kinetics/MKP-1/MKP1-tyr-deph /kinetics/MAPK/MAPK-tyr REAC sA B 
addmsg /kinetics/MKP-1/MKP1-thr-deph /kinetics/MAPK/MAPK-tyr MM_PRD pA 
addmsg /kinetics/MAPK/MKP-1**/MKP1*-tyr-deph /kinetics/MAPK/MAPK-tyr REAC sA B 
addmsg /kinetics/MAPK/MKP-1**/MKP1*-thr-deph /kinetics/MAPK/MAPK-tyr MM_PRD pA 
addmsg /kinetics/MKP-2/MKP2-tyr-deph /kinetics/MAPK/MAPK-tyr REAC sA B 
addmsg /kinetics/MKP-2/MKP2-thr-deph /kinetics/MAPK/MAPK-tyr MM_PRD pA 
addmsg /kinetics/MAPK/MAPKK*/MAPKKtyr /kinetics/MAPK/MAPKK* REAC eA B 
addmsg /kinetics/MAPK/MAPKK*/MAPKKthr /kinetics/MAPK/MAPKK* REAC eA B 
addmsg /kinetics/PPhosphatase2A/MAPKK-deph /kinetics/MAPK/MAPKK* REAC sA B 
addmsg /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.2 /kinetics/MAPK/MAPKK* MM_PRD pA 
addmsg /kinetics/MAPK/RGR/RGR.2 /kinetics/MAPK/MAPKK* MM_PRD pA 
addmsg /kinetics/MAPK/MAPKK* /kinetics/MAPK/MAPKK*/MAPKKtyr ENZYME n 
addmsg /kinetics/MAPK/MAPK /kinetics/MAPK/MAPKK*/MAPKKtyr SUBSTRATE n 
addmsg /kinetics/MAPK/MAPKK* /kinetics/MAPK/MAPKK*/MAPKKthr ENZYME n 
addmsg /kinetics/MAPK/MAPK-tyr /kinetics/MAPK/MAPKK*/MAPKKthr SUBSTRATE n 
addmsg /kinetics/PPhosphatase2A/MAPKK-deph /kinetics/MAPK/MAPKK-ser MM_PRD pA 
addmsg /kinetics/PPhosphatase2A/MAPKK-deph-ser /kinetics/MAPK/MAPKK-ser REAC sA B 
addmsg /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.1 /kinetics/MAPK/MAPKK-ser MM_PRD pA 
addmsg /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.2 /kinetics/MAPK/MAPKK-ser REAC sA B 
addmsg /kinetics/MAPK/RGR/RGR.1 /kinetics/MAPK/MAPKK-ser MM_PRD pA 
addmsg /kinetics/MAPK/RGR/RGR.2 /kinetics/MAPK/MAPKK-ser REAC sA B 
addmsg /kinetics/MAPK/RGR/RGR.1 /kinetics/MAPK/RGR REAC eA B 
addmsg /kinetics/MAPK/RGR/RGR.2 /kinetics/MAPK/RGR REAC eA B 
addmsg /kinetics/Ras-act-unphosph-raf /kinetics/MAPK/RGR REAC B A 
addmsg /kinetics/MAPK/RGR /kinetics/MAPK/RGR/RGR.1 ENZYME n 
addmsg /kinetics/MAPK/MAPKK /kinetics/MAPK/RGR/RGR.1 SUBSTRATE n 
addmsg /kinetics/MAPK/RGR /kinetics/MAPK/RGR/RGR.2 ENZYME n 
addmsg /kinetics/MAPK/MAPKK-ser /kinetics/MAPK/RGR/RGR.2 SUBSTRATE n 
addmsg /kinetics/MAPK/turnover_MKP1** /kinetics/MAPK/Ubiquitination REAC B A 
addmsg /kinetics/MAPK/turnover_MKP1 /kinetics/MAPK/Ubiquitination REAC B A 
addmsg /kinetics/MAPK/turnover_MKP1* /kinetics/MAPK/Ubiquitination REAC B A 
addmsg /kinetics/MKP-1 /kinetics/MAPK/turnover_MKP1 SUBSTRATE n 
addmsg /kinetics/MAPK/Ubiquitination /kinetics/MAPK/turnover_MKP1 PRODUCT n 
addmsg /kinetics/MAPK*/MKP-1-phosph /kinetics/MAPK/MKP-1-ser359* MM_PRD pA 
addmsg /kinetics/MAPK*/MKP-1-phosph2 /kinetics/MAPK/MKP-1-ser359* REAC sA B 
addmsg /kinetics/MAPK/turnover_MKP1* /kinetics/MAPK/MKP-1-ser359* REAC A B 
addmsg /kinetics/MAPK/MKP-1*dephosph /kinetics/MAPK/MKP-1-ser359* REAC A B 
addmsg /kinetics/MAPK/MKP-1**dephosph /kinetics/MAPK/MKP-1-ser359* REAC B A 
addmsg /kinetics/MAPK/turnover_MKP1** /kinetics/MAPK/MKP-1** REAC A B 
addmsg /kinetics/MAPK*/MKP-1-phosph2 /kinetics/MAPK/MKP-1** MM_PRD pA 
addmsg /kinetics/MAPK/MKP-1**dephosph /kinetics/MAPK/MKP-1** REAC A B 
addmsg /kinetics/MAPK/MKP-1** /kinetics/MAPK/MKP-1**/MKP1*-tyr-deph ENZYME n 
addmsg /kinetics/MAPK/MAPK-tyr /kinetics/MAPK/MKP-1**/MKP1*-tyr-deph SUBSTRATE n 
addmsg /kinetics/MAPK/MKP-1** /kinetics/MAPK/MKP-1**/MKP1*-thr-deph ENZYME n 
addmsg /kinetics/MAPK* /kinetics/MAPK/MKP-1**/MKP1*-thr-deph SUBSTRATE n 
addmsg /kinetics/MAPK/MKP-1** /kinetics/MAPK/turnover_MKP1** SUBSTRATE n 
addmsg /kinetics/MAPK/Ubiquitination /kinetics/MAPK/turnover_MKP1** PRODUCT n 
addmsg /kinetics/MAPK/MKP-1-ser359* /kinetics/MAPK/turnover_MKP1* SUBSTRATE n 
addmsg /kinetics/MAPK/Ubiquitination /kinetics/MAPK/turnover_MKP1* PRODUCT n 
addmsg /kinetics/MAPK/MKP-1-ser359* /kinetics/MAPK/MKP-1*dephosph SUBSTRATE n 
addmsg /kinetics/MKP-1 /kinetics/MAPK/MKP-1*dephosph PRODUCT n 
addmsg /kinetics/MAPK/MKP-1** /kinetics/MAPK/MKP-1**dephosph SUBSTRATE n 
addmsg /kinetics/MAPK/MKP-1-ser359* /kinetics/MAPK/MKP-1**dephosph PRODUCT n 
addmsg /kinetics/MAPK* /kinetics/MAPK/translocation SUBSTRATE n 
addmsg /kinetics/MAPK/nuc_MAPK* /kinetics/MAPK/translocation PRODUCT n 
addmsg /kinetics/MAPK* /kinetics/MAPK/translocation SUBSTRATE n 
addmsg /kinetics/MAPK/translocation /kinetics/MAPK/nuc_MAPK* REAC B A 
addmsg /kinetics/MAPK/nuc_MAPK*/act_transcription /kinetics/MAPK/nuc_MAPK* REAC eA B 
addmsg /kinetics/MAPK/nuc_MAPK* /kinetics/MAPK/nuc_MAPK*/act_transcription ENZYME n 
addmsg /kinetics/MAPK/Nucleotides /kinetics/MAPK/nuc_MAPK*/act_transcription SUBSTRATE n 
addmsg /kinetics/MAPK/MKP1_synth /kinetics/MAPK/MKP1-RNA REAC A B 
addmsg /kinetics/MAPK/nuc_MAPK*/act_transcription /kinetics/MAPK/MKP1-RNA MM_PRD pA 
addmsg /kinetics/MAPK/Basal_transcription /kinetics/MAPK/MKP1-RNA REAC B A 
addmsg /kinetics/MAPK/MKP1-RNA /kinetics/MAPK/MKP1_synth SUBSTRATE n 
addmsg /kinetics/MKP-1 /kinetics/MAPK/MKP1_synth PRODUCT n 
addmsg /kinetics/MAPK/nuc_MAPK*/act_transcription /kinetics/MAPK/Nucleotides REAC sA B 
addmsg /kinetics/MAPK/Basal_transcription /kinetics/MAPK/Nucleotides REAC A B 
addmsg /kinetics/MAPK/Nucleotides /kinetics/MAPK/Basal_transcription SUBSTRATE n 
addmsg /kinetics/MAPK/MKP1-RNA /kinetics/MAPK/Basal_transcription PRODUCT n 
addmsg /kinetics/Ras-act-craf /kinetics/MAPK/Raf*-GTP-Ras REAC B A 
addmsg /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.1 /kinetics/MAPK/Raf*-GTP-Ras REAC eA B 
addmsg /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.2 /kinetics/MAPK/Raf*-GTP-Ras REAC eA B 
addmsg /kinetics/MAPK/Raf*-GTP-Ras /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.1 ENZYME n 
addmsg /kinetics/MAPK/MAPKK /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.1 SUBSTRATE n 
addmsg /kinetics/MAPK/Raf*-GTP-Ras /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.2 ENZYME n 
addmsg /kinetics/MAPK/MAPKK-ser /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.2 SUBSTRATE n 
addmsg /kinetics/MAPK*/MAPK*-feedback /kinetics/MAPK* REAC eA B 
addmsg /kinetics/MAPK/MAPKK*/MAPKKthr /kinetics/MAPK* MM_PRD pA 
addmsg /kinetics/MKP-1/MKP1-thr-deph /kinetics/MAPK* REAC sA B 
addmsg /kinetics/MAPK*/MAPK* /kinetics/MAPK* REAC eA B 
addmsg /kinetics/MAPK*/MKP-1-phosph /kinetics/MAPK* REAC eA B 
addmsg /kinetics/MAPK/MKP-1**/MKP1*-thr-deph /kinetics/MAPK* REAC sA B 
addmsg /kinetics/MAPK*/MKP-1-phosph2 /kinetics/MAPK* REAC eA B 
addmsg /kinetics/MAPK/translocation /kinetics/MAPK* REAC A B 
addmsg /kinetics/MKP-2/MKP2-thr-deph /kinetics/MAPK* REAC sA B 
addmsg /kinetics/MAPK/translocation /kinetics/MAPK* REAC A B 
addmsg /kinetics/MAPK* /kinetics/MAPK*/MAPK*-feedback ENZYME n 
addmsg /kinetics/MAPK/craf-1* /kinetics/MAPK*/MAPK*-feedback SUBSTRATE n 
addmsg /kinetics/MAPK* /kinetics/MAPK*/MAPK* ENZYME n 
addmsg /kinetics/PLA2/PLA2-cytosolic /kinetics/MAPK*/MAPK* SUBSTRATE n 
addmsg /kinetics/MAPK* /kinetics/MAPK*/MKP-1-phosph ENZYME n 
addmsg /kinetics/MKP-1 /kinetics/MAPK*/MKP-1-phosph SUBSTRATE n 
addmsg /kinetics/MAPK* /kinetics/MAPK*/MKP-1-phosph2 ENZYME n 
addmsg /kinetics/MAPK/MKP-1-ser359* /kinetics/MAPK*/MKP-1-phosph2 SUBSTRATE n 
addmsg /kinetics/MAPK*/MKP-1-phosph /kinetics/MKP-1 REAC sA B 
addmsg /kinetics/MAPK/turnover_MKP1 /kinetics/MKP-1 REAC A B 
addmsg /kinetics/MAPK/MKP-1*dephosph /kinetics/MKP-1 REAC B A 
addmsg /kinetics/MAPK/MKP1_synth /kinetics/MKP-1 REAC B A 
addmsg /kinetics/MKP-1 /kinetics/MKP-1/MKP1-tyr-deph ENZYME n 
addmsg /kinetics/MAPK/MAPK-tyr /kinetics/MKP-1/MKP1-tyr-deph SUBSTRATE n 
addmsg /kinetics/MKP-1 /kinetics/MKP-1/MKP1-thr-deph ENZYME n 
addmsg /kinetics/MAPK* /kinetics/MKP-1/MKP1-thr-deph SUBSTRATE n 
addmsg /kinetics/MAPK/Raf*-GTP-Ras /kinetics/Ras-act-craf PRODUCT n 
addmsg /kinetics/MAPK/craf-1* /kinetics/Ras-act-craf SUBSTRATE n 
addmsg /kinetics/Ras/GTP-Ras /kinetics/Ras-act-craf SUBSTRATE n 
addmsg /kinetics/PPhosphatase2A/craf-deph /kinetics/PPhosphatase2A REAC eA B 
addmsg /kinetics/PPhosphatase2A/MAPKK-deph /kinetics/PPhosphatase2A REAC eA B 
addmsg /kinetics/PPhosphatase2A/MAPKK-deph-ser /kinetics/PPhosphatase2A REAC eA B 
addmsg /kinetics/PPhosphatase2A/craf**-deph /kinetics/PPhosphatase2A REAC eA B 
addmsg /kinetics/PPhosphatase2A /kinetics/PPhosphatase2A/craf-deph ENZYME n 
addmsg /kinetics/MAPK/craf-1* /kinetics/PPhosphatase2A/craf-deph SUBSTRATE n 
addmsg /kinetics/PPhosphatase2A /kinetics/PPhosphatase2A/MAPKK-deph ENZYME n 
addmsg /kinetics/MAPK/MAPKK* /kinetics/PPhosphatase2A/MAPKK-deph SUBSTRATE n 
addmsg /kinetics/PPhosphatase2A /kinetics/PPhosphatase2A/MAPKK-deph-ser ENZYME n 
addmsg /kinetics/MAPK/MAPKK-ser /kinetics/PPhosphatase2A/MAPKK-deph-ser SUBSTRATE n 
addmsg /kinetics/PPhosphatase2A /kinetics/PPhosphatase2A/craf**-deph ENZYME n 
addmsg /kinetics/MAPK/craf-1** /kinetics/PPhosphatase2A/craf**-deph SUBSTRATE n 
addmsg /kinetics/PLA2/PLA2-Ca-act /kinetics/PLA2/PLA2-cytosolic REAC A B 
addmsg /kinetics/PLA2/PIP2-PLA2-act /kinetics/PLA2/PLA2-cytosolic REAC A B 
addmsg /kinetics/PLA2/PIP2-PLA2* /kinetics/PLA2/PLA2-cytosolic CONSERVE n nInit 
addmsg /kinetics/PLA2/PIP2-Ca-PLA2* /kinetics/PLA2/PLA2-cytosolic CONSERVE n nInit 
addmsg /kinetics/PLA2/DAG-Ca-PLA2* /kinetics/PLA2/PLA2-cytosolic CONSERVE n nInit 
addmsg /kinetics/PLA2/PLA2-Ca* /kinetics/PLA2/PLA2-cytosolic CONSERVE n nInit 
addmsg /kinetics/PLA2/PLA2*-Ca /kinetics/PLA2/PLA2-cytosolic CONSERVE n nInit 
addmsg /kinetics/MAPK*/MAPK* /kinetics/PLA2/PLA2-cytosolic CONSERVE nComplex nComplexInit 
addmsg /kinetics/PLA2/PLA2*-Ca/kenz /kinetics/PLA2/PLA2-cytosolic CONSERVE nComplex nComplexInit 
addmsg /kinetics/PLA2/PLA2-Ca*/kenz /kinetics/PLA2/PLA2-cytosolic CONSERVE nComplex nComplexInit 
addmsg /kinetics/PLA2/DAG-Ca-PLA2*/kenz /kinetics/PLA2/PLA2-cytosolic CONSERVE nComplex nComplexInit 
addmsg /kinetics/PLA2/PIP2-Ca-PLA2*/kenz /kinetics/PLA2/PLA2-cytosolic CONSERVE nComplex nComplexInit 
addmsg /kinetics/PLA2/PIP2-PLA2*/kenz /kinetics/PLA2/PLA2-cytosolic CONSERVE nComplex nComplexInit 
addmsg /kinetics/MAPK*/MAPK* /kinetics/PLA2/PLA2-cytosolic REAC sA B 
addmsg /kinetics/PLA2/PLA2* /kinetics/PLA2/PLA2-cytosolic CONSERVE n nInit 
addmsg /kinetics/PLA2/dephosphorylate-PLA2* /kinetics/PLA2/PLA2-cytosolic REAC B A 
addmsg /kinetics/PLA2/PLA2-cytosolic /kinetics/PLA2/PLA2-Ca-act SUBSTRATE n 
addmsg /kinetics/Ca /kinetics/PLA2/PLA2-Ca-act SUBSTRATE n 
addmsg /kinetics/PLA2/PLA2-Ca* /kinetics/PLA2/PLA2-Ca-act PRODUCT n 
addmsg /kinetics/PLA2/PLA2-Ca-act /kinetics/PLA2/PLA2-Ca* REAC B A 
addmsg /kinetics/PLA2/PLA2-Ca*/kenz /kinetics/PLA2/PLA2-Ca* REAC eA B 
addmsg /kinetics/PLA2/PIP2-Ca-PLA2-act /kinetics/PLA2/PLA2-Ca* REAC A B 
addmsg /kinetics/PLA2/DAG-Ca-PLA2-act /kinetics/PLA2/PLA2-Ca* REAC A B 
addmsg /kinetics/PLA2/PLA2-Ca* /kinetics/PLA2/PLA2-Ca*/kenz ENZYME n 
addmsg /kinetics/PLA2/APC /kinetics/PLA2/PLA2-Ca*/kenz SUBSTRATE n 
addmsg /kinetics/temp-PIP2 /kinetics/PLA2/PIP2-PLA2-act SUBSTRATE n 
addmsg /kinetics/PLA2/PLA2-cytosolic /kinetics/PLA2/PIP2-PLA2-act SUBSTRATE n 
addmsg /kinetics/PLA2/PIP2-PLA2* /kinetics/PLA2/PIP2-PLA2-act PRODUCT n 
addmsg /kinetics/PLA2/PIP2-PLA2-act /kinetics/PLA2/PIP2-PLA2* REAC B A 
addmsg /kinetics/PLA2/PIP2-PLA2*/kenz /kinetics/PLA2/PIP2-PLA2* REAC eA B 
addmsg /kinetics/PLA2/PIP2-PLA2* /kinetics/PLA2/PIP2-PLA2*/kenz ENZYME n 
addmsg /kinetics/PLA2/APC /kinetics/PLA2/PIP2-PLA2*/kenz SUBSTRATE n 
addmsg /kinetics/temp-PIP2 /kinetics/PLA2/PIP2-Ca-PLA2-act SUBSTRATE n 
addmsg /kinetics/PLA2/PLA2-Ca* /kinetics/PLA2/PIP2-Ca-PLA2-act SUBSTRATE n 
addmsg /kinetics/PLA2/PIP2-Ca-PLA2* /kinetics/PLA2/PIP2-Ca-PLA2-act PRODUCT n 
addmsg /kinetics/PLA2/PIP2-Ca-PLA2-act /kinetics/PLA2/PIP2-Ca-PLA2* REAC B A 
addmsg /kinetics/PLA2/PIP2-Ca-PLA2*/kenz /kinetics/PLA2/PIP2-Ca-PLA2* REAC eA B 
addmsg /kinetics/PLA2/PIP2-Ca-PLA2* /kinetics/PLA2/PIP2-Ca-PLA2*/kenz ENZYME n 
addmsg /kinetics/PLA2/APC /kinetics/PLA2/PIP2-Ca-PLA2*/kenz SUBSTRATE n 
addmsg /kinetics/DAG /kinetics/PLA2/DAG-Ca-PLA2-act SUBSTRATE n 
addmsg /kinetics/PLA2/PLA2-Ca* /kinetics/PLA2/DAG-Ca-PLA2-act SUBSTRATE n 
addmsg /kinetics/PLA2/DAG-Ca-PLA2* /kinetics/PLA2/DAG-Ca-PLA2-act PRODUCT n 
addmsg /kinetics/PLA2/DAG-Ca-PLA2-act /kinetics/PLA2/DAG-Ca-PLA2* REAC B A 
addmsg /kinetics/PLA2/DAG-Ca-PLA2*/kenz /kinetics/PLA2/DAG-Ca-PLA2* REAC eA B 
addmsg /kinetics/PLA2/DAG-Ca-PLA2* /kinetics/PLA2/DAG-Ca-PLA2*/kenz ENZYME n 
addmsg /kinetics/PLA2/APC /kinetics/PLA2/DAG-Ca-PLA2*/kenz SUBSTRATE n 
addmsg /kinetics/PLA2/PLA2-Ca*/kenz /kinetics/PLA2/APC REAC sA B 
addmsg /kinetics/PLA2/PIP2-PLA2*/kenz /kinetics/PLA2/APC REAC sA B 
addmsg /kinetics/PLA2/PIP2-Ca-PLA2*/kenz /kinetics/PLA2/APC REAC sA B 
addmsg /kinetics/PLA2/DAG-Ca-PLA2*/kenz /kinetics/PLA2/APC REAC sA B 
addmsg /kinetics/PLA2/PLA2*-Ca/kenz /kinetics/PLA2/APC REAC sA B 
addmsg /kinetics/PLA2/Degrade-AA /kinetics/PLA2/APC REAC B A 
addmsg /kinetics/AA /kinetics/PLA2/Degrade-AA SUBSTRATE n 
addmsg /kinetics/PLA2/APC /kinetics/PLA2/Degrade-AA PRODUCT n 
addmsg /kinetics/PLA2/PLA2*-Ca/kenz /kinetics/PLA2/PLA2*-Ca REAC eA B 
addmsg /kinetics/PLA2/PLA2*-Ca-act /kinetics/PLA2/PLA2*-Ca REAC B A 
addmsg /kinetics/PLA2/PLA2*-Ca /kinetics/PLA2/PLA2*-Ca/kenz ENZYME n 
addmsg /kinetics/PLA2/APC /kinetics/PLA2/PLA2*-Ca/kenz SUBSTRATE n 
addmsg /kinetics/MAPK*/MAPK* /kinetics/PLA2/PLA2* MM_PRD pA 
addmsg /kinetics/PLA2/PLA2*-Ca-act /kinetics/PLA2/PLA2* REAC A B 
addmsg /kinetics/PLA2/dephosphorylate-PLA2* /kinetics/PLA2/PLA2* REAC A B 
addmsg /kinetics/PLA2/PLA2* /kinetics/PLA2/PLA2*-Ca-act SUBSTRATE n 
addmsg /kinetics/PLA2/PLA2*-Ca /kinetics/PLA2/PLA2*-Ca-act PRODUCT n 
addmsg /kinetics/Ca /kinetics/PLA2/PLA2*-Ca-act SUBSTRATE n 
addmsg /kinetics/PLA2/PLA2* /kinetics/PLA2/dephosphorylate-PLA2* SUBSTRATE n 
addmsg /kinetics/PLA2/PLA2-cytosolic /kinetics/PLA2/dephosphorylate-PLA2* PRODUCT n 
addmsg /kinetics/PLA2/PIP2-PLA2-act /kinetics/temp-PIP2 REAC A B 
addmsg /kinetics/PLA2/PIP2-Ca-PLA2-act /kinetics/temp-PIP2 REAC A B 
addmsg /kinetics/Ras/GEF* /kinetics/Ras/dephosph-GEF SUBSTRATE n 
addmsg /kinetics/Ras/inact-GEF /kinetics/Ras/dephosph-GEF PRODUCT n 
addmsg /kinetics/PKC-active/PKC-act-GEF /kinetics/Ras/inact-GEF REAC sA B 
addmsg /kinetics/Ras/dephosph-GEF /kinetics/Ras/inact-GEF REAC B A 
addmsg /kinetics/PKC-active/PKC-act-GEF /kinetics/Ras/GEF* MM_PRD pA 
addmsg /kinetics/Ras/dephosph-GEF /kinetics/Ras/GEF* REAC A B 
addmsg /kinetics/Ras/GEF*/GEF*-act-ras /kinetics/Ras/GEF* REAC eA B 
addmsg /kinetics/Ras/GEF* /kinetics/Ras/GEF*/GEF*-act-ras ENZYME n 
addmsg /kinetics/Ras/GDP-Ras /kinetics/Ras/GEF*/GEF*-act-ras SUBSTRATE n 
addmsg /kinetics/Ras/GAP/GAP-inact-ras /kinetics/Ras/GTP-Ras REAC sA B 
addmsg /kinetics/Ras/Ras-intrinsic-GTPase /kinetics/Ras/GTP-Ras REAC A B 
addmsg /kinetics/Ras/GEF*/GEF*-act-ras /kinetics/Ras/GTP-Ras MM_PRD pA 
addmsg /kinetics/Ras-act-craf /kinetics/Ras/GTP-Ras REAC A B 
addmsg /kinetics/Shc*.Sos.Grb2/Sos.Ras_GEF /kinetics/Ras/GTP-Ras MM_PRD pA 
addmsg /kinetics/Ras-act-unphosph-raf /kinetics/Ras/GTP-Ras REAC A B 
addmsg /kinetics/Ras/GAP/GAP-inact-ras /kinetics/Ras/GDP-Ras MM_PRD pA 
addmsg /kinetics/Ras/Ras-intrinsic-GTPase /kinetics/Ras/GDP-Ras REAC B A 
addmsg /kinetics/Ras/GEF*/GEF*-act-ras /kinetics/Ras/GDP-Ras REAC sA B 
addmsg /kinetics/Shc*.Sos.Grb2/Sos.Ras_GEF /kinetics/Ras/GDP-Ras REAC sA B 
addmsg /kinetics/Ras/GTP-Ras /kinetics/Ras/Ras-intrinsic-GTPase SUBSTRATE n 
addmsg /kinetics/Ras/GDP-Ras /kinetics/Ras/Ras-intrinsic-GTPase PRODUCT n 
addmsg /kinetics/Ras/GAP* /kinetics/Ras/dephosph-GAP SUBSTRATE n 
addmsg /kinetics/Ras/GAP /kinetics/Ras/dephosph-GAP PRODUCT n 
addmsg /kinetics/PKC-active/PKC-inact-GAP /kinetics/Ras/GAP* MM_PRD pA 
addmsg /kinetics/Ras/dephosph-GAP /kinetics/Ras/GAP* REAC A B 
addmsg /kinetics/Ras/GAP/GAP-inact-ras /kinetics/Ras/GAP REAC eA B 
addmsg /kinetics/PKC-active/PKC-inact-GAP /kinetics/Ras/GAP REAC sA B 
addmsg /kinetics/Ras/dephosph-GAP /kinetics/Ras/GAP REAC B A 
addmsg /kinetics/Ras/GAP /kinetics/Ras/GAP/GAP-inact-ras ENZYME n 
addmsg /kinetics/Ras/GTP-Ras /kinetics/Ras/GAP/GAP-inact-ras SUBSTRATE n 
addmsg /kinetics/MAPK/craf-1 /kinetics/Ras-act-unphosph-raf SUBSTRATE n 
addmsg /kinetics/MAPK/RGR /kinetics/Ras-act-unphosph-raf PRODUCT n 
addmsg /kinetics/Ras/GTP-Ras /kinetics/Ras-act-unphosph-raf SUBSTRATE n 
addmsg /kinetics/PDGFR/act_PDGFR /kinetics/PDGFR/PDGFR REAC A B 
addmsg /kinetics/PDGFR/PDGFR /kinetics/PDGFR/act_PDGFR SUBSTRATE n 
addmsg /kinetics/PDGFR/PDGF /kinetics/PDGFR/act_PDGFR SUBSTRATE n 
addmsg /kinetics/PDGFR/L.PDGFR /kinetics/PDGFR/act_PDGFR PRODUCT n 
addmsg /kinetics/PDGFR/act_PDGFR /kinetics/PDGFR/L.PDGFR REAC B A 
addmsg /kinetics/PDGFR/L.PDGFR/phosph_Shc /kinetics/PDGFR/L.PDGFR REAC eA B 
addmsg /kinetics/PDGFR/Internalize /kinetics/PDGFR/L.PDGFR REAC A B 
addmsg /kinetics/PDGFR/L.PDGFR/phosph_PLC_g /kinetics/PDGFR/L.PDGFR REAC eA B 
addmsg /kinetics/PDGFR/L.PDGFR /kinetics/PDGFR/L.PDGFR/phosph_Shc ENZYME n 
addmsg /kinetics/PDGFR/SHC /kinetics/PDGFR/L.PDGFR/phosph_Shc SUBSTRATE n 
addmsg /kinetics/PDGFR/L.PDGFR /kinetics/PDGFR/L.PDGFR/phosph_PLC_g ENZYME n 
addmsg /kinetics/PDGFR/act_PDGFR /kinetics/PDGFR/PDGF REAC A B 
addmsg /kinetics/PDGFR/dephosph_Shc /kinetics/PDGFR/SHC REAC B A 
addmsg /kinetics/PDGFR/L.PDGFR/phosph_Shc /kinetics/PDGFR/SHC REAC sA B 
addmsg /kinetics/PDGFR/dephosph_Shc /kinetics/PDGFR/SHC* REAC A B 
addmsg /kinetics/Sos/Shc_bind_Sos.Grb2 /kinetics/PDGFR/SHC* REAC A B 
addmsg /kinetics/PDGFR/L.PDGFR/phosph_Shc /kinetics/PDGFR/SHC* MM_PRD pA 
addmsg /kinetics/PDGFR/SHC* /kinetics/PDGFR/dephosph_Shc SUBSTRATE n 
addmsg /kinetics/PDGFR/SHC /kinetics/PDGFR/dephosph_Shc PRODUCT n 
addmsg /kinetics/PDGFR/Internalize /kinetics/PDGFR/Internal_L.PDGFR REAC B A 
addmsg /kinetics/PDGFR/L.PDGFR /kinetics/PDGFR/Internalize SUBSTRATE n 
addmsg /kinetics/PDGFR/Internal_L.PDGFR /kinetics/PDGFR/Internalize PRODUCT n 
addmsg /kinetics/MKP-1 /kinetics/tot_MKP1 SUMTOTAL n nInit 
addmsg /kinetics/MAPK/MKP-1-ser359* /kinetics/tot_MKP1 SUMTOTAL n nInit 
addmsg /kinetics/MAPK/MKP-1** /kinetics/tot_MKP1 SUMTOTAL n nInit 
addmsg /kinetics/MKP-2/MKP2-tyr-deph /kinetics/MKP-2 REAC eA B 
addmsg /kinetics/MKP-2/MKP2-thr-deph /kinetics/MKP-2 REAC eA B 
addmsg /kinetics/MKP-2 /kinetics/MKP-2/MKP2-tyr-deph ENZYME n 
addmsg /kinetics/MAPK/MAPK-tyr /kinetics/MKP-2/MKP2-tyr-deph SUBSTRATE n 
addmsg /kinetics/MKP-2 /kinetics/MKP-2/MKP2-thr-deph ENZYME n 
addmsg /kinetics/MAPK* /kinetics/MKP-2/MKP2-thr-deph SUBSTRATE n 
addmsg /kinetics/MAPK* /kinetics/tot_MAPK SUMTOTAL n nInit 
addmsg /kinetics/MAPK/nuc_MAPK* /kinetics/tot_MAPK SUMTOTAL n nInit 
addmsg /kinetics/MAPK* /graphs/conc1/MAPK*.Co PLOT Co *MAPK*.Co *orange 
addmsg /kinetics/PKC-active /graphs/conc1/PKC-active.Co PLOT Co *PKC-active.Co *yellow 
addmsg /kinetics/MAPK/nuc_MAPK* /graphs/conc1/nuc_MAPK*.Co PLOT Co *nuc_MAPK*.Co *47 
addmsg /kinetics/tot_MAPK /graphs/conc1/tot_MAPK.Co PLOT Co *tot_MAPK.Co *blue 
addmsg /kinetics/MKP-1 /graphs/conc2/MKP-1.Co PLOT Co *MKP-1.Co *hotpink 
addmsg /kinetics/PDGFR/PDGFR /graphs/conc2/PDGFR.Co PLOT Co *PDGFR.Co *red 
addmsg /kinetics/PDGFR/PDGF /graphs/conc2/PDGF.Co PLOT Co *PDGF.Co *red 
addmsg /kinetics/tot_MKP1 /graphs/conc2/tot_MKP1.Co PLOT Co *tot_MKP1.Co *30 
enddump
// End of dump

call /kinetics/Shc*.Sos.Grb2/notes LOAD \
"This three-way complex is one of the main GEFs for activating Ras."
call /kinetics/Shc*.Sos.Grb2/Sos.Ras_GEF/notes LOAD \
"Rates from Orita et al JBC 268(34):25542-25546"
call /kinetics/Sos/notes LOAD \
"This represents the mSos protein and the Grb2 adapter protein" \
"involved in Ras activation. This module provides for input from" \
"RTKs as well as feedback inhibition from MAPK, although the" \
"latter is not implemented in this specific model."
call /kinetics/Sos/Shc_bind_Sos.Grb2/notes LOAD \
"Sasaoka et al JBC 269:51 pp 32621 1994, table on pg" \
"32623 indicates that this pathway accounts for about " \
"50% of the GEF activation. (88% - 39%). Error is large," \
"about 20%. Fig 1 is most useful in constraining rates." \
"" \
"Chook et al JBC 271:48 pp 30472, 1996 say that the Kd is" \
"0.2 uM for Shc binding to EGFR. The Kd for Grb direct binding" \
"is 0.7, so we'll ignore it."
call /kinetics/Sos/Sos*.Grb2/notes LOAD \
"Inactive complex of Sos* with Grb2 due to phosphorylation of the Sos. [<a href = http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=pubmed&dopt=Abstract&list_uids=8621459>Porfiri E and McCormick F. (1996) J Biol Chem. 271(10):5871-5877</a>]"
call /kinetics/Sos/Grb2_bind_Sos*/notes LOAD \
"Same rates as Grb2_bind_Sos: Porfiri and McCormick JBC" \
"271:10 pp 5871 1996 show that the binding is not affected" \
"by the phosphorylation."
call /kinetics/Sos/Grb2/notes LOAD \
"There is probably a lot of it in the cell: it is also known" \
"as Ash (abundant src homology protein). Also " \
"Waters et al JBC 271:30 18224 1996 say that only a small" \
"fraction of cellular Grb is precipitated out when SoS is" \
"precipitated. As most of the Sos seems to be associated" \
"with Grb2, it would seem like there is a lot of the latter." \
"Say 1 uM. This would comfortably saturate the SoS."
call /kinetics/Sos/Sos.Grb2/notes LOAD \
"For simplicity I treat the activation of Sos as involving a" \
"single complex comprising Sos, Grb2 and Shc*. This is" \
"reasonably documented:" \
"Sasaoka et al 1994 JBC 269(51):32621-5" \
"Chook et al JBC 1996 271(48):30472" \
""
call /kinetics/Sos/Sos*/notes LOAD \
"Phosphorylated form of  SoS. Nominally this is an inactivation step" \
"mediated by MAPK, see Profiri and McCormick 1996 JBC 271(10):5871." \
"I have not put this inactivation in this pathway so this molecule " \
"currently only represents a potential interaction point."
call /kinetics/Sos/dephosph_Sos/notes LOAD \
"The best clue I have to these rates is from the time" \
"courses of the EGF activation, which is around 1 to 5 min." \
"The dephosph would be expected to be of the same order," \
"perhaps a bit longer. Lets use 0.002 which is about 8 min." \
"Sep 17: The transient activation curve matches better with" \
"kf = 0.001"
call /kinetics/Sos/Grb2_bind_Sos/notes LOAD \
"As there are 2 SH3 domains, this reaction could be 2nd order." \
"I have a Kd of 22 uM from peptide binding (Lemmon et al " \
"JBC 269:50 pg 31653). However, Chook et al JBC 271:48 pg30472" \
"say it is 0.4uM with purified proteins, so we believe them." \
"They say it is 1:1 binding." \
"Porfiri and McCormick JBC 271 also have related data." \
"After comparing with the time-course of 1 min and the efficacy" \
"of activation of Ras, settle on Kd of 0.672 which is close" \
"to the Chook et al value."
call /kinetics/Sos/Sos/notes LOAD \
"I have tried using low (0.02 uM) initial concs, but these" \
"give a very flat response to EGF stim although the overall" \
"activation of Ras is not too bad. I am reverting to 0.1 " \
"because we expect a sharp initial response, followed by" \
"a decline." \
""
call /kinetics/PKC/notes LOAD \
"Protein Kinase C. This module represents a weighted average of" \
"the alpha, beta and gamma isoforms. It takes inputs from" \
"Ca, DAG (Diacyl Glycerol) and AA (arachidonic acid)." \
"Regulation parameters are largely from Schaechter and Benowitz" \
"1993 J Neurosci 13(10):4361 who use synaptosomes from" \
"mammalian brain and in one paper look at all three inputs." \
"Shinomura et al 1991 PNAS 88:5149-5153 is also a useful source" \
"of data and helps to tighten the DAG inputs. " \
"General reviews include Azzi et al 1992 Eur J Bioch 208:541" \
"and Nishizuka 1988, Nature 334:661" \
"Concentration info from Kikkawa et al 1982 JBC 257(22):13341" \
"The process of parameterization is described in detail" \
"in several places. See Supplementary notes to " \
"Bhalla and Iyengar 1999 Science 284:92-96, available at the site" \
"http://www.ncbs.res.in/~bhalla/ltploop/pkc_example.html" \
"The parameterization is also described in a book chapter:" \
"Bhalla, 2000: Simulations of Biochemical Signaling in" \
"Computational Neuroscience: Realistic Modeling for Experimentalists." \
"Ed. E. De Schutter. CRC Press." \
""
call /kinetics/PKC/PKC-Ca/notes LOAD \
"This intermediate is strongly indicated by the synergistic" \
"activation of PKC by combinations of DAG and Ca, as well" \
"as AA and Ca. PKC by definition also has a direct Ca-activation," \
"to which this also contributes."
call /kinetics/PKC/PKC-act-by-Ca/notes LOAD \
"This Kd is a straightforward result from the Schaechter and Benowitz" \
"1993 J Neurosci 13(10):4361 curves. The time-course is based on the" \
"known rapid activation of PKC and also the fact that Ca association" \
"with proteins is typically quite fast. My guess is that this tau of" \
"2 sec is quite conservative and the actualy rate may be much faster." \
"The parameter is quite insensitive for most stimuli." \
"" \
""
call /kinetics/PKC/PKC-act-by-DAG/notes LOAD \
"Ca.PKC interaction with DAG is modeled by this reaction." \
"Kf based on Shinomura et al PNAS 88 5149-5153 1991 and" \
"Schaechter and Benowitz 1993 J Neurosci 13(10):4361 and uses" \
"the constraining procedure referred to in the general" \
"notes for PKC."
call /kinetics/PKC/PKC-Ca-to-memb/notes LOAD \
"Membrane translocation is a standard step in PKC activation." \
"It also turns out to be necessary to replicate the curves" \
"from Schaechter and Benowitz 1993 J Neurosci 13(10):4361" \
"and Shonomura et al 1991 PNAS 88:5149-5153. These rates" \
"are constrained by matching the curves in the above papers and" \
"by fixing a rather fast (sub-second) tau for PKC activation."
call /kinetics/PKC/PKC-DAG-to-memb/notes LOAD \
"membrane translocation step for Ca.DAG.PKC complex." \
"Rates constrained from Shinomura et al 1991 PNAS 88:5149-5153" \
" and Schaechter and Benowitz 1993 J Neurosci 13(10):4361" \
"as derived in the references cited in PKC general notes."
call /kinetics/PKC/PKC-act-by-Ca-AA/notes LOAD \
"Ca-dependent AA activation of PKC." \
"Note that this step combines the AA activation and also the " \
"membrane translocation." \
"From Schaechter and Benowitz 1993 J Neurosci 13(10):4361"
call /kinetics/PKC/PKC-act-by-DAG-AA/notes LOAD \
"Membrane translocation step for PKC-DAG-AA complex." \
"Rates from matching concentration-effect data in our" \
"two main references:" \
"Schaechter and Benowitz 1993 J Neurosci 13(10):4361 and" \
"Shinomura et al 1988 PNAS 88: 5149-5153"
call /kinetics/PKC/PKC-DAG-AA*/notes LOAD \
"Membrane translocated form of PKC-DAG-AA complex."
call /kinetics/PKC/PKC-Ca-AA*/notes LOAD \
"Membrane bound and active complex of PKC, Ca and AA."
call /kinetics/PKC/PKC-Ca-memb*/notes LOAD \
"This is the direct Ca-stimulated activity of PKC."
call /kinetics/PKC/PKC-DAG-memb*/notes LOAD \
"Active, membrane attached form of Ca.DAG.PKC complex."
call /kinetics/PKC/PKC-basal*/notes LOAD \
"This is the basal PKC activity which contributes about" \
"2% to the maximum."
call /kinetics/PKC/PKC-basal-act/notes LOAD \
"Basal activity of PKC is quite high, about 10% of max." \
"See Schaechter and Benowitz 1993 J Neurosci 13(10):4361 and" \
"Shinomura et al 1991 PNAS 88:5149-5153. This is partly due to" \
"basal levels of DAG, AA and Ca, but even when these are taken" \
"into account (see the derivations as per the PKC general notes)" \
"there is a small basal activity still to be accounted for. This" \
"reaction handles it by giving a 2% activity at baseline."
call /kinetics/PKC/PKC-AA*/notes LOAD \
"This is the membrane-bound and active form of the PKC-AA complex." \
""
call /kinetics/PKC/PKC-act-by-AA/notes LOAD \
"AA stimulates PKC activity even at rather low Ca." \
"Schaechter and Benowitz 1993 J Neurosci 13(10):4361" \
"Note that this one reaction combines the initial interaction" \
"and also membrane translocation."
call /kinetics/PKC/PKC-Ca-DAG/notes LOAD \
"This is the active PKC form involving Ca and DAG." \
"It has to translocate to the membrane."
call /kinetics/PKC/PKC-n-DAG/notes LOAD \
"Binding of PKC to DAG, non-Ca dependent." \
"" \
"Kf based on Shinomura et al PNAS 88 5149-5153 1991" \
"Tau estimated as fast and here it is about the same time-course" \
"as the formation of DAG so it will not be rate-limiting."
call /kinetics/PKC/PKC-DAG/notes LOAD \
"This is a DAG-bound intermediate used in synergistic activation" \
"of PKC by DAG and AA."
call /kinetics/PKC/PKC-n-DAG-AA/notes LOAD \
"This is one of the more interesting steps. Mechanistically" \
"it does not seem necessary at first glance. Turns out that" \
"one needs this step to quantitatively match the curves" \
"in Schaechter and Benowitz 1993 J Neurosci 13(10):4361" \
"and Shinomura et al 1991 PNAS 88:5149-5153. There is" \
"a synergy between DAG and AA activation even at low" \
"Ca levels, which is most simply represented by this reaction." \
"Tau is assumed to be fast." \
"Kd comes from matching the experimental curves."
call /kinetics/PKC/PKC-DAG-AA/notes LOAD \
"Complex of PKC, DAG and AA giving rise to synergistic" \
"activation of PKC by DAG and AA at resting Ca." \
""
call /kinetics/PKC/PKC-cytosolic/notes LOAD \
"Marquez et al J. Immun 149,2560(92) est 1e6/cell for chromaffin cells" \
"" \
"Kikkawa et al 1982 JBC 257(22):13341 have PKC levels in brain at " \
"about 1 uM." \
"" \
"The cytosolic form is the inactive PKC. This is really a composite" \
"of three isoforms: alpha, beta and gamma which have slightly" \
"different properties and respond to different combinations of" \
"Ca, AA and DAG."
call /kinetics/DAG/notes LOAD \
"Baseline in model is 11.661 uM." \
"DAG is pretty nasty to estimate. In this model we just hold" \
"it fixed at this baseline level. Data sources are many and" \
"varied and sometimes difficult to reconcile. " \
"Welsh and Cabot 1987 JCB 35:231-245: DAG degradation" \
"Bocckino et al JBC 260(26):14201-14207: " \
"     hepatocytes stim with vasopressin: 190 uM." \
"Bocckino et al 1987 JBC 262(31):15309-15315:" \
"     DAG rises from 70 to 200 ng/mg wet weight, approx 150 to 450 uM." \
"Prescott and Majerus 1983 JBC 258:764-769: Platelets: 6 uM." \
"     Also see Rittenhouse-Simmons 1979 J Clin Invest 63." \
"Sano et al JBC 258(3):2010-2013: Report a nearly 10 fold rise." \
"Habenicht et al 1981 JBC 256(23)12329-12335: " \
"     3T3 cells with PDGF stim: 27 uM" \
"Cornell and Vance 1987 BBA 919:23-36: 10x rise from 10 to 100 uM." \
"" \
"Summary: I see much lower rises in my PLC models," \
"but the baseline could be anywhere from" \
"5 to 100 uM. I have chosen about 11 uM based on the stimulus -response" \
"characteristics from the Schaechter and Benowitz paper and the" \
"Shinomura et al papers." \
"" \
"" \
""
call /kinetics/Ca/notes LOAD \
"This calcium pool is treated as being buffered to a" \
"steady 0.08 uM, which is the resting level. "
call /kinetics/AA/notes LOAD \
"Arachidonic Acid. This messenger diffuses through membranes" \
"as well as cytosolically, has been suggested as a possible" \
"retrograde messenger at synapses. "
call /kinetics/PKC-active/notes LOAD \
"This is the total active PKC. It is the sum of the respective" \
"activities of " \
"PKC-basal*" \
"PKC-Ca-memb*" \
"PKC-DAG-memb*" \
"PKC-Ca-AA*" \
"PKC-DAG-AA*" \
"PKC-AA*" \
"I treat PKC here in a two-state manner: Either it is in an active" \
"state (any one of the above list) or it is inactive. No matter what " \
"combination of stimuli activate the PKC, I treat it as having the same" \
"activity. The scaling comes in through the relative amounts of PKC" \
"which bind to the respecive stimuli." \
"The justification for this is the mode of action of PKC, which like" \
"most Ser/Thr kinases has a kinase domain normally bound to and blocked" \
"by a regulatory domain. I assume that all the activators simply free" \
"up the kinase domain." \
"A more general model would incorporate a different enzyme activity for" \
"each combination of activating inputs, as well as for each substrate." \
"The current model seems to be a decent and much simpler approximation" \
"for the available data." \
"One caveat of this way of representing PKC is that the summation" \
"procedure assumes that PKC does not saturate with its substrates. " \
"If this assumption fails, then the contributing PKC complexes would" \
"experience changes in availability which would affect their " \
"balance. Given the relatively low percentage of PKC usually activated," \
"and its high throughput as an enzyme, this is a safe assumption under" \
"physiological conditions." \
""
call /kinetics/PKC-active/PKC-act-raf/notes LOAD \
"Rate consts from Chen et al Biochem 32, 1032 (1993)" \
"k3 = 4" \
"Km for this substrate is trickier. Specific substrates are in the" \
"uM range, so we use a higher Km here. This may be too conservative" \
"in which case PKC would have a still higher effect on raf." \
"The presence of this phosphorylation and activation step is from" \
"Kolch et al 1993 Nature 364:249" \
"" \
""
call /kinetics/PKC-active/PKC-inact-GAP/notes LOAD \
"Rate consts are PKC generic rates." \
"This reaction inactivates GAP. The reaction is from the " \
"Boguski and McCormick 1993 review in Nature 366:643-654" \
"The phosphorylation Vmax is 6x higher to account for" \
"balance of GDP-Ras:GDP-Ras."
call /kinetics/PKC-active/PKC-act-GEF/notes LOAD \
"Rate constants are generic PKC rates. See <a href = http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=pubmed&dopt=Abstract&list_uids=8424932>Chen SJ, Klann E, Gower MC, Powell CM, Sessoms JS, Sweatt JD. (1993) Biochemistry 32(4):1032-9</a>. This reaction activates GEF. Gives >= 2X stim of ras, and a 2X stim of MAPK over amount from direct phosph of c-raf. Note that it is a push-pull reaction, and also get effect through phosph and inact of GAPs. " \
""
call /kinetics/MAPK/notes LOAD \
"The Mitogen Activated Protein Kinase (MAPK) cascade model " \
"here includes both the MAPK cascade and" \
"its regulation by two forms of MKP. MKP-1 is induced upon MAPK" \
"activation, whereas MKP-2 is treated as a steady level of" \
"protein. The phosphatase Protein phosphatase 2 A (PP2A) " \
"is also included in this model to balance the activity of" \
"the kinases."
call /kinetics/MAPK/craf-1/notes LOAD \
"Strom et al 1990 Oncogene 5 pp 345-51 report high general expression" \
"in all tissues." \
"Huang and Ferrell 1996 PNAS 93(19):10078 use a value of 3 nM for oocytes." \
"Here we stick with a much higher expression based on the Strom report." \
""
call /kinetics/MAPK/craf-1*/notes LOAD \
"Singly phosphorylated form of c-raf-1. This is the form that gets" \
"best activated by GTP.Ras."
call /kinetics/MAPK/MAPKK/notes LOAD \
"Conc is from Seger et al JBC 267:20 pp14373 (1992)" \
"mwt is 45/46 Kd" \
"We assume that phosphorylation on both ser and thr is needed for" \
"activiation. See Kyriakis et al Nature 358 417 1992" \
"Init conc of total is 0.18" \
""
call /kinetics/MAPK/MAPK/notes LOAD \
"Mol wt is 42 KDa." \
"conc is from Sanghera et al JBC 265 pp 52 (1990)" \
"They estimate MAPK is 1e-4x total protein, and protein is 15% of cell wt," \
"so MAPK is 1.5e-5g/ml = 0.36uM." \
"Lets use this." \
"Note though that Huang and Ferrell 1996 PNAS 93(19):10078" \
"report 1.2 uM in oocytes." \
"Also note that brain concs may be high." \
"Ortiz et al 1995  J. Neurosci 15(2):1285-1297 report " \
"0.3 ng/ug protein in Cingulate Gyrus and 1.2 ng/ug protein" \
"in nucleus accumbens. In hippocampus 270 ng/mg protein for ERK1 and" \
"820 ng/mg protein for ERK 2. " \
"If 15% of cell weight is protein, that means that about 300 * 0.15 ng/ul" \
"is ERK 1.  ie, 45e-9g/1e-6 litre = 45 mg/litre or about 1 uM. " \
"For non-neuronal tissues a lower value may be better."
call /kinetics/MAPK/craf-1**/notes LOAD \
"Negative feedback by MAPK* by hyperphosphorylating craf-1* gives" \
"rise to this pool." \
"Ueki et al JBC 269(22):15756-15761, 1994" \
""
call /kinetics/MAPK/MAPK-tyr/notes LOAD \
"Haystead et al FEBS Lett. 306(1) pp 17-22 show that phosphorylation" \
"is strictly sequential, first tyr185 then thr183."
call /kinetics/MAPK/MAPKK*/notes LOAD \
"MAPKK phosphorylates MAPK on both the tyr and thr residues, first" \
"tyr then thr. Refs: Seger et al JBC267:20 pp 14373 1992" \
"The MAPKK itself is phosphorylated on ser as well as thr residues." \
"Let us assume that the ser goes first, and that the sequential phosphorylation" \
"is needed. See Kyriakis et al Nature 358 417-421 1992"
call /kinetics/MAPK/MAPKK*/MAPKKtyr/notes LOAD \
"The actual MAPKK is 2 forms from Seger et al JBC 267:20 14373(1992)" \
"Vmax = 150nmol/min/mg" \
"From Haystead et al FEBS 306(1):17-22 we get Km=46.6nM for at least one" \
"of the phosphs." \
"Putting these together:" \
"k3=0.15/sec, ratio of 4 to get k2=0.6." \
"k1=0.75/46.6nM=2.7e-5" \
"In terms of Michaelis-Menten rates, " \
"Km = 0.046, Vmax = 0.15, ratio = 4."
call /kinetics/MAPK/MAPKK*/MAPKKthr/notes LOAD \
"Rate consts same as for MAPKKtyr."
call /kinetics/MAPK/MAPKK-ser/notes LOAD \
"Intermediately phophorylated, assumed inactive, form of MAPKK"
call /kinetics/MAPK/RGR/notes LOAD \
"Shorthand name for Raf.GTP.Ras. This refers to the complex between" \
"GTP.Ras and the unphosphorylated Raf. I treat this as having the " \
"same enzyme activity as the Raf*.GTP.Ras form."
call /kinetics/MAPK/RGR/RGR.1/notes LOAD \
"Kinetics are the same as for the craf-1* activity, ie.," \
"k1=5.5e-6, k2=.42, k3 =0.105" \
"These are based on Force et al PNAS USA 91 1270-1274 1994." \
""
call /kinetics/MAPK/RGR/RGR.2/notes LOAD \
"Same kinetics as other c-raf activated forms. See " \
"Force et al PNAS 91 1270-1274 1994." \
"k1 = 5.5e-6, k2 = .42, k3 = 0.105" \
""
call /kinetics/MAPK/Ubiquitination/notes LOAD \
"This is a representation of generic ubiquitination followed" \
"by degradation. Since this pool is buffered to zero, anything" \
"that ends up here is removed from the model."
call /kinetics/MAPK/turnover_MKP1/notes LOAD \
"Rate of turnover of non-phosph form of MKP1 " \
"is from Brondello et al Science 286:2514 1999. Tau is " \
"about 45 min. "
call /kinetics/MAPK/MKP-1-ser359*/notes LOAD \
"Singly phosphorylated form of MKP-1. Slower" \
"ubiquitination. See Brondello et al 1999 Science 286:2514" \
"From the simulations, this form has a rather low and transient" \
"level. So I do not include it in the dephosphorylation of" \
"MAPK."
call /kinetics/MAPK/MKP-1**/notes LOAD \
" Same enzyme activity as MKP-1, but different" \
"degradation rates. Brondello et al Science 286:2514 1999" \
"This form is doubly phosphorylated." \
""
call /kinetics/MAPK/MKP-1**/MKP1*-tyr-deph/notes LOAD \
"3 Feb 2000. Same rates as MKP-1."
call /kinetics/MAPK/MKP-1**/MKP1*-thr-deph/notes LOAD \
"3 Feb 2000. Same rates as MKP1"
call /kinetics/MAPK/turnover_MKP1**/notes LOAD \
" Rate of turnover of phosph form of MKP1 " \
"is from Brondello et al Science 286:2514 1999. Tau is " \
"about 150 min though it is not a clean exponential falloff. "
call /kinetics/MAPK/turnover_MKP1*/notes LOAD \
"Rate of turnover of phosph form of MKP1 " \
"is from Brondello et al Science 286:2514 1999. Tau is " \
"about 150 min though it is not a clean exponential falloff. "
call /kinetics/MAPK/MKP-1*dephosph/notes LOAD \
"24 Apr 2K: Based on 12 Feb 2K:" \
" Scaled up 10x so kf=0.01, kb=0" \
"13 Apr 2001: MKP1* Goes down too fast, equil amount too small." \
"Note that tau for turnover is 150 min. Doesn't make sense " \
"to have this tau be so much less. Go for 1e-4"
call /kinetics/MAPK/MKP-1**dephosph/notes LOAD \
"24 Apr 2K, based on 12 Feb 2K: Scaled up 10x to kf=0.01, kb=0" \
"13 Apr 2001. Slowing down to 0.0001 to match turnover" \
"rates."
call /kinetics/MAPK/translocation/notes LOAD \
"A nuclear translocation step. This lumps in all sorts of " \
"processes into a single set of rates constrained by time courses." \
"Furuno et al J Immunol 166:4416-4421 (2001):" \
"In within 6 min, out within 7. The outgoing path is dephosphorylated" \
"MAPK so this reac will be one-way." \
" Kf=0.01, Kb=0.005. The reaction is 2nd order in MAPK*," \
"to represent dimerization of transcription factors."
call /kinetics/MAPK/nuc_MAPK*/notes LOAD \
"" \
"Cytoplasmic MAPK declines 20%, nuclear MAPK rises 100%." \
"Furuno et al J Immunol 166:4416 (2001)" \
"Assume nuc vol 1/5 that of cytoplasm" \
"so nuc vol = 2e-16" \
"Note therefore that the concentrations here will be higher" \
"for the same number of molecules." \
""
call /kinetics/MAPK/nuc_MAPK*/act_transcription/notes LOAD \
"" \
"This is a 'black box' representation of a lot of steps." \
" Constraint provided by" \
"determining rate of formation of MKP-1."
call /kinetics/MAPK/MKP1-RNA/notes LOAD \
"The MKP-1 RNA is treated as the rate-limiting quantity" \
"for synthesis of MKP-1. "
call /kinetics/MAPK/MKP1_synth/notes LOAD \
"This is a 'black box' approximation to the process of" \
"translation. I assume that there is unity stoichiometry, that is," \
"a single RNA gives a single protein. This is obviously" \
"wrong, but since the RNA level itself is just a ratio" \
"against baseline, the conversion factor is absorbed there."
call /kinetics/MAPK/Nucleotides/notes LOAD \
"Infinite supply assumed for formation of mRNA."
call /kinetics/MAPK/Basal_transcription/notes LOAD \
"This is just a basal rate of MKP-1 transcription" \
"scaled to give reasonable basal levels of the MKP-1."
call /kinetics/MAPK/Raf*-GTP-Ras/notes LOAD \
"This is the main activated form of craf. It requires binding to ras for" \
"activation, but the presence of the phosphorylation increases this" \
"binding. See Leevers 1994 Nature 369:411-414 and" \
"Hallberg et al 1994 JBC 269(6):3913-3916"
call /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.1/notes LOAD \
"Kinetics are the same as for the craf-1* activity, ie.," \
"k1=1.1e-6, k2=.42, k3 =0.105" \
"These are based on Force et al PNAS USA 91 1270-1274 1994." \
"They report Km for MAPKK of 0.8 uM. and a Vmax of ~500 fm/min/ug." \
"These parms cannot reach the observed 4X stimulation of MAPK." \
"So we increase the affinity, ie, raise k1 5x to 5.5e-6" \
"which is equivalent to 5-fold reduction in Km to about 0.16." \
"This is, of course, dependent on the amount of MAPKK present." \
"" \
""
call /kinetics/MAPK/Raf*-GTP-Ras/Raf*-GTP-Ras.2/notes LOAD \
"Same kinetics as other c-raf activated forms. See " \
"Force et al PNAS 91 1270-1274 1994." \
""
call /kinetics/MAPK*/notes LOAD \
"This molecule is phosphorylated on both the tyr and thr residues and" \
"is active: Seger et al 1992 JBC 267(20):14373" \
"The rate consts are from two sources: Combine Sanghera et al" \
"JBC 265(1) :52-57 with Nemenoff et al JBC 93 pp 1960 to get" \
" k3 = 10, k2 = 40, k1 = 3.25e-6"
call /kinetics/MAPK*/MAPK*-feedback/notes LOAD \
"Ueki et al JBC 269(22):15756-15761 show the presence of" \
"this step, but not the rate consts, which are derived from" \
"Sanghera et al  JBC 265(1):52-57, 1990, see the deriv in the" \
"MAPK* notes."
call /kinetics/MAPK*/MAPK*/notes LOAD \
"Km = 25uM @ 50 uM ATP and 1mg/ml MBP (huge XS of substrate)" \
"Vmax = 4124 pmol/min/ml at a conc of 125 pmol/ml of enz." \
"Numbers are from Sanghera et al JBC 265 pp 52 , 1990. " \
"From Nemenoff et al 1993 JBC 268(3):1960-1964 - using Sanghera's 1e-4 ratio" \
"of MAPK to protein, we get k3 = 7/sec from 1000 pmol/min/mg total " \
"protein in fig 5" \
"I take the Vmax to be higher for PLA2 given the fold activation of PLA2" \
"by MAPK. This is actually a balance term between MAPK and the dephosphorylation" \
"step." \
""
call /kinetics/MAPK*/MKP-1-phosph/notes LOAD \
"3 Feb 2000. See Brondello et al Science 286:2514 1999." \
"Rates assumed standard MAPK rates based on Sanghera et" \
"al JBC 265(1):53-57 1990." \
"The Vmax is scaled down 10 fold to match the time-course" \
"of phosph by MAPK, from the Brondello paper."
call /kinetics/MAPK*/MKP-1-phosph2/notes LOAD \
"3 Feb 2000. See Brondello et al Science 286:2514 1999." \
"Rates assumed standard MAPK rates based on Sanghera et" \
"al JBC 265(1):53-57 1990." \
"The Vmax is scaled down by 10 fold to match the time-courses" \
"from the Brondello experiment. " \
""
call /kinetics/MKP-1/notes LOAD \
"MKP-1 dephosphorylates and inactivates MAPK in vivo: Sun et al Cell 75 " \
"487-493 1993. " \
"See Charles et al PNAS 90:5292-5296 1993 and" \
"Charles et al Oncogene 7 187-190 for half-life of" \
"MKP1/3CH is 40 min. 80% deph of MAPK in 20 min" \
"The protein is 40 KDa." \
"" \
"22 Apr 2001: CoInit =0.4nM but this is really an emergent property" \
"of the rate of induction of the phosphatase at steady-state in balance" \
"with the degradation rate."
call /kinetics/MKP-1/MKP1-tyr-deph/notes LOAD \
"The original kinetics from Bhalla and Iyengar Science 1999" \
"have now been modified to obey the k2 = 4 * k3 rule," \
"while keeping kcat and Km fixed. The main constraining" \
"data point is the time course of MAPK dephosphorylation, which this" \
"model satisfies." \
"See Charles et al 1993 PNAS 90:5292-5296 and" \
"Charles et al Oncogene 7:187-190" \
"" \
"Effective Km : 67 nM" \
"kcat = 1.43 umol/min/mg"
call /kinetics/MKP-1/MKP1-thr-deph/notes LOAD \
"See MKP1-tyr-deph"
call /kinetics/Ras-act-craf/notes LOAD \
"Assume binding is fast and limited only by available" \
"Ras*. So kf = kb/[craf-1] " \
"If kb is 1/sec, then kf = 1/0.2 uM = 1/(0.2 * 6e5) = 8.3e-6" \
"Later: Raise it by 10 X to about 1e-4, giving a Kf of 60 for Kb of 0.5" \
"and a tau of approx 2 sec." \
"Based on:" \
"Hallberg et al JBC 269:6 3913-3916 1994, 3% of cellular Raf is" \
"complexed with Ras." \
"This step needed to memb-anchor and activate Raf:" \
"Leevers et al Nature 369 411-414." \
"Also see Koide et al 1993 PNAS USA 90(18):8683-8686"
call /kinetics/PPhosphatase2A/notes LOAD \
"Refs: Pato et al Biochem J 293:35-41(93);" \
"CoInit values span a range depending on source." \
"Pato et al 1993 Biochem J 293:35-41 and" \
"Cohen et al 1988 Meth Enz 159:390-408 estimate 80 nM from muscle" \
"" \
"Zolneierowicz et al 1994 Biochem 33:11858-11867 report" \
"levels of 0.4 uM again from muscle, but expression" \
"is also strong in brain." \
"Our estimate of 0.224 is between these two." \
"" \
"There are many substrates for PP2A in this model, so I put" \
"the enzyme rate calculations here:" \
"Takai&Mieskes Biochem J 275:233-239 have mol wt 36 KDa. They" \
"report Vmax of 119 umol/min/mg i.e. 125/sec for k3 for pNPP substrate," \
"Km of 16 mM. This is obviously unreasonable for protein substrates." \
"For chicken gizzard myosin light chan, we have Vmax = 13 umol/min/mg" \
"or about k3 = 14/sec." \
"" \
"Pato et al 1993 Biochem J 293:35-41 report" \
"caldesmon: Km = 2.2 uM, Vmax = 0.24 umol/min/mg. They do not think " \
"caldesmon is a good substrate. " \
"Calponin: Km = 14.3, Vmax = 5." \
"Our values approximate these." \
"" \
""
call /kinetics/PPhosphatase2A/craf-deph/notes LOAD \
"See parent PPhosphatase2A for parms" \
""
call /kinetics/PPhosphatase2A/MAPKK-deph/notes LOAD \
"See: Kyriakis et al Nature 358 pp 417-421 1992" \
"Ahn et al Curr Op Cell Biol 4:992-999 1992 for this pathway." \
"See parent PPhosphatase2A for parms."
call /kinetics/PPhosphatase2A/MAPKK-deph-ser/notes LOAD \
"See parent PPhostphatase2A description for rate details"
call /kinetics/PPhosphatase2A/craf**-deph/notes LOAD \
"Ueki et al JBC 269(22) pp 15756-15761 1994 show hyperphosphorylation of" \
"craf, so this is there to dephosphorylate it. Identity of phosphatase is" \
"assumed to be PP2A."
call /kinetics/PLA2/notes LOAD \
"Main source of data: Leslie and Channon BBA 1045 (1990) pp 261-270." \
"Fig 6 is Ca curve. Fig 4a is PIP2 curve. Fig 4b is DAG curve. Also see" \
"Wijkander and Sundler JBC 202 (1991) pp873-880;" \
"Diez and Mong JBC 265(24) p14654;" \
"Leslie JBC 266(17) (1991) pp11366-11371" \
"Many inputs activate PLA2. In this model I simply take" \
"each combination of stimuli as binding to PLA2 to give a" \
"unique enzymatic activity. The Km and Vmax of these" \
"active complexes is scaled according to the" \
"relative activation reported in the papers above."
call /kinetics/PLA2/PLA2-cytosolic/notes LOAD \
"cPLA2 IV form has mol wt of 85 Kd." \
"Glaser et al 1993 TIPS 14:92-98." \
"" \
"Calculated cytosolic concentration is ~300 nM from Wijkander and Sundler" \
"1991 Eur J Biochem 202:873" \
"Leslie and Channon 1990 BBA 1045:261 use about 400 nM. " \
"Decent match. Use 400 nM." \
""
call /kinetics/PLA2/PLA2-Ca-act/notes LOAD \
"Direct activation of PLA2 by Ca." \
"From Leslie and Channon BBA 1045 (1990) 261-270 fig6 pp267."
call /kinetics/PLA2/PLA2-Ca*/notes LOAD \
"The generic Ca-activated form ofPLA2." \
"Leslie and Channon 1990 BBA 1045:261."
call /kinetics/PLA2/PLA2-Ca*/kenz/notes LOAD \
"Based on Leslie and Channon 1990 BBA 1045:261, in relation to the" \
"other PLA2 inputs (not including MAPK). Ca alone is rather a " \
"weak input."
call /kinetics/PLA2/PIP2-PLA2-act/notes LOAD \
"Activation of PLA2 by PIP2. From" \
"Leslie and Channon 1990 BBA 1045:261 the stimulation of PLA2" \
"activity by high PIP2 is 7x." \
"In this model we don't really expect any PIP2 stimulus." \
""
call /kinetics/PLA2/PIP2-PLA2*/kenz/notes LOAD \
"Based on Leslie and Channon 1990 BBA 1045:261."
call /kinetics/PLA2/PIP2-Ca-PLA2-act/notes LOAD \
"Synergistic activation of PLA2 by Ca and PIP2. Again from " \
"Leslie and Channon 1990 BBA 1045:261"
call /kinetics/PLA2/PIP2-Ca-PLA2*/kenz/notes LOAD \
"Based on AA generation by different stimuli according to" \
"Leslie and Channon 1990 BBA 1045:261"
call /kinetics/PLA2/DAG-Ca-PLA2-act/notes LOAD \
"Synergistic activation of PLA2 by Ca and DAG. " \
"Based on Leslie and Channon 1990 BBA 1045:261" \
"The Kd is rather large and may reflect the complications" \
"in measuring DAG. For this model it is not critical " \
"since DAG is held fixed."
call /kinetics/PLA2/DAG-Ca-PLA2*/kenz/notes LOAD \
"Based on Leslie and Channon 1990 BBA 1045:261."
call /kinetics/PLA2/APC/notes LOAD \
"arachodonylphosphatidylcholine is the favoured substrate" \
"from Wijkander and Sundler, JBC 202 pp 873-880, 1991." \
"Their assay used 30 uM substrate, which is what the kinetics in" \
"this model are based on. For the later model we should locate" \
"a more realistic value for APC. For now it is treated as" \
"a buffered metabolite."
call /kinetics/PLA2/Degrade-AA/notes LOAD \
"Degradation pathway for AA." \
"APC is a convenient buffered pool to dump it back into, though the" \
"actual metabolism is probably far more complex." \
"For the purposes of the full model we use a rate of degradation of" \
"0.4/sec to give a dynamic range of AA comparable to what is seen" \
"experimentally." \
"Wijkander and Sundler 1991 Eur J Biochem 202:873" \
"Leslie and Channon 1990 BBA 1045:261"
call /kinetics/PLA2/PLA2*-Ca/notes LOAD \
"Phosphorylated and active form of PLA2. Several kinases act on it:" \
"PKA: Wightman et al JBC 257 pp6650 1982" \
"PKC: Many refs, eg Gronich et al JBC 263 pp 16645, 1988 but see Lin etal" \
"MAPK: Lin et al, Cell 72 pp 269, 1993.  Show 3x with MAPK but not PKC alone" \
"The Nemenoff assays are conducted in rather high Ca so I have" \
"assumed a Ca binding step."
call /kinetics/PLA2/PLA2*-Ca/kenz/notes LOAD \
"This form should be 3 to 6 times as fast as the Ca-only form, from" \
"Lin et al 1993 Cell 269-278" \
"Nemenoff et al 1993 JBC 268:1960" \
"Several forms contribute to the Ca-stimulated form, so this rate has" \
"to be a factor larger than their total contribution. " \
"I assign Vmax as the scale factor here because there is lots of APC" \
"substrate, so all the PLA2 complex enzymes are limited primarily by Vmax."
call /kinetics/PLA2/PLA2*/notes LOAD \
"Phosphorylated PLA2. The site differs from the site" \
"phosphorylated by PKC. See" \
"Nemenoff et al 1993 JBC 268(3):1960-1964"
call /kinetics/PLA2/PLA2*-Ca-act/notes LOAD \
"Nemenoff et al 1993 JBC 268:1960 report a 2X to 4x activation of PLA2" \
"by MAPK, which seems dependent on Ca as well. This reaction " \
"represents this activation. Rates are scaled to give appropriate" \
"fold activation."
call /kinetics/PLA2/dephosphorylate-PLA2*/notes LOAD \
"Dephosphorylation reaction to balance MAPK phosphorylation of PLA2." \
"This is probably mediated by PP2A. " \
"Rates determined to keep the balance of phosphorylated and" \
"non-phosphorylated PLA2 reasonable. The constraining factor" \
"is the fold activation of PLA2 by MAPK."
call /kinetics/temp-PIP2/notes LOAD \
"This is a steady PIP2 input to PLA2. The sensitivity" \
"of PLA2 to PIP2 discussed below" \
"does not match with the reported free levels which are" \
"used by the phosphlipase Cs. My understanding is that" \
"there may be different pools of PIP2 available for stimulating" \
"PLA2 as opposed to being substrates for PLCs. For that reason" \
"I have given this PIP2 pool a separate identity. As it is" \
"a steady input this is not a problem in this model." \
"" \
"Majerus et al Cell 37:701-703 report a brain concentration of" \
"0.1 - 0.2 mole %" \
"Majerus et al Science 234:1519-1526 report a huge range of " \
"concentrations: from 1 to 10% of PI content, which is in turn" \
"2-8% of cell lipid. This gives 2e-4 to 8e-3 of cell lipid." \
"In concentrations in total volume of cell (a somewhat strange" \
"number given the compartmental considerations) this comes to" \
"anywhere from 4 uM to 200 uM." \
"" \
"PLA2 is stim 7x by PIP2 (Leslie and Channon BBA 1045:261(1990) " \
"Leslie and Channon say PIP2 is present at 0.1 - 0.2mol% range in membs," \
"so I'll use a value at the lower end of the scale for basal PIP2."
call /kinetics/Ras/notes LOAD \
" The main refs for Ras are" \
"Boguski and McCormick Nature 366 643-654 '93 Major review" \
"Eccleston et al JBC 268:36 pp 27012-19" \
"Orita et al JBC 268:34 25542-25546"
call /kinetics/Ras/dephosph-GEF/notes LOAD \
"This rate is based on the known ratio of GDP-Ras to GTP-Ras." \
"Basal: Ras.GTP = 7%" \
"Stimulated 15%" \
"Time course is within 10 min, probably much faster as not" \
"all early data points are there." \
"See Gibbs et al JBC 265(33):20437-20422"
call /kinetics/Ras/inact-GEF/notes LOAD \
"This is the amount of inactive GEF available to the system." \
"The value is the same as the estimated amount of SoS, though" \
"I treat it here as a different pool. Probably several molecules" \
"can function as GEFs and this is a simplification." \
"Orita et al JBC 268(34):25542-25546" \
"Gulbins et al 1994 Mol Cell Biol 14(2):906-913" \
""
call /kinetics/Ras/GEF*/notes LOAD \
"Phosphorylated and thereby activated form of GEF. See, e.g." \
"Orita et al JBC 268:34 25542-25546 1993, Gulbins et al." \
"It is not clear whether there is major specificity for tyr or ser/thr."
call /kinetics/Ras/GEF*/GEF*-act-ras/notes LOAD \
"Kinetics from Orita et al JBC 268(34):25542-25546." \
"Note that the Vmax is slow, but it does match" \
"the slow GTP hydrolysis rates." \
""
call /kinetics/Ras/GTP-Ras/notes LOAD \
"Only a very small fraction (7% unstim, 15% stim) of ras is GTP-bound." \
"Gibbs et al JBC 265(33) 20437" \
""
call /kinetics/Ras/GDP-Ras/notes LOAD \
"GDP bound form. See Rosen et al Neuron 12 1207-1221 June 1994." \
"the activation loop is based on Boguski and McCormick Nature 366 643-654 93" \
"Assume Ras is present at about the same level as craf-1, 0.2 uM." \
"Hallberg et al JBC 269:6 3913-3916 1994 estimate upto 5-10% of cellular" \
"Raf is assoc with Ras. Given that only 5-10% of Ras is GTP-bound, we" \
"need similar amounts of Ras as Raf."
call /kinetics/Ras/Ras-intrinsic-GTPase/notes LOAD \
"This is extremely slow (kf = 1e-4), but it is significant as so little GAP actually" \
"gets complexed with it that the total GTP turnover rises only by" \
"2-3 X (see Gibbs et al, JBC 265(33) 20437-20422) and " \
"Eccleston et al JBC 268(36) 27012-27019" \
"There is no back reaction as we assume this to be a regular" \
"irreversible Michaelis-Menten zeroth order hydrolysis." \
""
call /kinetics/Ras/dephosph-GAP/notes LOAD \
"Assume a reasonably good rate for dephosphorylating it, 0.1/sec." \
"This fits well with resting levels of active kinase and the" \
"degree of activation as well as time-course of turnoff of Ras activation," \
"but data is quite indirect."
call /kinetics/Ras/GAP*/notes LOAD \
"Phosphorylated and inactive GAP." \
"See Boguski and McCormick 1993 Nature 366:643-654 for a review."
call /kinetics/Ras/GAP/notes LOAD \
"GTPase-activating proteins. See Boguski and McCormick 1993 Nature 366:643-654" \
"Turn off Ras by helping to hydrolyze bound GTP. " \
"This one is probably NF1, ie.,  Neurofibromin as it is inhibited by AA and lipids," \
"and expressed in neural cells. p120-GAP is also a possible candidate, but" \
"is less regulated. Both may exist at similar levels." \
"See Eccleston et al JBC 268(36) pp27012-19" \
"Level=.002"
call /kinetics/Ras/GAP/GAP-inact-ras/notes LOAD \
"From Eccleston et al JBC 268(36)pp27012-19 get Kd < 2uM, kcat - 10/sec" \
"From Martin et al Cell 63 843-849 1990 get Kd ~ 250 nM, kcat = 20/min" \
"I will go with the Eccleston figures as there are good error bars (10%)." \
"The two sets of values are reasonably close." \
"k1 = 1.666e-3/sec, k2 = 1000/sec, k3 = 10/sec (note k3 is rate-limiting)" \
"This is one of the rare cases where we have direct info on the" \
"k3 being rate-limiting. Hence the ratio I use for the k2:k3 rates is" \
"100 rather than the usual 4."
call /kinetics/Ras-act-unphosph-raf/notes LOAD \
"Based on rates of Ras-act-craf which has Kf=60, Kb= 0.5." \
"This reaction was introduced to account for the PKC-independent" \
"activation of MAPK." \
"This reac should have less affinity" \
"but similar tau as compared to the Ras-cat-craf," \
" since the phosphorylated Raf form has" \
"a greater effect on MAPK." \
"" \
""
call /kinetics/PDGFR/notes LOAD \
"Platelet Derived Growth Factor Receptor pathway." \
"Possible outputs include phosphorylation of Shc (which " \
"couples ot Sos and Grb2 to activate Ras) and" \
"phosphorylation of PLC-gamma. The latter is not" \
"implemented in this specific model."
call /kinetics/PDGFR/PDGFR/notes LOAD \
"Berkers et al JBC 266 say 22K high affinity receptors per cell." \
"Sherrill and Kyte Biochemistry 35 use range 4-200 nM." \
"These values match reasonably." \
"Heidaran et al 1993 JBC 268(13):9287-9295" \
"use NIH3T3 cells and have 6.5e4 receptors/cell. This is" \
"also in the same general range. We use this last" \
"value because the cell type matches."
call /kinetics/PDGFR/act_PDGFR/notes LOAD \
"From Heidaran et al JBC268(13):9287 Fig 5." \
"Kd is ~0.5 nM"
call /kinetics/PDGFR/L.PDGFR/notes LOAD \
"This is terribly simplified: there are many interesting" \
"intermediate stages, including dimerization and assoc" \
"with adapter molecules like Shc, that contribute to the" \
"activation of the EGFR."
call /kinetics/PDGFR/L.PDGFR/phosph_Shc/notes LOAD \
"Rates from Okada et al JBC 270:35 pp 20737 1995" \
"Km = 0.70 to 0.85 uM, Vmax = 4.4 to 5.0 pmol/min. Unfortunately" \
"the amount of enzyme is not known, the prep is only" \
"partially purified." \
"Tau phosph is max within 30 sec, falls back within" \
"20 min. Ref: Sasaoka et al JBC 269:51 32621 1994." \
"Use k3 = 0.1 based on this tau." \
"27 Apr 2001: Lowered k3 to 0.05 to fix conc-effect of SHC phosph" \
"by PDGF. This gives results for downstream effects in" \
"agreement with other papers, e.g., the Brondello papers."
call /kinetics/PDGFR/L.PDGFR/phosph_PLC_g/notes LOAD \
"Hsu et al JBC 266:1 603-608 1991" \
"Km = 385 +- 100 uM, Vm = 5.1 +-1 pmol/min/ug for PLC-771." \
"Other sites have similar range, but are not stimulated as much" \
"by EGF." \
"k1 = 2.8e-2/385/6e5 = 1.2e-10. Phenomenally slow." \
"But Sherrill and Kyte say turnover # for angiotensin II is" \
"5/min for cell extt, and 2/min for placental. Also see" \
"Okada et al for Shc rates which are much faster." \
"In this model I do not use PLC-gamma as preliminary" \
"runs suggested that it has a relatively small effect" \
"on PKC and the MAPK pathway as compared to the direct" \
"effect through Ras."
call /kinetics/PDGFR/PDGF/notes LOAD \
"Platelet-derived growth factor. Heterodimer Mol wt. is approx 30 KDa" \
"Deuel et al 1985 Cancer Surv. 4(4):633-53" \
"Conc of 50 ng/ml is close to saturating, and is used by P. Ram (personal" \
"communication). Other refs use 65 ng/ml" \
"Weise RJ et al 1995 JBC 270(7):3442-3446" \
"A stimulus of 5 min is commonly used." \
"Conversion factor: " \
"1ng/ml = (1e-9/30K)* 1000 Moles/litre = 3e-11M = 3e-5 uM" \
"So 50 ng/ml ~ 1.5 nM."
call /kinetics/PDGFR/SHC/notes LOAD \
"There are 2 isoforms: 52 KDa and 46 KDa (See Okada et al" \
"JBC 270:35 pp 20737 1995). They are acted up on by the EGFR" \
"in very similar ways, and apparently both bind Grb2 similarly," \
"so we'll bundle them together here." \
"Sasaoka et al JBC 269:51 pp 32621 1994 show immunoprecs where" \
"it looks like there is at least as much Shc as Grb2. So" \
"we'll tentatively say there is 1 uM of Shc."
call /kinetics/PDGFR/SHC*/notes LOAD \
"Phosphorylated form of SHC. Binds to the SoS.Grb2 " \
"complex to give the activated GEF form upstream" \
"of Ras."
call /kinetics/PDGFR/dephosph_Shc/notes LOAD \
"Time course of decline of phosph is 20 min from Sasaoka" \
"et al 1994 JBC 269(51):32621. Part of this is" \
"the turnoff time of the EGFR itself. Lets assume a tau of" \
"10 min for this dephosphorylation as a first pass." \
"27 Apr 2001: Dephosph too slow, shifts SHC balance over to" \
"phosphorylated form. Increase Kf to 0.01. This gives a reasonable" \
"overall time-course."
call /kinetics/PDGFR/Internal_L.PDGFR/notes LOAD \
"The internalized PDGFR is treated as a generic pool in equilibrium with" \
"the surface receptor. This simplifies the turnover processes but" \
"fits reasonably well with data."
call /kinetics/PDGFR/Internalize/notes LOAD \
"Original model derived from EGFR model." \
"See Helin and Beguinot JBC 266:13 1991 pg 8363-8368." \
"In Fig 3 they have internalization tau about 10 min, " \
"equil at about 20% EGF available. So kf = 4x kb, and" \
"1/(kf + kb) = 600 sec so kb = 1/3K = 3.3e-4," \
"and kf = 1.33e-3. This doesn't take into account the" \
"unbound receptor, so we need to push the kf up a bit, to" \
"0.002" \
"26 apr 2001: Keq too low for the PDGF model." \
"Now Kf=0.001,Kb=0.00066" \
"The previously calculated internalization equilibrium" \
"led to very high internalization which shifted the effective" \
"dependence of the receptor on PDGF so it looked like the" \
"receptor binding was higher affinity than experimentally" \
"determined. Used two constraining factors: " \
"1. Time course of " \
"SHC phosphorylation/dephosphorylation which is fast on, but" \
"10-20 minutes off." \
"2. Conc dependence of MAPK on PDGF has a halfmax around 3ng/ml." \
"See Brondello et al 1997 JBC 272(2):1368-1376 and " \
"Brondello et al 1999 Science 286:2514-1517." \
"" \
"" \
""
call /kinetics/tot_MKP1/notes LOAD \
"Total available active MKP-1. This sums the levels" \
"of the non-phosphorylated, singly phosphorylated and" \
"doubly phosphorylated forms. It is used primarily" \
"for graphing the total MKP-1 level."
call /kinetics/MKP-2/notes LOAD \
"MKP2 is modeled to act as a relatively steady," \
"unregulated phosphatase for controlling MAPK activity." \
"From Brondello et al JBC 272(2):1368-1376 (1997), the" \
"blockage of MKP-1 induction increases MAPK activity by" \
"no more than 2x. So this phosphatase will play the steady" \
"role and the fully stimulated MKP-1 can come up to the" \
"level of this steady level." \
"From Chu et al 1995 JBC 271(11):6497-6501 it looks like" \
"both MKP-1 and MKP-2 have similar activities in dephosphorylating" \
"ERK2. So I use the same enzymatic rates for both."
call /kinetics/MKP-2/MKP2-tyr-deph/notes LOAD \
"22 Apr 2001: Based on MKP1 parms." \
"The original kinetics have been modified to obey the k2 = 4 * k3 rule," \
"while keeping kcat and Km fixed. The only constraining" \
"data point is the time course of MAPK dephosphorylation, which this" \
"model satisfies." \
"" \
"The rates are treated as the same as for MKP-1, based on" \
"Chu et al 1995 JBC 271(11):6497-6501"
call /kinetics/MKP-2/MKP2-thr-deph/notes LOAD \
"See MKP2-tyr-deph"
call /kinetics/tot_MAPK/notes LOAD \
"Total available active MAPK. This sums the levels" \
"of the cytosolic and nuclear localized forms."
call /kinetics/doqcsinfo/notes LOAD \
"This is a network involving the MAPK-PKC feedback loop with input from the PDGFR in the synapse. The distinctive feature of this model is that it includes MKP-1 induction by MAPK, and the consequent inhibitory regulation of MAPK and the feedback loop. Lots of interesting dynamics arise from this. This <a href=http://www.ncbs.res.in/~bhalla/ltploop/mkpfeedback/index.html>link</a> provides supplementary material for the paper <a href=http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=pubmed&dopt=Abstract&list_uids=12169734>Bhalla US et al. Science (2002) 297(5583):1018-23</a>. In the form of several example simulations and demos for the figures in the paper."
complete_loading
