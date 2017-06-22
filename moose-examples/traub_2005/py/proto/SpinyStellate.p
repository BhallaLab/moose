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
*set_global EREST_ACT -0.065
*set_compt_param ELEAK -0.065
*set_compt_param RM 5
*set_compt_param CM 0.009
*set_compt_param RA 2.5
comp_1	none	20	0	0	15	AR	2.5	CaPool	-2.75869e+16	CaL	5	CaT_A	1	K2	1	KA	300	KAHP_SLOWER	1	KC_FAST	100	KDR_FS	1000	KM	37.5	NaF2	1500	NaPF_SS	1.5
*set_compt_param ELEAK -0.065
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_54	comp_1	50	0	0	1.4	K2	1	KA	20	KDR_FS	4000	NaF2	4000
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_41	comp_1	40	0	0	2.12	AR	5	CaPool	-9.7595e+16	CaL	10	CaT_A	2	K2	2	KA	600	KAHP_SLOWER	2	KC_FAST	200	KDR_FS	1500	KM	75	NaF2	1500	NaPF_SS	1.5
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_28	comp_1	40	0	0	2.12	AR	5	CaPool	-9.7595e+16	CaL	10	CaT_A	2	K2	2	KA	600	KAHP_SLOWER	2	KC_FAST	200	KDR_FS	1500	KM	75	NaF2	1500	NaPF_SS	1.5
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_15	comp_1	40	0	0	2.12	AR	5	CaPool	-9.7595e+16	CaL	10	CaT_A	2	K2	2	KA	600	KAHP_SLOWER	2	KC_FAST	200	KDR_FS	1500	KM	75	NaF2	1500	NaPF_SS	1.5
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_2	comp_1	40	0	0	2.12	AR	5	CaPool	-9.7595e+16	CaL	10	CaT_A	2	K2	2	KA	600	KAHP_SLOWER	2	KC_FAST	200	KDR_FS	1500	KM	75	NaF2	1500	NaPF_SS	1.5
*set_compt_param ELEAK -0.065
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_55	comp_54	50	0	0	1.2	K2	1	KA	20	KDR_FS	4000	NaF2	4000
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_43	comp_41	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KDR_FS	1500	KM	75	NaF2	1500	NaPF_SS	1.5
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_42	comp_41	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KDR_FS	1500	KM	75	NaF2	1500	NaPF_SS	1.5
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_30	comp_28	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KDR_FS	1500	KM	75	NaF2	1500	NaPF_SS	1.5
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_29	comp_28	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KDR_FS	1500	KM	75	NaF2	1500	NaPF_SS	1.5
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_17	comp_15	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KDR_FS	1500	KM	75	NaF2	1500	NaPF_SS	1.5
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_16	comp_15	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KDR_FS	1500	KM	75	NaF2	1500	NaPF_SS	1.5
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_4	comp_2	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KDR_FS	1500	KM	75	NaF2	1500	NaPF_SS	1.5
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_3	comp_2	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KDR_FS	1500	KM	75	NaF2	1500	NaPF_SS	1.5
*set_compt_param ELEAK -0.065
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_58	comp_55	50	0	0	1	K2	1	KA	20	KDR_FS	4000	NaF2	4000
*set_compt_param ELEAK -0.065
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_56	comp_55	50	0	0	1	K2	1	KA	20	KDR_FS	4000	NaF2	4000
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_46	comp_43	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_45	comp_42	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_44	comp_42	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_33	comp_30	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_32	comp_29	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_31	comp_29	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_20	comp_17	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_19	comp_16	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_18	comp_16	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_7	comp_4	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_6	comp_3	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_5	comp_3	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KC_FAST	200	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_59	comp_58	50	0	0	1	K2	1	KA	20	KDR_FS	4000	NaF2	4000
*set_compt_param ELEAK -0.065
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_57	comp_56	50	0	0	1	K2	1	KA	20	KDR_FS	4000	NaF2	4000 spike 0.0
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_49	comp_46	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_48	comp_45	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_47	comp_44	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_36	comp_33	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_35	comp_32	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_34	comp_31	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_23	comp_20	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_22	comp_19	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_21	comp_18	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_10	comp_7	40	0	0	1.33333	AR	5	CaPool	-1.55176e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_9	comp_6	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_8	comp_5	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_50	comp_47	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_37	comp_34	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_24	comp_21	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_11	comp_8	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	10	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_51	comp_50	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	60	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_38	comp_37	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	60	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_25	comp_24	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	60	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_12	comp_11	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	60	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_52	comp_51	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	60	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_39	comp_38	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	60	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_26	comp_25	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	60	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_13	comp_12	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	60	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_53	comp_52	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	60	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_40	comp_39	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	60	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_27	comp_26	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	60	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
*set_compt_param ELEAK -0.065
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_14	comp_13	40	0	0	0.837945	AR	5	CaPool	-2.46915e+17	CaL	60	CaT_A	2	K2	2	KA	40	KAHP_SLOWER	2	KM	75	NaF2	100	NaPF_SS	0.1
