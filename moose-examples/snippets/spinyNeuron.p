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

/* NOTE: The format of the cell descriptor files does not allow for
   continuation to another line.  The following long line lists the
   soma channels with their "density" parameters.

   Ca_conc	-17.402e12
	Not really a channel, but a "Ca_concen" object.  Normally, the B 
	field is set to "dens"/compt_volume (m^3), but the negative sign
	causes the absolute value to be used with no scaling by volume.
   Na		300  
   Ca		 40
   K_DR		150
   K_AHP	  8
   K_C		100
   K_A		 50
*/

basal_3  none    0   22  0  3.84
basal_5  .    0   22  0  3.84
basal_6  .    0   22  0  3.84
basal_8  .	    0   11  0  3.84

soma	basal_8    0  12  0  8.46


apical_10 	soma  0  12  0  4.0
apical_11  apical_10  0   12  0  3
apical_12	apical_11  0   12  0  3
apical_13  	apical_12  0   12  0  2.6
apical_14  	apical_13  0   12  0  2.6
apical_15	apical_14  0   12  0  2.6
apical_16	apical_15  0   12  0  2.6
apical_17	apical_16  0   12  0  2.6
apical_18	apical_17  0   12  0  2.6
apical_19 apical_18	   0  12   0  2.6

lat_11_1  apical_10  -6   6  0  2
lat_11_2  	lat_11_1  -6   6  0  1.8
lat_11_3  	lat_11_2  0   8  0  1.5
lat_11_4  	lat_11_3  0   8  0  1.5

lat_13_1	apical_12  4   4  0  1.0
lat_13_2	lat_13_1  4   4  0  1.0

lat_14_1	lat_13_2  0   10  0  1.0
lat_14_2  	.  0   10  0  1.0
lat_14_3  	.  0   10  0  1.0
lat_14_4  	.  0   10  0  1.0
lat_14_5  	.  0   10  0  1.0
lat_14_6  	.  0   10  0  1.0
lat_14_7  	.  0   10  0  1.0
lat_14_8  	.  0   10  0  1.0
lat_14_9  	.  0   10  0  1.0
lat_14_10  	.  0   10  0  1.0
lat_14_11  	.  0   10  0  1.0
lat_14_12  	.  0   10  0  1.0
lat_14_13  	.  0   10  0  1.0
lat_15_1  	.  0   10  0  1.0
lat_15_2  	.  0   10  0  1.0

spine_neck_14_1	lat_14_1	0.5  0  0  0.1
spine_head_14_1	.  		0.5  0  0  0.5
spine_neck_14_2	lat_14_2	0.5  0  0  0.1
spine_head_14_2	.  		0.5  0  0  0.5
spine_neck_14_3	lat_14_3	0.5  0  0  0.1
spine_head_14_3	.  		0.5  0  0  0.5
spine_neck_14_4	lat_14_4	0.5  0  0  0.1
spine_head_14_4	.  		0.5  0  0  0.5
spine_neck_14_5	lat_14_5	0.5  0  0  0.1
spine_head_14_5	.  		0.5  0  0  0.5
spine_neck_14_6	lat_14_6	0.5  0  0  0.1
spine_head_14_6	.  		0.5  0  0  0.5
spine_neck_14_7	lat_14_7	0.5  0  0  0.1
spine_head_14_7	.  		0.5  0  0  0.5
spine_neck_14_8	lat_14_8	0.5  0  0  0.1
spine_head_14_8	.  		0.5  0  0  0.5
spine_neck_14_9	lat_14_9	0.5  0  0  0.1
spine_head_14_9	.  		0.5  0  0  0.5
spine_neck_14_10	lat_14_10	0.5  0  0  0.1
spine_head_14_10	.  		0.5  0  0  0.5
spine_neck_14_11	lat_14_11	0.5  0  0  0.1
spine_head_14_11	.  		0.5  0  0  0.5
spine_neck_14_12	lat_14_12	0.5  0  0  0.1
spine_head_14_12	.  		0.5  0  0  0.5
spine_neck_14_13	lat_14_13	0.5  0  0  0.1
spine_head_14_13	.  		0.5  0  0  0.5
