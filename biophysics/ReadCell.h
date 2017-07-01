/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-20011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef READCELL_H
#define READCELL_H
enum ParseStage { COMMENT, DATA, SCRIPT };

/**
 * The ReadCell class implements the old GENESIS cellreader
 * functionality.
 *
 * ReadCell is partially implemented but works for most common uses and
 * will evolve to some further point.
 *
 * One significant semantic difference from the GENESIS version is that
 * in MOOSE ReadCell can accept values of globals defined in the script,
 * but will NOT alter the script global values.
 */
class ReadCell
{
	public:
		// ReadCell( const vector< double >& globalParms );
		ReadCell();

		Id read(
			const string& filename,
			const string& cellname,
			Id parent );

		static void addChannelMessage( Id chan );
	private:
		bool innerRead( ifstream& fin );
		bool readData( const string& line );
		bool readScript( const string& line );
		Id buildCompartment(
			const string& name,
			const string& parent,
			double x0, double y0, double z0,
			double x, double y, double z,
			double d,
			double& length, // Length is sent back.
			vector< string >& argv );
		bool buildChannels(
			Id compt,
			vector< string >& argv,
			double diameter,
			double length);
		Id startGraftCell( const string& cellPath );
		Id findChannel( const string& name );
		Id addChannel(
			Id compt,
			Id chan,
			double value,
			double dia,
			double length );
		bool addCanonicalChannel(
			Id compt,
			Id chan,
			double value,
			double dia,
			double length );
		bool addSpikeGen(
			Id compt,
			Id chan,
			double value,
			double dia,
			double length );
		bool addCaConc(
			Id compt,
			Id chan,
			double value,
			double dia,
			double length );
		bool addNernst(
			Id compt,
			Id chan,
			double value );
/*
		void addKinModel(
			Id compt,
			double value,
			string name,
			string method );
		void addM2C(
			Id compt,
			double value,
			vector< string >::iterator args );
		void addC2M(
			Id compt,
			double value,
			vector< string >::iterator args );
*/

		void countProtos();

		// For error messages
		string fileName_;
		unsigned int lineNum_;

		double RM_;
		double CM_;
		double RA_;
		double EREST_ACT_;
		double ELEAK_;
#if 0                                           /* These are not used */
		double dendrDiam;
		double aveLength;
		double spineSurf;
		double spineDens;
		double spineFreq;
		double membFactor;
#endif

		bool erestFlag_;
		bool eleakFlag_;

		Id cell_;
		Id currCell_;
		Id lastCompt_;
		Id protoCompt_;

		unsigned int numCompartments_;
		unsigned int numChannels_;
		unsigned int numOthers_;

		unsigned int numProtoCompts_;
		unsigned int numProtoChans_;
		unsigned int numProtoOthers_;

		/**
		 * Flag indicating if we are building the main cell, or just a freely
		 * hanging branch which will be grafted on later.
		 */
		bool graftFlag_;
		bool polarFlag_;
		bool relativeCoordsFlag_;
		bool doubleEndpointFlag_;
		bool symmetricFlag_;

		map< string, Id > chanProtos_;

		Shell* shell_;
};
#endif
