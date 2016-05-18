/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2007 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include "header.h"
#include <stdio.h>
#include "Neutral.h"
#include "Shell.h"
#include "Wildcard.h"
// #define NOINDEX (UINT_MAX - 2)

static int wildcardRelativeFind( ObjId start, const vector< string >& path, 
		unsigned int depth, vector< ObjId >& ret );

static unsigned int findBraceContent( const string& path, 
				string& beforeBrace, string& insideBrace );

static bool matchName( ObjId id, unsigned int index,
	const string& beforeBrace, const string& insideBrace ); 

// static bool matchBeforeBrace( ObjId id, const string& name );

static bool matchInsideBrace( ObjId id, const string& inside );
/**
 * wildcardFieldComparison returns true if the value of the
 * specified field matches the value in the comparsion string mid.
 * Format is FIELD(name)=val
 * If the format or the value does not match, return 0
 */
static bool wildcardFieldComparison( ObjId oid, const string& mid )
{
	// where = could be the usual comparison operators and val
	// could be a number. No strings yet

	string::size_type pos = mid.find(')');
	if ( pos == string::npos )
		return 0;
	string fieldName = mid.substr( 0, pos );
	string::size_type pos2 = mid.find_last_of( "=<>" );
	if ( pos2 == string::npos )
		return 0;
	string op = mid.substr( pos + 1, pos2 - pos );

	string testValue = mid.substr( pos2 + 1 );

	if ( testValue.length() == 0 )
		return 0;

	/*
	const Finfo* f = id()->findFinfo( fieldName );
	if ( !f )
		return 0;

	string actualValue;
	bool ret = f->strGet( id.eref(), actualValue );
	*/
	string actualValue;

	bool ret = SetGet::strGet( oid, fieldName, actualValue );
	if ( ret == 0 )
		return 0;
	if ( op == "==" || op == "=" )
		return ( testValue == actualValue );
	if ( op == "!=" )
		return ( testValue != actualValue );
	
	double v1 = atof( actualValue.c_str() );
	double v2 = atof( testValue.c_str() );
	if ( op == ">" )
		return ( v1 > v2 );
	if ( op == ">=" )
		return ( v1 >= v2 );
	if ( op == "<" )
		return ( v1 < v2 );
	if ( op == "<=" )
		return ( v1 <= v2 );

	return 0;
}

/**
 * Does the wildcard find on a single path
 */
static int innerFind( const string& path, vector< ObjId >& ret)
{
	if ( path == "/" || path == "/root") {
		ret.push_back( Id() );
		return 1;
	}

	vector< string > names;
	vector< vector< unsigned int > > indices;
	bool isAbsolute = Shell::chopString( path, names, '/' );
	ObjId start; // set to root id.
	if ( !isAbsolute ) {
		Shell* s = reinterpret_cast< Shell* >( ObjId().data() );
		start = s->getCwe();
	}
		
		/*
	if ( path[0] == '/' ) {
		// separateString puts in a blank first entry if the first char
		// is a separator.
		separateString( path.substr( 1 ) , names, "/" );
	} else {
		Shell* s = reinterpret_cast< Shell* >( Id.eref().data() );
		separateString( path, names, "/" );
		start = s->getCwe();
	}
	*/
	return wildcardRelativeFind( start, names, 0, ret );
}

/*
static int wildcardRelativeFind( Id start, const vector< string >& path, 
		unsigned int depth, vector< Id >& ret )
		*/

/**
 * This is the basic wildcardFind function, working on a single
 * tree. It adds entries into the vector 'ret' with Ids found according
 * to the path string. It preserves the order of the returned Ids
 * as the order of elements traversed in the search. It does NOT
 * eliminate duplicates. This is a depth-first search.
 * Note that it does the dumb but backward compatible thing with
 * Ids of arrays: it lists every entry.
 *
 * It returns the number of Ids found here.
 */
int simpleWildcardFind( const string& path, vector< ObjId >& ret)
{
	if ( path.length() == 0 )
		return 0;
	unsigned int n = ret.size();
	vector< string > wildcards;
	Shell::chopString( path, wildcards, ',' );
	// separateString( path, wildcards, "," );
	vector< string >::iterator i;
	for ( i = wildcards.begin(); i != wildcards.end(); ++i )
		innerFind( *i, ret );

	return ret.size() - n;
}

static void myUnique(vector<ObjId>& ret)
{
	sort(ret.begin(), ret.end());
	unsigned int i, j;
	j = 0;
	for (i = 1; i < ret.size(); i++) {
		if (ret[j] != ret[i]) {
			ret[++j] = ret[i];
		}
	}
	j++;
	if (j < ret.size())
		ret.resize(j);
}

int wildcardFind(const string& path, vector<ObjId>& ret) 
{
	ret.resize( 0 );
	simpleWildcardFind( path, ret );
	myUnique( ret );
	return ret.size();
}

/**
 * 	singleLevelWildcard parses a single level of the path and returns all
 * 	ids that match it. If there is a suitable doublehash, it will recurse
 * 	into child elements.
 * 	Returns # of ids found.
 */
int singleLevelWildcard( ObjId start, const string& path, vector< ObjId >& ret )
{
	if ( path.length() == 0 )
		return 0;
	unsigned int nret = ret.size();

	string beforeBrace;
	string insideBrace;
	// This has to handle ghastly cases like foo[][FIELD(x)=12.3]
	unsigned int index = findBraceContent( path, beforeBrace, insideBrace );
	if ( beforeBrace == "##" )
		// recursive.
		return allChildren( start, index, insideBrace, ret ); 

	vector< Id > kids;
	Neutral::children( start.eref(), kids );
	vector< Id >::iterator i;
	for ( i = kids.begin(); i != kids.end(); i++ ) {
		if ( matchName( ObjId( *i, ALLDATA ), 
								index, beforeBrace, insideBrace ) ) {
			if ( index == ALLDATA ) {
				for ( unsigned int j = 0; j < i->element()->numData(); ++j )
					ret.push_back( ObjId( *i, j ) );
			} else if ( i->element()->hasFields() && index < i->element()->numField( start.dataIndex ) ) {
				ret.push_back( ObjId( *i, start.dataIndex, index ) );
			} else if ( !i->element()->hasFields() && index < i->element()->numData() ) {
				ret.push_back( ObjId( *i, index ) );
			}
		}
	}

	return ret.size() - nret;
}

/**
 * Parses the name and separates out the stuff before the brace, 
 * the stuff inside it, and if present, the index which is also in a 
 * brace. Returns the index, and if not found, zero.
 * Assume order is 
 * foo[index][insideBrace] 
 * or foo[index]
 * or foo[insideBrace]
 * or foo
 *
 * Note that for the index, an empty [] means ALLDATA, but the 
 * absence of the braces altogether means zero.
 *
 * Note also that if the name ends in the wildcard '#', then we assume
 * that all indices are OK, unless overridden by a subsequent specific
 * index via braces. 
 * So foo# gives all indices
 * but foo#[3] only gives index == 3.
 */
unsigned int findBraceContent( const string& path, string& beforeBrace, 
	string& insideBrace )
{
	int index = 0;
	beforeBrace = "";
	insideBrace = "";

	if ( path.length() == 0 )
		return 0;
	vector< string > names;
	Shell::chopString( path, names, '[' );
	if ( names.size() == 0 )
		return 0;
	if ( names.size() >= 1 )
		beforeBrace = names[0];
	unsigned int len = beforeBrace.length();
	if ( len > 0 && beforeBrace[len -1] == '#' )
		index = ALLDATA;
	if ( names.size() >= 2 ) {
		const string& n = names[1];
		if ( n == "]" ) { // A [] construct means use all indices.
			index = ALLDATA;
		} else if ( isdigit( n[0] ) ) {
			index = atoi( n.c_str() );
		} else { // some complicated text construct for the brace
			insideBrace = n.substr( 0, n.length() - 1 );
			return index; 
		}
		if ( names.size() == 3 ) { // name[number][another_string]
			string n1 = names[2].substr( 0, names[2].length() - 1 );
			insideBrace = n1;
		}
	}
	return index;
}

/**
 * Compares the various parts of the wildcard name with the id
 * Indexing is messy here because we may refer to any of 3 things:
 * - Regular array indexing
 * - Wildcards within the braces
 * - Simple elements with index as part of their names.
 */
bool matchName( ObjId id, unsigned int index,
	const string& beforeBrace, const string& insideBrace )
{
	string temp = id.element()->getName();
	if ( temp.length() <= 0 ){
	  return false;
	}

	// if ( index == ALLDATA || index == id.dataIndex ) {
		if ( matchBeforeBrace( id, beforeBrace ) ) {
			if ( insideBrace.length() == 0 ) {
				return true;
			} else {
				return matchInsideBrace( id, insideBrace );
			}
		}
	// }
	return 0;
}

/**
 * matchInsideBrace checks for element property matches
 * Still has some legacy hacks for reading GENESIS code.
 */
bool matchInsideBrace( ObjId id, const string& inside )
{
	/* Map from Genesis class names to Moose class names */
	// const map< string, string >& classNameMap = sliClassNameConvert();
	if ( inside == "" )
		return true; // empty means that there is no condition to apply.
	
	if ( inside.substr(0, 4 ) == "TYPE" ||
		inside.substr(0, 5 ) == "CLASS" ||
		inside.substr(0, 3 ) == "ISA" )
	{
		string::size_type pos = inside.rfind( "=" );
		if ( pos == string::npos ) 
			return false;
		bool isEquality = ( inside[ pos - 1 ] != '!' );
		string typeName = inside.substr( pos + 1 );
		if ( typeName == "membrane" )
			typeName = "Compartment";
		
		if ( inside.substr( 0, 5 ) == "CLASS" && typeName == "channel" )
			typeName = "HHChannel";
		
		bool isEqual;
		if ( inside.substr( 0, 3 ) == "ISA" ) {
			isEqual = id.element()->cinfo()->isA( typeName );
		} else {
			isEqual = ( typeName == id.element()->cinfo()->name() );
		}
		/*
		map< string, string >::const_iterator iter = classNameMap.find( typeName );
		if ( iter != classNameMap.end() )
			isEqual = ( iter->second == id()->className() );
		else
			isEqual = ( typeName == id()->className() );
			*/
		
		return ( isEqual == isEquality );
	} else if ( inside.substr( 0, 6 ) == "FIELD(" ) {
		if ( id.dataIndex == ALLDATA ) {
			return wildcardFieldComparison( id.id, inside.substr( 6 ) );
		} else {
			return wildcardFieldComparison( id, inside.substr( 6 ) );
		}
	}

	return false;
}

/**
 * Returns true if the name matches the wildcard string. Doesn't care about
 * following characters in 'name'. Single character wildcards are 
 * indicated with '?'
 */
bool alignedSingleWildcardMatch( const string& name, const string& wild )
{
	unsigned int len = wild.length();
	if ( name.length() < len )
		return false;
	for ( unsigned int i = 0; i < len; i++ ) {
		if ( wild[i] != '?' && name[i] != wild[i] )
			return false;
	}
	return true;
}

/**
 * Returns start index for match between string and wildcard using '?' to
 * indicate single character matches.
 * Scans forward along 'name' until it finds a match, or gives up.
 * The entire wildcard must be matched, otherwise returns ~0U
 * findWithSingleCharWildcard( "abc", 0, "a?c" ): return 0;
 * findWithSingleCharWildcard( "xyzabc", 1, "a?c" ): return 3;
 * findWithSingleCharWildcard( "xyzabb", 1, "a?c" ): return ~0U;
 */
unsigned int findWithSingleCharWildcard( 
				const string& name, unsigned int start, const string& wild )
{
	unsigned int len = wild.length();
	if ( len + start > name.length() )
		return ~0;
	unsigned int end = 1 + name.length() - len;
	for ( unsigned int i = start; i < end; ++i ) {
		if ( alignedSingleWildcardMatch( name.substr(i), wild ) )
			return i;
	}
	return ~0;
}

/**
 * matchBeforeBrace checks to see if the wildcard string 'name' matches
 * up with the name of the id.
 * Rules:
 *      # may be used at multiple places in the wildcard.
 *      It substitutes for any number of characters.
 *
 * 		? may be used any number of times in the wildcard, and
 * 		must substitute exactly for characters.
 *
 * 		If bracesInName, then the Id name itself includes braces.
 */
bool matchBeforeBrace( ObjId id, const string& wild )
{
	if ( wild == "#" || wild == "##" )
		return true;

	string ename = id.element()->getName();
	if ( wild == ename )
		return true;

	// Check if the wildcard string has any # or ? symbols.
	if ( wild.find_first_of( "#?" ) == string::npos )
		return false;

	// Break the 'wild' into the sections that must match, at the #s.
	// Then go through each of these sections doing a match to ename.
	// If not found, then return false.
	vector< string > chops;
	Shell::chopString( wild, chops, '#' );
	unsigned int prev = 0;
	unsigned int start = 0;

	for ( vector< string >::iterator 
				i = chops.begin(); i != chops.end(); ++i ) {
		start = findWithSingleCharWildcard( ename, prev, *i );
		if ( start == ~0U )
			return false;
		if ( prev == 0 && start > 0 && wild[0] != '#' )
			return false;
		prev = start + i->length();
	}
	return true;

	/*

	string::size_type pre = name.find( "#" );
	string::size_type post = name.rfind( "#" );

	// # may only be used once in the wildcard, but substitutes for any
	// number of characters.
	if ( pre != string::npos && post == pre ) {
		if ( ename.length() < ( name.length() - post - 1 ) )
				return false;
		unsigned int epos = ename.length() - ( name.length() - post - 1 );
		return ( name.substr( 0, pre ) == ename.substr( 0, pre ) &&
			name.substr( post + 1 ) == ename.substr( epos ) );
	}

	// ? may be used any number of times in the wildcard, and
	// must substitute exactly for characters.
	if ( name.length() != ename.length() )
		return 0;
	for ( unsigned int i = 0; i < name.length(); i++ )
		if ( name[i] != '?' && name[i] != ename[i] )
			return false;
	return true;
	*/
}

/**
 * Recursive function to compare all descendants and cram matches into ret.
 * Returns number of matches.
 */
int allChildren( ObjId start, 
	unsigned int index, const string& insideBrace, vector< ObjId >& ret )
{
	unsigned int nret = ret.size();
	vector< Id > kids;
	Neutral::children( start.eref(), kids );
	vector< Id >::iterator i;
	for ( i = kids.begin(); i != kids.end(); i++ ) {
		if ( i->element()->hasFields() ) {
			if ( matchInsideBrace( *i, insideBrace ) ) {
				if ( index == ALLDATA ) {
					ObjId oid( *i, start.dataIndex );
					ret.push_back( oid );
				} else if (index < i->element()->numField( start.dataIndex ) ) {
					ObjId oid( *i, start.dataIndex, index );
					ret.push_back( oid );
				}
			}
		} else {
			for ( unsigned int j = 0; j < i->element()->numData(); ++j )
		   	{
				ObjId oid( *i, j );
				allChildren( oid, index, insideBrace, ret );
				if ( (index == ALLDATA || index == j) && matchInsideBrace( oid, insideBrace ) )
					ret.push_back( oid );
			}
		}
	}
	return ret.size() - nret;
}

/**
 * This is the main recursive function of the wildcarding scheme.
 * It builds a wildcard list based on path. Puts found Ids into ret,
 * and returns # found.
 * The start ObjId is one that already matches.
 * depth is the position on the path.
 * This should work for multi-node wildcard searches since it only
 * refers to messaging and basic Element information that is present on
 * all nodes.
 */
int wildcardRelativeFind( ObjId start, const vector< string >& path, 
		unsigned int depth, vector< ObjId >& ret )
{
	int nret = 0;
	vector< ObjId > currentLevelIds;
	if ( depth == path.size() ) {
		if ( ret.size() == 0 || ret.back() != start ) {
			ret.push_back( start );
		}
		return 1;
	}

	if ( singleLevelWildcard( start, path[depth], currentLevelIds ) > 0 ) {
		vector< ObjId >::iterator i;
		for ( i = currentLevelIds.begin(); i != currentLevelIds.end(); ++i )
			nret += wildcardRelativeFind( *i, path, depth + 1, ret );
	}
	return nret;
}

void wildcardTestFunc( ObjId* elist, unsigned int ne, const string& path )
{
	vector< ObjId > ret;
	simpleWildcardFind( path, ret );
	if ( ne != ret.size() ) {
		cout << "!\nAssert	'" << path << "' : expected " <<
			ne << ", found " << ret.size() << "\n";
		assert( 0 );
	}
	sort( ret.begin(), ret.end() );
	for ( unsigned int i = 0; i < ne ; i++ ) {
		if ( elist[ i ] != ret[ i ] ) {
			cout << "!\nAssert	" << path << ": item " << i << 
				": " << elist[i].element()->getName() << " != " <<
					ret[i].element()->getName() << "\n";
			assert( 0 );
		}
	}
	cout << ".";
}

void testWildcard()
{
	unsigned long i;
	string bb;
	string ib;
	i = findBraceContent( "foo[23][TYPE=Compartment]", bb, ib );
	assert( bb == "foo" );
	assert( i == 23 );
	assert( ib == "TYPE=Compartment" );
	i = findBraceContent( "foo[][TYPE=Channel]", bb, ib );
	assert( i == ALLDATA );
	assert( bb == "foo" );
	assert( ib == "TYPE=Channel" );
	i = findBraceContent( "foo[TYPE=membrane]", bb, ib );
	assert( i == 0 );
	assert( bb == "foo" );
	assert( ib == "TYPE=membrane" );
	i = findBraceContent( "bar[]", bb, ib );
	assert( i == ALLDATA );
	assert( bb == "bar" );
	assert( ib == "" );
	i = findBraceContent( "zod[24]", bb, ib );
	assert( i == 24 );
	assert( bb == "zod" );
	assert( ib == "" );

	i = findBraceContent( "zod#", bb, ib );
	assert( i == ALLDATA );
	assert( bb == "zod#" );
	assert( ib == "" );
	i = findBraceContent( "zod#[]", bb, ib );
	assert( i == ALLDATA );
	assert( bb == "zod#" );
	assert( ib == "" );
	i = findBraceContent( "zod#[ISA=hippo]", bb, ib );
	assert( i == ALLDATA );
	assert( bb == "zod#" );
	assert( ib == "ISA=hippo" );
	i = findBraceContent( "zod#[3]", bb, ib );
	assert( i == 3 );
	assert( bb == "zod#" );
	assert( ib == "" );
	i = findBraceContent( "zod##", bb, ib );
	assert( i == ALLDATA );
	assert( bb == "zod##" );
	assert( ib == "" );

	bool ret = alignedSingleWildcardMatch( "a123456", "a123" );
	assert( ret == true );
	ret = alignedSingleWildcardMatch( "a123456", "1234" );
	assert( ret == false );
	ret = alignedSingleWildcardMatch( "a123456", "?1234" );
	assert( ret == true );
	ret = alignedSingleWildcardMatch( "a123456", "a????" );
	assert( ret == true );
	ret = alignedSingleWildcardMatch( "a123456", "??2??" );
	assert( ret == true );
	ret = alignedSingleWildcardMatch( "a123456", "??3??" );
	assert( ret == false );
	ret = alignedSingleWildcardMatch( "a1", "a?" );
	assert( ret == true );

	unsigned int j = findWithSingleCharWildcard( "a12345678", 0, "a123" );
	assert( j == 0 );
	j = findWithSingleCharWildcard( "a12345678", 0, "123" );
	assert( j == 1 );
	j = findWithSingleCharWildcard( "a12345678", 0, "?123" );
	assert( j == 0 );
	j = findWithSingleCharWildcard( "a12345678", 0, "?23456?" );
	assert( j == 1 );
	j = findWithSingleCharWildcard( "a12345678", 0, "??6?" );
	assert( j == 4 );

	Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
	Id a1 = shell->doCreate( "Neutral", Id(), "a1", 1 );
	Id c1 = shell->doCreate( "Arith", a1, "c1", 1 );
	Id c2 = shell->doCreate( "Arith", a1, "c2", 1 );
	Id c3 = shell->doCreate( "Arith", a1, "c3", 1 );
	Id cIndex = shell->doCreate( "Neutral", a1, "c4", 1 );
	Id c5 = shell->doCreate( "Neutral", a1, "Seg5_apical_1234_spine_234",1);

	ret = matchBeforeBrace( a1, "a1" );
	assert( ret );
	ret = matchBeforeBrace( a1, "a2" );
	assert( ret == 0 );
	ret = matchBeforeBrace( a1, "a?" );
	assert( ret == 1 );
	ret = matchBeforeBrace( a1, "?1" );
	assert( ret == 1 );
	ret = matchBeforeBrace( a1, "??" );
	assert( ret == 1 );
	ret = matchBeforeBrace( a1, "#" );
	assert( ret == 1 );
	ret = matchBeforeBrace( a1, "a#" );
	assert( ret == 1 );
	ret = matchBeforeBrace( a1, "#1" );
	assert( ret == 1 );

	ret = matchBeforeBrace( cIndex, "c4" );
	assert( ret == 1 );
	ret = matchBeforeBrace( cIndex, "##" );
	assert( ret == 1 );
	ret = matchBeforeBrace( cIndex, "#4" );
	assert( ret == 1 );
	ret = matchBeforeBrace( cIndex, "#" );
	assert( ret == 1 );
	ret = matchBeforeBrace( cIndex, "?4" );
	assert( ret == 1 );
	ret = matchBeforeBrace( cIndex, "c1" );
	assert( ret == 0 );
	ret = matchBeforeBrace( cIndex, "c?" );
	assert( ret == 1 );
	ret = matchBeforeBrace( cIndex, "??" );
	assert( ret == 1 );

	ret = matchBeforeBrace( c5, "Seg?_apical_#_spine_#" );
	assert( ret == true );
	ret = matchBeforeBrace( c5, "#Seg?_apical_#_spine_#" );
	assert( ret == true );
	ret = matchBeforeBrace( c5, "#?_apical_#_spine_#" );
	assert( ret == true );
	ret = matchBeforeBrace( c5, "#5_apical_#_spine_#" );
	assert( ret == true );
	ret = matchBeforeBrace( c5, "#e?5_apical_#_spine_#" );
	assert( ret == true );
	ret = matchBeforeBrace( c5, "#e5_apical_#_spine_#" );
	assert( ret == false );
	ret = matchBeforeBrace( c5, "#Seg5_apical_#_spine_#" );
	assert( ret == true );
	ret = matchBeforeBrace( c5, "#Seg#_apical_#_spine_#" );
	assert( ret == true );
	ret = matchBeforeBrace( c5, "Seg#_apical_#_spine_#" );
	assert( ret == true );
	ret = matchBeforeBrace( c5, "Seg#_a????l_#_spine_#" );
	assert( ret == true );
	ret = matchBeforeBrace( c5, "#?_a????l_#_spine_#" );
	assert( ret == true );
	ret = matchBeforeBrace( c5, "Seg?_a?????l_#_spine_#" );
	assert( ret == false );
	ret = matchBeforeBrace( c5, "Seg#_spine_#" );
	assert( ret == true );


	ret = matchInsideBrace( a1, "TYPE=Neutral" );
	assert( ret );
	ret = matchInsideBrace( a1, "TYPE==Neutral" );
	assert( ret );
	ret = matchInsideBrace( a1, "CLASS=Neutral" );
	assert( ret );
	ret = matchInsideBrace( a1, "ISA=Neutral" );
	assert( ret );
	ret = matchInsideBrace( a1, "CLASS=Neutral" );
	assert( ret );
	ret = matchInsideBrace( a1, "TYPE!=Channel" );
	assert( ret );
	ret = matchInsideBrace( a1, "CLASS!=Channel" );
	assert( ret );
	ret = matchInsideBrace( a1, "ISA!=Channel" );
	assert( ret );
	ret = matchInsideBrace( c3, "ISA==Neutral" ); // Everything is a Neutral
	assert( ret );
	ret = matchInsideBrace( c3, "ISA=Arith" );
	assert( ret );
	ret = matchInsideBrace( c3, "TYPE=membrane" );
	assert( !ret );

	Field<double>::set( ObjId( c3, 0 ), "outputValue", 123.5 );
	ret = matchInsideBrace( c3, "FIELD(outputValue)=123.5" );
	assert( ret );
	ret = matchInsideBrace( c3, "FIELD(outputValue)==123.5" );
	assert( ret );
	ret = matchInsideBrace( c3, "FIELD(outputValue)!=123.4" );
	assert( ret );
	ret = matchInsideBrace( c3, "FIELD(outputValue)>123.4" );
	assert( ret );
	ret = matchInsideBrace( c3, "FIELD(outputValue)<123.6" );
	assert( ret );
	ret = matchInsideBrace( c3, "FIELD(outputValue)>=123.4" );
	assert( ret );
	ret = matchInsideBrace( c3, "FIELD(outputValue)<=123.6" );
	assert( ret );
	ret = matchInsideBrace( c3, "FIELD(outputValue)>=123.5" );
	assert( ret );
	ret = matchInsideBrace( c3, "FIELD(outputValue)<=123.5" );
	assert( ret );
	ret = matchInsideBrace( c3, "FIELD(outputValue)==123.4" );
	assert( !ret );
	ret = matchInsideBrace( c3, "FIELD(outputValue)<123.4" );
	assert( !ret );


	ObjId el1[] = { ObjId(), a1, c1 };
	wildcardTestFunc( el1, 3, "/,/a1,/a1/c1" );
	ObjId el3[] = { c1, c2, c3, cIndex };
	wildcardTestFunc( el3, 4, "/a1/c#" );
	wildcardTestFunc( el3, 3, "/a1/c#[TYPE=Arith]" );

	ObjId el2[ 100 ];
	for ( i = 0 ; i < 100; i++ ) {
		char name[10];
		sprintf( name, "ch%ld", i );
		el2[i] = shell->doCreate( "Annotator", c1, name, 1 );
		//el2[i] = Neutral::create( "HHChannel", name, c1->id(), Id::scratchId() );
		Field< double >::set( ObjId( el2[i], 0 ), "z", i );
	}

	wildcardTestFunc( el2, 100, "/a1/c1/##" );
	wildcardTestFunc( el2, 100, "/a1/c1/#" );

	wildcardTestFunc( el2, 0, "/a1/##[TYPE=IntFire]" );
	wildcardTestFunc( el2, 100, "/a1/##[TYPE=Annotator]" );
	wildcardTestFunc( el2, 50, "/a1/c1/##[][FIELD(z)<50]" );

	// Here we set up some thoroughly ugly nesting.
	// Note the sequence: The wildcarding goes depth first,
	// and then in order of creation.
	ObjId el4[12];
	i = 0;
	el4[i] = shell->doCreate( "IntFire", el2[0], "g0", 1 ); ++i;
	el4[i] = shell->doCreate( "IntFire", el2[1], "g1", 1 ); ++i;
	el4[i] = shell->doCreate( "IntFire", el2[1], "g2", 1 ); ++i;
	el4[i] = shell->doCreate( "IntFire", el2[2], "g3", 1 ); ++i;
	el4[i] = shell->doCreate( "IntFire", el2[2], "g4", 1 ); ++i;
	el4[i] = shell->doCreate( "IntFire", el2[4], "g5", 1 ); ++i;
	el4[i] = shell->doCreate( "IntFire", el2[5], "g6", 1 ); ++i;
	el4[i] = shell->doCreate( "IntFire", el2[6], "g7", 1 ); ++i;
	el4[i] = shell->doCreate( "IntFire", el2[1], "g8", 1 ); ++i;
	el4[i] = shell->doCreate( "IntFire", el2[1], "g9", 1 ); ++i;
	el4[i] = shell->doCreate( "IntFire", c2, "g10", 1 ); ++i;
	el4[i] = shell->doCreate( "IntFire", c3, "g11", 1 ); ++i;

	wildcardTestFunc( el4, 12, "/a1/##[TYPE=IntFire]" );
	wildcardTestFunc( el4, 12, "/##[TYPE=IntFire]" );

	// Here I test wildcards with array Elements.
	Id x = shell->doCreate( "Arith", a1, "x", 5 );
	Id y = shell->doCreate( "Arith", ObjId( x, 2 ), "y", 5 );
	Id z = shell->doCreate( "Arith", ObjId( y, 3 ), "z", 5 );
	vector< ObjId > vec;
	simpleWildcardFind( "/a1/x[]/##", vec );
	assert( vec.size() == 10 );
	vec.clear();
	simpleWildcardFind( "/a1/x[]/##,/a1/x[]", vec );
	assert( vec.size() == 15 );
	vec.clear();
	simpleWildcardFind( "/a1/x[2]/y[]", vec );
	assert( vec.size() == 5 );

	// Here I test exclusive wildcards, should NOT get additional terms.
	Id xyzzy = shell->doCreate( "Arith", a1, "xyzzy", 5 );
	Id xdotp = shell->doCreate( "Arith", a1, "x.P", 5 );
	vec.clear();
	simpleWildcardFind( "/a1/x", vec );
	assert( vec.size() == 1 );
	vec.clear();
	simpleWildcardFind( "/a1/x[0]", vec );
	assert( vec.size() == 1 );
	vec.clear();
	simpleWildcardFind( "/a1/x[2]", vec );
	assert( vec.size() == 1 );
	vec.clear();
	simpleWildcardFind( "/a1/x[]", vec );
	assert( vec.size() == 5 );

	//a1.destroy();
	shell->doDelete( a1 );
	cout << "." << flush;
}

