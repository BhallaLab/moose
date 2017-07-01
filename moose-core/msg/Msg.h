/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _MSG_H
#define _MSG_H

/**
 * Manages data flow between two elements. Is always many-to-many, with
 * assorted variants.
 */

class Msg
{
	public:
		/// Constructor
		Msg( ObjId mid, Element* e1, Element* e2 );

		/// Destructor
		virtual ~Msg();

		/**
		 * Deletes a message identified by its mid.
		 */
		static void deleteMsg( ObjId mid );

		/**
		 * Obtain the first target Eref for the specified Src Eref
		 * It is really meant only to work with messages with a
		 * single target ObjId for each given src, typically OneToOne.
		 */
		virtual Eref firstTgt( const Eref& src ) const = 0;

		 /**
		  * Return all the sources of e2 from e1, that is, all the msgs
		  * coming into specified entries on e2 from e1.
		  *
		  * ALLDATA used when the
		  * sources include all data entries on a source.
		  * Indexing is v[dataId in range e2.numData][src list]
		  */
		 virtual void sources ( vector< vector< Eref > >& v ) const = 0;

		 /**
		  * Return all the targets of e1 on e2, that is, all the msgs
		  * going from specified entries on e1 to e2.
		  * ALLDATA used when the
		  * targets include all data entries on a target.
		  * Indexing is v[dataId in range e1.numData][tgt list]
		  */
		 virtual void targets( vector< vector< Eref > >& v ) const = 0;

		/**
		 * Return the first element
		 */
		Element* e1() const {
			return e1_;
		}

		/**
		 * Return the second element
		 */
		Element* e2() const {
			return e2_;
		}

		/**
		 * Return the first element id
		 */
		 Id getE1() const;

		/**
		 * Return the second element id
		 */
		 Id getE2() const;


		 /**
		  * Return names of SrcFinfos for messages going from e1 to e2.
		  */
		 vector< string > getSrcFieldsOnE1() const;

		 /**
		  * Return names of DestFinfos for messages going from e1 to e2.
		  */
		 vector< string > getDestFieldsOnE2() const;

		 /**
		  * Return names of SrcFinfos for messages going from e2 to e1.
		  */
		 vector< string > getSrcFieldsOnE2() const;

		 /**
		  * Return names of DestFinfos for messages going from e2 to e1.
		  */
		 vector< string > getDestFieldsOnE1() const;

		/**
		 * return the Msg Id.
		 */
		ObjId mid() const {
			return mid_;
		}

		/**
		 * Find the other end of this Msg. In most cases this is a
		 * straightforward return of e1 or e2, plus perhaps a DataId.
		 * But in some complex msgs we need to figure out
		 * DataIds that match with the target.
		 * In many-to-one cases we just return the first entry.
		 * If no Element match, return ObjId( Id(), DataId::bad() )
		 * If Element e matches but not DataId, return
		 * ObjId( e.id(), DataId::bad() )
		 */
		virtual ObjId findOtherEnd( ObjId ) const = 0;


		/**
		 * Wrapper for findOtherEnd - to expose it as a LookupFinfo.
		 */
		ObjId getAdjacent( ObjId ) const;

		/**
		 * Make a copy of this Msg. The original msg was on
		 * origSrc. The new Msg should go from newSrc to newTgt,
		 * and have the function fid, on bindIndex b.
		 * The copy may have to be a higher-order
		 * Msg type to handle arrays if n > 1.
		 * Note that n is not the number of msgs, but the multiplier
		 * by which numData will be scaled.
		 */
		virtual Msg* copy( Id origSrc, Id newSrc, Id newTgt,
			FuncId fid, unsigned int b, unsigned int n ) const = 0;

		/**
		 * Checks if the message is going forward.
		 * Now merged into Msg::addToQ for most cases.
		 */
		bool isForward( const Element* src ) const {
			return ( e1_ == src );
		}

		/**
		 * Looks up the message .
		 */
		static const Msg* getMsg( ObjId m );

		/**
		 * Set up the Msg to be accessed like an Element
		 * Although the bare Msg class is never seen, we will use it
		 * as a base class and set up some common fields here.
		 */
		static const Cinfo* initCinfo();

		/**
		 * Sets up the Element managers for each of the Msg classes
		 */
		static unsigned int initMsgManagers();

		/**
		 * Clears out the Msg data stored in each Msg subtype.
		 */
		static void clearAllMsgs();

		/**
		 * Returns the most recently constructed msg.
		 */
		static const Msg* lastMsg();

		/// True when MOOSE has been terminated and is being cleaned up.
		static bool isLastTrump();
	protected:
		ObjId mid_; /// Index of this Msg on the msg_ vector.

		Element* e1_; /// Element 1 attached to Msg.
		Element* e2_; /// Element 2 attached to Msg.

		/**
		 * Keeps track of the Id of the parent of all the MsgManager Elms.
		 */
		static Id msgManagerId_;

		/// Flag to indicate termination of program.
		static bool lastTrump_;

	private:
		static const Msg* lastMsg_;
};

#endif // _MSG_H
