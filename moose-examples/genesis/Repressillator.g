//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Sun May 18 16:56:17 2014
 
include kkit {argv 1}
 
FASTDT = 5e-05
SIMDT = 0.01
CONTROLDT = 10
PLOTDT = 10
MAXTIME = 10000
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
simobjdump text str
simundump geometry /kinetics/geometry 0 3e-18 3 sphere "" white black 0 0 0
simundump text /kinetics/notes 0 ""
call /kinetics/notes LOAD \
""
simundump group /kinetics/lac_gene 0 yellow black x 0 0 "" lac_gene \
  /home2/bhalla/scripts/modules/lac_gene_0.g 0 0 0 27 0 0
simundump text /kinetics/lac_gene/notes 0 \
  "This is a group of reactions resulting in the production\nof Lac"
call /kinetics/lac_gene/notes LOAD \
"This is a group of reactions resulting in the production" \
"of Lac"
simundump kpool /kinetics/lac_gene/l_op_2nd_bound[1] 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 52 yellow 26 -12 0
simundump text /kinetics/lac_gene/l_op_2nd_bound[1]/notes 0 \
  "it represents the promotor with both of its sites\n bound by the inhibitor"
call /kinetics/lac_gene/l_op_2nd_bound[1]/notes LOAD \
"it represents the promotor with both of its sites" \
" bound by the inhibitor"
simundump kenz /kinetics/lac_gene/l_op_2nd_bound[1]/lac_transcription1 0 0 0 \
  0 0 1800 0.0024998 0.002 0.0005 0 1 "" red 52 "" 26 -11 0
simundump text /kinetics/lac_gene/l_op_2nd_bound[1]/lac_transcription1/notes 0 \
  "Rates from\nBox 1 Elowitz et al Nature (2000)\n403: 335-338"
call /kinetics/lac_gene/l_op_2nd_bound[1]/lac_transcription1/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al Nature (2000)" \
"403: 335-338"
simundump kreac /kinetics/lac_gene/inhib_reac[1] 0 0.0055556 0.09 "" white \
  yellow 30 -13 0
simundump text /kinetics/lac_gene/inhib_reac[1]/notes 0 \
  "It signifies the first binding of the repressor\n with the operator\n\nKf=1 /nM/sec\nKb=9 /sec\n\nBox 1 Elowitz et al Nature 2000 vol 403:335-338"
call /kinetics/lac_gene/inhib_reac[1]/notes LOAD \
"It signifies the first binding of the repressor" \
" with the operator" \
"" \
"Kf=1 /nM/sec" \
"Kb=9 /sec" \
"" \
"Box 1 Elowitz et al Nature 2000 vol 403:335-338"
simundump kpool /kinetics/lac_gene/l_op_1rst_bound 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 2 yellow 33 -12 0
simundump text /kinetics/lac_gene/l_op_1rst_bound/notes 0 \
  "it represents the promotor with one of its sites\n bound by the inhibitor"
call /kinetics/lac_gene/l_op_1rst_bound/notes LOAD \
"it represents the promotor with one of its sites" \
" bound by the inhibitor"
simundump kenz /kinetics/lac_gene/l_op_1rst_bound/lac_transcription 0 0 0 0 0 \
  1800 0.0024998 0.002 0.0005 0 1 "" red 2 "" 33 -10 0
simundump text /kinetics/lac_gene/l_op_1rst_bound/lac_transcription/notes 0 \
  "Rates from\nBox 1 Elowitz et al Nature 2000\n403: 335-338"
call /kinetics/lac_gene/l_op_1rst_bound/lac_transcription/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al Nature 2000" \
"403: 335-338"
simundump kreac /kinetics/lac_gene/inhib_reac 0 0.0055556 2.24 "" white \
  yellow 36 -13 0
simundump text /kinetics/lac_gene/inhib_reac/notes 0 \
  "It signifies the first binding of the repressor\n with the operator\n\nKf = 1 /nM/sec\nKb = 224 /sec\n\nBox 1 Elowitz et al Nature 2000 vol 403:335-338"
call /kinetics/lac_gene/inhib_reac/notes LOAD \
"It signifies the first binding of the repressor" \
" with the operator" \
"" \
"Kf = 1 /nM/sec" \
"Kb = 224 /sec" \
"" \
"Box 1 Elowitz et al Nature 2000 vol 403:335-338"
simundump kpool /kinetics/lac_gene/lac_operator 0 0 0.022222 0.022222 40 40 0 \
  0 1800 0 /kinetics/geometry 36 yellow 40 -12 0
simundump text /kinetics/lac_gene/lac_operator/notes 0 \
  "The lac operator under normal conditions\n drives transcription of lac\n\nElowitz et al Nature 2000 Vol 403:335-338"
call /kinetics/lac_gene/lac_operator/notes LOAD \
"The lac operator under normal conditions" \
" drives transcription of lac" \
"" \
"Elowitz et al Nature 2000 Vol 403:335-338"
simundump kenz /kinetics/lac_gene/lac_operator/lac_transcription2 0 0 0 0 0 \
  1800 2.4998 2 0.5 0 1 "" red 36 "" 40 -11 0
simundump text /kinetics/lac_gene/lac_operator/lac_transcription2/notes 0 \
  "Rates from\nBox 1 Elowitz et al (2000)\n403:335-338"
call /kinetics/lac_gene/lac_operator/lac_transcription2/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al (2000)" \
"403:335-338"
simundump kpool /kinetics/lac_gene/lac_mRNA 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 12 yellow 26 -8 0
simundump text /kinetics/lac_gene/lac_mRNA/notes 0 \
  "The m-RNA transcribed by the lac operator\n\nElowitz et al Nature 2000 Vol 403:335-338"
call /kinetics/lac_gene/lac_mRNA/notes LOAD \
"The m-RNA transcribed by the lac operator" \
"" \
"Elowitz et al Nature 2000 Vol 403:335-338"
simundump kenz /kinetics/lac_gene/lac_mRNA/lac_translation 0 0 0 0 0 1800 \
  0.83494 0.668 0.167 0 1 "" red 12 "" 20 -10 0
simundump text /kinetics/lac_gene/lac_mRNA/lac_translation/notes 0 \
  "Rates from \nBox 1 Elowitz et al Nature (2000)\n403: 335-338"
call /kinetics/lac_gene/lac_mRNA/lac_translation/notes LOAD \
"Rates from " \
"Box 1 Elowitz et al Nature (2000)" \
"403: 335-338"
simundump kpool /kinetics/lac_gene/lac 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 62 yellow 20 -15 0
simundump text /kinetics/lac_gene/lac/notes 0 \
  "Lac translated from lac m-RNA\nDerived from Figure 1(c) stochastic version\n\nElowitz et al Nature 2000 Vol 403:335-338"
call /kinetics/lac_gene/lac/notes LOAD \
"Lac translated from lac m-RNA" \
"Derived from Figure 1(c) stochastic version" \
"" \
"Elowitz et al Nature 2000 Vol 403:335-338"
simundump kpool /kinetics/lac_gene/degraded_lac 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 2 yellow 20 -19 0
simundump text /kinetics/lac_gene/degraded_lac/notes 0 \
  "Degradation product of lac protein"
call /kinetics/lac_gene/degraded_lac/notes LOAD \
"Degradation product of lac protein"
simundump kpool /kinetics/lac_gene/degraded_lac_mRNA 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 1 yellow 33 -8 0
simundump text /kinetics/lac_gene/degraded_lac_mRNA/notes 0 \
  "Degradation product of lac m-RNA"
call /kinetics/lac_gene/degraded_lac_mRNA/notes LOAD \
"Degradation product of lac m-RNA"
simundump kreac /kinetics/lac_gene/lac_mRNA_degradation 0 0.005775 0 "" white \
  yellow 29 -7 0
simundump text /kinetics/lac_gene/lac_mRNA_degradation/notes 0 \
  "This reaction denotes the degradation of the Lac m-RNA,\n the half life of which is two minutes.\nt1/2=.693/k where k denotes kf"
call /kinetics/lac_gene/lac_mRNA_degradation/notes LOAD \
"This reaction denotes the degradation of the Lac m-RNA," \
" the half life of which is two minutes." \
"t1/2=.693/k where k denotes kf"
simundump kreac /kinetics/lac_gene/lac_degradation 0 0.016666 0 "" white \
  yellow 20 -17 0
simundump text /kinetics/lac_gene/lac_degradation/notes 0 \
  "This reaction denotes the degradation of the Lac\n the half life of which is ten minutes.\nt1/2=.693/k where k denotes kf\n\nNote: Rates had to scaled by approx 1.4 to get desired levels\n\nElowitz et al Nature 2000. Vol403 335-338"
call /kinetics/lac_gene/lac_degradation/notes LOAD \
"This reaction denotes the degradation of the Lac" \
" the half life of which is ten minutes." \
"t1/2=.693/k where k denotes kf" \
"" \
"Note: Rates had to scaled by approx 1.4 to get desired levels" \
"" \
"Elowitz et al Nature 2000. Vol403 335-338"
simundump kpool /kinetics/lac_gene/AminoAcids 0 0 0.00055555 0.00055555 \
  0.99999 0.99999 0 0 1800 4 /kinetics/geometry 52 yellow 20 -8 0
simundump text /kinetics/lac_gene/AminoAcids/notes 0 ""
call /kinetics/lac_gene/AminoAcids/notes LOAD \
""
simundump kpool /kinetics/lac_gene/Nucleotides 0 0 0.00055555 0.00055555 \
  0.99999 0.99999 0 0 1800 4 /kinetics/geometry 6 yellow 40 -8 0
simundump text /kinetics/lac_gene/Nucleotides/notes 0 ""
call /kinetics/lac_gene/Nucleotides/notes LOAD \
""
simundump group /kinetics/TetR_gene 0 61 black x 0 0 "" TetR_gene \
  /home2/bhalla/scripts/modules/TetR_gene_0.g 0 0 0 -2 0 0
simundump text /kinetics/TetR_gene/notes 0 \
  "This is a group of reactions resulting in the production\nof Tet R"
call /kinetics/TetR_gene/notes LOAD \
"This is a group of reactions resulting in the production" \
"of Tet R"
simundump kpool /kinetics/TetR_gene/l_op_2nd_bound[1] 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 46 61 -3 -12 0
simundump text /kinetics/TetR_gene/l_op_2nd_bound[1]/notes 0 \
  "it represents the promotor with both of its sites\n bound by the inhibitor"
call /kinetics/TetR_gene/l_op_2nd_bound[1]/notes LOAD \
"it represents the promotor with both of its sites" \
" bound by the inhibitor"
simundump kenz /kinetics/TetR_gene/l_op_2nd_bound[1]/TetR_transcription1 0 0 \
  0 0 0 1800 0.0024998 0.002 0.0005 0 1 "" red 46 "" -3 -11 0
simundump text /kinetics/TetR_gene/l_op_2nd_bound[1]/TetR_transcription1/notes 0 \
  "Rates from \nBox 1 Elowitz et al (2000)\nNature 403:335-338"
call /kinetics/TetR_gene/l_op_2nd_bound[1]/TetR_transcription1/notes LOAD \
"Rates from " \
"Box 1 Elowitz et al (2000)" \
"Nature 403:335-338"
simundump kreac /kinetics/TetR_gene/inhib_reac[1] 0 0.0055556 0.09 "" white \
  61 1 -13 0
simundump text /kinetics/TetR_gene/inhib_reac[1]/notes 0 \
  "It signifies the first binding of the repressor\n with the operator\n\nKf = 1 /nM/sec\nKb = 9 /sec\n\nBox 1 Elowitz et al Nature 2000 vol 403:335-338"
call /kinetics/TetR_gene/inhib_reac[1]/notes LOAD \
"It signifies the first binding of the repressor" \
" with the operator" \
"" \
"Kf = 1 /nM/sec" \
"Kb = 9 /sec" \
"" \
"Box 1 Elowitz et al Nature 2000 vol 403:335-338"
simundump kpool /kinetics/TetR_gene/l_op_1rst_bound 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 2 61 4 -12 0
simundump text /kinetics/TetR_gene/l_op_1rst_bound/notes 0 \
  "it represents the promotor with one of its sites\n bound by the inhibitor"
call /kinetics/TetR_gene/l_op_1rst_bound/notes LOAD \
"it represents the promotor with one of its sites" \
" bound by the inhibitor"
simundump kenz /kinetics/TetR_gene/l_op_1rst_bound/TetR_transcription 0 0 0 0 \
  0 1800 0.0024998 0.002 0.0005 0 1 "" red 2 "" 4 -10 0
simundump text /kinetics/TetR_gene/l_op_1rst_bound/TetR_transcription/notes 0 \
  "Rates from \nBox 1 Elowitz et al (2000)\nNature 403:335-338"
call /kinetics/TetR_gene/l_op_1rst_bound/TetR_transcription/notes LOAD \
"Rates from " \
"Box 1 Elowitz et al (2000)" \
"Nature 403:335-338"
simundump kreac /kinetics/TetR_gene/inhib_reac 0 0.0055556 2.24 "" white 61 7 \
  -13 0
simundump text /kinetics/TetR_gene/inhib_reac/notes 0 \
  "It signifies the first binding of the repressor\n with the operator\n\nKf=1 /nM/sec\nKb=224 /sec\n\nBox1 Elowitz etal Nature 2000 vol 403:335-338"
call /kinetics/TetR_gene/inhib_reac/notes LOAD \
"It signifies the first binding of the repressor" \
" with the operator" \
"" \
"Kf=1 /nM/sec" \
"Kb=224 /sec" \
"" \
"Box1 Elowitz etal Nature 2000 vol 403:335-338"
simundump kpool /kinetics/TetR_gene/TetR 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 11 61 -9 -15 0
simundump text /kinetics/TetR_gene/TetR/notes 0 \
  "Lac translated from lac m-RNA\nDerived from Figure 1(c) stochastic version\n\nElowitz etal Nature 2000 Vol 403:335-338"
call /kinetics/TetR_gene/TetR/notes LOAD \
"Lac translated from lac m-RNA" \
"Derived from Figure 1(c) stochastic version" \
"" \
"Elowitz etal Nature 2000 Vol 403:335-338"
simundump kpool /kinetics/TetR_gene/degraded_TetR 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 2 61 -9 -19 0
simundump text /kinetics/TetR_gene/degraded_TetR/notes 0 \
  "Degradation product of lac protein"
call /kinetics/TetR_gene/degraded_TetR/notes LOAD \
"Degradation product of lac protein"
simundump kreac /kinetics/TetR_gene/TetR_degradation 0 0.01666 0 "" white 61 \
  -9 -17 0
simundump text /kinetics/TetR_gene/TetR_degradation/notes 0 \
  "This reaction denotes the degradation of the Lac\n the half life of which is ten minutes.\nt1/2=.693/k where k denotes kf\n\nNote: Rates had to be scaled by 1.4 to get appropriate levels\n\nElowitz etal Nature 2000. Vol403 335-338"
call /kinetics/TetR_gene/TetR_degradation/notes LOAD \
"This reaction denotes the degradation of the Lac" \
" the half life of which is ten minutes." \
"t1/2=.693/k where k denotes kf" \
"" \
"Note: Rates had to be scaled by 1.4 to get appropriate levels" \
"" \
"Elowitz etal Nature 2000. Vol403 335-338"
simundump kpool /kinetics/TetR_gene/TetR_mRNA 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 12 61 -3 -8 0
simundump text /kinetics/TetR_gene/TetR_mRNA/notes 0 \
  "The m-RNA transcribed by the lac operator\n\nElowitz etal Nature 2000 Vol 403:335-338"
call /kinetics/TetR_gene/TetR_mRNA/notes LOAD \
"The m-RNA transcribed by the lac operator" \
"" \
"Elowitz etal Nature 2000 Vol 403:335-338"
simundump kenz /kinetics/TetR_gene/TetR_mRNA/TetR_translation 0 0 0 0 0 1800 \
  0.83494 0.668 0.167 0 1 "" red 12 "" -9 -10 0
simundump text /kinetics/TetR_gene/TetR_mRNA/TetR_translation/notes 0 \
  "Rates from\nBox 1 Elowitz et al (2000)\nNature 403:335-338"
call /kinetics/TetR_gene/TetR_mRNA/TetR_translation/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al (2000)" \
"Nature 403:335-338"
simundump kreac /kinetics/TetR_gene/TetR_mRNA_degradation 0 0.005775 0 "" \
  white 61 1 -7 0
simundump text /kinetics/TetR_gene/TetR_mRNA_degradation/notes 0 \
  "This reaction denotes the degradation of the Lac m-RNA,\n the half life of which is two minutes.\nt1/2=.693/k where k denotes kf"
call /kinetics/TetR_gene/TetR_mRNA_degradation/notes LOAD \
"This reaction denotes the degradation of the Lac m-RNA," \
" the half life of which is two minutes." \
"t1/2=.693/k where k denotes kf"
simundump kpool /kinetics/TetR_gene/degraded_TetR_mRNA 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 1 61 4 -8 0
simundump text /kinetics/TetR_gene/degraded_TetR_mRNA/notes 0 \
  "Degradation product of lac m-RNA"
call /kinetics/TetR_gene/degraded_TetR_mRNA/notes LOAD \
"Degradation product of lac m-RNA"
simundump kpool /kinetics/TetR_gene/TetR_operator 0 0 0.022222 0.022222 40 40 \
  0 0 1800 0 /kinetics/geometry 36 61 11 -12 0
simundump text /kinetics/TetR_gene/TetR_operator/notes 0 \
  "The lac operator under normal conditions\n drives transcription of lac\n\nElowitz etal Nature 2000 Vol 403:335-338"
call /kinetics/TetR_gene/TetR_operator/notes LOAD \
"The lac operator under normal conditions" \
" drives transcription of lac" \
"" \
"Elowitz etal Nature 2000 Vol 403:335-338"
simundump kenz /kinetics/TetR_gene/TetR_operator/TetR_transcription2 0 0 0 0 \
  0 1800 2.4998 2 0.5 0 1 "" red 36 "" 11 -11 0
simundump text /kinetics/TetR_gene/TetR_operator/TetR_transcription2/notes 0 \
  "Rates from\nBox 1 Elowitz et al (2000)\nNature 403:335-338"
call /kinetics/TetR_gene/TetR_operator/TetR_transcription2/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al (2000)" \
"Nature 403:335-338"
simundump kpool /kinetics/TetR_gene/Nucleotides 0 0 0.00055555 0.00055555 \
  0.99999 0.99999 0 0 1800 4 /kinetics/geometry blue 61 11 -8 0
simundump text /kinetics/TetR_gene/Nucleotides/notes 0 ""
call /kinetics/TetR_gene/Nucleotides/notes LOAD \
""
simundump kpool /kinetics/TetR_gene/AminoAcids 0 0 0.00055555 0.00055555 \
  0.99999 0.99999 0 0 1800 4 /kinetics/geometry blue 61 -9 -8 0
simundump text /kinetics/TetR_gene/AminoAcids/notes 0 ""
call /kinetics/TetR_gene/AminoAcids/notes LOAD \
""
simundump group /kinetics/Lcl 0 15 black x 0 0 "" yellow \
  /home2/bhalla/scripts/modules/yellow_0.g 0 0 0 56 0 0
simundump text /kinetics/Lcl/notes 0 \
  "This is a group of reactions resulting in the production\nof Lambda cl"
call /kinetics/Lcl/notes LOAD \
"This is a group of reactions resulting in the production" \
"of Lambda cl"
simundump kpool /kinetics/Lcl/l_op_2nd_bound[1] 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 52 15 55 -12 0
simundump text /kinetics/Lcl/l_op_2nd_bound[1]/notes 0 \
  "it represents the promotor with both of its sites\n bound by the inhibitor"
call /kinetics/Lcl/l_op_2nd_bound[1]/notes LOAD \
"it represents the promotor with both of its sites" \
" bound by the inhibitor"
simundump kenz /kinetics/Lcl/l_op_2nd_bound[1]/Lcl_transcription1 0 0 0 0 0 \
  1800 0.0024998 0.002 0.0005 0 1 "" red 52 "" 55 -11 0
simundump text /kinetics/Lcl/l_op_2nd_bound[1]/Lcl_transcription1/notes 0 ""
call /kinetics/Lcl/l_op_2nd_bound[1]/Lcl_transcription1/notes LOAD \
""
simundump kreac /kinetics/Lcl/inhib_reac[1] 0 0.0055556 0.09 "" white 15 59 \
  -13 0
simundump text /kinetics/Lcl/inhib_reac[1]/notes 0 \
  "It signifies the first binding of the repressor\n with the operator\n\nKf=1 /nM/sec\nKb=9 /sec\n\nBox1 Elowitz etal Nature 2000 vol 403:335-338"
call /kinetics/Lcl/inhib_reac[1]/notes LOAD \
"It signifies the first binding of the repressor" \
" with the operator" \
"" \
"Kf=1 /nM/sec" \
"Kb=9 /sec" \
"" \
"Box1 Elowitz etal Nature 2000 vol 403:335-338"
simundump kpool /kinetics/Lcl/l_op_1rst_bound 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 2 15 62 -12 0
simundump text /kinetics/Lcl/l_op_1rst_bound/notes 0 \
  "it represents the promotor with one of its sites\n bound by the inhibitor"
call /kinetics/Lcl/l_op_1rst_bound/notes LOAD \
"it represents the promotor with one of its sites" \
" bound by the inhibitor"
simundump kenz /kinetics/Lcl/l_op_1rst_bound/Lcl_transcription 0 0 0 0 0 1800 \
  0.0024998 0.002 0.0005 0 1 "" red 2 "" 62 -10 0
simundump text /kinetics/Lcl/l_op_1rst_bound/Lcl_transcription/notes 0 \
  "Rates from\nBox 1 Elowitz et al (2000)\nNature 403:335-338"
call /kinetics/Lcl/l_op_1rst_bound/Lcl_transcription/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al (2000)" \
"Nature 403:335-338"
simundump kreac /kinetics/Lcl/inhib_reac 0 0.0055556 2.24 "" white 15 65 -13 \
  0
simundump text /kinetics/Lcl/inhib_reac/notes 0 \
  "It signifies the first binding of the repressor\n with the operator\n\nKf=1 /nM/sec\nKb=224 /sec\n\nBox1 Elowitz etal Nature 2000 vol 403:335-338"
call /kinetics/Lcl/inhib_reac/notes LOAD \
"It signifies the first binding of the repressor" \
" with the operator" \
"" \
"Kf=1 /nM/sec" \
"Kb=224 /sec" \
"" \
"Box1 Elowitz etal Nature 2000 vol 403:335-338"
simundump kpool /kinetics/Lcl/AminoAcids 0 0 0.00055555 0.00055555 0.99999 \
  0.99999 0 0 1800 4 /kinetics/geometry 52 15 48 -8 0
simundump text /kinetics/Lcl/AminoAcids/notes 0 ""
call /kinetics/Lcl/AminoAcids/notes LOAD \
""
simundump kpool /kinetics/Lcl/Nucleotides 0 0 0.00055555 0.00055555 0.99999 \
  0.99999 0 0 1800 4 /kinetics/geometry 6 15 69 -8 0
simundump text /kinetics/Lcl/Nucleotides/notes 0 ""
call /kinetics/Lcl/Nucleotides/notes LOAD \
""
simundump kreac /kinetics/Lcl/Lcl_mRNA_degradation 0 0.005775 0 "" white 15 \
  58 -7 0
simundump text /kinetics/Lcl/Lcl_mRNA_degradation/notes 0 \
  "This reaction denotes the degradation of the Lac m-RNA,\n the half life of which is two minutes.\nt1/2=.693/k where k denotes kf"
call /kinetics/Lcl/Lcl_mRNA_degradation/notes LOAD \
"This reaction denotes the degradation of the Lac m-RNA," \
" the half life of which is two minutes." \
"t1/2=.693/k where k denotes kf"
simundump kpool /kinetics/Lcl/Lcl_mRNA 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 5 15 55 -8 0
simundump text /kinetics/Lcl/Lcl_mRNA/notes 0 \
  "The m-RNA transcribed by the lac operator\n\nElowitz etal Nature 2000 Vol 403:335-338"
call /kinetics/Lcl/Lcl_mRNA/notes LOAD \
"The m-RNA transcribed by the lac operator" \
"" \
"Elowitz etal Nature 2000 Vol 403:335-338"
simundump kenz /kinetics/Lcl/Lcl_mRNA/Lcl_translation 0 0 0 0 0 1800 0.83494 \
  0.668 0.167 0 1 "" red 5 "" 48 -10 0
simundump text /kinetics/Lcl/Lcl_mRNA/Lcl_translation/notes 0 \
  "Rates from\nBox 1 Elowitz et al (2003)\nNature 403:335-338"
call /kinetics/Lcl/Lcl_mRNA/Lcl_translation/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al (2003)" \
"Nature 403:335-338"
simundump kpool /kinetics/Lcl/Lcl 0 0 0.055555 0.055555 99.999 99.999 0 0 \
  1800 0 /kinetics/geometry 24 15 48 -15 0
simundump text /kinetics/Lcl/Lcl/notes 0 \
  "Lac translated from lac m-RNA\nDerived from Figure 1(c) stochastic version\n\nElowitz et al Nature 2000 Vol 403:335-338"
call /kinetics/Lcl/Lcl/notes LOAD \
"Lac translated from lac m-RNA" \
"Derived from Figure 1(c) stochastic version" \
"" \
"Elowitz et al Nature 2000 Vol 403:335-338"
simundump kreac /kinetics/Lcl/Lcl_degradation 0 0.016666 0 "" white 15 48 -17 \
  0
simundump text /kinetics/Lcl/Lcl_degradation/notes 0 \
  "This reaction denotes the degradation of the Lac\n the half life of which is ten minutes.\nt1/2=.693/k where k denotes kf\n\nNote: Rates had to be scaled by 1.4 to get the desired levels\n\nElowitz etal Nature 2000. Vol403 335-338"
call /kinetics/Lcl/Lcl_degradation/notes LOAD \
"This reaction denotes the degradation of the Lac" \
" the half life of which is ten minutes." \
"t1/2=.693/k where k denotes kf" \
"" \
"Note: Rates had to be scaled by 1.4 to get the desired levels" \
"" \
"Elowitz etal Nature 2000. Vol403 335-338"
simundump kpool /kinetics/Lcl/degraded_Lcl 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 2 15 48 -19 0
simundump text /kinetics/Lcl/degraded_Lcl/notes 0 \
  "Degradation product of lac protein"
call /kinetics/Lcl/degraded_Lcl/notes LOAD \
"Degradation product of lac protein"
simundump kpool /kinetics/Lcl/degraded_Lcl_mRNA 0 0 0 0 0 0 0 0 1800 0 \
  /kinetics/geometry 1 15 62 -8 0
simundump text /kinetics/Lcl/degraded_Lcl_mRNA/notes 0 \
  "Degradation product of lac m-RNA"
call /kinetics/Lcl/degraded_Lcl_mRNA/notes LOAD \
"Degradation product of lac m-RNA"
simundump kpool /kinetics/Lcl/Lcl_operator 0 0 0.022222 0.022222 40 40 0 0 \
  1800 0 /kinetics/geometry 36 15 69 -12 0
simundump text /kinetics/Lcl/Lcl_operator/notes 0 \
  "The lac operator under normal conditions\n drives transcription of lac\n\nElowitz etal Nature 2000 Vol 403:335-338"
call /kinetics/Lcl/Lcl_operator/notes LOAD \
"The lac operator under normal conditions" \
" drives transcription of lac" \
"" \
"Elowitz etal Nature 2000 Vol 403:335-338"
simundump kenz /kinetics/Lcl/Lcl_operator/Lcl_transcription2 0 0 0 0 0 1800 \
  2.4998 2 0.5 0 1 "" red 36 "" 69 -11 0
simundump text /kinetics/Lcl/Lcl_operator/Lcl_transcription2/notes 0 \
  "Rates from\nBox 1 Elowitz et al (2003)\nNature 403:335-338"
call /kinetics/Lcl/Lcl_operator/Lcl_transcription2/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al (2003)" \
"Nature 403:335-338"
simundump text /kinetics/geometry/notes 0 ""
call /kinetics/geometry/notes LOAD \
""
simundump xgraph /graphs/conc1 0 0 10000 0 2.0691 0
simundump xgraph /graphs/conc2 0 0 10000 0 3.33 0
simundump xplot /graphs/conc1/TetR.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 11 0 0 1
simundump xplot /graphs/conc1/lac.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 62 0 0 1
simundump xplot /graphs/conc1/Lcl.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 24 0 0 1
simundump xgraph /moregraphs/conc3 0 0 10000 0 95.756 0
simundump xgraph /moregraphs/conc4 0 0 10000 0 1 0
simundump xcoredraw /edit/draw 0 -11 71 -21 2
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"Repressilator model: A synthetic oscillatory network" \
"of transcriptional regulators designed by Michael B. Elowitz and" \
"Stanislas Leibler 2000 Nature 403(6767):335-338"
addmsg /kinetics/lac_gene/inhib_reac[1] /kinetics/lac_gene/l_op_2nd_bound[1] REAC B A 
addmsg /kinetics/lac_gene/l_op_2nd_bound[1] /kinetics/lac_gene/l_op_2nd_bound[1]/lac_transcription1 ENZYME n 
addmsg /kinetics/lac_gene/Nucleotides /kinetics/lac_gene/l_op_2nd_bound[1]/lac_transcription1 SUBSTRATE n 
addmsg /kinetics/lac_gene/l_op_1rst_bound /kinetics/lac_gene/inhib_reac[1] SUBSTRATE n 
addmsg /kinetics/lac_gene/l_op_2nd_bound[1] /kinetics/lac_gene/inhib_reac[1] PRODUCT n 
addmsg /kinetics/Lcl/Lcl /kinetics/lac_gene/inhib_reac[1] SUBSTRATE n 
addmsg /kinetics/lac_gene/inhib_reac /kinetics/lac_gene/l_op_1rst_bound REAC B A 
addmsg /kinetics/lac_gene/inhib_reac[1] /kinetics/lac_gene/l_op_1rst_bound REAC A B 
addmsg /kinetics/lac_gene/l_op_1rst_bound /kinetics/lac_gene/l_op_1rst_bound/lac_transcription ENZYME n 
addmsg /kinetics/lac_gene/Nucleotides /kinetics/lac_gene/l_op_1rst_bound/lac_transcription SUBSTRATE n 
addmsg /kinetics/lac_gene/lac_operator /kinetics/lac_gene/inhib_reac SUBSTRATE n 
addmsg /kinetics/lac_gene/l_op_1rst_bound /kinetics/lac_gene/inhib_reac PRODUCT n 
addmsg /kinetics/Lcl/Lcl /kinetics/lac_gene/inhib_reac SUBSTRATE n 
addmsg /kinetics/lac_gene/inhib_reac /kinetics/lac_gene/lac_operator REAC A B 
addmsg /kinetics/lac_gene/lac_operator /kinetics/lac_gene/lac_operator/lac_transcription2 ENZYME n 
addmsg /kinetics/lac_gene/Nucleotides /kinetics/lac_gene/lac_operator/lac_transcription2 SUBSTRATE n 
addmsg /kinetics/lac_gene/lac_mRNA_degradation /kinetics/lac_gene/lac_mRNA REAC A B 
addmsg /kinetics/lac_gene/lac_operator/lac_transcription2 /kinetics/lac_gene/lac_mRNA MM_PRD pA 
addmsg /kinetics/lac_gene/l_op_2nd_bound[1]/lac_transcription1 /kinetics/lac_gene/lac_mRNA MM_PRD pA 
addmsg /kinetics/lac_gene/l_op_1rst_bound/lac_transcription /kinetics/lac_gene/lac_mRNA MM_PRD pA 
addmsg /kinetics/lac_gene/lac_mRNA /kinetics/lac_gene/lac_mRNA/lac_translation ENZYME n 
addmsg /kinetics/lac_gene/AminoAcids /kinetics/lac_gene/lac_mRNA/lac_translation SUBSTRATE n 
addmsg /kinetics/lac_gene/lac_degradation /kinetics/lac_gene/lac REAC A B 
addmsg /kinetics/lac_gene/lac_mRNA/lac_translation /kinetics/lac_gene/lac MM_PRD pA 
addmsg /kinetics/TetR_gene/inhib_reac /kinetics/lac_gene/lac REAC A B 
addmsg /kinetics/TetR_gene/inhib_reac[1] /kinetics/lac_gene/lac REAC A B 
addmsg /kinetics/lac_gene/lac_degradation /kinetics/lac_gene/degraded_lac REAC B A 
addmsg /kinetics/lac_gene/lac_mRNA_degradation /kinetics/lac_gene/degraded_lac_mRNA REAC B A 
addmsg /kinetics/lac_gene/lac_mRNA /kinetics/lac_gene/lac_mRNA_degradation SUBSTRATE n 
addmsg /kinetics/lac_gene/degraded_lac_mRNA /kinetics/lac_gene/lac_mRNA_degradation PRODUCT n 
addmsg /kinetics/lac_gene/degraded_lac /kinetics/lac_gene/lac_degradation PRODUCT n 
addmsg /kinetics/lac_gene/lac /kinetics/lac_gene/lac_degradation SUBSTRATE n 
addmsg /kinetics/lac_gene/lac_mRNA/lac_translation /kinetics/lac_gene/AminoAcids REAC sA B 
addmsg /kinetics/lac_gene/lac_operator/lac_transcription2 /kinetics/lac_gene/Nucleotides REAC sA B 
addmsg /kinetics/lac_gene/l_op_2nd_bound[1]/lac_transcription1 /kinetics/lac_gene/Nucleotides REAC sA B 
addmsg /kinetics/lac_gene/l_op_1rst_bound/lac_transcription /kinetics/lac_gene/Nucleotides REAC sA B 
addmsg /kinetics/TetR_gene/inhib_reac[1] /kinetics/TetR_gene/l_op_2nd_bound[1] REAC B A 
addmsg /kinetics/TetR_gene/l_op_2nd_bound[1] /kinetics/TetR_gene/l_op_2nd_bound[1]/TetR_transcription1 ENZYME n 
addmsg /kinetics/TetR_gene/Nucleotides /kinetics/TetR_gene/l_op_2nd_bound[1]/TetR_transcription1 SUBSTRATE n 
addmsg /kinetics/TetR_gene/l_op_2nd_bound[1] /kinetics/TetR_gene/inhib_reac[1] PRODUCT n 
addmsg /kinetics/TetR_gene/l_op_1rst_bound /kinetics/TetR_gene/inhib_reac[1] SUBSTRATE n 
addmsg /kinetics/lac_gene/lac /kinetics/TetR_gene/inhib_reac[1] SUBSTRATE n 
addmsg /kinetics/TetR_gene/inhib_reac[1] /kinetics/TetR_gene/l_op_1rst_bound REAC A B 
addmsg /kinetics/TetR_gene/inhib_reac /kinetics/TetR_gene/l_op_1rst_bound REAC B A 
addmsg /kinetics/TetR_gene/l_op_1rst_bound /kinetics/TetR_gene/l_op_1rst_bound/TetR_transcription ENZYME n 
addmsg /kinetics/TetR_gene/Nucleotides /kinetics/TetR_gene/l_op_1rst_bound/TetR_transcription SUBSTRATE n 
addmsg /kinetics/TetR_gene/l_op_1rst_bound /kinetics/TetR_gene/inhib_reac PRODUCT n 
addmsg /kinetics/TetR_gene/TetR_operator /kinetics/TetR_gene/inhib_reac SUBSTRATE n 
addmsg /kinetics/lac_gene/lac /kinetics/TetR_gene/inhib_reac SUBSTRATE n 
addmsg /kinetics/TetR_gene/TetR_mRNA/TetR_translation /kinetics/TetR_gene/TetR MM_PRD pA 
addmsg /kinetics/TetR_gene/TetR_degradation /kinetics/TetR_gene/TetR REAC A B 
addmsg /kinetics/Lcl/inhib_reac[1] /kinetics/TetR_gene/TetR REAC A B 
addmsg /kinetics/Lcl/inhib_reac /kinetics/TetR_gene/TetR REAC A B 
addmsg /kinetics/TetR_gene/TetR_degradation /kinetics/TetR_gene/degraded_TetR REAC B A 
addmsg /kinetics/TetR_gene/TetR /kinetics/TetR_gene/TetR_degradation SUBSTRATE n 
addmsg /kinetics/TetR_gene/degraded_TetR /kinetics/TetR_gene/TetR_degradation PRODUCT n 
addmsg /kinetics/TetR_gene/l_op_2nd_bound[1]/TetR_transcription1 /kinetics/TetR_gene/TetR_mRNA MM_PRD pA 
addmsg /kinetics/TetR_gene/l_op_1rst_bound/TetR_transcription /kinetics/TetR_gene/TetR_mRNA MM_PRD pA 
addmsg /kinetics/TetR_gene/TetR_operator/TetR_transcription2 /kinetics/TetR_gene/TetR_mRNA MM_PRD pA 
addmsg /kinetics/TetR_gene/TetR_mRNA_degradation /kinetics/TetR_gene/TetR_mRNA REAC A B 
addmsg /kinetics/TetR_gene/TetR_mRNA /kinetics/TetR_gene/TetR_mRNA/TetR_translation ENZYME n 
addmsg /kinetics/TetR_gene/AminoAcids /kinetics/TetR_gene/TetR_mRNA/TetR_translation SUBSTRATE n 
addmsg /kinetics/TetR_gene/TetR_mRNA /kinetics/TetR_gene/TetR_mRNA_degradation SUBSTRATE n 
addmsg /kinetics/TetR_gene/degraded_TetR_mRNA /kinetics/TetR_gene/TetR_mRNA_degradation PRODUCT n 
addmsg /kinetics/TetR_gene/TetR_mRNA_degradation /kinetics/TetR_gene/degraded_TetR_mRNA REAC B A 
addmsg /kinetics/TetR_gene/inhib_reac /kinetics/TetR_gene/TetR_operator REAC A B 
addmsg /kinetics/TetR_gene/TetR_operator /kinetics/TetR_gene/TetR_operator/TetR_transcription2 ENZYME n 
addmsg /kinetics/TetR_gene/Nucleotides /kinetics/TetR_gene/TetR_operator/TetR_transcription2 SUBSTRATE n 
addmsg /kinetics/TetR_gene/TetR_operator/TetR_transcription2 /kinetics/TetR_gene/Nucleotides REAC sA B 
addmsg /kinetics/TetR_gene/l_op_1rst_bound/TetR_transcription /kinetics/TetR_gene/Nucleotides REAC sA B 
addmsg /kinetics/TetR_gene/l_op_2nd_bound[1]/TetR_transcription1 /kinetics/TetR_gene/Nucleotides REAC sA B 
addmsg /kinetics/TetR_gene/TetR_mRNA/TetR_translation /kinetics/TetR_gene/AminoAcids REAC sA B 
addmsg /kinetics/Lcl/inhib_reac[1] /kinetics/Lcl/l_op_2nd_bound[1] REAC B A 
addmsg /kinetics/Lcl/l_op_2nd_bound[1] /kinetics/Lcl/l_op_2nd_bound[1]/Lcl_transcription1 ENZYME n 
addmsg /kinetics/Lcl/Nucleotides /kinetics/Lcl/l_op_2nd_bound[1]/Lcl_transcription1 SUBSTRATE n 
addmsg /kinetics/Lcl/l_op_2nd_bound[1] /kinetics/Lcl/inhib_reac[1] PRODUCT n 
addmsg /kinetics/Lcl/l_op_1rst_bound /kinetics/Lcl/inhib_reac[1] SUBSTRATE n 
addmsg /kinetics/TetR_gene/TetR /kinetics/Lcl/inhib_reac[1] SUBSTRATE n 
addmsg /kinetics/Lcl/inhib_reac[1] /kinetics/Lcl/l_op_1rst_bound REAC A B 
addmsg /kinetics/Lcl/inhib_reac /kinetics/Lcl/l_op_1rst_bound REAC B A 
addmsg /kinetics/Lcl/l_op_1rst_bound /kinetics/Lcl/l_op_1rst_bound/Lcl_transcription ENZYME n 
addmsg /kinetics/Lcl/Nucleotides /kinetics/Lcl/l_op_1rst_bound/Lcl_transcription SUBSTRATE n 
addmsg /kinetics/Lcl/l_op_1rst_bound /kinetics/Lcl/inhib_reac PRODUCT n 
addmsg /kinetics/Lcl/Lcl_operator /kinetics/Lcl/inhib_reac SUBSTRATE n 
addmsg /kinetics/TetR_gene/TetR /kinetics/Lcl/inhib_reac SUBSTRATE n 
addmsg /kinetics/Lcl/Lcl_mRNA/Lcl_translation /kinetics/Lcl/AminoAcids REAC sA B 
addmsg /kinetics/Lcl/l_op_2nd_bound[1]/Lcl_transcription1 /kinetics/Lcl/Nucleotides REAC sA B 
addmsg /kinetics/Lcl/l_op_1rst_bound/Lcl_transcription /kinetics/Lcl/Nucleotides REAC sA B 
addmsg /kinetics/Lcl/Lcl_operator/Lcl_transcription2 /kinetics/Lcl/Nucleotides REAC sA B 
addmsg /kinetics/Lcl/Lcl_mRNA /kinetics/Lcl/Lcl_mRNA_degradation SUBSTRATE n 
addmsg /kinetics/Lcl/degraded_Lcl_mRNA /kinetics/Lcl/Lcl_mRNA_degradation PRODUCT n 
addmsg /kinetics/Lcl/l_op_2nd_bound[1]/Lcl_transcription1 /kinetics/Lcl/Lcl_mRNA MM_PRD pA 
addmsg /kinetics/Lcl/l_op_1rst_bound/Lcl_transcription /kinetics/Lcl/Lcl_mRNA MM_PRD pA 
addmsg /kinetics/Lcl/Lcl_operator/Lcl_transcription2 /kinetics/Lcl/Lcl_mRNA MM_PRD pA 
addmsg /kinetics/Lcl/Lcl_mRNA_degradation /kinetics/Lcl/Lcl_mRNA REAC A B 
addmsg /kinetics/Lcl/Lcl_mRNA /kinetics/Lcl/Lcl_mRNA/Lcl_translation ENZYME n 
addmsg /kinetics/Lcl/AminoAcids /kinetics/Lcl/Lcl_mRNA/Lcl_translation SUBSTRATE n 
addmsg /kinetics/Lcl/Lcl_mRNA/Lcl_translation /kinetics/Lcl/Lcl MM_PRD pA 
addmsg /kinetics/Lcl/Lcl_degradation /kinetics/Lcl/Lcl REAC A B 
addmsg /kinetics/lac_gene/inhib_reac /kinetics/Lcl/Lcl REAC A B 
addmsg /kinetics/lac_gene/inhib_reac[1] /kinetics/Lcl/Lcl REAC A B 
addmsg /kinetics/Lcl/Lcl /kinetics/Lcl/Lcl_degradation SUBSTRATE n 
addmsg /kinetics/Lcl/degraded_Lcl /kinetics/Lcl/Lcl_degradation PRODUCT n 
addmsg /kinetics/Lcl/Lcl_degradation /kinetics/Lcl/degraded_Lcl REAC B A 
addmsg /kinetics/Lcl/Lcl_mRNA_degradation /kinetics/Lcl/degraded_Lcl_mRNA REAC B A 
addmsg /kinetics/Lcl/inhib_reac /kinetics/Lcl/Lcl_operator REAC A B 
addmsg /kinetics/Lcl/Lcl_operator /kinetics/Lcl/Lcl_operator/Lcl_transcription2 ENZYME n 
addmsg /kinetics/Lcl/Nucleotides /kinetics/Lcl/Lcl_operator/Lcl_transcription2 SUBSTRATE n 
addmsg /kinetics/TetR_gene/TetR /graphs/conc1/TetR.Co PLOT Co *TetR.Co *11 
addmsg /kinetics/lac_gene/lac /graphs/conc1/lac.Co PLOT Co *lac.Co *62 
addmsg /kinetics/Lcl/Lcl /graphs/conc1/Lcl.Co PLOT Co *Lcl.Co *24 
enddump
// End of dump

call /kinetics/lac_gene/notes LOAD \
"This is a group of reactions resulting in the production" \
"of Lac"
call /kinetics/lac_gene/l_op_2nd_bound[1]/notes LOAD \
"it represents the promotor with both of its sites" \
" bound by the inhibitor"
call /kinetics/lac_gene/l_op_2nd_bound[1]/lac_transcription1/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al Nature (2000)" \
"403: 335-338"
call /kinetics/lac_gene/inhib_reac[1]/notes LOAD \
"It signifies the first binding of the repressor" \
" with the operator" \
"" \
"Kf=1 /nM/sec" \
"Kb=9 /sec" \
"" \
"Box 1 Elowitz et al Nature 2000 vol 403:335-338"
call /kinetics/lac_gene/l_op_1rst_bound/notes LOAD \
"it represents the promotor with one of its sites" \
" bound by the inhibitor"
call /kinetics/lac_gene/l_op_1rst_bound/lac_transcription/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al Nature 2000" \
"403: 335-338"
call /kinetics/lac_gene/inhib_reac/notes LOAD \
"It signifies the first binding of the repressor" \
" with the operator" \
"" \
"Kf = 1 /nM/sec" \
"Kb = 224 /sec" \
"" \
"Box 1 Elowitz et al Nature 2000 vol 403:335-338"
call /kinetics/lac_gene/lac_operator/notes LOAD \
"The lac operator under normal conditions" \
" drives transcription of lac" \
"" \
"Elowitz et al Nature 2000 Vol 403:335-338"
call /kinetics/lac_gene/lac_operator/lac_transcription2/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al (2000)" \
"403:335-338"
call /kinetics/lac_gene/lac_mRNA/notes LOAD \
"The m-RNA transcribed by the lac operator" \
"" \
"Elowitz et al Nature 2000 Vol 403:335-338"
call /kinetics/lac_gene/lac_mRNA/lac_translation/notes LOAD \
"Rates from " \
"Box 1 Elowitz et al Nature (2000)" \
"403: 335-338"
call /kinetics/lac_gene/lac/notes LOAD \
"Lac translated from lac m-RNA" \
"Derived from Figure 1(c) stochastic version" \
"" \
"Elowitz et al Nature 2000 Vol 403:335-338"
call /kinetics/lac_gene/degraded_lac/notes LOAD \
"Degradation product of lac protein"
call /kinetics/lac_gene/degraded_lac_mRNA/notes LOAD \
"Degradation product of lac m-RNA"
call /kinetics/lac_gene/lac_mRNA_degradation/notes LOAD \
"This reaction denotes the degradation of the Lac m-RNA," \
" the half life of which is two minutes." \
"t1/2=.693/k where k denotes kf"
call /kinetics/lac_gene/lac_degradation/notes LOAD \
"This reaction denotes the degradation of the Lac" \
" the half life of which is ten minutes." \
"t1/2=.693/k where k denotes kf" \
"" \
"Note: Rates had to scaled by approx 1.4 to get desired levels" \
"" \
"Elowitz et al Nature 2000. Vol403 335-338"
call /kinetics/TetR_gene/notes LOAD \
"This is a group of reactions resulting in the production" \
"of Tet R"
call /kinetics/TetR_gene/l_op_2nd_bound[1]/notes LOAD \
"it represents the promotor with both of its sites" \
" bound by the inhibitor"
call /kinetics/TetR_gene/l_op_2nd_bound[1]/TetR_transcription1/notes LOAD \
"Rates from " \
"Box 1 Elowitz et al (2000)" \
"Nature 403:335-338"
call /kinetics/TetR_gene/inhib_reac[1]/notes LOAD \
"It signifies the first binding of the repressor" \
" with the operator" \
"" \
"Kf = 1 /nM/sec" \
"Kb = 9 /sec" \
"" \
"Box 1 Elowitz et al Nature 2000 vol 403:335-338"
call /kinetics/TetR_gene/l_op_1rst_bound/notes LOAD \
"it represents the promotor with one of its sites" \
" bound by the inhibitor"
call /kinetics/TetR_gene/l_op_1rst_bound/TetR_transcription/notes LOAD \
"Rates from " \
"Box 1 Elowitz et al (2000)" \
"Nature 403:335-338"
call /kinetics/TetR_gene/inhib_reac/notes LOAD \
"It signifies the first binding of the repressor" \
" with the operator" \
"" \
"Kf=1 /nM/sec" \
"Kb=224 /sec" \
"" \
"Box1 Elowitz etal Nature 2000 vol 403:335-338"
call /kinetics/TetR_gene/TetR/notes LOAD \
"Lac translated from lac m-RNA" \
"Derived from Figure 1(c) stochastic version" \
"" \
"Elowitz etal Nature 2000 Vol 403:335-338"
call /kinetics/TetR_gene/degraded_TetR/notes LOAD \
"Degradation product of lac protein"
call /kinetics/TetR_gene/TetR_degradation/notes LOAD \
"This reaction denotes the degradation of the Lac" \
" the half life of which is ten minutes." \
"t1/2=.693/k where k denotes kf" \
"" \
"Note: Rates had to be scaled by 1.4 to get appropriate levels" \
"" \
"Elowitz etal Nature 2000. Vol403 335-338"
call /kinetics/TetR_gene/TetR_mRNA/notes LOAD \
"The m-RNA transcribed by the lac operator" \
"" \
"Elowitz etal Nature 2000 Vol 403:335-338"
call /kinetics/TetR_gene/TetR_mRNA/TetR_translation/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al (2000)" \
"Nature 403:335-338"
call /kinetics/TetR_gene/TetR_mRNA_degradation/notes LOAD \
"This reaction denotes the degradation of the Lac m-RNA," \
" the half life of which is two minutes." \
"t1/2=.693/k where k denotes kf"
call /kinetics/TetR_gene/degraded_TetR_mRNA/notes LOAD \
"Degradation product of lac m-RNA"
call /kinetics/TetR_gene/TetR_operator/notes LOAD \
"The lac operator under normal conditions" \
" drives transcription of lac" \
"" \
"Elowitz etal Nature 2000 Vol 403:335-338"
call /kinetics/TetR_gene/TetR_operator/TetR_transcription2/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al (2000)" \
"Nature 403:335-338"
call /kinetics/Lcl/notes LOAD \
"This is a group of reactions resulting in the production" \
"of Lambda cl"
call /kinetics/Lcl/l_op_2nd_bound[1]/notes LOAD \
"it represents the promotor with both of its sites" \
" bound by the inhibitor"
call /kinetics/Lcl/inhib_reac[1]/notes LOAD \
"It signifies the first binding of the repressor" \
" with the operator" \
"" \
"Kf=1 /nM/sec" \
"Kb=9 /sec" \
"" \
"Box1 Elowitz etal Nature 2000 vol 403:335-338"
call /kinetics/Lcl/l_op_1rst_bound/notes LOAD \
"it represents the promotor with one of its sites" \
" bound by the inhibitor"
call /kinetics/Lcl/l_op_1rst_bound/Lcl_transcription/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al (2000)" \
"Nature 403:335-338"
call /kinetics/Lcl/inhib_reac/notes LOAD \
"It signifies the first binding of the repressor" \
" with the operator" \
"" \
"Kf=1 /nM/sec" \
"Kb=224 /sec" \
"" \
"Box1 Elowitz etal Nature 2000 vol 403:335-338"
call /kinetics/Lcl/Lcl_mRNA_degradation/notes LOAD \
"This reaction denotes the degradation of the Lac m-RNA," \
" the half life of which is two minutes." \
"t1/2=.693/k where k denotes kf"
call /kinetics/Lcl/Lcl_mRNA/notes LOAD \
"The m-RNA transcribed by the lac operator" \
"" \
"Elowitz etal Nature 2000 Vol 403:335-338"
call /kinetics/Lcl/Lcl_mRNA/Lcl_translation/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al (2003)" \
"Nature 403:335-338"
call /kinetics/Lcl/Lcl/notes LOAD \
"Lac translated from lac m-RNA" \
"Derived from Figure 1(c) stochastic version" \
"" \
"Elowitz et al Nature 2000 Vol 403:335-338"
call /kinetics/Lcl/Lcl_degradation/notes LOAD \
"This reaction denotes the degradation of the Lac" \
" the half life of which is ten minutes." \
"t1/2=.693/k where k denotes kf" \
"" \
"Note: Rates had to be scaled by 1.4 to get the desired levels" \
"" \
"Elowitz etal Nature 2000. Vol403 335-338"
call /kinetics/Lcl/degraded_Lcl/notes LOAD \
"Degradation product of lac protein"
call /kinetics/Lcl/degraded_Lcl_mRNA/notes LOAD \
"Degradation product of lac m-RNA"
call /kinetics/Lcl/Lcl_operator/notes LOAD \
"The lac operator under normal conditions" \
" drives transcription of lac" \
"" \
"Elowitz etal Nature 2000 Vol 403:335-338"
call /kinetics/Lcl/Lcl_operator/Lcl_transcription2/notes LOAD \
"Rates from" \
"Box 1 Elowitz et al (2003)" \
"Nature 403:335-338"
complete_loading
