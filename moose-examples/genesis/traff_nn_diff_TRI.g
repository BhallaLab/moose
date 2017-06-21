//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Sun Jul 15 11:47:11 2012
 
include kkit {argv 1}
 
FASTDT = 0.0001
SIMDT = 0.002
CONTROLDT = 5
PLOTDT = 5
MAXTIME = 1000
TRANSIENT_TIME = 2
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
simundump text /kinetics/notes 0 ""
call /kinetics/notes LOAD \
""
simundump text /kinetics/geometry/notes 0 ""
call /kinetics/geometry/notes LOAD \
""
simundump kreac /kinetics/exo 0 0.00028 0.00028 "" white black 3 11 0
simundump text /kinetics/exo/notes 0 ""
call /kinetics/exo/notes LOAD \
""
simundump kreac /kinetics/endo 0 0.001 0.001 "" white black -6 11 0
simundump text /kinetics/endo/notes 0 ""
call /kinetics/endo/notes LOAD \
""
simundump kpool /kinetics/stot 0 0 0 0 0 0 0 0 6 0 /kinetics/geometry blue \
  black 6 15 0
simundump text /kinetics/stot/notes 0 ""
call /kinetics/stot/notes LOAD \
""
simundump group /kinetics/B 0 yellow black x 0 0 "" Bulk defaultfile.g 0 0 0 \
  -7 2 0
simundump text /kinetics/B/notes 0 ""
call /kinetics/B/notes LOAD \
""
simundump kpool /kinetics/B/P 0 0 0.2 0.2 1.2 1.2 0 0 6 0 /kinetics/geometry \
  4 yellow -2 -2 0
simundump text /kinetics/B/P/notes 0 ""
call /kinetics/B/P/notes LOAD \
""
simundump kenz /kinetics/B/P/kenz 0 0 0 0 0 6 4.1667 4 1 0 0 "" red 4 "" -2 \
  -1 0
simundump text /kinetics/B/P/kenz/notes 0 ""
call /kinetics/B/P/kenz/notes LOAD \
""
simundump kreac /kinetics/B/basal 0 0.01 0 "" white yellow -2 -5 0
simundump text /kinetics/B/basal/notes 0 ""
call /kinetics/B/basal/notes LOAD \
""
simundump kpool /kinetics/B/M 0 0 2.8 2.8 16.8 16.8 0 0 6 0 \
  /kinetics/geometry 62 yellow -4 2 0
simundump text /kinetics/B/M/notes 0 ""
call /kinetics/B/M/notes LOAD \
""
simundump kpool /kinetics/B/M* 0 0 0 0 0 0 0 0 6 0 /kinetics/geometry 28 \
  yellow 3 2 0
simundump text /kinetics/B/M*/notes 0 ""
call /kinetics/B/M*/notes LOAD \
""
simundump kenz /kinetics/B/M*/kenz 0 0 0 0 0 59.999 0.83333 4 1 0 0 "" red 28 \
  "" 1 5 0
simundump text /kinetics/B/M*/kenz/notes 0 ""
call /kinetics/B/M*/kenz/notes LOAD \
""
simundump group /kinetics/A 0 16 black x 0 0 "" PSD defaultfile.g 0 0 0 -5 20 \
  0
simundump text /kinetics/A/notes 0 ""
call /kinetics/A/notes LOAD \
""
simundump kreac /kinetics/A/basal 0 0.01 0 "" white 16 1 27 0
simundump text /kinetics/A/basal/notes 0 ""
call /kinetics/A/basal/notes LOAD \
""
simundump kpool /kinetics/A/P 0 0 0.2 0.2 1.2 1.2 0 0 5.9999 0 \
  /kinetics/geometry 3 16 0 15 0
simundump text /kinetics/A/P/notes 0 ""
call /kinetics/A/P/notes LOAD \
""
simundump kenz /kinetics/A/P/kenz 0 0 0 0 0 5.9999 4.1667 4 1 0 0 "" red 3 "" \
  0 16 0
simundump text /kinetics/A/P/kenz/notes 0 ""
call /kinetics/A/P/kenz/notes LOAD \
""
simundump kpool /kinetics/A/M 0 0 0 0 0 0 0 0 5.9999 0 /kinetics/geometry 53 \
  16 -3 20 0
simundump text /kinetics/A/M/notes 0 "2.235\n"
call /kinetics/A/M/notes LOAD \
"2.235" \
""
simundump kpool /kinetics/A/M* 0 0 0 0 0 0 0 0 5.9999 0 /kinetics/geometry 47 \
  16 5 20 0
simundump text /kinetics/A/M*/notes 0 ""
call /kinetics/A/M*/notes LOAD \
""
simundump kenz /kinetics/A/M*/kenz 0 0 0 0 0 5.9999 0.83335 4 1 0 0 "" red 57 \
  "" 1 23 0
simundump text /kinetics/A/M*/kenz/notes 0 ""
call /kinetics/A/M*/kenz/notes LOAD \
""
simundump xgraph /graphs/conc1 0 0 1000 0.001 3 0
simundump xgraph /graphs/conc2 0 0 1000 0 3 0
simundump xplot /graphs/conc1/M.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 53 0 0 1
simundump xplot /graphs/conc1/M*.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 47 0 0 1
simundump xplot /graphs/conc2/M.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 62 0 0 1
simundump xplot /graphs/conc2/M*.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 28 0 0 1
simundump xgraph /moregraphs/conc3 0 0 1000 0 1 0
simundump xgraph /moregraphs/conc4 0 0 1000 0 1 0
simundump xcoredraw /edit/draw 0 -9 8 -7 29
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"8 Jul 2009. diff_dup_neg_TRI.g. Renamed mols, and a bit of fine-tuning" \
"of diff_dup_neg_tristable.g" \
"" \
"26 May 2009. diff_dup_neg_tristable.g" \
"This is the tristable model with the parameters as predicted by" \
"diff_dup_noanch.xls." \
"" \
"25 May 2009. diff_dup_neg4.g Based on neg4.g, so that both" \
"parts are neg slope but not bistable." \
"diff_dup_neg5.g: Based on diff_dup_neg4.g, this one fixes up" \
"some parameters and is nicely bistable." \
"" \
"" \
"20 May 2009. neg4.g. Based on diff_dup_neg2.g, which turned out" \
"to be bistable. This one really is not bistable." \
"" \
"" \
"16 May 2009. diff_dup_neg2.g. Based on diff_dup_neg.g, filled" \
"in rates and diffusion reacs." \
"" \
"15 May 2009. diff_dup_bis2.g. Based on diff_dup_bis.g, altered" \
"rates toward quad stable prediction." \
"fix_bis1.g: Testing bistability. This variant has range of " \
"about 30%, from 1.25 to 1.65." \
"" \
"diff_dup_neg.g: Based on fix_bis1.g, working out whether it is " \
"clearly neg slope." \
"" \
"13 May 2009. diff_dup_bis.g Based on diff_eq_bis3.g, set up" \
"duplicate reaction set in the bulk." \
"" \
"10 May 2009. diff_eq_bis2.g. Based on diff_eq_bis.g, here I fixed" \
"up the nasty sumtotal operation and now the C_psd* molecule " \
"handles its own enzyme site rather than setting up a slave" \
"pool and using that to do the enzyme." \
"diff_eq_bis3.g: Based on diff_eq_bis2.g, restored original" \
"cycling parameters as diff_eq_bis.g" \
"" \
"" \
"9 May 2009. diff_eq_bis.g. Based on min6.g. Implement" \
"diffusive exchange of equil reaction with bistable." \
"" \
"11 Apr 2009: min5.g: Eliminated the anchor protein from min4.g" \
"min6.g: Replace the entire bulk bistable system" \
"with a single dephosphorylation reaction." \
"" \
"min4.g: Includes trafficking too." \
"" \
"min3.g: Includes both bulk and PSD reactions" \
"" \
"min2psd.g: Same as min2.g, moved to PSD." \
"" \
"min2.g: This version is bistable."
addmsg /kinetics/B/M* /kinetics/exo SUBSTRATE n 
addmsg /kinetics/A/M* /kinetics/exo PRODUCT n 
addmsg /kinetics/B/M /kinetics/endo SUBSTRATE n 
addmsg /kinetics/A/M /kinetics/endo PRODUCT n 
addmsg /kinetics/A/M /kinetics/stot SUMTOTAL n nInit 
addmsg /kinetics/A/M* /kinetics/stot SUMTOTAL n nInit 
addmsg /kinetics/A/M*/kenz /kinetics/stot SUMTOTAL nComplex nComplexInit 
addmsg /kinetics/A/P/kenz /kinetics/stot SUMTOTAL nComplex nComplexInit 
addmsg /kinetics/B/P/kenz /kinetics/B/P REAC eA B 
addmsg /kinetics/B/P /kinetics/B/P/kenz ENZYME n 
addmsg /kinetics/B/M* /kinetics/B/P/kenz SUBSTRATE n 
addmsg /kinetics/B/M /kinetics/B/basal SUBSTRATE n 
addmsg /kinetics/B/M* /kinetics/B/basal PRODUCT n 
addmsg /kinetics/B/basal /kinetics/B/M REAC A B 
addmsg /kinetics/B/P/kenz /kinetics/B/M MM_PRD pA 
addmsg /kinetics/B/M*/kenz /kinetics/B/M REAC sA B 
addmsg /kinetics/endo /kinetics/B/M REAC A B 
addmsg /kinetics/B/basal /kinetics/B/M* REAC B A 
addmsg /kinetics/B/P/kenz /kinetics/B/M* REAC sA B 
addmsg /kinetics/B/M*/kenz /kinetics/B/M* REAC eA B 
addmsg /kinetics/B/M*/kenz /kinetics/B/M* MM_PRD pA 
addmsg /kinetics/exo /kinetics/B/M* REAC A B 
addmsg /kinetics/B/M* /kinetics/B/M*/kenz ENZYME n 
addmsg /kinetics/B/M /kinetics/B/M*/kenz SUBSTRATE n 
addmsg /kinetics/A/M /kinetics/A/basal SUBSTRATE n 
addmsg /kinetics/A/M* /kinetics/A/basal PRODUCT n 
addmsg /kinetics/A/P/kenz /kinetics/A/P REAC eA B 
addmsg /kinetics/A/P /kinetics/A/P/kenz ENZYME n 
addmsg /kinetics/A/M* /kinetics/A/P/kenz SUBSTRATE n 
addmsg /kinetics/A/M*/kenz /kinetics/A/M REAC sA B 
addmsg /kinetics/A/P/kenz /kinetics/A/M MM_PRD pA 
addmsg /kinetics/A/basal /kinetics/A/M REAC A B 
addmsg /kinetics/endo /kinetics/A/M REAC B A 
addmsg /kinetics/A/M*/kenz /kinetics/A/M* MM_PRD pA 
addmsg /kinetics/A/P/kenz /kinetics/A/M* REAC sA B 
addmsg /kinetics/A/basal /kinetics/A/M* REAC B A 
addmsg /kinetics/A/M*/kenz /kinetics/A/M* REAC eA B 
addmsg /kinetics/exo /kinetics/A/M* REAC B A 
addmsg /kinetics/A/M /kinetics/A/M*/kenz SUBSTRATE n 
addmsg /kinetics/A/M* /kinetics/A/M*/kenz ENZYME n 
addmsg /kinetics/A/M /graphs/conc1/M.Co PLOT Co *M.Co *53 
addmsg /kinetics/A/M* /graphs/conc1/M*.Co PLOT Co *M*.Co *47 
addmsg /kinetics/B/M /graphs/conc2/M.Co PLOT Co *M.Co *62 
addmsg /kinetics/B/M* /graphs/conc2/M*.Co PLOT Co *M*.Co *28 
enddump
// End of dump

call /kinetics/A/M/notes LOAD \
"2.235" \
""
complete_loading
