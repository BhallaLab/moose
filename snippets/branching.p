// genesis
// Artificial binary branching neuron geometry, 4 levels of branch.
*cartesian
*relative

*set_global RM 1.0	//ohm*m^2
*set_global RA 1.0	//ohm*m
*set_global CM 0.03     //F/m^2
*set_global EREST_ACT	-0.06	// volts

// The format for each compartment parameter line is :
// name  parent  x       y       z       d       ch      dens ...
// For channels, "dens" =  maximum conductance per unit area of compartment

soma	none    0  15  0  15

dend1    soma    0      20      0       4

dend11   dend1  -10     10      0       3
dend12   dend1   10     10      0       3

dend111  dend11 -10     0       0       2.12
dend112  dend11   0     10      0       2.12
dend121  dend12   0     10      0       2.12
dend122  dend12  10     0       0       2.12

dend1111 dend111 -5    -5       0       1.5
dend1112 dend111 -5     5       0       1.5
dend1121 dend112 -5     5       0       1.5
dend1122 dend112  5     5       0       1.5
dend1211 dend121 -5     5       0       1.5
dend1212 dend121  5     5       0       1.5
dend1221 dend122  5     5       0       1.5
dend1222 dend122  5    -5       0       1.5
