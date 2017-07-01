#ifndef _HHGate2D_h
#define _HHGate2D_h
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
class HHGate2D
{
	public:
		HHGate2D();
		HHGate2D( Id originalChanId, Id originalGateId );

		double lookupA( vector< double > v ) const;
		double lookupB( vector< double > v ) const;

		// void gateFunc( const Conn* c, double v1, double v2 );

		/**
		 * Single call to get both A and B values in a single
		 * lookup
		 */
		void lookupBoth( double v, double c, double* A, double* B) const;

		/**
		 * Checks if the provided Id is the one that the HHGate was created
		 * on. If true, fine, otherwise complains about trying to set the
		 * field.
		 */
		bool checkOriginal( Id id, const string& field ) const;

		/**
		 * isOriginalChannel returns true if the provided Id is the Id of
		 * the channel on which the HHGate was created.
		 */
		bool isOriginalChannel( Id id ) const;

		/**
		 * isOriginalChannel returns true if the provided Id is the Id of
		 * the Gate created at the same time as the original channel.
		 */
		bool isOriginalGate( Id id ) const;

		/**
		 * Returns the Id of the original Channel.
		 */
		Id originalChannelId() const;

		/**
		 * Returns the A interpol
		 */
                vector< vector< double > > getTableA(const Eref& e) const;
                void setTableA(const Eref& e, vector< vector< double > > value);


		/**
		 * Returns the B interpol
		 */
                vector< vector< double > > getTableB( const Eref& e ) const;
                void setTableB( const Eref& e, vector< vector< double > > value);

                ///
                // Setting table parameters
                ///
                double getXminA(const Eref& e) const;
                void setXminA(const Eref& e, double value);
                double getXmaxA(const Eref& e) const;
                void setXmaxA(const Eref& e, double value);
                unsigned int getXdivsA(const Eref& e) const;
                void setXdivsA(const Eref& e, unsigned int value);
                double getYminA(const Eref& e) const;
                void setYminA(const Eref& e, double value);
                double getYmaxA(const Eref& e) const;
                void setYmaxA(const Eref& e, double value);
                unsigned int getYdivsA(const Eref& e) const;
                void setYdivsA(const Eref& e, unsigned int value);
                double getXminB(const Eref& e) const;
                void setXminB(const Eref& e, double value);
                double getXmaxB(const Eref& e) const;
                void setXmaxB(const Eref& e, double value);
                unsigned int getXdivsB(const Eref& e) const;
                void setXdivsB(const Eref& e, unsigned int value);
                double getYminB(const Eref& e) const;
                void setYminB(const Eref& e, double value);
                double getYmaxB(const Eref& e) const;
                void setYmaxB(const Eref& e, double value);
                unsigned int getYdivsB(const Eref& e) const;
                void setYdivsB(const Eref& e, unsigned int value);

		static const Cinfo* initCinfo();
	private:
		Interpol2D A_;
		Interpol2D B_;

		Id originalChanId_;
		Id originalGateId_;
};

// Used by solver, readcell, etc.

#endif // _HHGate2D_h
