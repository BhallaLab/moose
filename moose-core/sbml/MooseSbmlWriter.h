/*******************************************************************
 * File:            MooseSbmlWriter.h
 * Description:      
 * Author:          HarsnaRani
 * E-mail:          hrani@ncbs.res.in
 ********************************************************************/
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2015 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _SBMLWRITER_H
#define _SBMLWRITER_H
#ifdef USE_SBML
#include <sbml/SBMLTypes.h>
namespace moose{
class SbmlWriter
{
		
	public:
		SbmlWriter() {;}
		~SbmlWriter() {;}
		int write( string location, string filename );
#ifdef USE_SBML
		void createModel( string filename, SBMLDocument& doc ,string target);
		bool validateModel(SBMLDocument* sbmlDoc );
		bool writeModel( const SBMLDocument* sbmlDoc, const string& filename );
		 
	private:
		vector < string >nameList_;
		Model* cremodel_;	
		string nameString( string str );
		string nameString1( string str );
		string changeName( string parent,string child );
		string idBeginWith(string name );
		string cleanNameId( Id itrid,int index);
		string parmUnit( double rct_order );
		void getSubPrd(Reaction* rec,string type,string enztype,Id itrRE, int index,ostringstream& rate_law,double &rct_order,bool w, string re_enClass);
		void getModifier(ModifierSpeciesReference* mspr,vector < Id> mod, int index,ostringstream& rate_law,double &rct_order,bool w);
		void printParameters( KineticLaw* kl,string k,double kvalue,string unit );
		string findNotes(Id itr);
		string getGroupinfo(Id itr);
#endif
};
} // namespace moose
//extern const Cinfo* initCinfo();
#endif //USE_SBML
#endif // _SBMLWRITER_H
