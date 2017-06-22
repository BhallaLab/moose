///***********************************************************************
// * This code was generated from a NEURON model using stub hoc code
// * by Subhasis Ray, NCBS, Bangalore.
// * It corresponds to  cell in the model by Traub, et al. 2005
// * Reference: http://senselab.med.yale.edu/ModelDb/ShowModel.asp?model=45539
//
// * Reference: Roger D. Traub, Eberhard H. Buhl, Tengis Gloveli, and 
// * Miles A. Whittington. Fast Rhythmic Bursting Can Be Induced in 
// * Layer 2/3 Cortical Neurons by Enhancing Persistent Na+ Conductance
// * or by Blocking BK Channels J Neurophysiol 89: 909-921, 2003
// * Pubmed: http://www.ncbi.nlm.nih.gov/pubmed/12574468?dopt=Abstract
//
// * Reference: Roger D. Traub, Diego Contreras, Mark O. Cunningham,
// * Hilary Murray, Fiona E. N. LeBeau, Anita Roopun, Andrea Bibbig, 
// * W. Bryan Wilent, Michael J. Higley, and Miles A. Whittington
// * Single-column thalamocortical network model exhibiting 
// * gamma oscillations, sleep spindles, and epileptogenic bursts.
// * J. Neurophysiol. 93, 2194-2232, 2005
// * Pubmed: http://www.ncbi.nlm.nih.gov/pubmed/15525801?dopt=Abstract
// ***********************************************************************/
*cartesian
*relative
*asymmetric
*set_global EREST_ACT -0.07
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_1	none	42	0	0	20	AR	2.5	CaPool	-1.97049e+15	CaL	5	CaT	5	K2	20	KA	60	KAHP_SLOWER	0.5	KC	120	KDR	337.5	KM	5	NaF_TCR	1000	NaPF_TCR	2
*set_compt_param ELEAK -0.07
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_132	comp_1	50	0	0	1.6	K2	5	KA	10	KDR	1800	NaF_TCR	4000
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_119	comp_1	20	0	0	1.46	AR	5	CaPool	-1.13371e+17	CaL	5	CaT	50	K2	20	KA	60	KAHP_SLOWER	0.5	KC	120	KDR	225	KM	5	NaF_TCR	1000	NaPF_TCR	2
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_106	comp_1	20	0	0	1.46	AR	5	CaPool	-1.13371e+17	CaL	5	CaT	50	K2	20	KA	60	KAHP_SLOWER	0.5	KC	120	KDR	225	KM	5	NaF_TCR	1000	NaPF_TCR	2
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_93	comp_1	20	0	0	1.46	AR	5	CaPool	-1.13371e+17	CaL	5	CaT	50	K2	20	KA	60	KAHP_SLOWER	0.5	KC	120	KDR	225	KM	5	NaF_TCR	1000	NaPF_TCR	2
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_80	comp_1	20	0	0	1.46	AR	5	CaPool	-1.13371e+17	CaL	5	CaT	50	K2	20	KA	60	KAHP_SLOWER	0.5	KC	120	KDR	225	KM	5	NaF_TCR	1000	NaPF_TCR	2
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_67	comp_1	20	0	0	1.46	AR	5	CaPool	-1.13371e+17	CaL	5	CaT	50	K2	20	KA	60	KAHP_SLOWER	0.5	KC	120	KDR	225	KM	5	NaF_TCR	1000	NaPF_TCR	2
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_54	comp_1	20	0	0	1.46	AR	5	CaPool	-1.13371e+17	CaL	5	CaT	50	K2	20	KA	60	KAHP_SLOWER	0.5	KC	120	KDR	225	KM	5	NaF_TCR	1000	NaPF_TCR	2
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_41	comp_1	20	0	0	1.46	AR	5	CaPool	-1.13371e+17	CaL	5	CaT	50	K2	20	KA	60	KAHP_SLOWER	0.5	KC	120	KDR	225	KM	5	NaF_TCR	1000	NaPF_TCR	2
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_28	comp_1	20	0	0	1.46	AR	5	CaPool	-1.13371e+17	CaL	5	CaT	50	K2	20	KA	60	KAHP_SLOWER	0.5	KC	120	KDR	225	KM	5	NaF_TCR	1000	NaPF_TCR	2
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_15	comp_1	20	0	0	1.46	AR	5	CaPool	-1.13371e+17	CaL	5	CaT	50	K2	20	KA	60	KAHP_SLOWER	0.5	KC	120	KDR	225	KM	5	NaF_TCR	1000	NaPF_TCR	2
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_2	comp_1	20	0	0	1.46	AR	5	CaPool	-1.13371e+17	CaL	5	CaT	50	K2	20	KA	60	KAHP_SLOWER	0.5	KC	120	KDR	225	KM	5	NaF_TCR	1000	NaPF_TCR	2
*set_compt_param ELEAK -0.07
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_133	comp_132	50	0	0	1.4	K2	5	KA	10	KDR	1800	NaF_TCR	4000
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_122	comp_119	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_121	comp_119	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_120	comp_119	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_109	comp_106	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_108	comp_106	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_107	comp_106	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_96	comp_93	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_95	comp_93	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_94	comp_93	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_83	comp_80	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_82	comp_80	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_81	comp_80	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_70	comp_67	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_69	comp_67	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_68	comp_67	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_57	comp_54	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_56	comp_54	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_55	comp_54	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_44	comp_41	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_43	comp_41	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_42	comp_41	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_31	comp_28	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_30	comp_28	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_29	comp_28	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_18	comp_15	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_17	comp_15	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_16	comp_15	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_5	comp_2	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_4	comp_2	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_3	comp_2	57.5	0	0	1.168	AR	3	CaPool	-4.92916e+16	CaL	2.5	CaT	30	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_136	comp_133	50	0	0	1	K2	5	KA	10	KDR	1800	NaF_TCR	4000
*set_compt_param ELEAK -0.07
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_134	comp_133	50	0	0	1	K2	5	KA	10	KDR	1800	NaF_TCR	4000
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_131	comp_122	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_130	comp_122	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_129	comp_122	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_128	comp_121	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_127	comp_121	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_126	comp_121	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_125	comp_120	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_124	comp_120	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_123	comp_120	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_118	comp_109	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_117	comp_109	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_116	comp_109	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_115	comp_108	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_114	comp_108	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_113	comp_108	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_112	comp_107	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_111	comp_107	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_110	comp_107	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_105	comp_96	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_104	comp_96	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_103	comp_96	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_102	comp_95	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_101	comp_95	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_100	comp_95	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_99	comp_94	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_98	comp_94	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_97	comp_94	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_92	comp_83	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_91	comp_83	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_90	comp_83	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_89	comp_82	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_88	comp_82	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_87	comp_82	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_86	comp_81	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_85	comp_81	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_84	comp_81	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_79	comp_70	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_78	comp_70	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_77	comp_70	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_76	comp_69	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_75	comp_69	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_74	comp_69	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_73	comp_68	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_72	comp_68	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_71	comp_68	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_66	comp_57	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_65	comp_57	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_64	comp_57	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_63	comp_56	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_62	comp_56	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_61	comp_56	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_60	comp_55	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_59	comp_55	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_58	comp_55	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_53	comp_44	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_52	comp_44	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_51	comp_44	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_50	comp_43	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_49	comp_43	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_48	comp_43	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_47	comp_42	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_46	comp_42	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_45	comp_42	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_40	comp_31	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_39	comp_31	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_38	comp_31	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_37	comp_30	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_36	comp_30	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_35	comp_30	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_34	comp_29	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_33	comp_29	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_32	comp_29	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_27	comp_18	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_26	comp_18	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_25	comp_18	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_24	comp_17	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_23	comp_17	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_22	comp_17	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_21	comp_16	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_20	comp_16	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_19	comp_16	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_14	comp_5	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_13	comp_5	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_12	comp_5	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_11	comp_4	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_10	comp_4	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_9	comp_4	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_8	comp_3	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_7	comp_3	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.64
*set_compt_param CM 0.009
*set_compt_param RA 1.75
comp_6	comp_3	57.5	0	0	0.876	AR	3	CaPool	-6.57221e+16	CaL	2.5	CaT	5	K2	20	KA	2	KAHP_SLOWER	0.5	KC	200	KM	5	NaF_TCR	50	NaPF_TCR	0.1
*set_compt_param ELEAK -0.07
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_137	comp_136	50	0	0	1	K2	5	KA	10	KDR	1800	NaF_TCR	4000
*set_compt_param ELEAK -0.07
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_135	comp_134	50	0	0	1	K2	5	KA	10	KDR	1800	NaF_TCR	4000 spike 0.0
