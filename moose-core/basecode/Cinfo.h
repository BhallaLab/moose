/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _CINFO_H
#define _CINFO_H

class DinfoBase;

/**
 * Class to manage class information for all the other classes.
 */
class Cinfo
{
		public:
			/**
			 * The Cinfo intializer is used for static initialization
			 * of all the MOOSE Cinfos. Each MOOSE class must set up
			 * a function to build its Cinfo. This function must be
			 * called statically in the MOOSE class .cpp file.
			 * Note how it takes the base *Cinfo as an argument. This
			 * lets us call the base Cinfo initializer when making
			 * each Cinfo class, thus ensuring the correct static
			 * initialization sequence, despite the somewhat loose
			 * semantics for this sequence in most C++ compilers.
			 */
			Cinfo( const std::string& name,
					const Cinfo* baseCinfo, // Base class
					Finfo** finfoArray,	// Field information array
					unsigned int nFinfos,
					DinfoBase* d,	// A handle to lots of utility functions for the Data class.
					const string* doc = 0,
					unsigned int numDoc = 0,
					bool banCreation = false
			);

			/**
			 * This initializer is used only as a dummy, to keep Dinfo happy
			 */
			Cinfo();

			/**
			 * This is also a dummy initializer for Dinfo.
			 */
			Cinfo( const Cinfo& other );

			~Cinfo();
//////////////////////////////////////////////////////////////////////////
			/**
			 * Initializes the Cinfo. Must be called exactly once for
			 * each Cinfo.
			 */
			void init( Finfo** finfoArray, unsigned int nFinfos );

			/**
			 * registerFinfo:
			 * Puts Finfo information into Cinfo, and updates fields on
			 * Finfo as necessary.
			 */
			void registerFinfo( Finfo* f );

			/**
			 * Registers the OpFunc, assigns it a FuncId and returns the
			 * FuncId.
			 */
			FuncId registerOpFunc( const OpFunc* f );

			/**
			 * Used in derived classes, to replace the original OpFunc with
			 * the new one.
			 */
			void overrideFunc( FuncId fid, const OpFunc* f );

			/**
			 * Returns the next free value for BindIndex, and keeps track
			 * of the total number set up.
			 */
			BindIndex registerBindIndex();

			/**
			 * Handles any operations that must be done after an Element
			 * is created. Scans through all Finfos as they are the ones to
			 * manage such requests. Examples are to create FieldElements.
			 */
			void postCreationFunc( Id newId, Element* newElm ) const;

			/**
			 * Registers a finfo as needing post-creation work
			 */
			void registerPostCreationFinfo( const Finfo* f );

			/**
			 * True if this class should never be instantiated in MOOSE.
			 * This may happen if it is a pure virtual class, or if
			 * this Cinfo is for a FieldElement which cannot be created
			 * in isolation but only as a child of another class.
			 */
			bool banCreation() const;
//////////////////////////////////////////////////////////////////////////

			const OpFunc* getOpFunc( FuncId fid ) const;
			// FuncId getOpFuncId( const string& funcName ) const;

			/**
			 * Return the name of the Cinfo
			 */
			const std::string& name() const;

			/**
			 * Finds the Cinfo with the specified name.
			 */
			static const Cinfo* find( const std::string& name );

			/*
			 * Returns base Cinfo class. The Neutral returns 0.
			 */
			const Cinfo* baseCinfo() const;

			/**
			 * Finds Finfo by name in the list for this class,
			 * ignoring any element-specific fields.
			 * Returns 0 on failure.
			 */
			const Finfo* findFinfo( const string& name) const;

			/**
			 * Finds the funcId by name. Returns 0 on failure.
			const FuncId findFuncId( const string& name) const;
			 */

			/**
			 * Number of SrcMsgs in total. Each has a unique number to
			 * bind to an entry in the msgBinding vector in the Element.
			 */
			unsigned int numBindIndex() const;

			/**
			 * Returns the map between name and field info
			 */
			const map< string, Finfo* >& finfoMap() const;

			/**
			 * Returns the Dinfo, which manages creation and destruction
			 * of the data, and knows about its size.
			 */
			const DinfoBase* dinfo() const;

			/**
			 * Returns true if the current Cinfo is derived from
			 * the ancestor
			 */
			bool isA( const string& ancestor ) const;

			/**
			 * Utility function for debugging
			 */
			void reportFids() const;

		/////////////////////////////////////////////////////////////////
		// Functions here for the MOOSE Cinfo inspection class
		/////////////////////////////////////////////////////////////////

			/**
			 * Return the documentation string
			 */
			string getDocs() const;

			/**
			 * Return the name of the base class
			 */
			string getBaseClass() const;


			/**
			 * Return the specified SrcFinfo
			 */
			Finfo* getSrcFinfo( unsigned int i ) const;

			/**
			 * Return number of SrcFinfos
			 */
			unsigned int getNumSrcFinfo() const;

			/**
			 * Return the specified DestFinfo
			 */
			Finfo* getDestFinfo( unsigned int i ) const;

			/**
			 * Return number of DestFinfo
			 */
			unsigned int getNumDestFinfo() const;

			/**
			 * Return the specified ValueFinfo
			 */
			Finfo* getValueFinfo( unsigned int i ) const;

			/**
			 * Return number of ValueFinfo
			 */
			unsigned int getNumValueFinfo() const;

			/**
			 * Return the specified LookupFinfo
			 */
			Finfo* getLookupFinfo( unsigned int i ) const;
			/**
			 * Return number of LookupFinfo
			 */
			unsigned int getNumLookupFinfo() const;

			/**
			 * Return the specified SharedFinfo
			 */
			Finfo* getSharedFinfo( unsigned int i );

			/**
			 * Return number of SharedFinfos
			 */
			unsigned int getNumSharedFinfo() const;

			/**
			 * Return the specified FieldElementFinfo
			 */
			Finfo* getFieldElementFinfo( unsigned int i ) const;

			/**
			 * Return number of FieldElementFinfos
			 */
			unsigned int getNumFieldElementFinfo() const;

			/**
			 * Dummy function. We never allow this assignment.
			 */
			void setNumFinfo( unsigned int v );

			/**
			 * Returns the name of the SrcFinfo having the specified
			 * BindIndex, on this Cinfo.
			 * Returns "" on failure.
			 */
			 const string& srcFinfoName( BindIndex bid ) const;

			/**
			 * Returns the name of the DestFinfo having the specified
			 * FuncId, on this Cinfo.
			 * Returns "" on failure.
			 */
			 const string& destFinfoName( FuncId fid ) const;



			/**
			 * Utility function used at init to create the inspection
			 * Elements for each of the Cinfos.
			 */
			static void makeCinfoElements( Id parent );

			/**
			 * Ensures that the Func Ids are always in a fixed order,
			 * regardless of the sequence of static initialization of
			 * Cinfos.
			 */
			static void rebuildOpIndex();

			/**
			 * Initializer for the MOOSE fields for Cinfo
			 */
			static const Cinfo* initCinfo();

		private:
			string name_;

			// const std::string author_;
			// const std::string description_;
			const Cinfo* baseCinfo_;
			const DinfoBase* dinfo_;

			BindIndex numBindIndex_;
			std::map< std::string, std::string > doc_;

			bool banCreation_;

			/**
			 * This looks up Finfos by name.
			 */
			map< string, Finfo* > finfoMap_;

			/// Keep track of all SrcFinfos
			vector< Finfo* > srcFinfos_;

			/// Keep track of all DestFinfos
			vector< Finfo* > destFinfos_;

			/// Keep track of all ValueFinfos
			vector< Finfo* > valueFinfos_;

			/// Keep track of all LookupFinfos
			vector< Finfo* > lookupFinfos_;

			/// Keep track of all SharedFinfos
			vector< Finfo* > sharedFinfos_;

			/// Keep track of all FieldElementFinfos
			vector< Finfo* > fieldElementFinfos_;

			/**
			 * These are special Finfos which have to be invoked
			 * after the Element is created, and their postCreationFuncs
			 * called. They are typically things like FieldElementFinfos.
			 */
			vector< const Finfo* > postCreationFinfos_;
			vector< const OpFunc* > funcs_;

			// Useful to know in case we have transient OpFuncs made and
			//destroyed.
			static unsigned int numCoreOpFunc_;
//			map< string, FuncId > opFuncNames_;

			static map< string, Cinfo* >& cinfoMap();

			// Many opfuncs share same FuncId
			// static map< OpFunc*, FuncId >& funcMap();
};

#endif // _CINFO_H
