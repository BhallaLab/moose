//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Mon Feb 13 16:21:21 2006
 
include kkit {argv 1}
 
FASTDT = 5e-05
SIMDT = 0.01
CONTROLDT = 0.1
PLOTDT = 20
MAXTIME = 20000
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 1
DEFAULT_VOL = 3e-18
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
simundump geometry /kinetics/geometry 0 3e-18 3 sphere "" white black 394 4 0
simundump group /kinetics/Lambda-cl_gene 0 15 yellow x 0 0 "" Lambda-cl_gene \
  /home2/bhalla/scripts/modules/yellow_0.g 0 0 0 386 1 0
simundump kpool /kinetics/Lambda-cl_gene/Nucleotides 0 0 0.00055555 \
  0.00055555 0.99999 0.99999 0 0 1800 4 /kinetics/geometry 6 15 400 -4 0
simundump kpool /kinetics/Lambda-cl_gene/Amino_Acids 0 0 0.00055555 \
  0.00055555 0.99999 0.99999 0 0 1800 4 /kinetics/geometry 52 15 379 -4 0
simundump kpool /kinetics/Lambda-cl_gene/lambda-cl_mRNA 0 0 0 0 0 0 0 0 1800 \
  0 /kinetics/geometry 5 15 386 -4 0
simundump kenz /kinetics/Lambda-cl_gene/lambda-cl_mRNA/lambda-cl_translation 0 \
  0 0 0 0 1800 0.83494 0.668 0.167 0 1 "" red 5 "" 379 -6 0
simundump kpool /kinetics/Lambda-cl_gene/Degraded_lambda-cl_mRNA 0 0 0 0 0 0 \
  0 0 1800 4 /kinetics/geometry 1 15 393 -4 0
simundump kpool /kinetics/Lambda-cl_gene/Lambda-cl_operator 0 0 0.022222 \
  0.022222 40 40 0 0 1800 0 /kinetics/geometry 36 15 400 -8 0
simundump kenz /kinetics/Lambda-cl_gene/Lambda-cl_operator/Lambda-cl_basal_transcription 0 \
  0 0 0 0 1800 2.4998 2 0.5 0 1 "" red 36 "" 400 -7 0
simundump kreac /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction 0 \
  0.55556 224 "" white 15 396 -9 0
simundump kreac /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction[1] 0 \
  0.55556 9 "" white 15 390 -9 0
simundump kpool /kinetics/Lambda-cl_gene/Lambda-cl_operator_1st_bound 0 0 0 0 \
  0 0 0 0 1800 0 /kinetics/geometry 2 15 393 -8 0
simundump kenz /kinetics/Lambda-cl_gene/Lambda-cl_operator_1st_bound/Lambda-cl_transcription 0 \
  0 0 0 0 1800 0.0024998 0.002 0.0005 0 1 "" red 2 "" 393 -6 0
simundump kpool /kinetics/Lambda-cl_gene/Degraded_Lambda-cl 0 0 0 0 0 0 0 0 \
  1800 4 /kinetics/geometry 2 15 379 -15 0
simundump kreac /kinetics/Lambda-cl_gene/lambda-cl_mRNA_degradation 0 \
  0.005775 0 "" white 15 389 -3 0
simundump kreac /kinetics/Lambda-cl_gene/Lambda-cl_degradation 0 0.016666 0 \
  "" white 15 379 -13 0
simundump kpool /kinetics/Lambda-cl_gene/Lambda-cl_operator_2nd_bound 0 0 0 0 \
  0 0 0 0 1800 0 /kinetics/geometry 52 15 386 -8 0
simundump kenz /kinetics/Lambda-cl_gene/Lambda-cl_operator_2nd_bound/Lambda-cl_transcription1 0 \
  0 0 0 0 1800 0.0024998 0.002 0.0005 0 1 "" red 52 "" 386 -7 0
simundump kpool /kinetics/Lambda-cl_gene/Lambda-cl 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 27 15 379 -11 0
simundump group /kinetics/Lac_gene 0 yellow yellow x 0 0 "" Lac_gene \
  /home2/bhalla/scripts/modules/lac_gene_0.g 0 0 0 345 1 0
simundump kpool /kinetics/Lac_gene/lac_mRNA 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 12 yellow 344 -4 0
simundump kenz /kinetics/Lac_gene/lac_mRNA/lac_translation 0 0 0 0 0 1800 \
  0.83494 0.668 0.167 0 1 "" red 12 "" 338 -6 0
simundump kpool /kinetics/Lac_gene/Nucleotides 0 0 0.00055555 0.00055555 \
  0.99999 0.99999 0 0 1800 4 /kinetics/geometry 6 yellow 358 -4 0
simundump kreac /kinetics/Lac_gene/lac_mRNA_degradation 0 0.005775 0 "" white \
  yellow 347 -3 0
simundump kpool /kinetics/Lac_gene/Degraded_lac_mRNA 0 0 0 0 0 0 0 0 1800 4 \
  /kinetics/geometry 1 yellow 351 -4 0
simundump kpool /kinetics/Lac_gene/Lac_operator 0 0 0.022222 0.022222 40 40 0 \
  0 1800 0 /kinetics/geometry 36 yellow 358 -8 0
simundump kenz /kinetics/Lac_gene/Lac_operator/Lac_basal_transcription 0 0 0 \
  0 0 1800 2.4998 2 0.5 0 1 "" red 36 "" 358 -7 0
simundump kpool /kinetics/Lac_gene/Lac_operator_1st_bound 0 0 0 0 0 0 0 0 \
  1800 0 /kinetics/geometry 2 yellow 351 -8 0
simundump kenz /kinetics/Lac_gene/Lac_operator_1st_bound/Lac_transcription 0 \
  0 0 0 0 1800 0.0024998 0.002 0.0005 0 1 "" red 2 "" 351 -6 0
simundump kpool /kinetics/Lac_gene/Lac_operator_2nd_bound 0 0 0 0 0 0 0 0 \
  1800 0 /kinetics/geometry 52 yellow 344 -8 0
simundump kenz /kinetics/Lac_gene/Lac_operator_2nd_bound/Lac_transcription1 0 \
  0 0 0 0 1800 0.0024998 0.002 0.0005 0 1 "" red 52 "" 344 -7 0
simundump kreac /kinetics/Lac_gene/Lac_degradation 0 0.016666 0 "" white \
  yellow 338 -13 0
simundump kpool /kinetics/Lac_gene/Degraded_lac 0 0 0 0 0 0 0 0 1800 4 \
  /kinetics/geometry 2 yellow 338 -15 0
simundump kpool /kinetics/Lac_gene/Amino_Acids 0 0 0.00055555 0.00055555 \
  0.99999 0.99999 0 0 1800 4 /kinetics/geometry 52 yellow 338 -4 0
simundump kreac /kinetics/Lac_gene/Lac_operator_inhibition_reaction 0 0.55556 \
  224 "" white yellow 354 -9 0
simundump kreac /kinetics/Lac_gene/Lac_operator_inhibition_reaction[1] 0 \
  0.55556 9 "" white yellow 348 -9 0
simundump kpool /kinetics/Lac_gene/Lac 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 1 yellow 338 -11 0
simundump group /kinetics/TetR_gene 0 61 yellow x 0 0 "" TetR_gene \
  /home2/bhalla/scripts/modules/TetR_gene_0.g 0 0 0 311 1 0
simundump kreac /kinetics/TetR_gene/TetR_degradation 0 0.01666 0 "" white 61 \
  303 -13 0
simundump kpool /kinetics/TetR_gene/TetR_operator 0 0 0.022222 0.022222 40 40 \
  0 0 1800 0 /kinetics/geometry 36 61 323 -8 0
simundump kenz /kinetics/TetR_gene/TetR_operator/TetR_basal_transcription 0 0 \
  0 0 0 1800 2.4998 2 0.5 0 1 "" red 36 "" 323 -7 0
simundump kpool /kinetics/TetR_gene/Nucleotides 0 0 0.00055555 0.00055555 \
  0.99999 0.99999 0 0 1800 4 /kinetics/geometry blue 61 323 -4 0
simundump kpool /kinetics/TetR_gene/TetR_operator_1st_bound 0 0 0 0 0 0 0 0 \
  1800 0 /kinetics/geometry 2 61 316 -8 0
simundump kenz /kinetics/TetR_gene/TetR_operator_1st_bound/TetR_transcription 0 \
  0 0 0 0 1800 0.0024998 0.002 0.0005 0 1 "" red 2 "" 316 -6 0
simundump kpool /kinetics/TetR_gene/TetR_operator_2nd_bound 0 0 0 0 0 0 0 0 \
  1800 0 /kinetics/geometry 46 61 309 -8 0
simundump kenz /kinetics/TetR_gene/TetR_operator_2nd_bound/TetR_transcription1 0 \
  0 0 0 0 1800 0.0024998 0.002 0.0005 0 1 "" red 46 "" 309 -7 0
simundump kpool /kinetics/TetR_gene/Amino_Acids 0 0 0.00055555 0.00055555 \
  0.99999 0.99999 0 0 1800 4 /kinetics/geometry blue 61 303 -4 0
simundump kpool /kinetics/TetR_gene/tetR_mRNA 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 12 61 309 -4 0
simundump kenz /kinetics/TetR_gene/tetR_mRNA/tetR_translation 0 0 0 0 0 1800 \
  0.83494 0.668 0.167 0 1 "" red 12 "" 303 -6 0
simundump kreac /kinetics/TetR_gene/tetR_mRNA_degradation 0 0.005775 0 "" \
  white 61 313 -3 0
simundump kpool /kinetics/TetR_gene/Degraded_tetR_mRNA 0 0 0 0 0 0 0 0 1800 4 \
  /kinetics/geometry 1 61 316 -4 0
simundump kpool /kinetics/TetR_gene/Degraded_TetR 0 0 0 0 0 0 0 0 1800 4 \
  /kinetics/geometry 2 61 303 -15 0
simundump kreac /kinetics/TetR_gene/TetR_operator_inhibition_reaction 0 \
  0.55556 224 "" white 61 319 -9 0
simundump kreac /kinetics/TetR_gene/TetR_operator_inhibition_reaction[1] 0 \
  0.55556 9 "" white 61 313 -9 0
simundump kpool /kinetics/TetR_gene/TetR 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 16 61 303 -11 0
simundump doqcsinfo /kinetics/doqcsinfo 0 /home/harsha/Repressilator.g \
  Repressilator network "Amrita Sekhar and Sharat J. Vayttaden, NCBS" \
  "Michael B. Elowitz, Stanislas Lelbler" "" "E. Coli" \
  "E.coli Expression, Transposon, Lambda phage" Cytosol Qualitative \
  "<a href=http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=pubmed&dopt=Abstract&list_uids=10659856&query_hl=1&itool=pubmed_docsum>Elowitz MB, Leibler S., Nature. 2000 Jan 20;403(6767):335-8</a> ( Peer-reviewed publication )" \
  "Exact GENESIS implementation" "Approximates original data " 374 4 0
simundump xgraph /graphs/conc1 0 9500 14500 0 1.65 0
simundump xgraph /graphs/conc2 0 0 1e+05 0 1.2 0
simundump xplot /graphs/conc1/TetR.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 16 0 0 1
simundump xplot /graphs/conc1/Lambda-cl.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 27 0 0 1
simundump xplot /graphs/conc1/Lac.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 1 0 0 1
simundump xgraph /moregraphs/conc3 0 0 1e+05 0 1.8 0
simundump xgraph /moregraphs/conc4 0 0 1e+05 0 0.115 0
simundump xcoredraw /edit/draw 0 301 402 -17 6
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"Repressilator model: A synthetic oscillatory network" \
"of transcriptional regulators designed by Michael B. Elowitz and" \
"Stanislas Leibler 2000 Nature 403(6767):335-338"
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_2nd_bound/Lambda-cl_transcription1 /kinetics/Lambda-cl_gene/Nucleotides REAC sA B 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_1st_bound/Lambda-cl_transcription /kinetics/Lambda-cl_gene/Nucleotides REAC sA B 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator/Lambda-cl_basal_transcription /kinetics/Lambda-cl_gene/Nucleotides REAC sA B 
addmsg /kinetics/Lambda-cl_gene/lambda-cl_mRNA/lambda-cl_translation /kinetics/Lambda-cl_gene/Amino_Acids REAC sA B 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_2nd_bound/Lambda-cl_transcription1 /kinetics/Lambda-cl_gene/lambda-cl_mRNA MM_PRD pA 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_1st_bound/Lambda-cl_transcription /kinetics/Lambda-cl_gene/lambda-cl_mRNA MM_PRD pA 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator/Lambda-cl_basal_transcription /kinetics/Lambda-cl_gene/lambda-cl_mRNA MM_PRD pA 
addmsg /kinetics/Lambda-cl_gene/lambda-cl_mRNA_degradation /kinetics/Lambda-cl_gene/lambda-cl_mRNA REAC A B 
addmsg /kinetics/Lambda-cl_gene/lambda-cl_mRNA /kinetics/Lambda-cl_gene/lambda-cl_mRNA/lambda-cl_translation ENZYME n 
addmsg /kinetics/Lambda-cl_gene/Amino_Acids /kinetics/Lambda-cl_gene/lambda-cl_mRNA/lambda-cl_translation SUBSTRATE n 
addmsg /kinetics/Lambda-cl_gene/lambda-cl_mRNA_degradation /kinetics/Lambda-cl_gene/Degraded_lambda-cl_mRNA REAC B A 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction /kinetics/Lambda-cl_gene/Lambda-cl_operator REAC A B 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator /kinetics/Lambda-cl_gene/Lambda-cl_operator/Lambda-cl_basal_transcription ENZYME n 
addmsg /kinetics/Lambda-cl_gene/Nucleotides /kinetics/Lambda-cl_gene/Lambda-cl_operator/Lambda-cl_basal_transcription SUBSTRATE n 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_1st_bound /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction PRODUCT n 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction SUBSTRATE n 
addmsg /kinetics/TetR_gene/TetR /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction SUBSTRATE n 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_2nd_bound /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction[1] PRODUCT n 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_1st_bound /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction[1] SUBSTRATE n 
addmsg /kinetics/TetR_gene/TetR /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction[1] SUBSTRATE n 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction[1] /kinetics/Lambda-cl_gene/Lambda-cl_operator_1st_bound REAC A B 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction /kinetics/Lambda-cl_gene/Lambda-cl_operator_1st_bound REAC B A 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_1st_bound /kinetics/Lambda-cl_gene/Lambda-cl_operator_1st_bound/Lambda-cl_transcription ENZYME n 
addmsg /kinetics/Lambda-cl_gene/Nucleotides /kinetics/Lambda-cl_gene/Lambda-cl_operator_1st_bound/Lambda-cl_transcription SUBSTRATE n 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_degradation /kinetics/Lambda-cl_gene/Degraded_Lambda-cl REAC B A 
addmsg /kinetics/Lambda-cl_gene/lambda-cl_mRNA /kinetics/Lambda-cl_gene/lambda-cl_mRNA_degradation SUBSTRATE n 
addmsg /kinetics/Lambda-cl_gene/Degraded_lambda-cl_mRNA /kinetics/Lambda-cl_gene/lambda-cl_mRNA_degradation PRODUCT n 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl /kinetics/Lambda-cl_gene/Lambda-cl_degradation SUBSTRATE n 
addmsg /kinetics/Lambda-cl_gene/Degraded_Lambda-cl /kinetics/Lambda-cl_gene/Lambda-cl_degradation PRODUCT n 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction[1] /kinetics/Lambda-cl_gene/Lambda-cl_operator_2nd_bound REAC B A 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_2nd_bound /kinetics/Lambda-cl_gene/Lambda-cl_operator_2nd_bound/Lambda-cl_transcription1 ENZYME n 
addmsg /kinetics/Lambda-cl_gene/Nucleotides /kinetics/Lambda-cl_gene/Lambda-cl_operator_2nd_bound/Lambda-cl_transcription1 SUBSTRATE n 
addmsg /kinetics/Lambda-cl_gene/lambda-cl_mRNA/lambda-cl_translation /kinetics/Lambda-cl_gene/Lambda-cl MM_PRD pA 
addmsg /kinetics/Lac_gene/Lac_operator_inhibition_reaction /kinetics/Lambda-cl_gene/Lambda-cl REAC A B 
addmsg /kinetics/Lac_gene/Lac_operator_inhibition_reaction[1] /kinetics/Lambda-cl_gene/Lambda-cl REAC A B 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_degradation /kinetics/Lambda-cl_gene/Lambda-cl REAC A B 
addmsg /kinetics/Lac_gene/Lac_operator/Lac_basal_transcription /kinetics/Lac_gene/lac_mRNA MM_PRD pA 
addmsg /kinetics/Lac_gene/Lac_operator_2nd_bound/Lac_transcription1 /kinetics/Lac_gene/lac_mRNA MM_PRD pA 
addmsg /kinetics/Lac_gene/Lac_operator_1st_bound/Lac_transcription /kinetics/Lac_gene/lac_mRNA MM_PRD pA 
addmsg /kinetics/Lac_gene/lac_mRNA_degradation /kinetics/Lac_gene/lac_mRNA REAC A B 
addmsg /kinetics/Lac_gene/lac_mRNA /kinetics/Lac_gene/lac_mRNA/lac_translation ENZYME n 
addmsg /kinetics/Lac_gene/Amino_Acids /kinetics/Lac_gene/lac_mRNA/lac_translation SUBSTRATE n 
addmsg /kinetics/Lac_gene/Lac_operator/Lac_basal_transcription /kinetics/Lac_gene/Nucleotides REAC sA B 
addmsg /kinetics/Lac_gene/Lac_operator_2nd_bound/Lac_transcription1 /kinetics/Lac_gene/Nucleotides REAC sA B 
addmsg /kinetics/Lac_gene/Lac_operator_1st_bound/Lac_transcription /kinetics/Lac_gene/Nucleotides REAC sA B 
addmsg /kinetics/Lac_gene/lac_mRNA /kinetics/Lac_gene/lac_mRNA_degradation SUBSTRATE n 
addmsg /kinetics/Lac_gene/Degraded_lac_mRNA /kinetics/Lac_gene/lac_mRNA_degradation PRODUCT n 
addmsg /kinetics/Lac_gene/lac_mRNA_degradation /kinetics/Lac_gene/Degraded_lac_mRNA REAC B A 
addmsg /kinetics/Lac_gene/Lac_operator_inhibition_reaction /kinetics/Lac_gene/Lac_operator REAC A B 
addmsg /kinetics/Lac_gene/Lac_operator /kinetics/Lac_gene/Lac_operator/Lac_basal_transcription ENZYME n 
addmsg /kinetics/Lac_gene/Nucleotides /kinetics/Lac_gene/Lac_operator/Lac_basal_transcription SUBSTRATE n 
addmsg /kinetics/Lac_gene/Lac_operator_inhibition_reaction /kinetics/Lac_gene/Lac_operator_1st_bound REAC B A 
addmsg /kinetics/Lac_gene/Lac_operator_inhibition_reaction[1] /kinetics/Lac_gene/Lac_operator_1st_bound REAC A B 
addmsg /kinetics/Lac_gene/Lac_operator_1st_bound /kinetics/Lac_gene/Lac_operator_1st_bound/Lac_transcription ENZYME n 
addmsg /kinetics/Lac_gene/Nucleotides /kinetics/Lac_gene/Lac_operator_1st_bound/Lac_transcription SUBSTRATE n 
addmsg /kinetics/Lac_gene/Lac_operator_inhibition_reaction[1] /kinetics/Lac_gene/Lac_operator_2nd_bound REAC B A 
addmsg /kinetics/Lac_gene/Lac_operator_2nd_bound /kinetics/Lac_gene/Lac_operator_2nd_bound/Lac_transcription1 ENZYME n 
addmsg /kinetics/Lac_gene/Nucleotides /kinetics/Lac_gene/Lac_operator_2nd_bound/Lac_transcription1 SUBSTRATE n 
addmsg /kinetics/Lac_gene/Degraded_lac /kinetics/Lac_gene/Lac_degradation PRODUCT n 
addmsg /kinetics/Lac_gene/Lac /kinetics/Lac_gene/Lac_degradation SUBSTRATE n 
addmsg /kinetics/Lac_gene/Lac_degradation /kinetics/Lac_gene/Degraded_lac REAC B A 
addmsg /kinetics/Lac_gene/lac_mRNA/lac_translation /kinetics/Lac_gene/Amino_Acids REAC sA B 
addmsg /kinetics/Lac_gene/Lac_operator /kinetics/Lac_gene/Lac_operator_inhibition_reaction SUBSTRATE n 
addmsg /kinetics/Lac_gene/Lac_operator_1st_bound /kinetics/Lac_gene/Lac_operator_inhibition_reaction PRODUCT n 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl /kinetics/Lac_gene/Lac_operator_inhibition_reaction SUBSTRATE n 
addmsg /kinetics/Lac_gene/Lac_operator_1st_bound /kinetics/Lac_gene/Lac_operator_inhibition_reaction[1] SUBSTRATE n 
addmsg /kinetics/Lac_gene/Lac_operator_2nd_bound /kinetics/Lac_gene/Lac_operator_inhibition_reaction[1] PRODUCT n 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl /kinetics/Lac_gene/Lac_operator_inhibition_reaction[1] SUBSTRATE n 
addmsg /kinetics/Lac_gene/Lac_degradation /kinetics/Lac_gene/Lac REAC A B 
addmsg /kinetics/Lac_gene/lac_mRNA/lac_translation /kinetics/Lac_gene/Lac MM_PRD pA 
addmsg /kinetics/TetR_gene/TetR_operator_inhibition_reaction /kinetics/Lac_gene/Lac REAC A B 
addmsg /kinetics/TetR_gene/TetR_operator_inhibition_reaction[1] /kinetics/Lac_gene/Lac REAC A B 
addmsg /kinetics/TetR_gene/TetR /kinetics/TetR_gene/TetR_degradation SUBSTRATE n 
addmsg /kinetics/TetR_gene/Degraded_TetR /kinetics/TetR_gene/TetR_degradation PRODUCT n 
addmsg /kinetics/TetR_gene/TetR_operator_inhibition_reaction /kinetics/TetR_gene/TetR_operator REAC A B 
addmsg /kinetics/TetR_gene/TetR_operator /kinetics/TetR_gene/TetR_operator/TetR_basal_transcription ENZYME n 
addmsg /kinetics/TetR_gene/Nucleotides /kinetics/TetR_gene/TetR_operator/TetR_basal_transcription SUBSTRATE n 
addmsg /kinetics/TetR_gene/TetR_operator/TetR_basal_transcription /kinetics/TetR_gene/Nucleotides REAC sA B 
addmsg /kinetics/TetR_gene/TetR_operator_1st_bound/TetR_transcription /kinetics/TetR_gene/Nucleotides REAC sA B 
addmsg /kinetics/TetR_gene/TetR_operator_2nd_bound/TetR_transcription1 /kinetics/TetR_gene/Nucleotides REAC sA B 
addmsg /kinetics/TetR_gene/TetR_operator_inhibition_reaction[1] /kinetics/TetR_gene/TetR_operator_1st_bound REAC A B 
addmsg /kinetics/TetR_gene/TetR_operator_inhibition_reaction /kinetics/TetR_gene/TetR_operator_1st_bound REAC B A 
addmsg /kinetics/TetR_gene/TetR_operator_1st_bound /kinetics/TetR_gene/TetR_operator_1st_bound/TetR_transcription ENZYME n 
addmsg /kinetics/TetR_gene/Nucleotides /kinetics/TetR_gene/TetR_operator_1st_bound/TetR_transcription SUBSTRATE n 
addmsg /kinetics/TetR_gene/TetR_operator_inhibition_reaction[1] /kinetics/TetR_gene/TetR_operator_2nd_bound REAC B A 
addmsg /kinetics/TetR_gene/TetR_operator_2nd_bound /kinetics/TetR_gene/TetR_operator_2nd_bound/TetR_transcription1 ENZYME n 
addmsg /kinetics/TetR_gene/Nucleotides /kinetics/TetR_gene/TetR_operator_2nd_bound/TetR_transcription1 SUBSTRATE n 
addmsg /kinetics/TetR_gene/tetR_mRNA/tetR_translation /kinetics/TetR_gene/Amino_Acids REAC sA B 
addmsg /kinetics/TetR_gene/TetR_operator_2nd_bound/TetR_transcription1 /kinetics/TetR_gene/tetR_mRNA MM_PRD pA 
addmsg /kinetics/TetR_gene/TetR_operator_1st_bound/TetR_transcription /kinetics/TetR_gene/tetR_mRNA MM_PRD pA 
addmsg /kinetics/TetR_gene/TetR_operator/TetR_basal_transcription /kinetics/TetR_gene/tetR_mRNA MM_PRD pA 
addmsg /kinetics/TetR_gene/tetR_mRNA_degradation /kinetics/TetR_gene/tetR_mRNA REAC A B 
addmsg /kinetics/TetR_gene/tetR_mRNA /kinetics/TetR_gene/tetR_mRNA/tetR_translation ENZYME n 
addmsg /kinetics/TetR_gene/Amino_Acids /kinetics/TetR_gene/tetR_mRNA/tetR_translation SUBSTRATE n 
addmsg /kinetics/TetR_gene/tetR_mRNA /kinetics/TetR_gene/tetR_mRNA_degradation SUBSTRATE n 
addmsg /kinetics/TetR_gene/Degraded_tetR_mRNA /kinetics/TetR_gene/tetR_mRNA_degradation PRODUCT n 
addmsg /kinetics/TetR_gene/tetR_mRNA_degradation /kinetics/TetR_gene/Degraded_tetR_mRNA REAC B A 
addmsg /kinetics/TetR_gene/TetR_degradation /kinetics/TetR_gene/Degraded_TetR REAC B A 
addmsg /kinetics/TetR_gene/TetR_operator_1st_bound /kinetics/TetR_gene/TetR_operator_inhibition_reaction PRODUCT n 
addmsg /kinetics/TetR_gene/TetR_operator /kinetics/TetR_gene/TetR_operator_inhibition_reaction SUBSTRATE n 
addmsg /kinetics/Lac_gene/Lac /kinetics/TetR_gene/TetR_operator_inhibition_reaction SUBSTRATE n 
addmsg /kinetics/TetR_gene/TetR_operator_2nd_bound /kinetics/TetR_gene/TetR_operator_inhibition_reaction[1] PRODUCT n 
addmsg /kinetics/TetR_gene/TetR_operator_1st_bound /kinetics/TetR_gene/TetR_operator_inhibition_reaction[1] SUBSTRATE n 
addmsg /kinetics/Lac_gene/Lac /kinetics/TetR_gene/TetR_operator_inhibition_reaction[1] SUBSTRATE n 
addmsg /kinetics/TetR_gene/tetR_mRNA/tetR_translation /kinetics/TetR_gene/TetR MM_PRD pA 
addmsg /kinetics/TetR_gene/TetR_degradation /kinetics/TetR_gene/TetR REAC A B 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction[1] /kinetics/TetR_gene/TetR REAC A B 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction /kinetics/TetR_gene/TetR REAC A B 
addmsg /kinetics/TetR_gene/TetR /graphs/conc1/TetR.Co PLOT Co *TetR.Co *16 
addmsg /kinetics/Lambda-cl_gene/Lambda-cl /graphs/conc1/Lambda-cl.Co PLOT Co *Lambda-cl.Co *27 
addmsg /kinetics/Lac_gene/Lac /graphs/conc1/Lac.Co PLOT Co *Lac.Co *1 
enddump
// End of dump

call /kinetics/Lambda-cl_gene/notes LOAD \
"This is a group of reactions resulting in the production" \
"of Lambda cl"
call /kinetics/Lambda-cl_gene/lambda-cl_mRNA/notes LOAD \
"The m-RNA transcribed by the Lcl operator" \
"" \
"Elowitz etal Nature 2000 Vol 403:335-338"
call /kinetics/Lambda-cl_gene/lambda-cl_mRNA/lambda-cl_translation/notes LOAD \
"Translation the lambda cl mRNA." \
"" \
"Rates from Box 1 " \
"Elowitz, M. et al (2000)" \
"Nature 403:335-338"
call /kinetics/Lambda-cl_gene/Degraded_lambda-cl_mRNA/notes LOAD \
"Degradation product of lambda cl m-RNA"
call /kinetics/Lambda-cl_gene/Lambda-cl_operator/notes LOAD \
"The lambda cl operator under normal conditions" \
"drives transcription of lambda cl" \
"" \
"Elowitz, M. et al. (2000) " \
"Nature 403:335-338"
call /kinetics/Lambda-cl_gene/Lambda-cl_operator/Lambda-cl_basal_transcription/notes LOAD \
"Basal transcription of the Lambda cl gene." \
"" \
"Rates from Box 1 " \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction/notes LOAD \
"It signifies the first binding of the repressor" \
"with the operator" \
"" \
"Kf = 1 /nM/sec" \
"Kb = 224 /sec" \
"" \
"Box 1 " \
"Elowitz, M. et al. (2000) " \
"Nature 403:335-338"
call /kinetics/Lambda-cl_gene/Lambda-cl_operator_inhibition_reaction[1]/notes LOAD \
"It signifies the second binding of the repressor" \
"with the operator" \
"" \
"Kf = 1 /nM/sec" \
"Kb = 9 /sec" \
"" \
"Box 1 " \
"Elowitz, M. et al. (2000) " \
"Nature 403:335-338"
call /kinetics/Lambda-cl_gene/Lambda-cl_operator_1st_bound/notes LOAD \
"This represents the promoter " \
"with one of its sites bound by the inhibitor." \
"" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/Lambda-cl_gene/Lambda-cl_operator_1st_bound/Lambda-cl_transcription/notes LOAD \
"Transcription of the Lambda cl gene " \
"with only one of its promoter sites activated." \
"" \
"Rates from Box 1 " \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/Lambda-cl_gene/Degraded_Lambda-cl/notes LOAD \
"Degradation product of Lambda-cl protein"
call /kinetics/Lambda-cl_gene/lambda-cl_mRNA_degradation/notes LOAD \
"This reaction denotes the degradation of the lambda cl m-RNA," \
"the half life of which is two minutes." \
"" \
"t1/2 = 0.693/k where k denotes kf"
call /kinetics/Lambda-cl_gene/Lambda-cl_degradation/notes LOAD \
"This reaction denotes the degradation of Lambda cl" \
"the half life of which is ten minutes." \
"t1/2 = 0.693/k where k denotes kf" \
"" \
"Note: Rates had to scaled by approx 1.4 to get desired levels" \
"" \
"Elowitz, M. et al. (2000) " \
"Nature 403:335-338"
call /kinetics/Lambda-cl_gene/Lambda-cl_operator_2nd_bound/notes LOAD \
"This represents the promoter " \
"with both of its sites bound by the inhibitor." \
"" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/Lambda-cl_gene/Lambda-cl_operator_2nd_bound/Lambda-cl_transcription1/notes LOAD \
"Transcription of the Lambda cl gene" \
"with both its promoter sites activated." \
"" \
"Rates from Box 1" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/Lambda-cl_gene/Lambda-cl/notes LOAD \
"Lambda-cl translated from lcl m-RNA" \
"Derived from Figure 1(c) stochastic version" \
"" \
"Elowitz, M. et al. (2000) " \
"Nature 403:335-338"
call /kinetics/Lac_gene/notes LOAD \
"This is a group of reactions resulting in the production" \
"of Lac." \
"" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/Lac_gene/lac_mRNA/notes LOAD \
"The m-RNA transcribed by the lac operator" \
"" \
"Elowitz, M. et al. (2000) " \
"Nature 403:335-338"
call /kinetics/Lac_gene/lac_mRNA/lac_translation/notes LOAD \
"Translation of the lac mRNA." \
"" \
"Rates from Box 1 " \
"Elowitz, M. et al (2000)" \
"Nature 403: 335-338"
call /kinetics/Lac_gene/lac_mRNA_degradation/notes LOAD \
"This reaction denotes the degradation of the lac m-RNA," \
"the half life of which is two minutes." \
"t1/2 = 0.693/k where k denotes kf" \
"" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/Lac_gene/Degraded_lac_mRNA/notes LOAD \
"Degradation product of lac m-RNA"
call /kinetics/Lac_gene/Lac_operator/notes LOAD \
"The Lac operator under normal conditions" \
"drives transcription of Lac" \
"" \
"Elowitz, M. et al. (2000) " \
"Nature 403:335-338"
call /kinetics/Lac_gene/Lac_operator/Lac_basal_transcription/notes LOAD \
"Basal transcription of the Lac gene." \
"" \
"Rates from Box 1 " \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/Lac_gene/Lac_operator_1st_bound/notes LOAD \
"This represents the promoter " \
"with one of its sites bound by the inhibitor." \
"" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/Lac_gene/Lac_operator_1st_bound/Lac_transcription/notes LOAD \
"Transcription of the lac gene " \
"with only one of its promoter sites activated." \
"" \
"Rates from Box 1 " \
"Elowitz, M. et al. (2000) N" \
"Nature 403: 335-338"
call /kinetics/Lac_gene/Lac_operator_2nd_bound/notes LOAD \
"This represents the promoter " \
"with both of its sites bound by the inhibitor." \
"" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/Lac_gene/Lac_operator_2nd_bound/Lac_transcription1/notes LOAD \
"Transcription of the Lac gene " \
"with both its promoter sites activated." \
"" \
"Rates from Box 1 " \
"Elowitz, M. et al. (2000) " \
"Nature 403: 335-338"
call /kinetics/Lac_gene/Lac_degradation/notes LOAD \
"This reaction denotes the degradation of Lac" \
"the half life of which is ten minutes." \
"t1/2 = 0.693/k where k denotes kf" \
"" \
"Note: Rates had to scaled by approx 1.4 to get desired levels" \
"" \
"Elowitz, M. et al. (2000) " \
"Nature 403:335-338"
call /kinetics/Lac_gene/Degraded_lac/notes LOAD \
"Degradation product of Lac protein"
call /kinetics/Lac_gene/Lac_operator_inhibition_reaction/notes LOAD \
"It signifies the first binding of the repressor" \
"with the operator" \
"" \
"Kf = 1 /nM/sec" \
"Kb = 224 /sec" \
"" \
"Box 1 " \
"Elowitz, M. et al. (2000) " \
"Nature 403:335-338"
call /kinetics/Lac_gene/Lac_operator_inhibition_reaction[1]/notes LOAD \
"It signifies the second binding of the repressor" \
"with the operator" \
"" \
"Kf = 1 /nM/sec" \
"Kb = 9 /sec" \
"" \
"Box 1 " \
"Elowitz, M. et al (2000)" \
"Nature 403:335-338"
call /kinetics/Lac_gene/Lac/notes LOAD \
"Lac translated from lac m-RNA" \
"Derived from Figure 1(c) stochastic version" \
"" \
"Elowitz, M. et al. (2000) " \
"Nature 403:335-338"
call /kinetics/TetR_gene/notes LOAD \
"This is a group of reactions resulting in the production" \
"of Tet R" \
"" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/TetR_gene/TetR_degradation/notes LOAD \
"This reaction denotes the degradation of Tet R" \
"the half life of which is ten minutes." \
"t1/2 = 0.693/k where k denotes kf" \
"" \
"Note: Rates had to be scaled by 1.4 to get appropriate levels" \
"" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/TetR_gene/TetR_operator/notes LOAD \
"The TetR operator under normal conditions" \
"drives transcription of TetR" \
"" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/TetR_gene/TetR_operator/TetR_basal_transcription/notes LOAD \
"Basal transcription of the Tet R gene." \
"" \
"Rates from Box 1 " \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/TetR_gene/TetR_operator_1st_bound/notes LOAD \
"This represents the promoter " \
"with one of its sites bound by the inhibitor" \
"" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/TetR_gene/TetR_operator_1st_bound/TetR_transcription/notes LOAD \
"Transcription of the TetR gene " \
"with only one of its promoter sites activated." \
"" \
"Rates from Box 1 " \
"Elowitz, M. et al (2000)" \
"Nature 403:335-338"
call /kinetics/TetR_gene/TetR_operator_2nd_bound/notes LOAD \
"This represents the promoter " \
"with both of its sites bound by the inhibitor." \
"" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/TetR_gene/TetR_operator_2nd_bound/TetR_transcription1/notes LOAD \
"Transcription of the TetR gene " \
"with both its promoter sites activated." \
"" \
"Rates from Box 1 " \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/TetR_gene/tetR_mRNA/notes LOAD \
"The mRNA transcribed by the TetR operator" \
"" \
"Elowitz, M, et al. (2000) " \
"Nature 403:335-338"
call /kinetics/TetR_gene/tetR_mRNA/tetR_translation/notes LOAD \
"Translation of the tetR mRNA." \
"" \
"Rates from Box 1 " \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/TetR_gene/tetR_mRNA_degradation/notes LOAD \
"This reaction denotes the degradation of the tetR m-RNA," \
"the half life of which is two minutes." \
"t1/2 = 0.693/k where k denotes kf" \
"" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/TetR_gene/Degraded_tetR_mRNA/notes LOAD \
"Degradation product of tetR m-RNA"
call /kinetics/TetR_gene/Degraded_TetR/notes LOAD \
"Degradation product of TetR protein"
call /kinetics/TetR_gene/TetR_operator_inhibition_reaction/notes LOAD \
"It signifies the first binding of the repressor " \
"with the operator" \
"" \
"Kf = 1 /nM/sec" \
"Kb = 224 /sec" \
"" \
"Box 1 " \
"Elowitz, M. et al. (2000) " \
"Nature 403:335-338"
call /kinetics/TetR_gene/TetR_operator_inhibition_reaction[1]/notes LOAD \
"It signifies the second binding of the repressor" \
"with the operator" \
"" \
"Kf = 1 /nM/sec" \
"Kb = 9 /sec" \
"" \
"Box 1 " \
"Elowitz, M. et al. (2000) " \
"Nature 403:335-338"
call /kinetics/TetR_gene/TetR/notes LOAD \
"TetR translated from tetR m-RNA" \
"Derived from Figure 1(c) stochastic version" \
"" \
"Elowitz, M. et al. (2000)" \
"Nature 403:335-338"
call /kinetics/doqcsinfo/notes LOAD \
"This is a network model termed as Repressilator in which LacI inhibit transcription of tetR which in turn inhibit transcription of Lambda phage. This model simulates the result as per the Fig 1c of the paper : <a href = http://www.ncbi.nlm.nih.gov/entrez/query.fcgi?cmd=Retrieve&db=pubmed&dopt=Abstract&list_uids=10659856&query_hl=1&itool=pubmed_docsum>Elowitz MB, Leibler S., Nature. 2000 Jan 20;403(6767):335-8.</a>"
complete_loading
