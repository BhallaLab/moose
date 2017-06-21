// genesis
// cell parameter file for multiscale modeling snippet. 
*cartesian
*relative

*set_global RM 1.0	//ohm*m^2
*set_global RA 1.0	//ohm*m
*set_global CM 0.03     //F/m^2
*set_global EREST_ACT	-0.06	// volts

// The format for each compartment parameter line is :
// name  parent  x       y       z       d       ch      dens ...
// For channels, "dens" =  maximum conductance per unit area of compartment

// soma	none    0  12  0  12 Ca_conc -17.402e12 Na 300 Ca 40 K_DR 250 K_A 50 K_C 250
soma	none    0  12  0  12 Ca_conc -1e13 Na 300 Ca 50 K_DR 250 K_A 200
