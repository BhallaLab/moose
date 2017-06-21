//genesis
// kkit Version 9 flat dumpfile
 
// Saved on Mon Dec 22 14:54:02 2003
 
include kkit {argv 1}
 
FASTDT = 0.001
SIMDT = 0.005
CONTROLDT = 10
PLOTDT = 10
MAXTIME = 2000
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 1
DEFAULT_VOL = 1e-15
VERSION = 9.0
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
simobjdump kpool CoTotal CoInit Co n nInit nTotal nMin vol slave_enable notes \
  xtree_fg_req xtree_textfg_req x y z
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
simobjdump linkinfo xtree_fg_req xtree_textfg_req uplink downlink x y z
simobjdump uplink xtree_fg_req xtree_textfg_req x y z
simobjdump downlink xtree_fg_req xtree_textfg_req x y z
simobjdump mirror notes xtree_fg_req x y z
simundump kpool /kinetics/PKC-active 1 0.15 0.09 0.09 54000 54000 90000 0 \
  6e+05 6 "" red black 7 22 0
simundump kenz /kinetics/PKC-active/PKC-act-raf 1 0 0 0 0 6e+05 5e-07 16 4 0 \
  0 "" red yellow "" 8 7 0
simundump kenz /kinetics/PKC-active/PKC-inact-GAP 1 0 0 0 0 1 1e-05 16 4 0 0 \
  "" red yellow "" 3 13 0
simundump kenz /kinetics/PKC-active/PKC-act-GEF 1 0 0 0 0 1 1e-05 16 4 0 0 "" \
  red yellow "" 7.2909 20.733 0
simundump kpool /kinetics/MAPK* 1 1.6667e-06 0 0 0 0 1 0 6e+05 0 "" orange \
  yellow 12 1 0
simundump kenz /kinetics/MAPK*/MAPK*-feedback 1 0 0 0 0 6e+05 3.25e-06 40 10 \
  0 0 "" red orange "" 4.934 23.653 0
simundump kenz /kinetics/MAPK*/phosph_Sos 1 0 0 0 0 6e+05 3.25e-05 40 10 0 0 \
  "" red orange "" 18 40 0
simundump kpool /kinetics/BetaGamma 1 1.6 0.0094 0.0094 5640 5640 9.6e+05 0 \
  6e+05 4 "" yellow black 18 16 0
simundump group /kinetics/MAPK 0 brown black x 0 0 "" defaultfile \
  defaultfile.g 0 0 0 14.616 11.191 0
simundump kpool /kinetics/MAPK/craf-1 0 0.2 0.2 0.2 1.2e+05 1.2e+05 1.2e+05 0 \
  6e+05 0 "" pink brown 6.326 8.1168 0
simundump kpool /kinetics/MAPK/craf-1* 0 0.2 0 0 0 0 1.2e+05 0 6e+05 0 "" \
  pink brown 9.2401 7.7115 0
simundump kpool /kinetics/MAPK/MAPKK 0 0.18 0.18 0.18 1.08e+05 1.08e+05 \
  1.08e+05 0 6e+05 0 "" pink brown 6.3315 3.9894 0
simundump kpool /kinetics/MAPK/MAPK 0 0.36 0.36 0.36 2.16e+05 2.16e+05 \
  2.16e+05 0 6e+05 0 "" pink brown 6.0656 1.0863 0
simundump kpool /kinetics/MAPK/craf-1** 1 0.2 0 0 0 0 1.2e+05 0 6e+05 0 "" \
  hotpink brown 12.464 7.9022 0
simundump kpool /kinetics/MAPK/MAPK-tyr 1 0.36 0 0 0 0 2.16e+05 0 6e+05 0 "" \
  orange brown 8.4147 0.82034 0
simundump kpool /kinetics/MAPK/MAPKK* 0 0.18 0 0 0 0 1.08e+05 0 6e+05 0 "" \
  pink brown 12.548 4.0256 0
simundump kenz /kinetics/MAPK/MAPKK*/MAPKKtyr 0 0 0 0 0 6e+05 2.7e-05 0.6 \
  0.15 0 0 "" red pink "" 8.8914 3.5531 0
simundump kenz /kinetics/MAPK/MAPKK*/MAPKKthr 1 0 0 0 0 6e+05 2.7e-05 0.6 \
  0.15 0 0 "" red pink "" 12.961 3.0363 0
simundump kpool /kinetics/MAPK/MAPKK-ser 1 0.18 0 0 0 0 1.08e+05 0 6e+05 0 "" \
  pink brown 9.2652 4.1657 0
simundump kpool /kinetics/MAPK/Raf-GTP-Ras* 1 0.0104 0 0 0 0 6240 0 6e+05 0 \
  "" red brown 4.9054 6.7814 0
simundump kenz /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.1 1 0 0 0 0 1 5.5e-06 \
  0.42 0.105 0 0 "" red red "" 7.6179 6.2189 0
simundump kenz /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.2 1 0 0 0 0 1 5.5e-06 \
  0.42 0.105 0 0 "" red red "" 10.698 6.0688 0
simundump kpool /kinetics/MKP-1 1 0.008 0.0024 0.0024 1440 1440 4800 0 6e+05 \
  0 "" hotpink black 5.0816 2.4407 0
simundump kenz /kinetics/MKP-1/MKP1-tyr-deph 1 0 0 0 0 6e+05 0.000125 4 1 0 0 \
  "" red hotpink "" 6.2781 3.0684 0
simundump kenz /kinetics/MKP-1/MKP1-thr-deph 1 0 0 0 0 6e+05 0.000125 4 1 0 0 \
  "" red hotpink "" 10.789 2.9311 0
simundump kreac /kinetics/Ras-act-craf 1 4e-05 0.5 "" white black 3.5614 \
  10.091 0
simundump kpool /kinetics/PPhosphatase2A 1 0.224 0.224 0.224 1.344e+05 \
  1.344e+05 1.344e+05 0 6e+05 0 "" hotpink yellow 9.3898 9.1309 0
simundump kenz /kinetics/PPhosphatase2A/craf-deph 1 0 0 0 0 6e+05 3.3e-06 25 \
  6 0 0 "" red hotpink "" 7.8013 10.215 0
simundump kenz /kinetics/PPhosphatase2A/MAPKK-deph 1 0 0 0 0 6e+05 3.3e-06 25 \
  6 0 0 "" red hotpink "" 13.159 6.0736 0
simundump kenz /kinetics/PPhosphatase2A/MAPKK-deph-ser 1 0 0 0 0 6e+05 \
  3.3e-06 25 6 0 0 "" red hotpink "" 4.8651 5.9208 0
simundump kenz /kinetics/PPhosphatase2A/craf**-deph 1 0 0 0 0 1 3.3e-06 25 6 \
  0 0 "" red hotpink "" 12.446 9.9054 0
simundump group /kinetics/Ras 1 blue black x 0 0 "" defaultfile defaultfile.g \
  0 0 0 14.513 16.351 0
simundump kreac /kinetics/Ras/bg-act-GEF 1 1e-05 1 "" white blue 13.468 \
  14.838 0
simundump kpool /kinetics/Ras/GEF-Gprot-bg 1 0.1 0 0 0 0 60000 0 6e+05 0 "" \
  hotpink blue 10.373 17.271 0
simundump kenz /kinetics/Ras/GEF-Gprot-bg/GEF-bg_act-ras 1 0 0 0 0 6e+05 \
  3.3e-07 0.08 0.02 0 0 "" red hotpink "" 10.402 16.523 0
simundump kreac /kinetics/Ras/dephosph-GEF 1 1 0 "" white blue 9.0702 17.881 \
  0
simundump kpool /kinetics/Ras/inact-GEF 1 0.1 0.1 0.1 60000 60000 60000 0 \
  6e+05 0 "" hotpink blue 12 20 0
simundump kpool /kinetics/Ras/GEF* 1 0.1 0 0 0 0 60000 0 6e+05 0 "" hotpink \
  blue 6.4483 17.246 0
simundump kenz /kinetics/Ras/GEF*/GEF*-act-ras 1 0 0 0 0 6e+05 3.3e-07 0.08 \
  0.02 0 0 "" red hotpink "" 7.0855 16.086 0
simundump kpool /kinetics/Ras/GTP-Ras 1 0.2 0 0 0 0 1.2e+05 0 6e+05 0 "" \
  orange blue 12.564 13.084 0
simundump kpool /kinetics/Ras/GDP-Ras 1 0.2 0.2 0.2 1.2e+05 1.2e+05 1.2e+05 0 \
  6e+05 0 "" pink blue 6.1309 14.165 0
simundump kreac /kinetics/Ras/Ras-intrinsic-GTPase 1 1e-04 0 "" white blue \
  9.0979 13.5 0
simundump kreac /kinetics/Ras/dephosph-GAP 1 0.1 0 "" white blue 4.0234 \
  15.524 0
simundump kpool /kinetics/Ras/GAP* 1 0.05 0 0 0 0 30000 0 6e+05 0 "" red blue \
  1.3498 14.349 0
simundump kpool /kinetics/Ras/GAP 1 0.002 0.002 0.002 1200 1200 1200 0 6e+05 \
  0 "" red blue 6.6549 12.338 0
simundump kenz /kinetics/Ras/GAP/GAP-inact-ras 1 0 0 0 0 6e+05 8.2476e-05 40 \
  10 0 0 "" red red "" 9.0121 12.403 0
simundump kpool /kinetics/Ras/inact-GEF* 1 1.6667e-06 0 0 0 0 1 0 6e+05 0 "" \
  hotpink blue 15 20 0
simundump kreac /kinetics/Ras/CaM-bind-GEF 1 1e-04 1 "" white blue 2.4861 \
  21.679 0
simundump kpool /kinetics/Ras/CaM-GEF 1 1.6667e-06 0 0 0 0 1 0 6e+05 0 "" \
  pink blue 5.3451 19.58 0
simundump kenz /kinetics/Ras/CaM-GEF/CaM-GEF-act-ras 1 0 0 0 0 6e+05 3.3e-07 \
  0.08 0.02 0 0 "" red pink "" 5.0223 18.657 0
simundump kreac /kinetics/Ras/dephosph-inact-GEF* 1 1 0 "" white blue 14.431 \
  21.995 0
simundump kpool /kinetics/PKA-active 1 0.015 0.015 0.015 9000 9000 9000 0 \
  6e+05 4 "" yellow black 18 18 0
simundump kenz /kinetics/PKA-active/PKA-phosph-GEF 1 0 0 0 0 6e+05 1e-05 36 9 \
  0 0 "" red yellow "" 14 18 0
simundump kpool /kinetics/CaM-Ca4 1 5 0 0 0 0 3e+06 0 6e+05 4 "" blue yellow \
  0 19 0
simundump kpool /kinetics/Shc*.Sos.Grb2 1 1.6667e-06 0 0 0 0 1 0 6e+05 0 "" \
  brown yellow 11.263 27.112 0
simundump kenz /kinetics/Shc*.Sos.Grb2/Sos.Ras_GEF 1 0 0 0 0 6e+05 3.3e-07 \
  0.08 0.02 0 0 "" red brown "" 11.266 24.47 0
simundump group /kinetics/EGFR 1 yellow black x 0 0 "" defaultfile \
  defaultfile.g 0 0 0 7.0249 39.57 0
simundump kpool /kinetics/EGFR/EGFR 1 0.16667 0.16667 0.16667 1e+05 1e+05 \
  1e+05 0 6e+05 0 "" red yellow 1.9551 39.853 0
simundump kreac /kinetics/EGFR/act_EGFR 1 7e-06 0.25 "" white yellow 4.4894 \
  38.493 0
simundump kpool /kinetics/EGFR/L.EGFR 1 1.6667e-06 0 0 0 0 1 0 6e+05 0 "" red \
  yellow 6.2195 36.599 0
simundump kenz /kinetics/EGFR/L.EGFR/phosph_Shc 1 0 0 0 0 6e+05 2e-06 0.8 0.2 \
  0 0 "" red red "" 9.0331 36.49 0
simundump kpool /kinetics/EGFR/EGF 1 1 0 0 0 0 6e+05 0 6e+05 4 "" red yellow \
  2.2719 36.309 0
simundump kpool /kinetics/EGFR/SHC 1 1 0.5 0.5 3e+05 3e+05 6e+05 0 6e+05 0 "" \
  orange yellow 8.3857 33.936 0
simundump kpool /kinetics/EGFR/SHC* 1 1.6667e-06 0 0 0 0 1 0 6e+05 0 "" \
  orange yellow 12.832 33.029 0
simundump kreac /kinetics/EGFR/dephosph_Shc 1 0.0016667 0 "" white yellow \
  9.7373 31.442 0
simundump kpool /kinetics/EGFR/Internal_L.EGFR 1 1.6667e-06 0 0 0 0 1 0 6e+05 \
  0 "" red yellow 6.3061 41.93 0
simundump kreac /kinetics/EGFR/Internalize 1 0.002 0.00033 "" white yellow \
  4.5213 39.863 0
simundump group /kinetics/Sos 1 blue black x 0 0 "" defaultfile defaultfile.g \
  0 0 0 19.547 34.811 0
simundump kreac /kinetics/Sos/Shc_bind_Sos.Grb2 1 8.333e-07 0.1 "" white blue \
  10.23 29.891 0
simundump kpool /kinetics/Sos/Sos*.Grb2 1 1.6667e-06 0 0 0 0 1 0 6e+05 0 "" \
  orange blue 12.274 41.661 0
simundump kreac /kinetics/Sos/Grb2_bind_Sos* 1 4.1667e-08 0.0168 "" white \
  blue 10.533 38.235 0
simundump kpool /kinetics/Sos/Grb2 1 1 1 1 6e+05 6e+05 6e+05 0 6e+05 0 "" \
  orange blue 14.742 35.301 0
simundump kpool /kinetics/Sos/Sos.Grb2 1 1.6667e-06 0 0 0 0 1 0 6e+05 0 "" \
  orange blue 13.988 30.097 0
simundump kpool /kinetics/Sos/Sos* 1 1.6667e-06 0 0 0 0 1 0 6e+05 0 "" red \
  blue 15.421 40.215 0
simundump kreac /kinetics/Sos/dephosph_Sos 1 0.001 0 "" white blue 13.185 \
  37.153 0
simundump kreac /kinetics/Sos/Grb2_bind_Sos 1 4.1667e-08 0.0168 "" white blue \
  16.422 33.133 0
simundump kpool /kinetics/Sos/Sos 1 0.1 0.1 0.1 60000 60000 60000 0 6e+05 0 \
  "" red blue 17.381 36.794 0
simundump xgraph /graphs/conc1 0 0 2000 0 0.01146 0
simundump xgraph /graphs/conc2 0 360.16 2000 0.015494 0.49511 0
simundump xplot /graphs/conc1/MAPK*.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" orange 0 0 1
simundump xgraph /moregraphs/conc3 0 0 2000 -1.1176e-08 1 0
simundump xgraph /moregraphs/conc4 0 0 2000 0 1 0
simundump xcoredraw /edit/draw 0 -4.5315 19.369 -1.5185 44.269
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"1 Sep 2003: Based on nonscaf_syn1c.g, which in turn is just " \
"like nonscaf_syn1b.g" \
"with all plots but MAPK* stripped out." \
"The current version provides the Ca input through a fast" \
"(10 msec) time-course reaction step so that the Ca can undergo" \
"fluctuations." \
"1 Oct 2003: Identical to nonscaf_syn1e.g, added a few plots." \
"9 Dec 2003: Identical to nonscaf_syn1f.g, added a plot for Ca" \
"22 Dec 2003: Based on nonscaf_syn1g.g, eliminated all except the" \
"core MAPK pathway and its inputs. Cleared out plots too." \
"Set basic values for PKC, PKA, CaM-Ca4 and Betagamma."
addmsg /kinetics/PKC-active/PKC-act-raf /kinetics/PKC-active REAC eA B 
addmsg /kinetics/PKC-active/PKC-inact-GAP /kinetics/PKC-active REAC eA B 
addmsg /kinetics/PKC-active/PKC-act-GEF /kinetics/PKC-active REAC eA B 
addmsg /kinetics/PKC-active /kinetics/PKC-active/PKC-act-raf ENZYME n 
addmsg /kinetics/MAPK/craf-1 /kinetics/PKC-active/PKC-act-raf SUBSTRATE n 
addmsg /kinetics/PKC-active /kinetics/PKC-active/PKC-inact-GAP ENZYME n 
addmsg /kinetics/Ras/GAP /kinetics/PKC-active/PKC-inact-GAP SUBSTRATE n 
addmsg /kinetics/PKC-active /kinetics/PKC-active/PKC-act-GEF ENZYME n 
addmsg /kinetics/Ras/inact-GEF /kinetics/PKC-active/PKC-act-GEF SUBSTRATE n 
addmsg /kinetics/MAPK*/MAPK*-feedback /kinetics/MAPK* REAC eA B 
addmsg /kinetics/MAPK/MAPKK*/MAPKKthr /kinetics/MAPK* MM_PRD pA 
addmsg /kinetics/MKP-1/MKP1-thr-deph /kinetics/MAPK* REAC sA B 
addmsg /kinetics/MAPK*/phosph_Sos /kinetics/MAPK* REAC eA B 
addmsg /kinetics/MAPK* /kinetics/MAPK*/MAPK*-feedback ENZYME n 
addmsg /kinetics/MAPK/craf-1* /kinetics/MAPK*/MAPK*-feedback SUBSTRATE n 
addmsg /kinetics/MAPK* /kinetics/MAPK*/phosph_Sos ENZYME n 
addmsg /kinetics/Sos/Sos /kinetics/MAPK*/phosph_Sos SUBSTRATE n 
addmsg /kinetics/Ras/bg-act-GEF /kinetics/BetaGamma REAC A B 
addmsg /kinetics/PKC-active/PKC-act-raf /kinetics/MAPK/craf-1 REAC sA B 
addmsg /kinetics/PPhosphatase2A/craf-deph /kinetics/MAPK/craf-1 MM_PRD pA 
addmsg /kinetics/PKC-active/PKC-act-raf /kinetics/MAPK/craf-1* MM_PRD pA 
addmsg /kinetics/MAPK*/MAPK*-feedback /kinetics/MAPK/craf-1* REAC sA B 
addmsg /kinetics/PPhosphatase2A/craf-deph /kinetics/MAPK/craf-1* REAC sA B 
addmsg /kinetics/PPhosphatase2A/craf**-deph /kinetics/MAPK/craf-1* MM_PRD pA 
addmsg /kinetics/Ras-act-craf /kinetics/MAPK/craf-1* REAC A B 
addmsg /kinetics/PPhosphatase2A/MAPKK-deph-ser /kinetics/MAPK/MAPKK MM_PRD pA 
addmsg /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.1 /kinetics/MAPK/MAPKK REAC sA B 
addmsg /kinetics/MAPK/MAPKK*/MAPKKtyr /kinetics/MAPK/MAPK REAC sA B 
addmsg /kinetics/MKP-1/MKP1-tyr-deph /kinetics/MAPK/MAPK MM_PRD pA 
addmsg /kinetics/MAPK*/MAPK*-feedback /kinetics/MAPK/craf-1** MM_PRD pA 
addmsg /kinetics/PPhosphatase2A/craf**-deph /kinetics/MAPK/craf-1** REAC sA B 
addmsg /kinetics/MAPK/MAPKK*/MAPKKtyr /kinetics/MAPK/MAPK-tyr MM_PRD pA 
addmsg /kinetics/MAPK/MAPKK*/MAPKKthr /kinetics/MAPK/MAPK-tyr REAC sA B 
addmsg /kinetics/MKP-1/MKP1-tyr-deph /kinetics/MAPK/MAPK-tyr REAC sA B 
addmsg /kinetics/MKP-1/MKP1-thr-deph /kinetics/MAPK/MAPK-tyr MM_PRD pA 
addmsg /kinetics/MAPK/MAPKK*/MAPKKtyr /kinetics/MAPK/MAPKK* REAC eA B 
addmsg /kinetics/MAPK/MAPKK*/MAPKKthr /kinetics/MAPK/MAPKK* REAC eA B 
addmsg /kinetics/PPhosphatase2A/MAPKK-deph /kinetics/MAPK/MAPKK* REAC sA B 
addmsg /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.2 /kinetics/MAPK/MAPKK* MM_PRD pA 
addmsg /kinetics/MAPK/MAPKK* /kinetics/MAPK/MAPKK*/MAPKKtyr ENZYME n 
addmsg /kinetics/MAPK/MAPK /kinetics/MAPK/MAPKK*/MAPKKtyr SUBSTRATE n 
addmsg /kinetics/MAPK/MAPKK* /kinetics/MAPK/MAPKK*/MAPKKthr ENZYME n 
addmsg /kinetics/MAPK/MAPK-tyr /kinetics/MAPK/MAPKK*/MAPKKthr SUBSTRATE n 
addmsg /kinetics/PPhosphatase2A/MAPKK-deph /kinetics/MAPK/MAPKK-ser MM_PRD pA 
addmsg /kinetics/PPhosphatase2A/MAPKK-deph-ser /kinetics/MAPK/MAPKK-ser REAC sA B 
addmsg /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.1 /kinetics/MAPK/MAPKK-ser MM_PRD pA 
addmsg /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.2 /kinetics/MAPK/MAPKK-ser REAC sA B 
addmsg /kinetics/Ras-act-craf /kinetics/MAPK/Raf-GTP-Ras* REAC B A 
addmsg /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.1 /kinetics/MAPK/Raf-GTP-Ras* REAC eA B 
addmsg /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.2 /kinetics/MAPK/Raf-GTP-Ras* REAC eA B 
addmsg /kinetics/MAPK/Raf-GTP-Ras* /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.1 ENZYME n 
addmsg /kinetics/MAPK/MAPKK /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.1 SUBSTRATE n 
addmsg /kinetics/MAPK/Raf-GTP-Ras* /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.2 ENZYME n 
addmsg /kinetics/MAPK/MAPKK-ser /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.2 SUBSTRATE n 
addmsg /kinetics/MKP-1/MKP1-tyr-deph /kinetics/MKP-1 REAC eA B 
addmsg /kinetics/MKP-1/MKP1-thr-deph /kinetics/MKP-1 REAC eA B 
addmsg /kinetics/MKP-1 /kinetics/MKP-1/MKP1-tyr-deph ENZYME n 
addmsg /kinetics/MAPK/MAPK-tyr /kinetics/MKP-1/MKP1-tyr-deph SUBSTRATE n 
addmsg /kinetics/MKP-1 /kinetics/MKP-1/MKP1-thr-deph ENZYME n 
addmsg /kinetics/MAPK* /kinetics/MKP-1/MKP1-thr-deph SUBSTRATE n 
addmsg /kinetics/MAPK/Raf-GTP-Ras* /kinetics/Ras-act-craf PRODUCT n 
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
addmsg /kinetics/BetaGamma /kinetics/Ras/bg-act-GEF SUBSTRATE n 
addmsg /kinetics/Ras/inact-GEF /kinetics/Ras/bg-act-GEF SUBSTRATE n 
addmsg /kinetics/Ras/GEF-Gprot-bg /kinetics/Ras/bg-act-GEF PRODUCT n 
addmsg /kinetics/Ras/GEF-Gprot-bg/GEF-bg_act-ras /kinetics/Ras/GEF-Gprot-bg REAC eA B 
addmsg /kinetics/Ras/bg-act-GEF /kinetics/Ras/GEF-Gprot-bg REAC B A 
addmsg /kinetics/Ras/GEF-Gprot-bg /kinetics/Ras/GEF-Gprot-bg/GEF-bg_act-ras ENZYME n 
addmsg /kinetics/Ras/GDP-Ras /kinetics/Ras/GEF-Gprot-bg/GEF-bg_act-ras SUBSTRATE n 
addmsg /kinetics/Ras/GEF* /kinetics/Ras/dephosph-GEF SUBSTRATE n 
addmsg /kinetics/Ras/inact-GEF /kinetics/Ras/dephosph-GEF PRODUCT n 
addmsg /kinetics/Ras/bg-act-GEF /kinetics/Ras/inact-GEF REAC A B 
addmsg /kinetics/PKC-active/PKC-act-GEF /kinetics/Ras/inact-GEF REAC sA B 
addmsg /kinetics/Ras/dephosph-GEF /kinetics/Ras/inact-GEF REAC B A 
addmsg /kinetics/PKA-active/PKA-phosph-GEF /kinetics/Ras/inact-GEF REAC sA B 
addmsg /kinetics/Ras/CaM-bind-GEF /kinetics/Ras/inact-GEF REAC A B 
addmsg /kinetics/Ras/dephosph-inact-GEF* /kinetics/Ras/inact-GEF REAC B A 
addmsg /kinetics/PKC-active/PKC-act-GEF /kinetics/Ras/GEF* MM_PRD pA 
addmsg /kinetics/Ras/dephosph-GEF /kinetics/Ras/GEF* REAC A B 
addmsg /kinetics/Ras/GEF*/GEF*-act-ras /kinetics/Ras/GEF* REAC eA B 
addmsg /kinetics/Ras/GEF* /kinetics/Ras/GEF*/GEF*-act-ras ENZYME n 
addmsg /kinetics/Ras/GDP-Ras /kinetics/Ras/GEF*/GEF*-act-ras SUBSTRATE n 
addmsg /kinetics/Ras/GEF-Gprot-bg/GEF-bg_act-ras /kinetics/Ras/GTP-Ras MM_PRD pA 
addmsg /kinetics/Ras/GAP/GAP-inact-ras /kinetics/Ras/GTP-Ras REAC sA B 
addmsg /kinetics/Ras/Ras-intrinsic-GTPase /kinetics/Ras/GTP-Ras REAC A B 
addmsg /kinetics/Ras/GEF*/GEF*-act-ras /kinetics/Ras/GTP-Ras MM_PRD pA 
addmsg /kinetics/Ras/CaM-GEF/CaM-GEF-act-ras /kinetics/Ras/GTP-Ras MM_PRD pA 
addmsg /kinetics/Ras-act-craf /kinetics/Ras/GTP-Ras REAC A B 
addmsg /kinetics/Shc*.Sos.Grb2/Sos.Ras_GEF /kinetics/Ras/GTP-Ras MM_PRD pA 
addmsg /kinetics/Ras/GEF-Gprot-bg/GEF-bg_act-ras /kinetics/Ras/GDP-Ras REAC sA B 
addmsg /kinetics/Ras/GAP/GAP-inact-ras /kinetics/Ras/GDP-Ras MM_PRD pA 
addmsg /kinetics/Ras/Ras-intrinsic-GTPase /kinetics/Ras/GDP-Ras REAC B A 
addmsg /kinetics/Ras/GEF*/GEF*-act-ras /kinetics/Ras/GDP-Ras REAC sA B 
addmsg /kinetics/Ras/CaM-GEF/CaM-GEF-act-ras /kinetics/Ras/GDP-Ras REAC sA B 
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
addmsg /kinetics/PKA-active/PKA-phosph-GEF /kinetics/Ras/inact-GEF* MM_PRD pA 
addmsg /kinetics/Ras/dephosph-inact-GEF* /kinetics/Ras/inact-GEF* REAC A B 
addmsg /kinetics/Ras/inact-GEF /kinetics/Ras/CaM-bind-GEF SUBSTRATE n 
addmsg /kinetics/Ras/CaM-GEF /kinetics/Ras/CaM-bind-GEF PRODUCT n 
addmsg /kinetics/CaM-Ca4 /kinetics/Ras/CaM-bind-GEF SUBSTRATE n 
addmsg /kinetics/Ras/CaM-bind-GEF /kinetics/Ras/CaM-GEF REAC B A 
addmsg /kinetics/Ras/CaM-GEF/CaM-GEF-act-ras /kinetics/Ras/CaM-GEF REAC eA B 
addmsg /kinetics/Ras/CaM-GEF /kinetics/Ras/CaM-GEF/CaM-GEF-act-ras ENZYME n 
addmsg /kinetics/Ras/GDP-Ras /kinetics/Ras/CaM-GEF/CaM-GEF-act-ras SUBSTRATE n 
addmsg /kinetics/Ras/inact-GEF* /kinetics/Ras/dephosph-inact-GEF* SUBSTRATE n 
addmsg /kinetics/Ras/inact-GEF /kinetics/Ras/dephosph-inact-GEF* PRODUCT n 
addmsg /kinetics/PKA-active/PKA-phosph-GEF /kinetics/PKA-active REAC eA B 
addmsg /kinetics/PKA-active /kinetics/PKA-active/PKA-phosph-GEF ENZYME n 
addmsg /kinetics/Ras/inact-GEF /kinetics/PKA-active/PKA-phosph-GEF SUBSTRATE n 
addmsg /kinetics/Ras/CaM-bind-GEF /kinetics/CaM-Ca4 REAC A B 
addmsg /kinetics/Sos/Shc_bind_Sos.Grb2 /kinetics/Shc*.Sos.Grb2 REAC B A 
addmsg /kinetics/Shc*.Sos.Grb2/Sos.Ras_GEF /kinetics/Shc*.Sos.Grb2 REAC eA B 
addmsg /kinetics/Shc*.Sos.Grb2 /kinetics/Shc*.Sos.Grb2/Sos.Ras_GEF ENZYME n 
addmsg /kinetics/Ras/GDP-Ras /kinetics/Shc*.Sos.Grb2/Sos.Ras_GEF SUBSTRATE n 
addmsg /kinetics/EGFR/act_EGFR /kinetics/EGFR/EGFR REAC A B 
addmsg /kinetics/EGFR/EGFR /kinetics/EGFR/act_EGFR SUBSTRATE n 
addmsg /kinetics/EGFR/EGF /kinetics/EGFR/act_EGFR SUBSTRATE n 
addmsg /kinetics/EGFR/L.EGFR /kinetics/EGFR/act_EGFR PRODUCT n 
addmsg /kinetics/EGFR/act_EGFR /kinetics/EGFR/L.EGFR REAC B A 
addmsg /kinetics/EGFR/L.EGFR/phosph_Shc /kinetics/EGFR/L.EGFR REAC eA B 
addmsg /kinetics/EGFR/Internalize /kinetics/EGFR/L.EGFR REAC A B 
addmsg /kinetics/EGFR/L.EGFR /kinetics/EGFR/L.EGFR/phosph_Shc ENZYME n 
addmsg /kinetics/EGFR/SHC /kinetics/EGFR/L.EGFR/phosph_Shc SUBSTRATE n 
addmsg /kinetics/EGFR/act_EGFR /kinetics/EGFR/EGF REAC A B 
addmsg /kinetics/EGFR/dephosph_Shc /kinetics/EGFR/SHC REAC B A 
addmsg /kinetics/EGFR/L.EGFR/phosph_Shc /kinetics/EGFR/SHC REAC sA B 
addmsg /kinetics/EGFR/dephosph_Shc /kinetics/EGFR/SHC* REAC A B 
addmsg /kinetics/Sos/Shc_bind_Sos.Grb2 /kinetics/EGFR/SHC* REAC A B 
addmsg /kinetics/EGFR/L.EGFR/phosph_Shc /kinetics/EGFR/SHC* MM_PRD pA 
addmsg /kinetics/EGFR/SHC* /kinetics/EGFR/dephosph_Shc SUBSTRATE n 
addmsg /kinetics/EGFR/SHC /kinetics/EGFR/dephosph_Shc PRODUCT n 
addmsg /kinetics/EGFR/Internalize /kinetics/EGFR/Internal_L.EGFR REAC B A 
addmsg /kinetics/EGFR/L.EGFR /kinetics/EGFR/Internalize SUBSTRATE n 
addmsg /kinetics/EGFR/Internal_L.EGFR /kinetics/EGFR/Internalize PRODUCT n 
addmsg /kinetics/Sos/Sos.Grb2 /kinetics/Sos/Shc_bind_Sos.Grb2 SUBSTRATE n 
addmsg /kinetics/EGFR/SHC* /kinetics/Sos/Shc_bind_Sos.Grb2 SUBSTRATE n 
addmsg /kinetics/Shc*.Sos.Grb2 /kinetics/Sos/Shc_bind_Sos.Grb2 PRODUCT n 
addmsg /kinetics/Sos/Grb2_bind_Sos* /kinetics/Sos/Sos*.Grb2 REAC B A 
addmsg /kinetics/Sos/Sos* /kinetics/Sos/Grb2_bind_Sos* SUBSTRATE n 
addmsg /kinetics/Sos/Grb2 /kinetics/Sos/Grb2_bind_Sos* SUBSTRATE n 
addmsg /kinetics/Sos/Sos*.Grb2 /kinetics/Sos/Grb2_bind_Sos* PRODUCT n 
addmsg /kinetics/Sos/Grb2_bind_Sos /kinetics/Sos/Grb2 REAC A B 
addmsg /kinetics/Sos/Grb2_bind_Sos* /kinetics/Sos/Grb2 REAC A B 
addmsg /kinetics/Sos/Grb2_bind_Sos /kinetics/Sos/Sos.Grb2 REAC B A 
addmsg /kinetics/Sos/Shc_bind_Sos.Grb2 /kinetics/Sos/Sos.Grb2 REAC A B 
addmsg /kinetics/MAPK*/phosph_Sos /kinetics/Sos/Sos* MM_PRD pA 
addmsg /kinetics/Sos/Grb2_bind_Sos* /kinetics/Sos/Sos* REAC A B 
addmsg /kinetics/Sos/dephosph_Sos /kinetics/Sos/Sos* REAC A B 
addmsg /kinetics/Sos/Sos* /kinetics/Sos/dephosph_Sos SUBSTRATE n 
addmsg /kinetics/Sos/Sos /kinetics/Sos/dephosph_Sos PRODUCT n 
addmsg /kinetics/Sos/Grb2 /kinetics/Sos/Grb2_bind_Sos SUBSTRATE n 
addmsg /kinetics/Sos/Sos.Grb2 /kinetics/Sos/Grb2_bind_Sos PRODUCT n 
addmsg /kinetics/Sos/Sos /kinetics/Sos/Grb2_bind_Sos SUBSTRATE n 
addmsg /kinetics/Sos/Grb2_bind_Sos /kinetics/Sos/Sos REAC A B 
addmsg /kinetics/MAPK*/phosph_Sos /kinetics/Sos/Sos REAC sA B 
addmsg /kinetics/Sos/dephosph_Sos /kinetics/Sos/Sos REAC B A 
addmsg /kinetics/MAPK* /graphs/conc1/MAPK*.Co PLOT Co *MAPK*.Co *orange 
enddump
// End of dump

call /kinetics/PKC-active/PKC-act-raf/notes LOAD \
"Rate consts from Chen et al Biochem 32, 1032 (1993)" \
"k3 = k2 = 4" \
"k1 = 9e-5" \
"recalculated gives 1.666e-5, which is not very different." \
"Looks like k3 is rate-limiting in this case: there is a huge amount" \
"of craf locked up in the enz complex. Let us assume a 10x" \
"higher Km, ie, lower affinity.  k1 drops by 10x." \
"Also changed k2 to 4x k3." \
"Lowerd k1 to 1e-6 to balance 10X DAG sensitivity of PKC"
call /kinetics/PKC-active/PKC-inact-GAP/notes LOAD \
"Rate consts copied from PCK-act-raf" \
"This reaction inactivates GAP. The idea is from the " \
"Boguski and McCormick review."
call /kinetics/PKC-active/PKC-act-GEF/notes LOAD \
"Rate consts from PKC-act-raf." \
"This reaction activates GEF. It can lead to at least 2X stim of ras, and" \
"a 2X stim of MAPK over and above that obtained via direct phosph of" \
"c-raf. Note that it is a push-pull reaction, and there is also a contribution" \
"through the phosphorylation and inactivation of GAPs." \
"The original PKC-act-raf rate consts are too fast. We lower K1 by 10 X"
call /kinetics/MAPK*/MAPK*-feedback/notes LOAD \
"Ueki et al JBC 269(22):15756-15761 show the presence of" \
"this step, but not the rate consts, which are derived from" \
"Sanghera et al  JBC 265(1):52-57, 1990, see the deriv in the" \
"MAPK* notes."
call /kinetics/MAPK*/phosph_Sos/notes LOAD \
"See Porfiri and McCormick JBC 271:10 pp5871 1996 for the" \
"existence of this step. We'll take the rates from the ones" \
"used for the phosph of Raf by MAPK." \
"Sep 17 1997: The transient activation curve matches better" \
"with k1 up  by 10 x."
call /kinetics/BetaGamma/notes LOAD \
"These exist in a nebulous sense in this model, basically only to balance" \
"the conservation equations. The details of their reassociation with G-GDP" \
"are not modeled" \
"Resting level =0.0094, stim level =.0236 from all42.g ish."
call /kinetics/MAPK/craf-1/notes LOAD \
"Couldn't find any ref to the actual conc of craf-1 but I" \
"should try Strom et al Oncogene 5 pp 345" \
"In line with the other kinases in the cascade, I estimate the conc to be" \
"0.2 uM. To init we use 0.15, which is close to equil"
call /kinetics/MAPK/MAPKK/notes LOAD \
"Conc is from Seger et al JBC 267:20 pp14373 (1992)" \
"mwt is 45/46 Kd" \
"We assume that phosphorylation on both ser and thr is needed for" \
"activiation. See Kyriakis et al Nature 358 417 1992" \
"Init conc of total is 0.18" \
""
call /kinetics/MAPK/MAPK/notes LOAD \
"conc is from Sanghera et al JBC 265 pp 52 (1990)" \
"A second calculation gives 3.1 uM, from same paper." \
"They est MAPK is 1e-4x total protein, and protein is 15% of cell wt," \
"so MAPK is 1.5e-5g/ml = 0.36uM. which is closer to our first estimate." \
"Lets use this."
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
"k3=0.15/sec, scale to get k2=0.6." \
"k1=0.75/46.6nM=2.7e-5"
call /kinetics/MAPK/MAPKK*/MAPKKthr/notes LOAD \
"Rate consts same as for MAPKKtyr."
call /kinetics/MAPK/MAPKK-ser/notes LOAD \
"Intermediately phophorylated, assumed inactive, form of MAPKK"
call /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.1/notes LOAD \
"Kinetics are the same as for the craf-1* activity, ie.," \
"k1=1.1e-6, k2=.42, k3 =0.105" \
"These are based on Force et al PNAS USA 91 1270-1274 1994." \
"These parms cannot reach the observed 4X stim of MAPK. So lets" \
"increase the affinity, ie, raise k1 10X to 1.1e-5" \
"Lets take it back down to where it was." \
"Back up to 5X: 5.5e-6"
call /kinetics/MAPK/Raf-GTP-Ras*/Raf-GTP-Ras*.2/notes LOAD \
"Same kinetics as other c-raf activated forms. See " \
"Force et al PNAS 91 1270-1274 1994." \
"k1 = 1.1e-6, k2 = .42, k3 = 1.05" \
"raise k1 to 5.5e-6" \
""
call /kinetics/MKP-1/notes LOAD \
"MKP-1 dephosphoryates and inactivates MAPK in vivo: Sun et al Cell 75 " \
"487-493 1993. Levels of MKP-1" \
"are regulated, and rise in 1 hour. " \
"Kinetics from Charles et al PNAS 90:5292-5296 1993. They refer" \
"to Charles et al Oncogene 7 187-190 who show that half-life of MKP1/3CH134" \
"is 40 min. 80% deph of MAPK in 20 min" \
"Sep 17 1997: CoInit now 0.4x to 0.0032. See parm searches" \
"from jun96 on."
call /kinetics/MKP-1/MKP1-tyr-deph/notes LOAD \
"The original kinetics have been modified to obey the k2 = 4 * k3 rule," \
"while keeping kcat and Km fixed. As noted in the NOTES, the only constraining" \
"data point is the time course of MAPK dephosphorylation, which this" \
"model satisfies. It would be nice to have more accurate estimates of" \
"rate consts and MKP-1 levels from the literature. " \
"Effective Km : 67 nM" \
"kcat = 1.43 umol/min/mg"
call /kinetics/MKP-1/MKP1-thr-deph/notes LOAD \
"See MKP1-tyr-deph"
call /kinetics/Ras-act-craf/notes LOAD \
"Assume the binding is fast and limited only by the amount of " \
"Ras* available. So kf=kb/[craf-1]" \
"If kb is 1/sec, then kf = 1/0.2 uM = 1/(0.2 * 6e5) = 8.3e-6" \
"Later: Raise it by 10 X to 4e-5" \
"From Hallberg et al JBC 269:6 3913-3916 1994, 3% of cellular Raf is" \
"complexed with Ras. So we raise kb 4x to 4" \
"This step needed to memb-anchor and activate Raf: Leevers et al Nature" \
"369 411-414" \
"(I don't...."
call /kinetics/PPhosphatase2A/notes LOAD \
"Refs: Pato et al Biochem J 293:35-41(93);" \
"Takai&Mieskes Biochem J 275:233-239" \
"k1=1.46e-4, k2=1000,k3=250. these use" \
"kcat values for calponin. Also, units of kcat may be in min!" \
"revert to Vmax base:" \
"k3=6, k2=25,k1=3.3e-6 or 6,6,1e-6" \
"CoInit assumed 0.1 uM." \
"See NOTES for MAPK_Ras50.g. CoInit now 0.08" \
"Sep 17 1997: Raise CoInt 1.4x to 0.224, see parm" \
"searches from jun 96 on." \
""
call /kinetics/PPhosphatase2A/craf-deph/notes LOAD \
"See parent PPhosphatase2A for parms" \
""
call /kinetics/PPhosphatase2A/MAPKK-deph/notes LOAD \
"See: Kyriakis et al Nature 358 pp 417-421 1992" \
"Ahn et al Curr Op Cell Biol 4:992-999 1992 for this pathway." \
"See parent PPhosphatase2A for parms."
call /kinetics/PPhosphatase2A/craf**-deph/notes LOAD \
"Ueki et al JBC 269(22) pp 15756-15761 1994 show hyperphosphorylation of" \
"craf, so this is there to dephosphorylate it. Identity of phosphatase is not" \
"known to me, but it may be PP2A like the rest, so I have made it so."
call /kinetics/Ras/notes LOAD \
"Ras has now gotten to be a big enough component of the model to" \
"deserve its own group. The main refs are" \
"Boguski and McCormick Nature 366 643-654 '93 Major review" \
"Eccleston et al JBC 268:36 pp 27012-19" \
"Orita et al JBC 268:34 2554246"
call /kinetics/Ras/bg-act-GEF/notes LOAD \
"SoS/GEF is present at 50 nM ie 3e4/cell. BetaGamma maxes out at 9e4." \
"Assume we have 1/3 of the GEF active when the BetaGamma is 1.5e4." \
"so 1e4 * kb = 2e4 * 1.5e4 * kf, so kf/kb = 3e-5. The rate of this equil should" \
"be reasonably fast, say 1/sec" \
""
call /kinetics/Ras/GEF-Gprot-bg/notes LOAD \
"Guanine nucleotide exchange factor. This activates raf by exchanging bound" \
"GDP with GTP. I have left the GDP/GTP out of this reaction, it would be" \
"trivial to put them in. See Boguski & McCormick." \
"Possible candidate molecules: RasGRF, smgGDS, Vav (in dispute). " \
"rasGRF: Kcat= 1.2/min    Km = 680 nM" \
"smgGDS: Kcat: 0.37 /min, Km = 220 nM." \
"vav: Turnover up over baseline by 10X, " \
""
call /kinetics/Ras/GEF-Gprot-bg/GEF-bg_act-ras/notes LOAD \
"Kinetics based on the activation of Gq by the receptor complex in the" \
"Gq model (in turn based on the Mahama and Linderman model)" \
"k1 = 2e-5, k2 = 1e-10, k3 = 10 (I do not know why they even bother with k2)." \
"Lets put k1 at 2e-6 to get a reasonable equilibrium" \
"More specific values from, eg.g: Orita et al JBC 268(34) 25542-25546" \
"from rasGRF and smgGDS: k1=3.3e-7; k2 = 0.08, k3 = 0.02" \
""
call /kinetics/Ras/inact-GEF/notes LOAD \
"Assume that SoS is present only at 50 nM." \
"Revised to 100 nM to get equil to experimentally known levels."
call /kinetics/Ras/GEF*/notes LOAD \
"phosphorylated and thereby activated form of GEF. See, e.g." \
"Orita et al JBC 268:34 25542-25546 1993, Gulbins et al." \
"It is not clear whether there is major specificity for tyr or ser/thr."
call /kinetics/Ras/GEF*/GEF*-act-ras/notes LOAD \
"Kinetics same as GEF-bg-act-ras" \
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
"This is extremely slow (1e-4), but it is significant as so little GAP actually" \
"gets complexed with it that the total GTP turnover rises only by" \
"2-3 X (see Gibbs et al, JBC 265(33) 20437-20422) and " \
"Eccleston et al JBC 268(36) 27012-27019" \
"kf = 1e-4" \
""
call /kinetics/Ras/dephosph-GAP/notes LOAD \
"Assume a reasonably good rate for dephosphorylating it, 1/sec"
call /kinetics/Ras/GAP/notes LOAD \
"GTPase-activating proteins. See Boguski and McCormick." \
"Turn off Ras by helping to hydrolyze bound GTP. " \
"This one is probably NF1, ie.,  Neurofibromin as it is inhibited by AA and lipids," \
"and expressed in neural cells. p120-GAP is also a possible candidate, but" \
"is less regulated. Both may exist at similar levels." \
"See Eccleston et al JBC 268(36) pp27012-19" \
"Level=.002"
call /kinetics/Ras/GAP/GAP-inact-ras/notes LOAD \
"From Eccleston et al JBC 268(36)pp27012-19 get Kd < 2uM, kcat - 10/sec" \
"From Martin et al Cell 63 843-849 1990 get Kd ~ 250 nM, kcat = 20/min" \
"I will go with the Eccleston figures as there are good error bars (10%). In general" \
"the values are reasonably close." \
"k1 = 1.666e-3/sec, k2 = 1000/sec, k3 = 10/sec (note k3 is rate-limiting)" \
"5 Nov 2002: Changed ratio term to 4 from 100. Now we have" \
"k1=8.25e-5; k2=40, k3=10. k3 is still rate-limiting."
call /kinetics/Ras/inact-GEF*/notes LOAD \
"Phosphorylation-inactivated form of GEF. See" \
"Hordijk et al JBC 269:5 3534-3538 1994" \
"and " \
"Buregering et al EMBO J 12:11 4211-4220 1993" \
""
call /kinetics/Ras/CaM-bind-GEF/notes LOAD \
"We have no numbers for this. It is probably between" \
"the two extremes represented by the CaMKII phosph states," \
"and I have used guesses based on this." \
"kf=1e-4" \
"kb=1" \
"The reaction is based on Farnsworth et al Nature 376 524-527" \
"1995"
call /kinetics/Ras/CaM-GEF/notes LOAD \
"See Farnsworth et al Nature 376 524-527 1995"
call /kinetics/Ras/CaM-GEF/CaM-GEF-act-ras/notes LOAD \
"Kinetics same as GEF-bg_act-ras" \
""
call /kinetics/PKA-active/PKA-phosph-GEF/notes LOAD \
"This pathway inhibits Ras when cAMP is elevated. See:" \
"Hordijk et al JBC 269:5 3534-3538 1994" \
"Burgering et al EMBO J 12:11 4211-4220 1993" \
"The rates are the same as used in PKA-phosph-I1"
call /kinetics/CaM-Ca4/notes LOAD \
"Resting level is ~1e-5 uM, small enough to ignore."
call /kinetics/EGFR/EGFR/notes LOAD \
"Berkers et al JBC 266 say 22K hi aff recs." \
"Sherrill and Kyte Biochemistry 35 use range 4-200 nM." \
"These match, lets use them."
call /kinetics/EGFR/act_EGFR/notes LOAD \
"Affinity of EGFR for EGF is complex: depends on [EGFR]." \
"We'll assume fixed [EGFR] and use exptal" \
"affinity ~20 nM (see Sherrill and Kyte" \
"Biochem 1996 35 5705-5718, Berkers et al JBC 266:2 922-927" \
"1991, Sorokin et al JBC 269:13 9752-9759 1994). " \
"Tau =~2 min (Davis et al JBC 263:11 5373-5379 1988)" \
"or Berkers Kass = 6.2e5/M/sec, Kdiss=3.5e-4/sec." \
"Sherrill and Kyte have Hill Coeff=1.7" \
""
call /kinetics/EGFR/L.EGFR/notes LOAD \
"This is terribly simplified: there are many interesting" \
"intermediate stages, including dimerization and assoc" \
"with adapter molecules like Shc, that contribute to the" \
"activation of the EGFR."
call /kinetics/EGFR/L.EGFR/phosph_Shc/notes LOAD \
"Rates from Okada et al JBC 270:35 pp 20737 1995" \
"Km = 0.70 to 0.85 uM, Vmax = 4.4 to 5.0 pmol/min. Unfortunately" \
"the amount of enzyme is not known, the prep is only" \
"partially purified." \
"Time course of phosph is max within 30 sec, falls back within" \
"20 min. Ref: Sasaoka et al JBC 269:51 32621 1994." \
"Use k3 = 0.1 based on this tau." \
""
call /kinetics/EGFR/SHC/notes LOAD \
"There are 2 isoforms: 52 KDa and 46 KDa (See Okada et al" \
"JBC 270:35 pp 20737 1995). They are acted up on by the EGFR" \
"in very similar ways, and apparently both bind Grb2 similarly," \
"so we'll bundle them together here." \
"Sasaoka et al JBC 269:51 pp 32621 1994 show immunoprecs where" \
"it looks like there is at least as much Shc as Grb2. So" \
"we'll tentatively say there is 1 uM of Shc."
call /kinetics/EGFR/dephosph_Shc/notes LOAD \
"Time course of decline of phosph is 20 min. Part of this is" \
"the turnoff time of the EGFR itself. Lets assume a tau of" \
"10 min for this dephosph. It may be wildly off."
call /kinetics/EGFR/Internalize/notes LOAD \
"See Helin and Beguinot JBC 266:13 1991 pg 8363-8368." \
"In Fig 3 they have internalization tau about 10 min, " \
"equil at about 20% EGF available. So kf = 4x kb, and" \
"1/(kf + kb) = 600 sec so kb = 1/3K = 3.3e-4," \
"and kf = 1.33e-3. This doesn't take into account the" \
"unbound receptor, so we need to push the kf up a bit, to" \
"0.002"
call /kinetics/Sos/Shc_bind_Sos.Grb2/notes LOAD \
"Sasaoka et al JBC 269:51 pp 32621 1994, table on pg" \
"32623 indicates that this pathway accounts for about " \
"50% of the GEF activation. (88% - 39%). Error is large," \
"about 20%. Fig 1 is most useful in constraining rates." \
"" \
"Chook et al JBC 271:48 pp 30472, 1996 say that the Kd is" \
"0.2 uM for Shc binding to EGFR. The Kd for Grb direct binding" \
"is 0.7, so we'll ignore it."
call /kinetics/Sos/Grb2_bind_Sos*/notes LOAD \
"Same rates as Grb2_bind_Sos: Porfiri and McCormick JBC" \
"271:10 pp 5871 1996 show that the binding is not affected" \
"by the phosph."
call /kinetics/Sos/Grb2/notes LOAD \
"There is probably a lot of it in the cell: it is also known" \
"as Ash (abundant src homology protein I think). Also " \
"Waters et al JBC 271:30 18224 1996 say that only a small" \
"fraction of cellular Grb is precipitated out when SoS is" \
"precipitated. As most of the Sos seems to be associated" \
"with Grb2, it would seem like there is a lot of the latter." \
"Say 1 uM. I haven't been able to find a decent...."
call /kinetics/Sos/dephosph_Sos/notes LOAD \
"The only clue I have to these rates is from the time" \
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
"They say it is 1:1 binding."
call /kinetics/Sos/Sos/notes LOAD \
"I have tried using low (0.02 uM) initial concs, but these" \
"give a very flat response to EGF stim although the overall" \
"activation of Ras is not too bad. I am reverting to 0.1 " \
"because we expect a sharp initial response, followed by" \
"a decline." \
"Sep 17 1997: The transient activation curve looks better with" \
"[Sos] = 0.05." \
"Apr 26 1998: Some error there, it is better where it was" \
"at 0.1"
complete_loading
