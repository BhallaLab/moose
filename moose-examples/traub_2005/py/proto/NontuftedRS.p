///***********************************************************************
// * This code was generated from a NEURON model using stub hoc code
// * by Subhasis Ray, NCBS, Bangalore.
// * It corresponds to nontuftRS cell in the model by Traub, et al. 2005
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
*set_compt_param RM 5
*set_compt_param CM 0.009
*set_compt_param RA 2.5
comp_1	none	20	0	0	16	AR	2.5	CaPool	-1.29313e+15	CaL	2	CaT	1	K2	1	KA	1190	KAHP_DP	2	KC	150	KDR	1700	KM	42	NaF	2000	NaP	0.8
*set_compt_param ELEAK -0.07
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_45	comp_1	25	0	0	1.8	K2	1	KA	40	KDR	4500	NaF	4500
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_35	comp_1	50	0	0	3	AR	5	CaPool	-1.37934e+16	CaL	4	CaT	2	K2	2	KA	2380	KAHP_DP	4	KC	300	KDR	2400	KM	84	NaF	3000	NaP	1.2
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_6	comp_1	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KC	300	KDR	1500	KM	84	NaF	1500	NaP	0.6
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_5	comp_1	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KC	300	KDR	1500	KM	84	NaF	1500	NaP	0.6
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_4	comp_1	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KC	300	KDR	1500	KM	84	NaF	1500	NaP	0.6
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_3	comp_1	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KC	300	KDR	1500	KM	84	NaF	1500	NaP	0.6
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_2	comp_1	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KC	300	KDR	1500	KM	84	NaF	1500	NaP	0.6
*set_compt_param ELEAK -0.07
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_46	comp_45	50	0	0	1.4	K2	1	KA	40	KDR	4500	NaF	4500
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_36	comp_35	50	0	0	2.8	AR	5	CaPool	-1.47787e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KC	300	KDR	1500	KM	84	NaF	1500	NaP	0.6
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_12	comp_35	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KC	300	KDR	1500	KM	84	NaF	1500	NaP	0.6
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_7	comp_35	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KC	300	KDR	1500	KM	84	NaF	1500	NaP	0.6
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_17	comp_6	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_16	comp_5	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_15	comp_4	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_14	comp_3	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_13	comp_2	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_49	comp_46	50	0	0	1	K2	1	KA	40	KDR	4500	NaF	4500
*set_compt_param ELEAK -0.07
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_47	comp_46	50	0	0	1	K2	1	KA	40	KDR	4500	NaF	4500
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_11	comp_36	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KC	300	KDR	1500	KM	84	NaF	1500	NaP	0.6
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_8	comp_36	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KC	300	KDR	1500	KM	84	NaF	1500	NaP	0.6
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_37	comp_36	50	0	0	2.6	AR	5	CaPool	-1.59155e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_23	comp_12	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_18	comp_7	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_28	comp_17	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_27	comp_16	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_26	comp_15	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_25	comp_14	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_24	comp_13	60	0	0	1.7	AR	5	CaPool	-2.02845e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_50	comp_49	50	0	0	1	K2	1	KA	40	KDR	4500	NaF	4500
*set_compt_param ELEAK -0.07
*set_compt_param RM 0.1
*set_compt_param CM 0.009
*set_compt_param RA 1
comp_48	comp_47	50	0	0	1	K2	1	KA	40	KDR	4500	NaF	4500 spike 0.0
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_22	comp_11	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_19	comp_8	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_10	comp_37	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KC	300	KDR	1500	KM	84	NaF	1500	NaP	0.6
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_9	comp_37	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KC	300	KDR	1500	KM	84	NaF	1500	NaP	0.6
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_38	comp_37	50	0	0	2.4	AR	5	CaPool	-1.72418e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_34	comp_23	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_29	comp_18	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_33	comp_22	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_30	comp_19	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_21	comp_10	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_20	comp_9	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_39	comp_38	50	0	0	2.2	AR	5	CaPool	-1.88092e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_32	comp_21	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_31	comp_20	60	0	0	1.24	AR	5	CaPool	-2.78093e+16	CaL	4	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_40	comp_39	50	0	0	2	AR	5	CaPool	-2.06901e+16	CaL	40	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_41	comp_40	50	0	0	1.8	AR	5	CaPool	-2.2989e+16	CaL	40	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_42	comp_41	50	0	0	1.6	AR	5	CaPool	-2.58627e+16	CaL	40	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_43	comp_42	50	0	0	1.4	AR	5	CaPool	-2.95573e+16	CaL	40	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
*set_compt_param ELEAK -0.07
*set_compt_param RM 2.5
*set_compt_param CM 0.018
*set_compt_param RA 2.5
comp_44	comp_43	50	0	0	1.2	AR	5	CaPool	-3.44836e+16	CaL	40	CaT	2	K2	2	KA	272	KAHP_DP	4	KM	84	NaF	100	NaP	0.04
