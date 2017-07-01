/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _LOOKUP_SIZE_FROM_MESH_H
#define _LOOKUP_SIZE_FROM_MESH_H


/**
 * Used to provide the conversion factor from preferred units to SI.
 */
extern const double CONC_UNIT_CONV;

/**
 * Utility function for kinetics classes to find their volume from
 * the attached mesh. Works for Pools and subclasses.
 */
double lookupVolumeFromMesh( const Eref& e );

/**
 * Utility function to get volumes for all reactants (substrates or
 * products) of Reacs or Enzymes. Does NOT get volumes for the Enzyme
 * itself.
 */
unsigned int getReactantVols( const Eref& reac, const SrcFinfo* pools,
	vector< double >& vols );

/**
 * Generates conversion factor for rates from concentration to mol# units.
 * Assumes that all reactant pools (substrate and product) are within the
 * same mesh entry and therefore have the same volume.
 * The outcome of this calculation is:
 * kf = Kf * convertConcToNumRate.
 * The Eref is an Enz or Reac.
 * The SrcFinfo is a message to the pools.
 * The meshIndex specifies the mesh voxel to use.
 * The scale term is a conversion from the conc units to SI:
 * The scale term is 1 if conc units are in SI, which is
 * equal to moles per cubic metre, which is equal to millimolar.
 * The scale term is 1e-3 for micromolar, uM.
 * The doPartialConversion flag tells the function that there are other
 * substrates not in the 'pools' list, and so it should compute the
 * conversion for all pools, not n-1. This flag defaults to 0.
 */
double convertConcToNumRateUsingMesh( const Eref& e, const SrcFinfo* pools,
	bool doPartialConversion );

/**
 * Generates conversion factor for rates from concentration to mol# units.
 * This variant already knows the volume, but has to figure out # of
 * reactants.
 * The scale term is a conversion from the conc units to SI:
 * The scale term is 1 if conc units are in SI, which is
 * equal to moles per cubic metre, which is equal to millimolar.
 * The scale term is 1e-3 for micromolar, uM.
 * The doPartialConversion flag tells the function that there are other
 * substrates not in the 'pools' list, and so it should compute the
 * conversion for all pools, not n-1. This flag defaults to 0.
 */
double convertConcToNumRateUsingVol( const Eref& e, const SrcFinfo* pools,
	double volume, double scale, bool doPartialConversion );

/**
 * Generates conversion factor for rates from concentration to mol# units.
 * This variant is used when the reactants are in different compartments
 * or mesh entries, and may therefore have different volumes.
 * We already know the reactants and their affiliations.
 * We use the first vol, v1, as the reference.
 * The outcome of this calculation is:
 * kf = Kf * convertConcToNumRate.
 * The scale term is a conversion from the conc units to SI:
 * The scale term is 1 if conc units are in SI, which is
 * equal to moles per cubic metre, which is equal to millimolar.
 * The scale term is 1e-3 for micromolar, uM.
 */
double convertConcToNumRateInTwoCompts( double v1, unsigned int n1,
	double v2, unsigned int n2, double scale );

/**
 * Returns the compartment in which the specified object is located.
 * Traverses the tree toward the rood till it finds a compartment.
 */
ObjId getCompt( Id id );

#endif	// _LOOKUP_SIZE_FROM_MESH_H
