//genesis
// kkit Version 11 flat dumpfile
 
// Saved on Mon Jul  8 15:42:18 2013
 
include kkit {argv 1}
 
FASTDT = 1e-05
SIMDT = 0.001
CONTROLDT = 10
PLOTDT = 10
MAXTIME = 1000
TRANSIENT_TIME = 2
VARIABLE_DT_FLAG = 1
DEFAULT_VOL = 9e-20
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
simundump group /kinetics/PSD 0 0 black x 0 1 "" PSD defaultfile.g 0 0 0 -10 \
  -3 0
simundump text /kinetics/PSD/notes 0 ""
call /kinetics/PSD/notes LOAD \
""
simundump kpool /kinetics/PSD/tot_PSD_R 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry blue 0 -4 7 0
simundump text /kinetics/PSD/tot_PSD_R/notes 0 ""
call /kinetics/PSD/tot_PSD_R/notes LOAD \
""
simundump kpool /kinetics/PSD/actCaMKII 0 0 0 0.83333 5 0 0 0 6 0 \
  /kinetics/geometry 35 0 0 7 0
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
simundump group /kinetics/PSD/PP1_PSD 0 yellow 0 x 0 0 "" PP1_PSD \
  defaultfile.g 0 0 0 7 10 0
simundump text /kinetics/PSD/PP1_PSD/notes 0 ""
call /kinetics/PSD/PP1_PSD/notes LOAD \
""
simundump kpool /kinetics/PSD/PP1_PSD/PP1-active 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry blue yellow -4 -5 0
simundump text /kinetics/PSD/PP1_PSD/PP1-active/notes 0 ""
call /kinetics/PSD/PP1_PSD/PP1-active/notes LOAD \
""
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
simundump kreac /kinetics/PSD/move_to_PSD 0 0.5 15 "" white 0 -8 -5 0
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
simundump kpool /kinetics/PSD/inact_CaMKII 0 0 8 8 48 48 0 0 6 0 \
  /kinetics/geometry[1] blue 0 5 11 0
simundump text /kinetics/PSD/inact_CaMKII/notes 0 ""
call /kinetics/PSD/inact_CaMKII/notes LOAD \
""
simundump kreac /kinetics/PSD/CaMKII_bind_CaM 0 0.1 0.1 "" white 0 3 9 0
simundump text /kinetics/PSD/CaMKII_bind_CaM/notes 0 ""
call /kinetics/PSD/CaMKII_bind_CaM/notes LOAD \
""
simundump kpool /kinetics/PSD/CaM.CaMKII 0 0 0 0 0 0 0 0 6 0 \
  /kinetics/geometry[2] blue 0 0 11 0
simundump text /kinetics/PSD/CaM.CaMKII/notes 0 ""
call /kinetics/PSD/CaM.CaMKII/notes LOAD \
""
simundump kpool /kinetics/PSD/Basal_CaMKII 0 0 0.83333 0.83333 5 5 0 0 6 0 \
  /kinetics/geometry[2] blue 0 -5 9 0
simundump text /kinetics/PSD/Basal_CaMKII/notes 0 ""
call /kinetics/PSD/Basal_CaMKII/notes LOAD \
""
simundump group /kinetics/BULK 0 yellow black x 0 0 "" BULK defaultfile.g 0 0 \
  0 -6 -16 0
simundump text /kinetics/BULK/notes 0 ""
call /kinetics/BULK/notes LOAD \
""
simundump kpool /kinetics/BULK/iR 0 0 4.4444 4.4444 240 240 0 0 54 0 \
  /kinetics/geometry 0 yellow -8 -10 0
simundump text /kinetics/BULK/iR/notes 0 "Same as Fus3\n"
call /kinetics/BULK/iR/notes LOAD \
"Same as Fus3" \
""
simundump kpool /kinetics/BULK/PP1-inactive 0 0 0.66667 0.66667 36 36 0 0 54 \
  0 /kinetics/geometry[1] blue yellow -4 -10 0
simundump text /kinetics/BULK/PP1-inactive/notes 0 ""
call /kinetics/BULK/PP1-inactive/notes LOAD \
""
simundump kpool /kinetics/BULK/Ca 0 0 0.08 0.08 4.32 4.32 0 0 54 4 \
  /kinetics/geometry[1] 54 yellow 8 0 0
simundump text /kinetics/BULK/Ca/notes 0 ""
call /kinetics/BULK/Ca/notes LOAD \
""
simundump kpool /kinetics/BULK/CaN 0 0 0.037037 0.037037 2 2 0 0 54 0 \
  /kinetics/geometry[1] 9 yellow 8 -5 0
simundump text /kinetics/BULK/CaN/notes 0 ""
call /kinetics/BULK/CaN/notes LOAD \
""
simundump kpool /kinetics/BULK/Ca4.CaN 0 0 0 0 0 0 0 0 54 0 \
  /kinetics/geometry[1] 7 yellow 1 -5 0
simundump text /kinetics/BULK/Ca4.CaN/notes 0 ""
call /kinetics/BULK/Ca4.CaN/notes LOAD \
""
simundump kenz /kinetics/BULK/Ca4.CaN/CaN_enz 0 0 0 0 0 54 0.92593 4 1 0 0 "" \
  red 7 "" 1 -7 0
simundump text /kinetics/BULK/Ca4.CaN/CaN_enz/notes 0 ""
call /kinetics/BULK/Ca4.CaN/CaN_enz/notes LOAD \
""
simundump kreac /kinetics/BULK/CaN_bind_Ca 0 4e-06 0.1 "" white yellow 4 -7 0
simundump text /kinetics/BULK/CaN_bind_Ca/notes 0 ""
call /kinetics/BULK/CaN_bind_Ca/notes LOAD \
""
simundump kreac /kinetics/BULK/CaM_bind_Ca 0 1e-08 1 "" white yellow 4 3 0
simundump text /kinetics/BULK/CaM_bind_Ca/notes 0 ""
call /kinetics/BULK/CaM_bind_Ca/notes LOAD \
""
simundump kpool /kinetics/BULK/CaM 0 0 0.88889 0.88889 48 48 0 0 54 0 \
  /kinetics/geometry[1] 25 yellow 8 5 0
simundump text /kinetics/BULK/CaM/notes 0 ""
call /kinetics/BULK/CaM/notes LOAD \
""
simundump kpool /kinetics/BULK/Ca4.CaM 0 0 0 0 0 0 0 0 54 0 \
  /kinetics/geometry[1] 26 yellow 1 5 0
simundump text /kinetics/BULK/Ca4.CaM/notes 0 ""
call /kinetics/BULK/Ca4.CaM/notes LOAD \
""
simundump kreac /kinetics/BULK/inactivate_PP1 0 0.05 0.002 "" white yellow -6 \
  -7 0
simundump text /kinetics/BULK/inactivate_PP1/notes 0 ""
call /kinetics/BULK/inactivate_PP1/notes LOAD \
""
simundump xgraph /graphs/conc1 0 0 3523.1 -2.2424 40 0
simundump xgraph /graphs/conc2 0 0 3523.1 0 40 0
simundump xplot /graphs/conc1/tot_PSD_R.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /graphs/conc1/R.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 4 0 0 1
simundump xplot /graphs/conc1/Ca.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 54 0 0 1
simundump xplot /graphs/conc2/iR.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 0 0 0 1
simundump xplot /graphs/conc2/Rpp.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 28 0 0 1
simundump xgraph /moregraphs/conc3 0 0 3523.1 1.8672e-07 0.093991 0
simundump xgraph /moregraphs/conc4 0 0 3523.1 0 11.572 0
simundump xplot /moregraphs/conc3/Ca4.CaN.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 7 0 0 1
simundump xplot /moregraphs/conc3/Ca4.CaM.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 26 0 0 1
simundump xplot /moregraphs/conc4/PP1-active.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" blue 0 0 1
simundump xplot /moregraphs/conc4/actCaMKII.Co 3 524288 \
  "delete_plot.w <s> <d>; edit_plot.D <w>" 35 0 0 1
simundump xcoredraw /edit/draw 0 -12 10 -18 13
simundump xtree /edit/draw/tree 0 \
  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \
  "edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>" auto 0.6
simundump xtext /file/notes 0 1
xtextload /file/notes \
"" \
"" \
"16 Dec 2011. Completely new PSD version, where the R, Rp and Rpp" \
"all refer to AMPAR bound to two stargazins, and each p is on one" \
"of the stargazins. " \
"" \
"stargazin_psd2.g: tweaked traffic parameters to get bistability." \
"Very bistable." \
"" \
"17 Dec 2011. stargazin_psd3.g: Minor change: reduced kcat for" \
"CaN from 10 to 2." \
"" \
"stargazin_psd4.g: Changed some rates around so that it responds" \
"in a more relevant range for CaMKII activation in the full model." \
"Also renamed the Receptor-Stargazin pools to clarify that the" \
"phospho steps are on the stargazin." \
"" \
"stargazin_psd5.g: Incorporated traffic rates from the " \
"traffillator analysis." \
"" \
"stargazin_psd6.g: Raised CaN Km from 5 to 10 uM." \
"" \
"stargazin_psd8.g: Increased level of PP2A by 3x so we have at least" \
"2 molecules of it in the PSD. Lowered kcat to match." \
"" \
"stargazin_psd9.g: Fine-tuning trafficking parameters to see if we" \
"can improve Kramer time for off-to-on transition." \
"" \
"stargazin_psd11.g: first pass at revised stargazin using PP1-active" \
"instead of CaN." \
"" \
"stargazin_psd12.g: Fine-tuned version of 11, with the traffic" \
"rates estimated from traffillator analysis." \
"" \
"stargazin_psd12a.g: Minor naming change." \
"" \
"stargazin_psd12b.g: More naming change: PP2A becomes " \
"basal_phosphatase." \
"" \
"07 July 2013: Adding in toy regulation by Ca." \
"stargazin_snapse2.g: turnon happens but not turnoff." \
"Did doser for PP1-active. Halfmax is only 0.08. Hence problem." \
"" \
"stargazin_synapse4.g: Scaled up doser for PP1-active so halfmax" \
"is now up to 0.2 uM." \
"stargazin_synapse5.g: A big of fine-tuning on the levels of PP1 " \
"and CaMKII so that the bidirectional flip is possible." \
"stargazin_synapse6.g: OK, now a set of parameters that do" \
"bidirectional state flips with reasonable Ca levels." \
"0.08 is bistable" \
"2.0 turns on" \
"0.2 turns off." \
"" \
"08 July 2013. Stargazin_synapse7.g: Raised CaM to match CaMKII" \
"levels, so that the " \
"turnon is faster." \
"stargazin_synapse8.g: Further raise of CaM and CaMKII, now" \
"turnon happens within 20 sec." \
"stargazin_synapse9.g: Added Ca plot." \
""
addmsg /kinetics/PSD/R_S2 /kinetics/PSD/tot_PSD_R SUMTOTAL n nInit 
addmsg /kinetics/PSD/R_SpS /kinetics/PSD/tot_PSD_R SUMTOTAL n nInit 
addmsg /kinetics/PSD/R_SpSp /kinetics/PSD/tot_PSD_R SUMTOTAL n nInit 
addmsg /kinetics/PSD/actCaMKII/CaMKII_1 /kinetics/PSD/actCaMKII REAC eA B 
addmsg /kinetics/PSD/actCaMKII/CaMKII_2 /kinetics/PSD/actCaMKII REAC eA B 
addmsg /kinetics/PSD/CaM.CaMKII /kinetics/PSD/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/Basal_CaMKII /kinetics/PSD/actCaMKII SUMTOTAL n nInit 
addmsg /kinetics/PSD/actCaMKII /kinetics/PSD/actCaMKII/CaMKII_1 ENZYME n 
addmsg /kinetics/PSD/R_S2 /kinetics/PSD/actCaMKII/CaMKII_1 SUBSTRATE n 
addmsg /kinetics/PSD/actCaMKII /kinetics/PSD/actCaMKII/CaMKII_2 ENZYME n 
addmsg /kinetics/PSD/R_SpS /kinetics/PSD/actCaMKII/CaMKII_2 SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/PP1_2 /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active/PP1_1 /kinetics/PSD/PP1_PSD/PP1-active REAC eA B 
addmsg /kinetics/BULK/Ca4.CaN/CaN_enz /kinetics/PSD/PP1_PSD/PP1-active MM_PRD pA 
addmsg /kinetics/BULK/inactivate_PP1 /kinetics/PSD/PP1_PSD/PP1-active REAC A B 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/PP1_2 ENZYME n 
addmsg /kinetics/PSD/R_SpSp /kinetics/PSD/PP1_PSD/PP1-active/PP1_2 SUBSTRATE n 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/PSD/PP1_PSD/PP1-active/PP1_1 ENZYME n 
addmsg /kinetics/PSD/R_SpS /kinetics/PSD/PP1_PSD/PP1-active/PP1_1 SUBSTRATE n 
addmsg /kinetics/BULK/iR /kinetics/PSD/move_to_PSD SUBSTRATE n 
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
addmsg /kinetics/PSD/CaMKII_bind_CaM /kinetics/PSD/inact_CaMKII REAC A B 
addmsg /kinetics/BULK/Ca4.CaM /kinetics/PSD/CaMKII_bind_CaM SUBSTRATE n 
addmsg /kinetics/PSD/inact_CaMKII /kinetics/PSD/CaMKII_bind_CaM SUBSTRATE n 
addmsg /kinetics/PSD/CaM.CaMKII /kinetics/PSD/CaMKII_bind_CaM PRODUCT n 
addmsg /kinetics/PSD/CaMKII_bind_CaM /kinetics/PSD/CaM.CaMKII REAC B A 
addmsg /kinetics/PSD/move_to_PSD /kinetics/BULK/iR REAC A B 
addmsg /kinetics/BULK/Ca4.CaN/CaN_enz /kinetics/BULK/PP1-inactive REAC sA B 
addmsg /kinetics/BULK/inactivate_PP1 /kinetics/BULK/PP1-inactive REAC B A 
addmsg /kinetics/BULK/CaM_bind_Ca /kinetics/BULK/Ca REAC A B 
addmsg /kinetics/BULK/CaM_bind_Ca /kinetics/BULK/Ca REAC A B 
addmsg /kinetics/BULK/CaN_bind_Ca /kinetics/BULK/Ca REAC A B 
addmsg /kinetics/BULK/CaN_bind_Ca /kinetics/BULK/Ca REAC A B 
addmsg /kinetics/BULK/CaM_bind_Ca /kinetics/BULK/Ca REAC A B 
addmsg /kinetics/BULK/CaM_bind_Ca /kinetics/BULK/Ca REAC A B 
addmsg /kinetics/BULK/CaN_bind_Ca /kinetics/BULK/Ca REAC A B 
addmsg /kinetics/BULK/CaN_bind_Ca /kinetics/BULK/Ca REAC A B 
addmsg /kinetics/BULK/CaN_bind_Ca /kinetics/BULK/CaN REAC A B 
addmsg /kinetics/BULK/Ca4.CaN/CaN_enz /kinetics/BULK/Ca4.CaN REAC eA B 
addmsg /kinetics/BULK/CaN_bind_Ca /kinetics/BULK/Ca4.CaN REAC B A 
addmsg /kinetics/BULK/Ca4.CaN /kinetics/BULK/Ca4.CaN/CaN_enz ENZYME n 
addmsg /kinetics/BULK/PP1-inactive /kinetics/BULK/Ca4.CaN/CaN_enz SUBSTRATE n 
addmsg /kinetics/BULK/CaN /kinetics/BULK/CaN_bind_Ca SUBSTRATE n 
addmsg /kinetics/BULK/Ca /kinetics/BULK/CaN_bind_Ca SUBSTRATE n 
addmsg /kinetics/BULK/Ca /kinetics/BULK/CaN_bind_Ca SUBSTRATE n 
addmsg /kinetics/BULK/Ca /kinetics/BULK/CaN_bind_Ca SUBSTRATE n 
addmsg /kinetics/BULK/Ca /kinetics/BULK/CaN_bind_Ca SUBSTRATE n 
addmsg /kinetics/BULK/Ca4.CaN /kinetics/BULK/CaN_bind_Ca PRODUCT n 
addmsg /kinetics/BULK/CaM /kinetics/BULK/CaM_bind_Ca SUBSTRATE n 
addmsg /kinetics/BULK/Ca /kinetics/BULK/CaM_bind_Ca SUBSTRATE n 
addmsg /kinetics/BULK/Ca /kinetics/BULK/CaM_bind_Ca SUBSTRATE n 
addmsg /kinetics/BULK/Ca /kinetics/BULK/CaM_bind_Ca SUBSTRATE n 
addmsg /kinetics/BULK/Ca /kinetics/BULK/CaM_bind_Ca SUBSTRATE n 
addmsg /kinetics/BULK/Ca4.CaM /kinetics/BULK/CaM_bind_Ca PRODUCT n 
addmsg /kinetics/BULK/CaM_bind_Ca /kinetics/BULK/CaM REAC A B 
addmsg /kinetics/PSD/CaMKII_bind_CaM /kinetics/BULK/Ca4.CaM REAC A B 
addmsg /kinetics/BULK/CaM_bind_Ca /kinetics/BULK/Ca4.CaM REAC B A 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /kinetics/BULK/inactivate_PP1 SUBSTRATE n 
addmsg /kinetics/BULK/PP1-inactive /kinetics/BULK/inactivate_PP1 PRODUCT n 
addmsg /kinetics/PSD/tot_PSD_R /graphs/conc1/tot_PSD_R.Co PLOT Co *tot_PSD_R.Co *blue 
addmsg /kinetics/PSD/R_S2 /graphs/conc1/R.Co PLOT Co *R.Co *4 
addmsg /kinetics/BULK/Ca /graphs/conc1/Ca.Co PLOT Co *Ca.Co *54 
addmsg /kinetics/BULK/iR /graphs/conc2/iR.Co PLOT Co *iR.Co *0 
addmsg /kinetics/PSD/R_SpSp /graphs/conc2/Rpp.Co PLOT Co *Rpp.Co *28 
addmsg /kinetics/BULK/Ca4.CaN /moregraphs/conc3/Ca4.CaN.Co PLOT Co *Ca4.CaN.Co *7 
addmsg /kinetics/BULK/Ca4.CaM /moregraphs/conc3/Ca4.CaM.Co PLOT Co *Ca4.CaM.Co *26 
addmsg /kinetics/PSD/PP1_PSD/PP1-active /moregraphs/conc4/PP1-active.Co PLOT Co *PP1-active.Co *blue 
addmsg /kinetics/PSD/actCaMKII /moregraphs/conc4/actCaMKII.Co PLOT Co *actCaMKII.Co *35 
enddump
// End of dump

call /kinetics/PSD/basal_phosphatase/notes LOAD \
"There isn't any clear info for this. I had originally called " \
"it PP2A, but that causes odd interactions with other pathways." \
""
call /kinetics/BULK/iR/notes LOAD \
"Same as Fus3" \
""
complete_loading
