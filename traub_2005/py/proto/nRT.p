///***********************************************************************
// * This code was generated from a NEURON model using stub hoc code
// * by Subhasis Ray, NCBS, Bangalore.
// * It corresponds to nRT cell in the model by Traub, et al. 2005
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
// * Single-column thalamocortical network model exhibiting gamma
// * oscillations, sleep spindles, and epileptogenic bursts.
// * J. Neurophysiol. 93, 2194-2232, 2005 Pubmed:
// * http://www.ncbi.nlm.nih.gov/pubmed/15525801?dopt=Abstract
// * **********************************************************************/

// 2010-04-12 11:39:27 (+0530) CaT_A renamed to CaT to conform with
// addmsg1 field in CaPool
// 2010-05-07 12:16:11 (+0530) reverted it back to CaT_A to avoid copying CaT from library. CaT does not influence CaPool after all. It was CaL.
*cartesian
*relative
*asymmetric
*set_global EREST_ACT -0.075
  
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_1	none	30	0	0	18.68	AR	0.25	CaPool	-5.90725e+14	CaL	5	CaT_A	0.5	K2	5	KA	50	KAHP_SLOWER	1	KC	100	KDR_FS	600	KM	5	NaF2_nRT	600	NaPF	6
*set_compt_param ELEAK -0.075
*set_compt_param RM 0.1
*set_compt_param CM 0.01
*set_compt_param RA 1
comp_54	comp_1	50	0	0	1.6	K2	5	KA	10	KDR_FS	4000	NaF2_nRT	4000
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_41	comp_1	75	0	0	2.12	AR	0.25	CaPool	-5.20507e+16	CaL	5	CaT_A	0.5	K2	5	KA	50	KAHP_SLOWER	1	KC	100	KDR_FS	600	KM	5	NaF2_nRT	600	NaPF	6
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_28	comp_1	75	0	0	2.12	AR	0.25	CaPool	-5.20507e+16	CaL	5	CaT_A	0.5	K2	5	KA	50	KAHP_SLOWER	1	KC	100	KDR_FS	600	KM	5	NaF2_nRT	600	NaPF	6
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_15	comp_1	75	0	0	2.12	AR	0.25	CaPool	-5.20507e+16	CaL	5	CaT_A	0.5	K2	5	KA	50	KAHP_SLOWER	1	KC	100	KDR_FS	600	KM	5	NaF2_nRT	600	NaPF	6
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_2	comp_1	75	0	0	2.12	AR	0.25	CaPool	-5.20507e+16	CaL	5	CaT_A	0.5	K2	5	KA	50	KAHP_SLOWER	1	KC	100	KDR_FS	600	KM	5	NaF2_nRT	600	NaPF	6
*set_compt_param ELEAK -0.075
*set_compt_param RM 0.1
*set_compt_param CM 0.01
*set_compt_param RA 1
comp_55	comp_54	50	0	0	1.4	K2	5	KA	10	KDR_FS	4000	NaF2_nRT	4000
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_43	comp_41	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	0.5	K2	5	KA	50	KAHP_SLOWER	1	KC	100	KDR_FS	600	KM	5	NaF2_nRT	600	NaPF	6
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_42	comp_41	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	0.5	K2	5	KA	50	KAHP_SLOWER	1	KC	100	KDR_FS	600	KM	5	NaF2_nRT	600	NaPF	6
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_30	comp_28	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	0.5	K2	5	KA	50	KAHP_SLOWER	1	KC	100	KDR_FS	600	KM	5	NaF2_nRT	600	NaPF	6
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_29	comp_28	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	0.5	K2	5	KA	50	KAHP_SLOWER	1	KC	100	KDR_FS	600	KM	5	NaF2_nRT	600	NaPF	6
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_17	comp_15	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	0.5	K2	5	KA	50	KAHP_SLOWER	1	KC	100	KDR_FS	600	KM	5	NaF2_nRT	600	NaPF	6
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_16	comp_15	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	0.5	K2	5	KA	50	KAHP_SLOWER	1	KC	100	KDR_FS	600	KM	5	NaF2_nRT	600	NaPF	6
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_4	comp_2	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	0.5	K2	5	KA	50	KAHP_SLOWER	1	KC	100	KDR_FS	600	KM	5	NaF2_nRT	600	NaPF	6
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_3	comp_2	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	0.5	K2	5	KA	50	KAHP_SLOWER	1	KC	100	KDR_FS	600	KM	5	NaF2_nRT	600	NaPF	6
*set_compt_param ELEAK -0.075
*set_compt_param RM 0.1
*set_compt_param CM 0.01
*set_compt_param RA 1
comp_58	comp_55	50	0	0	1	K2	5	KA	10	KDR_FS	4000	NaF2_nRT	4000
*set_compt_param ELEAK -0.075
*set_compt_param RM 0.1
*set_compt_param CM 0.01
*set_compt_param RA 1
comp_56	comp_55	50	0	0	1	K2	5	KA	10	KDR_FS	4000	NaF2_nRT	4000
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_46	comp_43	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_45	comp_42	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_44	comp_42	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_33	comp_30	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_32	comp_29	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_31	comp_29	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_20	comp_17	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_19	comp_16	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_18	comp_16	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_7	comp_4	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_6	comp_3	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_5	comp_3	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 0.1
*set_compt_param CM 0.01
*set_compt_param RA 1
comp_59	comp_58	50	0	0	1	K2	5	KA	10	KDR_FS	4000	NaF2_nRT	4000	spike 0.0
*set_compt_param ELEAK -0.075
*set_compt_param RM 0.1
*set_compt_param CM 0.01
*set_compt_param RA 1
comp_57	comp_56	50	0	0	1	K2	5	KA	10	KDR_FS	4000	NaF2_nRT	4000
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_49	comp_46	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_48	comp_45	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_47	comp_44	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_36	comp_33	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_35	comp_32	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_34	comp_31	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_23	comp_20	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_22	comp_19	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_21	comp_18	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_10	comp_7	75	0	0	1.33333	AR	0.25	CaPool	-8.27606e+16	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_9	comp_6	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_8	comp_5	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_50	comp_47	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_37	comp_34	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_24	comp_21	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_11	comp_8	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_51	comp_50	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_38	comp_37	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_25	comp_24	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_12	comp_11	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_52	comp_51	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_39	comp_38	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_26	comp_25	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_13	comp_12	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_53	comp_52	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_40	comp_39	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_27	comp_26	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
*set_compt_param ELEAK -0.075
*set_compt_param RM 2
*set_compt_param CM 0.01
*set_compt_param RA 2.5
comp_14	comp_13	75	0	0	0.837945	AR	0.25	CaPool	-1.31688e+17	CaL	5	CaT_A	20	K2	5	KA	10	KAHP_SLOWER	1	KC	100	KDR_FS	100	KM	5	NaF2_nRT	100	NaPF	1
