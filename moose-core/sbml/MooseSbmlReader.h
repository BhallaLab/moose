/*******************************************************************
 * File:            moose::SbmlReader.h
 * Description:
 * Author:          HarshaRani G.V
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

#ifndef _MOOSESBMLREADER_H
#define _MOOSESBMLREADER_H
#ifdef USE_SBML

#include <sbml/SBMLTypes.h>

#include "../basecode/Id.h"
//class Id;
typedef struct {
    string enzyme;
    Id complex;
    vector<Id> substrates;
    vector<Id> products;
    double k1;
    double k2;
    double k3;
    int stage;
    string group;
    string xcord;
    string ycord;
} EnzymeInfo;

namespace moose{
class SbmlReader {
public:
    SbmlReader() {
        errorFlag_ = false;
    }
    ~SbmlReader() {
        ;
    }
    Id read(string filename,string location,string solverClass);
    map< string, Id> createCompartment(string location,Id parentId,string modelName,Id base_);
    typedef  map<string, Id> sbmlStr_mooseId;
    //typedef  map< string
    //                                    , tuple<string,double,bool>
    //                                    > sbmlId_convUnit;
    const sbmlStr_mooseId createMolecule(map<string,Id> &);
    void  createReaction(const sbmlStr_mooseId &);

private:
    bool errorFlag_;
    //sbmlId_convUnit poolMap_;
    Model* model_;
    SBMLDocument* document_;
    /* SBMLReader reader_; */
    map< string, Id >molSidMIdMap_;
    int noOfsub_;
    int noOfprd_;
    Id baseId;
    double transformUnits( double msize,UnitDefinition * ud,string type,bool hasonlySubUnit );
    double unitsforRates();
    void getRules();
    string nameString( string str );
    void printMembers( const ASTNode* p,vector <string> & ruleMembers );
    void addSubPrd(Reaction * reac,Id reaction_,string type);
    void getKLaw( KineticLaw * klaw,bool rev,vector< double > & rate ,string & amt_Conc);
    void pushParmstoVector( const ASTNode* p,vector <string> & parameters );
    void getParameters( const ASTNode* node,vector <string> & parameters );
    void setupMMEnzymeReaction( Reaction * reac,string id ,string name,const map<string, Id> &);
    pair<string, pair<string, string> > getAnnotation_Spe_Reac( XMLNode * annotationSpe_Rec );
    string getAnnotation( Reaction* reaction,map<string,EnzymeInfo> & );
    void setupEnzymaticReaction( const EnzymeInfo & einfo,string name,const map< string, Id > & ,string name1,string notes);
    void findModelParent( Id cwe, const string& path,Id& parentId, string& modelName );
    void getGlobalParameter();
};
} // namespace moose
#endif //USE_SBML
#endif // _MOOSESBMLREADER_H

