/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

/**
 * this class is used to take data that has already been converted into
 * a buffer by one or more Conv<> operations, and send it between nodes.
 * Useful when one doesn't know the data type being transferred.
 */
class PrepackedBuffer
{
	public:
		/**
		 * Constructor.
		 * Here data is a pointer to the entire data block.
		 * dataSize is the size of the entire data block to be transferred,
		 *  dataSize = individualDataSize * numEntries.
		 * numEntries is the # of array entries. For non-arrays it defaults
		 * to 0.
		 */
		PrepackedBuffer( const char* data, unsigned int dataSize,
			unsigned int numEntries = 0 );

		PrepackedBuffer( const PrepackedBuffer& other );

		/**
		 * Constructor
		 * Here the char buffer is a serialized version of the
		 * Prepacked buffer
		 */
		PrepackedBuffer( const char* buf );

		PrepackedBuffer();

		~PrepackedBuffer();

		const char* data() const {
			return data_;
		}

		/**
		 * looks up entry. If index exceeds numEntries_, then goes back to
		 * beginning. Lets us tile the target with repeating sequences.
		 * Most commonly just repeat one entry.
		 */
		const char* operator[]( unsigned int index ) const;

		/**
		 * 	Returns the size of the data contents.
		 */
		unsigned int dataSize() const {
			return dataSize_;
		}

		/**
		 * Returns the size of the entire PrepackedBuffer converted to char*
		 */
		unsigned int size() const {
			return dataSize_ + 2 * sizeof( unsigned int );
		}

		/**
		 * Converts to a buffer. Buf must be preallocated.
		 */
		unsigned int conv2buf( char* buf ) const;

		/**
		 * Returns number of data entries: size of transferred array.
		 */
		unsigned int numEntries() const {
			return numEntries_;
		}

		/**
		 * Flag: is the data type a single value or a vector?
		 */
		bool isVector() const {
			return numEntries_ > 0;
		}
	private:
		unsigned int dataSize_; // Size of data.
		unsigned int numEntries_; // Number of data entries, if array.
		unsigned int individualDataSize_; // size of each entry.
		char* data_; // Converted data.
};
