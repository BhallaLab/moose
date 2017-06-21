// genesis
// cell parameter file for the 1991 Traub CA1 hippocampal cell
// "phi" parameter reduced by e-3
*cartesian
*relative

*set_global RM 1.0	//ohm*m^2
*set_global RA 1.0	//ohm*m
*set_global CM 0.03     //F/m^2
*set_global EREST_ACT	-0.06	// volts

// The format for each compartment parameter line is :
// name  parent  x       y       z       d       ch      dens ...
// For channels, "dens" =  maximum conductance per unit area of compartment


dend_3  none    0   220  0  3.84
dend_5  .    0   220  0  3.84
dend_6  .    0   220  0  3.84
dend_8  .	    0   110  0  3.84

soma	dend_8    0  125  0  8.46


apical_10 	soma  0  120  0  4.0
apical_11  apical_10  0   120  0  3
apical_12	apical_11  0   120  0  3
apical_13  	apical_12  0   120  0  2.6
apical_14  	apical_13  0   120  0  2.6
apical_15	apical_14  0   120  0  2.6
apical_16	apical_15  0   120  0  2.6
apical_17	apical_16  0   120  0  2.6
apical_18	apical_17  0   120  0  2.6
apical_19 apical_18	   0  120   0  2.6

apical_11_1  apical_10  -60   60  0  2
apical_11_2  	apical_11_1  -60   60  0  1.8
apical_11_3  	apical_11_2  0   80  0  1.5
apical_11_4  	apical_11_3  0   80  0  1.5

apical_13_1	apical_12  40   40  0  1.0
apical_13_2	apical_13_1  40   40  0  1.0

apical_14_1	apical_13_2  0   10  0  1.0
apical_14_2  	.  0   10  0  1.0
apical_14_3  	.  0   10  0  1.0
apical_14_4  	.  0   10  0  1.0
apical_14_5  	.  0   10  0  1.0
apical_14_6  	.  0   10  0  1.0
apical_14_7  	.  0   10  0  1.0
apical_14_8  	.  0   10  0  1.0
apical_14_9  	.  0   10  0  1.0
apical_14_10  	.  0   10  0  1.0
apical_14_11  	.  0   10  0  1.0
apical_14_12  	.  0   10  0  1.0
apical_14_13  	.  0   10  0  1.0
apical_15_1  	.  0   30  0  1.0
apical_15_2  	.  0   60  0  1.0
