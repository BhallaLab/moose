/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2006 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SPARSE_MATRIX_H
#define _SPARSE_MATRIX_H

#include <vector>
using namespace std;

/**
 * Template for specialized SparseMatrix. Used both for the Kinetic
 * solver and for handling certain kinds of messages. Speciality is that
 * it can extract entire rows efficiently, for marching through a
 * specified row for a matrix multiplication or for traversing messages.
 *
 * Requires that type T have an equality operator ==
 */

extern const unsigned int SM_MAX_ROWS;
extern const unsigned int SM_MAX_COLUMNS;
extern const unsigned int SM_RESERVE;

template< class T > class Triplet
{
public:
    Triplet()
    {
        ;
    }

    Triplet( T a, unsigned int b, unsigned int c )
        : a_( a ), b_( b ), c_( c )
    {
        ;
    }
    bool operator< ( const Triplet< T >& other ) const
    {
        return ( c_ < other.c_ );
    }
    static bool cmp( const Triplet< T >& p, const Triplet< T >& q )
    {
        if ( p.b_ == q.b_ )
            return ( p.c_ < q.c_ );
        else if ( p.b_ < q.b_ )
            return true;
        return false;
    }

    T a_;
    unsigned int b_; // row
    unsigned int c_; // col
};


typedef std::vector< class T >::const_iterator constTypeIter;
template < class T > class SparseMatrix
{
public:
    //////////////////////////////////////////////////////////////////
    // Constructors
    //////////////////////////////////////////////////////////////////
    SparseMatrix()
        : nrows_( 0 ), ncolumns_( 0 ), rowStart_( 1, 0 )
    {
        N_.resize( 0 );
        N_.reserve( SM_RESERVE );
        colIndex_.resize( 0 );
        colIndex_.reserve( SM_RESERVE );
    }

    SparseMatrix( unsigned int nrows, unsigned int ncolumns )
    {
        setSize( nrows, ncolumns );
    }

    //////////////////////////////////////////////////////////////////
    // Information operations.
    //////////////////////////////////////////////////////////////////

    unsigned int nRows() const
    {
        return nrows_;
    }

    unsigned int nColumns() const
    {
        return ncolumns_;
    }

    unsigned int nEntries() const
    {
        return N_.size();
    }

    /*
    bool operator==()( const SparseMatrix& other ) {
    	if (
    		nrows_ == other.nrows_ &&
    		ncolumns_ == other.ncolumns_ &&
    		&& N_.size() == other.N_.size() &&
    		rowStart_ == other.rowStart_ &&
    		colIndex_ == other.colIndex_ ) {
    		for ( unsigned int i = 0; i < N_.size(); ++i )
    			if ( !doubleEq( N_[i], other.N_[i] ) )
    				return false;
    		return true;
    	}
    	return false;
    }
    */

    //////////////////////////////////////////////////////////////////
    // Individual entry Access operations.
    //////////////////////////////////////////////////////////////////
    /**
     * Should be called only at the start. Subsequent resizing destroys
     * the contents.
     */
    void setSize( unsigned int nrows, unsigned int ncolumns )
    {
        if ( nrows == 0 || ncolumns == 0 )
        {
            N_.clear();
            rowStart_.resize( 1 );
            rowStart_[0] = 0;
            colIndex_.clear();
            nrows_ = 0;
            ncolumns_ = 0;
            return;
        }
        if ( nrows < SM_MAX_ROWS && ncolumns < SM_MAX_COLUMNS )
        {
            N_.clear();
            N_.reserve( 2 * nrows );
            nrows_ = nrows;
            ncolumns_ = ncolumns;
            rowStart_.clear();
            rowStart_.resize( nrows + 1, 0 );
            colIndex_.clear();
            colIndex_.reserve( 2 * nrows );
        }
        else
        {
            cerr << "Error: SparseMatrix::setSize( " <<
                 nrows << ", " << ncolumns << ") out of range: ( " <<
                 SM_MAX_ROWS << ", " << SM_MAX_COLUMNS << ")\n";
        }
    }

    /**
     * Assigns and if necessary adds an entry in the matrix.
     * This variant does NOT remove any existing entry.
     */
    void set( unsigned int row, unsigned int column, T value )
    {
        if ( nrows_ == 0 || ncolumns_ == 0 )
            return;
        vector< unsigned int >::iterator i;
        vector< unsigned int >::iterator begin =
            colIndex_.begin() + rowStart_[ row ];
        vector< unsigned int >::iterator end =
            colIndex_.begin() + rowStart_[ row + 1 ];

        if ( begin == end )   // Entire row was empty.
        {
            unsigned long offset = begin - colIndex_.begin();
            colIndex_.insert( colIndex_.begin() + offset, column );
            N_.insert( N_.begin() + offset, value );
            for ( unsigned int j = row + 1; j <= nrows_; j++ )
                rowStart_[ j ]++;
            return;
        }

        if ( column > *( end - 1 ) )   // add entry at end of row.
        {
            unsigned long offset = end - colIndex_.begin();
            colIndex_.insert( colIndex_.begin() + offset, column );
            N_.insert( N_.begin() + offset, value );
            for ( unsigned int j = row + 1; j <= nrows_; j++ )
                rowStart_[ j ]++;
            return;
        }
        for ( i = begin; i != end; i++ )
        {
            if ( *i == column )   // Found desired entry. By defn it is nonzero.
            {
                N_[ i - colIndex_.begin()] = value;
                return;
            }
            else if ( *i > column )     // Desired entry is blank.
            {
                unsigned long offset = i - colIndex_.begin();
                colIndex_.insert( colIndex_.begin() + offset, column );
                N_.insert( N_.begin() + offset, value );
                for ( unsigned int j = row + 1; j <= nrows_; j++ )
                    rowStart_[ j ]++;
                return;
            }
        }
    }

    /**
     * Removes specified entry.
     */
    void unset( unsigned int row, unsigned int column )
    {
        if ( nrows_ == 0 || ncolumns_ == 0 )
            return;
        vector< unsigned int >::iterator i;
        vector< unsigned int >::iterator begin =
            colIndex_.begin() + rowStart_[ row ];
        vector< unsigned int >::iterator end =
            colIndex_.begin() + rowStart_[ row + 1 ];

        if ( begin == end )   // Entire row was empty. Ignore
        {
            return;
        }

        if ( column > *( end - 1 ) )   // End of row. Ignore
        {
            return;
        }
        for ( i = begin; i != end; i++ )
        {
            if ( *i == column )   // Found desired entry. Zap it.
            {
                unsigned long offset = i - colIndex_.begin();
                colIndex_.erase( i );
                N_.erase( N_.begin() + offset );
                for ( unsigned int j = row + 1; j <= nrows_; j++ )
                    rowStart_[ j ]--;
                return;
            }
            else if ( *i > column )     //Desired entry is blank. Ignore
            {
                return;
            }
        }
    }

    /**
     * Returns the entry identified by row, column. Returns T(0)
     * if not found
     */
    T get( unsigned int row, unsigned int column ) const
    {
        if ( nrows_ == 0 || ncolumns_ == 0 )
            return 0;
        assert( row < nrows_ && column < ncolumns_ );
        vector< unsigned int >::const_iterator i;
        vector< unsigned int >::const_iterator begin =
            colIndex_.begin() + rowStart_[ row ];
        vector< unsigned int >::const_iterator end =
            colIndex_.begin() + rowStart_[ row + 1 ];

        i = find( begin, end, column );
        if ( i == end )   // most common situation for a sparse Stoich matrix.
        {
            return 0;
        }
        else
        {
            return N_[ rowStart_[row] + (i - begin) ];
        }
    }
    //////////////////////////////////////////////////////////////////
    // Row/Column Access operations.
    //////////////////////////////////////////////////////////////////

    /**
     * Used to get an entire row of entries.
     * Returns # entries.
     * Passes back iterators for the row and for the column index.
     *
     * Ideally I should provide a forall type function so that the
     * user passes in their operation as a functor, and it is
     * applied to the entire row.
     *
     */
    unsigned int getRow( unsigned int row,
                         const T** entry, const unsigned int** colIndex ) const
    {
        if ( row >= nrows_ || ncolumns_ == 0 )
        {
            entry = 0;
            colIndex = 0;
            return 0;
        }
        unsigned int rs = rowStart_[row];
        if ( rs >= N_.size() )
        {
            entry = 0;
            colIndex = 0;
            return 0;
        }
        *entry = &( N_[ rs ] );
        *colIndex = &( colIndex_[rs] );
        return rowStart_[row + 1] - rs;
    }

    /**
     * getRow: vector version.
     * Used to get an entire row of entries.
     * Returns # entries.
     */
    unsigned int getRow( unsigned int row,
                         vector< T >& e, vector< unsigned int >& c ) const
    {
        e.clear();
        c.clear();
        if ( row >= nrows_ || ncolumns_ == 0 )
        {
            return 0;
        }
        unsigned int rs = rowStart_[row];
        if ( rs >= N_.size() )
        {
            return 0;
        }
        unsigned int ret = rowStart_[row + 1] - rs;
        e.insert( e.begin(),
                  N_.begin() + rs, N_.begin() + rs + ret );
        c.insert( c.begin(),
                  colIndex_.begin() + rs, colIndex_.begin() + rs + ret );
        return ret;
    }


    /**
     * This is an unnatural lookup here, across the grain of the
     * sparse matrix.
     * Ideally should use copy_if, but the C++ chaps forgot it.
     */
    unsigned int getColumn( unsigned int col,
                            vector< T >& entry,
                            vector< unsigned int >& rowIndex ) const
    {
        entry.resize( 0 );
        rowIndex.resize( 0 );

        unsigned int row = 0;
        for ( unsigned int i = 0; i < N_.size(); ++i )
        {
            if ( col == colIndex_[i] )
            {
                entry.push_back( N_[i] );
                while ( rowStart_[ row + 1 ] <= i )
                    row++;
                rowIndex.push_back( row );
            }
        }
        return entry.size();
    }

#if 0
    void rowOperation( unsigned int row, unary_function< T, void>& f )
    {
        assert( row < nrows_ );

        constTypeIter i;
        // vector< T >::const_iterator i;
        unsigned int rs = rowStart_[row];
        vector< unsigned int >::const_iterator j = colIndex_.begin() + rs;
        // vector< T >::const_iterator end =
        constTypeIter end =
            N_.begin() + rowStart_[ row + 1 ];

        // for_each
        for ( i = N_.begin() + rs; i != end; ++i )
            f( *i );
    }
#endif

    /**
     * Adds a row to the sparse matrix, must go strictly in row order.
     * This is dangerous as there is a test for an empty entry ~0.
     * For many types, ~0 may actually be a perfectly legal entry.
     */
    void addRow( unsigned int rowNum, const vector< T >& row )
    {
        assert( rowNum < nrows_ );
        assert( rowStart_.size() == (nrows_ + 1 ) );
        assert( N_.size() == colIndex_.size() );
        if ( ncolumns_ == 0 )
            return;
        for ( unsigned int i = 0; i < ncolumns_; ++i )
        {
            if ( row[i] != T( ~0 ) )
            {
                N_.push_back( row[i] );
                colIndex_.push_back( i );
            }
        }
        rowStart_[rowNum + 1] = N_.size();
    }

    /**
     * Used to set an entire row of entries, already in sparse form.
     * Assumes that the SparseMatrix has been suitably allocated.
     * rowNum must be done in increasing order in successive calls.
     */
    void addRow( unsigned int rowNum,
                 const vector < T >& entry,
                 const vector< unsigned int >& colIndexArg )
    {
        assert( rowNum < nrows_ );
        assert( rowStart_.size() == (nrows_ + 1 ) );
        assert( rowStart_[ rowNum ] == N_.size() );
        assert( entry.size() == colIndexArg.size() );
        assert( N_.size() == colIndex_.size() );
        if ( ncolumns_ == 0 )
            return;
        N_.insert( N_.end(), entry.begin(), entry.end() );
        colIndex_.insert( colIndex_.end(),
                          colIndexArg.begin(), colIndexArg.end() );
        rowStart_[rowNum + 1] = N_.size();
    }
	/// Here we expose the sparse matrix for MOOSE use.
	const vector< T >& matrixEntry() const
	{
		return N_;
	}
	const vector< unsigned int >& colIndex() const
	{
		return colIndex_;
	}
	const vector< unsigned int >& rowStart() const
	{
		return rowStart_;
	}
    //////////////////////////////////////////////////////////////////
    // Operations on entire matrix.
    //////////////////////////////////////////////////////////////////

    void clear()
    {
        N_.resize( 0 );
        colIndex_.resize( 0 );
        assert( rowStart_.size() == (nrows_ + 1) );
        rowStart_.assign( nrows_ + 1, 0 );
    }


    /**
     * Does a transpose, using as workspace a vector of size 3 N_
     * 0257 -> 0011122
     */
    void transpose()
    {
        vector< Triplet< T > > t;

        unsigned int rowIndex = 0;
        if ( rowStart_.size() < 2 )
            return;
        /*
        for ( unsigned int i = 0; i < rowStart_.size(); ++i )
        	cout << rowStart_[i] << " ";
        cout << endl;
        */
        // cout << "rowNum = ";
        unsigned int rs = rowStart_[0];
        for ( unsigned int i = 0; i < N_.size(); ++i )
        {
            while( rs == rowStart_[ rowIndex + 1 ] )
            {
                rowIndex++;
            }
            rs++;

            /*
            if ( i == rowStart_[j] ) {
            	rowNum++;
            	j++;
            }
            */
            // cout << rowNum << " ";
            // The rowNum is going to be the new colIndex.
            Triplet< T > x( N_[i], rowIndex, colIndex_[i] );
            t.push_back( x );
        }
        // cout << endl;
        // cout << "before sort\n"; printTriplet( t );
        stable_sort( t.begin(), t.end() );
        // cout << "after sort\n"; printTriplet( t );

        unsigned int j = ~0;
        rowStart_.resize( 0 );
        rowStart_.push_back( 0 );
        unsigned int ci = 0;
        for ( unsigned int i = 0; i < N_.size(); ++i )
        {
            N_[i] = t[i].a_;
            colIndex_[i] = t[i].b_;

            while ( ci != t[i].c_ )
            {
                rowStart_.push_back( i );
                ci++;
            }

            /*
            if ( t[i].c_ != j ) {
            	j = t[i].c_;
            	rowStart_.push_back( i );
            }
            */
        }
        for ( j = ci; j < ncolumns_; ++j )
            rowStart_.push_back( N_.size() );
        // rowStart_.push_back( N_.size() );
        j = nrows_;
        nrows_ = ncolumns_;
        ncolumns_ = j;
        assert( rowStart_.size() == nrows_ + 1 );
    }

    /**
     * Reorder columns from the matrix based on a map of old to new
     * column indices.
     * newCols contains the indices of columns from the old matrix
     * that are to be retained, in the order that they will occupy
     * in the new
     * matrix. It can have fewer or more indices than the original.
     */
    void reorderColumns( const vector< unsigned int >& colMap )
    {
        unsigned int numNewColumns = colMap.size();;
        SparseMatrix< T > old = *this;
        setSize( nrows_, numNewColumns );
        if ( numNewColumns == 0 )
            return;
        for ( unsigned int i = 0; i < old.nrows_; ++i )
        {
            const T* entry;
            const unsigned int* colIndex;
            unsigned int n = old.getRow( i, &entry, &colIndex );
            // Make the full-length vectors of the new row.
            vector< T > newEntry( numNewColumns );
            vector< bool > isNewEntry( numNewColumns, false );
            unsigned int numOccupiedEntries = 0;
            for ( unsigned int j = 0; j < n; ++j )
            {
                assert( colIndex[j] < old.ncolumns_ );
                for ( unsigned int q = 0; q < colMap.size(); ++q )
                {
                    if ( colMap[q] == colIndex[j] )
                    {
                        isNewEntry[q] = true;
                        newEntry[q] = entry[j];
                        ++numOccupiedEntries;
                    }
                }
            }
            // Compress the full-length vector into the sparse form
            vector< T > sparseEntry;
            vector< unsigned int > sparseCols;
            sparseEntry.reserve( numOccupiedEntries );
            sparseCols.reserve( numOccupiedEntries );
            for ( unsigned int q = 0; q < numNewColumns; ++q )
            {
                if ( isNewEntry[q] )
                {
                    sparseEntry.push_back( newEntry[q] );
                    sparseCols.push_back( q );
                }
            }
            addRow( i, sparseEntry, sparseCols );
        }
    }

    //////////////////////////////////////////////////////////////////
    // Utility operations.
    //////////////////////////////////////////////////////////////////

    void tripletFill( const vector< unsigned int >& row,
                      const vector< unsigned int >& col,
                      const vector< T >& z, bool retainSize = false )
    {
        unsigned int len = row.size();
        if ( len > col.size() ) len = col.size();
        if ( len > z.size() ) len = z.size();
        vector< Triplet< T > > trip( len );
        for ( unsigned int i = 0; i < len; ++i )
            trip[i]= Triplet< T >(z[i], row[i], col[i] );
        sort( trip.begin(), trip.end(), Triplet< T >::cmp );
    	unsigned int nr = nrows_;
    	unsigned int nc = ncolumns_;
		if ( !retainSize ) {
    		nr = trip.back().b_ + 1;
    		nc = 0;
    		for ( typename vector< Triplet< T > >::iterator i =
                trip.begin(); i != trip.end(); ++i )
    		{
        		if ( nc < i->c_ )
            		nc = i->c_;
    		}
    		nc++;
		}
   		setSize( nr, nc );

        vector< unsigned int > colIndex( nc );
        vector< T > entry( nc );

        typename vector< Triplet< T > >::iterator j = trip.begin();
        for ( unsigned int i = 0; i < nr; ++i )
        {
            colIndex.clear();
            entry.clear();
            while( j != trip.end() && j->b_ == i )
            {
                colIndex.push_back( j->c_ );
                entry.push_back( j->a_ );
                j++;
            }
            addRow( i, entry, colIndex );
        }
    }

    void pairFill( const vector< unsigned int >& row,
                   const vector< unsigned int >& col, T value )
    {
        vector< T > z( row.size(), value );
        tripletFill( row, col, z );
    }

    //////////////////////////////////////////////////////////////////
    // Printing operations.
    //////////////////////////////////////////////////////////////////

    void printTriplet( const vector< Triplet< T > >& t )
    {
        for ( unsigned int i = 0; i < t.size(); ++i )
        {
            cout << i << "	" << t[i].a_ << "	" << t[i].b_ <<
                 "	" << t[i].c_ << endl;
        }
    }

    /**
     * Prints out the contents in matrix form
     */
    void print() const
    {
        for ( unsigned int i = 0; i < nrows_; ++i )
        {
            unsigned int k = rowStart_[i];
            unsigned int end = rowStart_[i + 1];
            unsigned int nextColIndex = colIndex_[k];
            for ( unsigned int j = 0; j < ncolumns_; ++j )
            {
                if ( j < nextColIndex )
                {
                    cout << "0	";
                }
                else if ( k < end )
                {
                    cout << N_[k] << "	";
                    ++k;
                    nextColIndex = colIndex_[k];
                }
                else
                {
                    cout << "0	";
                }
            }
            cout << endl;
        }
    }

    /**
     * Prints out the contents in internal form
     */
    void printInternal() const
    {
        unsigned int max = (nrows_ < N_.size() ) ? N_.size() : nrows_+1;
        cout << "#	";
        for ( unsigned int i = 0; i < max; ++i )
            cout << i << "	";
        cout << "\nrs	";
        for ( unsigned int i = 0; i < rowStart_.size(); ++i )
            cout << rowStart_[i] << "	";
        cout << "\ncol	";
        for ( unsigned int i = 0; i < N_.size(); ++i )
            cout << colIndex_[i] << "	";
        cout << "\nN	";
        for ( unsigned int i = 0; i < N_.size(); ++i )
            cout << N_[i] << "	";
        cout << endl;
    }


protected:
    unsigned int nrows_;
    unsigned int ncolumns_;
    vector< T > N_;	/// Non-zero entries in the SparseMatrix.

    /*
     * Column index of each non-zero entry.
     * This matches up entry-by entry with the N_ vector.
     */
    vector< unsigned int > colIndex_;

    /// Start index in the N_ and colIndex_ vectors, of each row.
    vector< unsigned int > rowStart_;
};

#endif // _SPARSE_MATRIX_H
