/*******************************************************************
 * File:            MooseSbmlReader.cpp
 * Description:
 * Author:          HarshaRani
 * E-mail:          hrani@ncbs.res.in
 ********************************************************************/
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2016 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
/****************************
* Change log:

* Originally created by Siji for l2v4 for 'trunk ' branch
* Modified / adapted Harsharani for both l2v4 and l3v1

hasOnlySubstanceUnit : false means if the compartment size is changed then it is assumed that its the concentration that must be updated
to account for the size change.

When importing SBML Level 2 models species for which the 
A. initial value is given as initialAmount and hasOnlySubstanceUnits is set to true 
(or compartment dimension is zero) are treated as amounts. 

B. If hasOnlySubstanceUnits is set to false 
but the initial value is given as amount the corresponding species are not converted to concentration, rather I substitue amount
as moose can take nInit. 

C. Species for which the initial value is given as initialConcentration are treated as concentrations by converted to milli

It is then assumed that the value of species appearing in the kinetic rate laws have either amount or concentration units. 

All rules are evaluated as given by the SBML model.
According to the SBML standard rate laws of reactions are assumed to deliver a rate in amount/time. 
In the case a species value is defined as concentration the rate law is converted to concentration/time.

In models that have only compartments of a constant size of 1 the distinction between amounts and concentrations is not necessary. 
***************/


#ifdef USE_SBML

#include <cmath>
#include <stdexcept>
#include <sbml/SBMLTypes.h>
#include <sbml/UnitDefinition.h>
#include <sbml/units/UnitFormulaFormatter.h>
#include <sbml/units/FormulaUnitsData.h>
#include <string>
#include <stdlib.h>
#include "header.h"
#include "../shell/Shell.h"
#include "../shell/Wildcard.h"
//#include "../manager/SimManager.h"
#include "MooseSbmlReader.h"
//#include "../kinetics/FuncPool.h"

using namespace std;
map< string,double > parmValueMap;
map< string,double> :: iterator pvm_iter;
bool unitsDefined = true;
/*  Harsha : TODO in
    -Compartment
      --Need to add group
      --Need to deal with compartment outside
    -Molecule
      -- Need to add group (done commited to 6964)
      -- Func pool and its math calculation need to be added.
    -Loading Model from SBML
      --Tested 1-30 testcase example model provided by l3v1 and l2v4 std.
        ---These are the models that worked (sbml testcase)1-6,10,14-15,17-21,23-25,34,35,58
	---Need to check
	 ----what to do when boundarycondition is true i.e.,
             differential equation derived from the reaction definitions
             should not be calculated for the species(7-9,11-13,16)
         ----kineticsLaw, Math fun has fraction,ceiling,reminder,power 28etc.
         ----Events to be added 26
	 ----initial Assisgment for compartment 27
         ----when stoichiometry is rational number 22
	 ---- For Michaelis¡VMenten kinetics km is not defined which is most of the case
	      need to calculate
*/

/**
 * @brief Reads a given SBML file and loads it into MOOSE.
 *
 * @param filename Name of file, std::string.
 * @param location 
 * @param solverClass
 *
 * @return  Id on success. Some expcetion on failure.
 */
Id moose::SbmlReader::read( string filename, string location, string solverClass) 
{   stringstream global_warning;
    FILE * fp = fopen( filename.c_str(), "r" );
    if ( fp == NULL) {
        stringstream ss;
        ss << "File " << filename << " does not exist." << endl;
        throw runtime_error(ss.str());
    }

    document_ = readSBML( filename.c_str() );
    unsigned num_errors = document_->getNumErrors();
    if ( num_errors > 0 ) {
        cerr << "Errors encountered while reading" << endl;
        document_->printErrors( cerr );
        errorFlag_ = true;
        return baseId;
    }
    model_= document_->getModel();
    if ( model_ == 0 ) {
        cout << "SBML: Error: No model present." << endl;
        errorFlag_ = true;
        return baseId;
    }
    if ( !errorFlag_ )
        getGlobalParameter();

    if ( !errorFlag_ ) {
        
        string modelName;
        Id parentId;
        findModelParent ( Id(), location, parentId, modelName ) ;
        Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
        Id baseId_ = s->doCreate( "Neutral", parentId, modelName, 1, MooseGlobal);
        Id base_ =s->doCreate("Neutral",baseId_,"model",1,MooseGlobal);
        assert( base_ != Id() );
        //Map Compartment's SBML id to Moose ID
        map< string,Id > comptSidMIdMap;
        // Map between Molecule's SBML id to which it belongs compartment Moose Id
        map< string, Id > molSidcmptMIdMap;

        if ( !errorFlag_ ){
            unitsDefined = true;
            comptSidMIdMap = createCompartment(location, parentId, modelName, base_);
            //comptUnitDefined is set true is checked if warning is set or not, only once its set.
            if (unitsDefined == false)
                global_warning << "The default volume unit has not been set in the model. "<<
                                "Assuming liter as the default volume unit, MOOSE will convert to cubicMeter which is the default units for volume in MOOSE. \n";
        }
        if ( !errorFlag_ ){
            unitsDefined = true;
            molSidcmptMIdMap = createMolecule( comptSidMIdMap);
            if (unitsDefined == false)
                //SpeciesUnitDefined is set true is checked if warning is set or not, only once its set.
                global_warning << "The default substance unit has not been set in the model. "<<
                                "Assuming mole as the default substance unit, MOOSE will convert to milliMolar which is the default units for Substabce in MOOSE  \n";
        }
        if ( !errorFlag_ )
            getRules();

        if ( !errorFlag_ )
            createReaction( molSidcmptMIdMap );
        // or we get
        //createReaction (result);

        if ( errorFlag_ )
            return baseId;
        else {
            // SimManager* sm = reinterpret_cast< SimManager* >(baseId.eref().data());
            //Shell* s = reinterpret_cast< Shell* >(baseId.eref().data());
            XMLNode * annotationNode = model_->getAnnotation();
            if( annotationNode != NULL ) {
                unsigned int num_children = annotationNode->getNumChildren();
                for( unsigned int child_no = 0; child_no < num_children; child_no++ ) {
                    XMLNode childNode = annotationNode->getChild( child_no );
                    if ( childNode.getPrefix() == "moose" && childNode.getName() == "ModelAnnotation" ) {
                        unsigned int num_gchildren = childNode.getNumChildren();
                        for( unsigned int gchild_no = 0; gchild_no < num_gchildren; gchild_no++ ) {
                            XMLNode &grandChildNode = childNode.getChild( gchild_no );
                            string nodeName = grandChildNode.getName();
                            if (grandChildNode.getNumChildren() == 1 ) {
                                string plotValue;
                                //double nodeValue;
                                if(nodeName == "plots") {
                                    Id graphs;
                                    // Carrying on with the policy that all graphs will be created under /modelName
                                    string datapath = baseId_.path() +"/data";
                                    Id graphpath(datapath);
                                    graphs = datapath;
                                    graphs = s->doCreate("Neutral",baseId_,"data",1);
                                    assert(graphs != Id());
                                    Id graph;
                                    string datagrph = graphs.path()+"/graph_1";
                                    Id graph1(datagrph);
                                    graph = s->doCreate("Neutral",graphs,"graph_0",1);
                                    assert(graph != Id());
                                    /*
                                    // if plots exist then will be placing at "/data"
                                    Id graphs;
                                    //Id dataId;
                                    Id dataIdTest;
                                    if (parentId2 == Id())
                                        graphs = s->doCreate( "Neutral", parentId2, "data", 1);
                                    else
                                        // need to check how to put / while coming from gui as the path is /model/modelName??? 27 jun 2014
                                        findModelParent ( Id(), modelName, dataIdTest, modelName ) ;
                                        string test = "/data";
                                        Id tgraphs(test);
                                        graphs=tgraphs;
                                        //graphs = s->doCreate("Neutral",parentId,"data",1);
                                        //Id dataId;
                                        //if (dataId == Id())
                                        //    cout << "Id " << dataId;
                                        //    graphs = s->doCreate( "Neutral",dataId, "data", 1);
                                        assert( graphs != Id() );
                                        */  
                                    plotValue = (grandChildNode.getChild(0).toXMLString()).c_str();
                                    istringstream pltVal(plotValue);
                                    string pltClean;
                                    while (getline(pltVal,pltClean, ';')) {
                                        pltClean.erase( remove( pltClean.begin(), pltClean.end(), ' ' ), pltClean.end() );
                                        //string plotPath = location+pltClean;
                                        string plotPath = base_.path()+pltClean;
                                        Id plotSId(plotPath);
                                        size_t pos = pltClean.find('/');
                                        if (pos != std::string::npos)
                                            pltClean = pltClean.substr(pos+1,pltClean.length());
                                        /*
                                        #Harsha:To create a tableName, e.g:'/compartmentName/groupName/ObjectName'
                                        #       I have changed '/' to '@' and To keep the index of the ObjectName
                                        #       I have changed '[' to '<' and ']' to '>'.
                                        #       The same is follwed in the GUI
                                        */
                                        replace(pltClean.begin(),pltClean.end(),'/','@');
                                        replace(pltClean.begin(),pltClean.end(),'[','<');
                                        replace(pltClean.begin(),pltClean.end(),']','>');
                                        // size_t Iindex = 0;
                                        // while(true)
                                        //     { size_t sindex = pltClean.find('[',Iindex);
                                        //       size_t eindex = pltClean.find(']',Iindex);
                                        //       if (sindex == std::string::npos) break;
                                        //       pltClean.erase(sindex,eindex-sindex+1);
                                        //       Iindex = eindex;
                                        //     } //while true
                                        string plotName =  pltClean + ".conc";
                                        Id pltPath(graph.path());
                                        Id tab = s->doCreate( "Table2", pltPath, plotName, 1 );
                                        if (tab != Id())
                                            s->doAddMsg("Single",tab,"requestOut",plotSId,"getConc");
                                    }//while
                                    /* passing /model and /data to clocks         */
                                    //commented due to automatic scheduling
                                    
                                    /*
                                    string comptPath =base_.path()+"/##";
                                    s->doUseClock(comptPath,"process",4);

                                    string tablePath = graphs.path()+"/##[TYPE=Table]";
                                    s->doUseClock( tablePath, "process",8 );
                                    */
                                }//plots
                                /*else
                                  nodeValue = atof ((grandChildNode.getChild(0).toXMLString()).c_str());

                                  if (nodeName == "runTime")
                                  sm->setRunTime(nodeValue);
                                  else if (nodeName == "simdt")
                                  sm->setSimDt(nodeValue);
                                  else if(nodeName == "plotdt")
                                  sm->setPlotDt(nodeValue);
                                  */

                            } //grandChild
                            else
                                cout << "Warning: expected exactly ONE child of " << nodeName << " but none found "<<endl;
                        } //gchild
                    } //moose and modelAnnotation
                }
            }//annotation Node
            else {
                //4 for simdt and 8 for plotdt
                //Harsha:Since scheduling is automatically done commeting this
                
                //s->doUseClock(base_.path()+"/##","process",4);
                //s->doUseClock(+"/data/##[TYPE=Table]","process",8);
                //s->doSetClock(4,0.1);
                //s->doSetClock(8,0.1);
                /*
                s->doUseClock( "/data/##[TYPE=Table]", "proc", 16 );
                double simdt = 0.1;
                double plotdt = 1;
                s->doSetClock( 11, simdt );
                s->doSetClock( 12, simdt );
                s->doSetClock( 13, simdt );
                s->doSetClock( 14, simdt );
                s->doSetClock( 16, plotdt );
                s->doSetClock( 17, plotdt );
                s->doSetClock( 18, plotdt );
                */
            }
            vector< ObjId > compts;
            string comptpath = base_.path()+"/##[ISA=ChemCompt]";
            wildcardFind( comptpath, compts );
            vector< ObjId >::iterator i = compts.begin();
            string comptName = nameString(Field<string> :: get(ObjId(*i),"name"));
            string simpath = base_.path() + "/##";
            //s->doUseClock( simpath, "process", 4 );

            //wildcardFind( plotpath, plots );
            //Id pathexist(base_.path()+"/kinetics");
            /*
               if (solverClass.empty())
               {
               if( pathexist != Id())
               sm->build(base_.eref(),&q,"rk5");
               else
               sm->buildForSBML(base_.eref(),&q,"rk5");
               }
               else
               { if(pathexist != Id())
               sm->build(base_.eref(),&q,solverClass);
               else
               sm->buildForSBML(base_.eref(),&q,solverClass);
               }
               */
            //cout << "base_ " <<base_.path() << "baseId_ " << baseId_.path();
            return baseId_;
        }

    } else
        return baseId;
}

/**
 * @brief Map SBML compartments to MOOSE.
 *
 * @param location 
 * @param parentId string. Name of parent compartment.
 * @param modelName string. Name of model.
 * @param base_ Id, Id of parent.
 *
 * @return std::map<string, Id>.
 */
map< string,Id > moose::SbmlReader::createCompartment(string location, Id parentId, string modelName, Id base_) 
{
    /* In compartment: pending to add
       -- the group
       -- outside    
       -- units of volume
    */
    Shell* s = reinterpret_cast< Shell* >( Id().eref().data() );
    map< string,Id > comptSidMIdMap;
    map< string,string > outsideMap;
    map< string,string > ::iterator iter;
    double msize = 0.0, size = 0.0;

    ::Compartment* compt;
    unsigned int num_compts = model_->getNumCompartments();

    if (num_compts == 0) {
        errorFlag_ = true;
        stringstream ss;
        return comptSidMIdMap;
    }

    baseId = base_;
    for ( unsigned int i = 0; i < num_compts; i++ ) {
        compt = model_->getCompartment(i);
        std::string id = "";
        if ( compt->isSetId() ) {
            id = compt->getId();
        }

        std::string name = "";
        if ( compt->isSetName() ) {
            name = compt->getName();
            name = nameString(name);
        }

        std::string outside = "";
        if ( compt->isSetOutside() ) {
            outside = compt->getOutside ();
        }
        if ( compt->isSetSize() ) {
            msize = compt->getSize();
        }

        UnitDefinition * ud = compt->getDerivedUnitDefinition();
        size = transformUnits( msize,ud , "compartment",0);
        unsigned int dimension = compt->getSpatialDimensions();

        if (dimension < 3)
            cout << "\n ###### Spatial Dimension is " << dimension <<" volume should not be converted from liter to cubicmeter which is happening as default check \n";

        if(name.empty() && id.empty())
            cout <<  "Compartment name and id are empty" << endl;

        if (name.empty()) {
            if(! id.empty() )
                name = id;
        }

        Id compt = s->doCreate( "CubeMesh", base_, name,  1);
        comptSidMIdMap[id] = compt;
        if (size != 0.0)
            Field< double >::set( compt, "volume", size );
        if (dimension != 0)
            continue;
        //Field < int > :: set(compt, "numDimensions", dimension);
    }
    return comptSidMIdMap;
}

/* create MOLECULE  */
const moose::SbmlReader::sbmlStr_mooseId moose::SbmlReader::createMolecule( map< string,Id > &comptSidMIdMap) {
    Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
    map< string, Id >molSidcmptMIdMap;
    double transvalue = 0.0;
    int num_species = model_->getNumSpecies();
    if (num_species == 0) {
        baseId = Id();
        errorFlag_ = true;
        return molSidcmptMIdMap;
    }

    for ( int sindex = 0; sindex < num_species; sindex++ ) {
        Species* spe = model_->getSpecies(sindex);
        
        if (!spe) {
            continue;
        }
        std::string compt = "";
        if ( spe->isSetCompartment() ) {
            compt = spe->getCompartment();
        }
        if (compt.length()< 1) {
            //cout << "compt is empty for species "<< sindex << endl;
            continue;
        }
        string id = spe->getId();
        if (id.length() < 1) {
            continue;
        }
        std::string name = "";
        if ( spe->isSetName() ) {
            name = spe->getName();
            name = nameString(name);
        }
        if (name.empty())
            name = id;
        string speciesNotes = "";
        if (spe->isSetNotes())
        {
            XMLNode* xnode = spe->getNotes();
            string testnotes = spe->getNotesString();
            XMLNode nodec = xnode->getChild(0);
            XMLNode tnodec = nodec.getChild(0);
            speciesNotes = tnodec.getCharacters();
        }

        Id comptEl = comptSidMIdMap[compt];
        Id meshEntry = Neutral::child( comptEl.eref(), "mesh" );
        string comptPath = Field<string> :: get(comptEl,"path");

        // Get groupName if exist in annotation (in case of Genesis)
        XMLNode * annotationSpe = spe->getAnnotation();
        pair<string,pair<string, string> > group = getAnnotation_Spe_Reac(annotationSpe);
        string groupName = group.first;
        string xCord = group.second.first;
        string yCord = group.second.second;
        string groupString = comptPath+'/'+groupName;

        Id groupId;
        if (!groupName.empty())
        {   groupId = Id( comptPath + "/"+groupName );
            if ( groupId == Id() ) 
                groupId = shell->doCreate( "Neutral", comptEl, groupName, 1 );
            assert( groupId != Id() );
            
        }
        bool constant = spe->getConstant();
        bool boundaryCondition = spe->getBoundaryCondition();
        // if (boundaryCondition == true)
        //     cout << name << " species having BoundaryCondition true " <<endl;
        Id pool;
        //If constant or boundary condition is true then its equivalent to BuffPool in moose
        if (boundaryCondition == true)
            //if( (boundaryCondition == true) && (constant==false))
            if (groupId == Id())
                pool = shell->doCreate("BufPool",comptEl,name,1);
            else
                pool = shell->doCreate("BufPool",groupId,name,1);
            
        else
            if (groupId == Id())
                pool = shell->doCreate("Pool", comptEl, name ,1);
            
            else
                pool = shell->doCreate("Pool", groupId, name ,1);
                
        molSidcmptMIdMap[id] = comptEl;
        if(pool != Id())
        {   
            //Map to Molecule SBML id to Moose Id
            molSidMIdMap_[id] = pool;

            //shell->doAddMsg( "OneToOne",pool, "mesh", meshEntry, "mesh" );
            bool bcondition = spe->getBoundaryCondition();
            if ( constant == true && bcondition == false)
                cout <<"The species "<< name << " should not appear in reactant or product as per sbml Rules"<< endl;

            unsigned int spatialDimen =Field< unsigned int >::get( comptEl, "numDimensions");

            UnitDefinition * ud = spe->getDerivedUnitDefinition();
            assert(ud != NULL);
            double initvalue = 0.0;
            bool hasonlySubUnit = spe->getHasOnlySubstanceUnits();
            transvalue = transformUnits(1,ud,"substance",hasonlySubUnit);
            
            if ( spe->isSetInitialConcentration() ) {
                initvalue = spe->getInitialConcentration();
                //transValue will take care of multiplying any units are defined
                // pow(1e3) will make sure the concentration Unit are converted from mole to milliMolar (Molar = mole/size)
                initvalue = initvalue * transvalue * pow(1e+3,1);
                Field <double> :: set(pool, "concInit",initvalue);
            }
            else if ( spe->isSetInitialAmount() ) {
                initvalue = spe->getInitialAmount();   
                //If Amount is set then moose is capable of populating number nInit
                // hasonlySubstanceUnit is not checked, if we populate nInit then
                //moose automatically calculate the concInit.
                //transValue will take care of multiplying any units are defined
                // pow(NA) the avogadro's number to convert to number #
                initvalue = initvalue * transvalue * pow(NA,1);
                Field < double> :: set( pool, "nInit", initvalue);     
            }
            else {
                unsigned int nr = model_->getNumRules();
                bool found = false;
                for ( unsigned int r = 0; r < nr; r++ ) {
                    Rule * rule = model_->getRule(r);
                    bool assignRule = rule->isAssignment();
                    if ( assignRule ) {
                        string rule_variable = rule->getVariable();
                        if (rule_variable.compare(id) == 0) {
                            found = true;
                            break;
                        }
                    }
                }
                if (found == false) {
                    cout << "Invalid SBML: Either initialConcentration or initialAmount must be set or it should be found in assignmentRule but non happening for " << spe->getName() <<endl;
                    return molSidcmptMIdMap;
                }
            }
        
        if (!xCord.empty() and !yCord.empty()) {
            Id poolInfo;
            string poolPath = Field<string> :: get(pool,"path");
            poolInfo = Id( poolPath + "/info");
            if ( poolInfo == Id() ) 
                poolInfo = shell->doCreate( "Annotator", pool, "info", 1 );
            assert( poolInfo != Id() );
            double x = atof( xCord.c_str() );
            double y = atof( yCord.c_str() );
            Field< double >::set( poolInfo, "x", x );
            Field< double >::set( poolInfo, "y", y );
            }
        if (!speciesNotes.empty()){
            Id poolInfo;
            string poolPath = Field<string> :: get(pool,"path");
            poolInfo = Id( poolPath + "/info");
            if ( poolInfo == Id() ) 
                poolInfo = shell->doCreate( "Annotator", pool, "info", 1 );
            assert( poolInfo != Id() );
            speciesNotes.erase(std::remove(speciesNotes.begin(), speciesNotes.end(), '\n'), speciesNotes.end());
            speciesNotes.erase(std::remove(speciesNotes.begin(), speciesNotes.end(), '\t'), speciesNotes.end());
            Field< string >::set( poolInfo, "notes", speciesNotes );
            }
        }//Pool_ != Id()
    }
    return molSidcmptMIdMap;
}

/* Assignment Rule */

void moose::SbmlReader::getRules() {
    unsigned int nr = model_->getNumRules();
    //if (nr > 0)
    //  cout << "\n ##### Need to populate funcpool and sumtotal which is pending due to equations \n";
    Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
    for ( unsigned int r = 0; r < nr; r++ ) {
        Rule * rule = model_->getRule(r);
        bool assignRule = rule->isAssignment();
        if ( assignRule ) {
            string rule_variable = rule->getVariable();
            map< string,Id >::iterator v_iter;
            map< string,Id >::iterator m_iter;
            v_iter = molSidMIdMap_.find( rule_variable );
            if (v_iter != molSidMIdMap_.end()) {
                Id rVariable = molSidMIdMap_.find(rule_variable)->second;
                //string rstring =molSidMIdMap_.find(rule_variable)->first;
                //Id sumId = shell->doCreate( "SumFunc", rVariable, "func", 1 );
                Id sumId = shell->doCreate( "Function", rVariable, "func", 1 );
                //rVariable.element()->zombieSwap( FuncPool::initCinfo() );
                //ObjId ret = shell->doAddMsg( "single",
                //                             ObjId( sumId, 0 ), "output",
                //                             ObjId( rVariable, 0 ), "input" );
                ObjId ret = shell->doAddMsg( "single",
                                             ObjId( sumId, 0 ), "valueOut",
                                             ObjId( rVariable, 0 ), "setN" );
                assert( ret != ObjId() );
                const ASTNode * ast = rule->getMath();
                vector< string > ruleMembers;
                ruleMembers.clear();
                printMembers( ast,ruleMembers );
                string rulePar = "";
                string comma = "";
                for ( unsigned int rm = 0; rm < ruleMembers.size(); rm++ ) {
                    m_iter = molSidMIdMap_.find( ruleMembers[rm] );
                    if ( m_iter != molSidMIdMap_.end() ) {
                        Id rMember = molSidMIdMap_.find(ruleMembers[rm])->second;
                        string rMember_str = molSidMIdMap_.find(ruleMembers[rm])->first;
                        unsigned int numVars = Field< unsigned int >::get( sumId, "numVars" );
                        ObjId xi( sumId.value() + 1, 0, numVars );
                        Field< unsigned int >::set( sumId, "numVars", numVars + 1 );
                        // ObjId ret = shell_->doAddMsg( "single", ObjId( srcId, 0 ), "nOut", xi, "input" ); 
                        ObjId ret = shell->doAddMsg( "single",
                                                     ObjId( rMember, 0 ), "nOut",
                                                     xi, "input" );

                        // ObjId ret = shell->doAddMsg( "single",
                        //                              ObjId( rMember, 0 ), "nOut",
                        //                              ObjId( sumId, 0 ), "input" );
                        string test = molSidMIdMap_.find(ruleMembers[rm])->first;
                    stringstream ss;
                    for ( unsigned int i = 0; i < numVars; ++i ) 
                        {
                            ss << "x" << i << "+";
                        }
                    ss << "x" << numVars;
                    Field< string >::set( sumId, "expr", ss.str() );
                    } 
                    else {
                        
                        rulePar +=  comma;
                        rulePar += ruleMembers[rm];
                        comma = ',';
                        // In assignment rule there are constants instead of molecule which is yet to deal in moose.
                        errorFlag_ = true;
                    }
                }
                if (!rulePar.empty())
                {   string t = "moose::SbmlReader::getRules: Assignment rule \"";
                    t += rule_variable;
                    t += "\" variable member \"";
                    t += rulePar;
                    t += "\" is not a species not handle in moose";
                    cerr << t<< endl;
                }

            }
        }
        bool rateRule = rule->isRate();
        if ( rateRule ) {
            string rule_variable1 = rule->getVariable();
            cout << "Warning : For now this \"" << rule_variable1 << "\" rate Rule is not handled in moose "  << endl;
            errorFlag_ = true;
        }
        bool  algebRule = rule->isAlgebraic();
        if ( algebRule ) {
            string rule_variable1 = rule->getVariable();
            cout << "Warning: For now this " << rule_variable1 <<" Algebraic Rule is not handled in moose" << endl;
            errorFlag_ = true;
        }
    }
}

//REACTION

void moose::SbmlReader::createReaction(const map< string, Id > &molSidcmptMIdMap ) {
    Reaction* reac;

    map< string,double > rctMap;
    map< string,double >::iterator rctMap_iter;
    map< string,double >prdMap;
    map< string,double >::iterator prdMap_iter;
    map< string,EnzymeInfo >enzInfoMap;

    for ( unsigned int r = 0; r < model_->getNumReactions(); r++ ) {
        Id reaction_;
        reac = model_->getReaction( r );
        noOfsub_ = 0;
        noOfprd_ = 0;
        std:: string id; //=reac->getId();
        if ( reac->isSetId() )
            id = reac->getId();

        std::string name;
        if ( reac->isSetName() ) {
            name = reac->getName();
            name = nameString(name);
        }
        if (name.empty()) {
            if (id.empty())
                assert("Reaction id and name is empty");
            else
                name = id;
        }
        string grpname = getAnnotation( reac,enzInfoMap );
        string reactionNotes = "";
       if (reac->isSetNotes())
        {
            XMLNode* xnode = reac->getNotes();
            string testnotes = reac->getNotesString();
            XMLNode nodec = xnode->getChild(0);
            XMLNode tnodec = nodec.getChild(0);
            reactionNotes = tnodec.getCharacters();
        }

       if ( (grpname != "") && (enzInfoMap[grpname].stage == 3) )
            setupEnzymaticReaction( enzInfoMap[grpname],grpname ,molSidcmptMIdMap,name, reactionNotes);
        
        // if (grpname != "")
        // {
        // cout << "\n enz matic reaction " << enzInfoMap[grpname].stage;
        // setupEnzymaticReaction( enzInfoMap[grpname],grpname ,molSidcmptMIdMap,name);
        // }

        else if ( grpname == "" ) {
            if (reac->getNumModifiers() > 0)
                setupMMEnzymeReaction( reac,id,name ,molSidcmptMIdMap);
            else {
                bool rev=reac->getReversible();
                bool fast=reac->getFast();
                if ( fast ) {
                    cout<<"warning: for now fast attribute is not handled"<<endl;
                    errorFlag_ = true;
                }
                // Get groupName if exist in annotation (in case of Genesis)
                XMLNode * annotationRea = reac->getAnnotation();
                //string groupName = getAnnotation_Spe_Reac(annotationRea);
                pair<string, pair<string,string> > group = getAnnotation_Spe_Reac(annotationRea);
                string groupName = group.first;
                string xCord = group.second.first;
                string yCord = group.second.second;
                
                int numRcts = reac->getNumReactants();
                int numPdts = reac->getNumProducts();
                if (numRcts != 0 )
                {  // In moose, reactions compartment are decided from first Substrate compartment info
                   // substrate is missing then I get compartment from Reaction itself.
                   // Ideally  that reaction should not be created but at this point I am putting a warning that substrate is missing
                    const SpeciesReference* rect=reac->getReactant(0);
                    std::string sp=rect->getSpecies();
                    Id comptRef = molSidcmptMIdMap.find(sp)->second; //gives compartment of sp
                    Id meshEntry = Neutral::child( comptRef.eref(), "mesh" );
                    Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
                    string comptPath = Field<string> :: get(comptRef,"path");
                    string groupString = comptPath+'/'+groupName;
                    Id groupId;
                    if (!groupName.empty())
                    {   groupId = Id( comptPath + "/"+groupName );
                        if ( groupId == Id() ) 
                            groupId = shell->doCreate( "Neutral", comptRef, groupName, 1 );
                        assert( groupId != Id() );
                    }
                    if (groupId == Id())
                        reaction_ = shell->doCreate("Reac", comptRef, name, 1);
                    else
                        reaction_ = shell->doCreate("Reac", groupId, name, 1);

                    //shell->doAddMsg( "Single", meshEntry, "remeshReacs", reaction_, "remesh");
                    //Get Substrate
                    if (numRcts != 0)
                        addSubPrd(reac,reaction_,"sub");
                    //Get Product
                    if (numPdts != 0)
                        addSubPrd(reac,reaction_,"prd");

                    if (!xCord.empty() and !yCord.empty())
                    {  Id reacInfo;
                        string reacPath = Field<string> :: get(reaction_,"path");
                        reacInfo = Id( reacPath + "/info");
                        if ( reacInfo == Id() ) 
                            reacInfo = shell->doCreate( "Annotator", reaction_, "info", 1 );
                        assert( reacInfo != Id() );
                        double x = atof( xCord.c_str() );
                        double y = atof( yCord.c_str() );
                        Field< double >::set( reacInfo, "x", x );
                        Field< double >::set( reacInfo, "y", y );
                    }
                    if (!reactionNotes.empty())
                    {   Id reacInfo;
                        string reacPath = Field<string> :: get(reaction_,"path");
                        reacInfo = Id( reacPath + "/info");
                        if ( reacInfo == Id() ) 
                            reacInfo = shell->doCreate( "Annotator", reaction_, "info", 1 );
                        assert( reacInfo != Id() );
                        reactionNotes.erase(std::remove(reactionNotes.begin(), reactionNotes.end(), '\n'), reactionNotes.end());
                        reactionNotes.erase(std::remove(reactionNotes.begin(), reactionNotes.end(), '\t'), reactionNotes.end());
                        Field< string >::set( reacInfo, "notes", reactionNotes );
                    }
                    if ( reac->isSetKineticLaw() ) {
                        KineticLaw * klaw=reac->getKineticLaw();
                        //vector< double > rate = getKLaw( klaw,rev );
                        vector< double > rate;
                        rate.clear();
                        string amt_Conc;
                        getKLaw( klaw,rev,rate,amt_Conc );
                        if ( errorFlag_ )
                            return;
                        else if ( !errorFlag_ ) {
                            if (amt_Conc == "amount")
                            {   Field < double > :: set( reaction_, "numKf", rate[0] );
                                Field < double > :: set( reaction_, "numKb", rate[1] );
                            }
                            else if (amt_Conc == "concentration")
                            {   Field < double > :: set( reaction_, "Kf", rate[0] );
                                Field < double > :: set( reaction_, "Kb", rate[1] );
                            }
                            /*if (numRcts > 1)
                            rate[0] = rate[0]*pow(1e3,1.0);
                                 Field < double > :: set( reaction_, "Kf", rate[0] );
                                 Field < double > :: set( reaction_, "Kb", rate[1] );
                                 */
                        }
                    } //issetKineticLaw
                }
                else
                    cout << " Warning:Reaction \" " << name << "\" doesnot have substrate, this is not read into moose"<<endl;
            } //else
        } // else grpname == ""
    }//for unsigned
} //reaction

/* Enzymatic Reaction  */
void moose::SbmlReader::setupEnzymaticReaction( const EnzymeInfo & einfo,string enzname, const map< string, Id > &molSidcmptMIdMap,string name1,string enzNotes) {
    string enzPool = einfo.enzyme;
    Id comptRef = molSidcmptMIdMap.find(enzPool)->second; //gives compartment of sp
    Id meshEntry = Neutral::child( comptRef.eref(), "mesh" );
    Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
    string xCord = einfo.xcord;
    string yCord = einfo.ycord;
    //Creating enz pool to enzyme site
    Id enzPoolId = molSidMIdMap_.find(enzPool)->second;

    string enzparentpath = Field<string> :: get(enzPoolId,"path");
    Id enzId = Id( enzparentpath + "/"+name1 );
    
    Id enzyme_ = shell->doCreate("Enz", enzPoolId, name1, 1);
    //shell->doAddMsg( "Single", meshEntry, "remeshReacs", enzyme_, "remesh");

    if (enzyme_ != Id())
    {
        Id complex = einfo.complex;
        string clxpath = Field<string> :: get(complex,"path");
        //Moving enzyme site under enzyme
        shell->doMove(complex,enzyme_);
        shell->doAddMsg("OneToAll",enzyme_,"cplx",complex,"reac");

        shell->doAddMsg("OneToOne",enzyme_,"enz",enzPoolId,"reac");

        vector< Id >::const_iterator sub_itr;
        for ( sub_itr = einfo.substrates.begin(); sub_itr != einfo.substrates.end(); sub_itr++ ) {
            Id S = (*sub_itr);
            Id b = shell->doAddMsg( "OneToOne", enzyme_, "sub" ,S , "reac" );
        }

        vector< Id >::const_iterator prd_itr;
        for ( prd_itr = einfo.products.begin(); prd_itr != einfo.products.end(); prd_itr++ ) {
            Id P = (*prd_itr);
            shell->doAddMsg ("OneToOne",enzyme_,"prd", P,"reac");
        }
        // populate k3,k2,k1 in this order only.
        Field < double > :: set( enzyme_, "k3", einfo.k3 );
        Field < double > :: set( enzyme_, "k2", einfo.k2 );
        Field < double > :: set( enzyme_, "k1", einfo.k1 );
        if (!xCord.empty() and !yCord.empty())
        {  Id enzInfo;
            string enzPath = Field<string> :: get(enzyme_,"path");
            enzInfo = Id( enzPath + "/info");
            if ( enzInfo == Id() ) 
                enzInfo = shell->doCreate( "Annotator", enzyme_, "info", 1 );
            assert( enzInfo != Id() );
            double x = atof( xCord.c_str() );
            double y = atof( yCord.c_str() );
            Field< double >::set( enzInfo, "x", x );
            Field< double >::set( enzInfo, "y", y );
        } //xCord.empty
        if (!enzNotes.empty())
        {  Id enzInfo;
            string enzPath = Field<string> :: get(enzyme_,"path");
            enzInfo = Id( enzPath + "/info");
            if ( enzInfo == Id() ) 
                enzInfo = shell->doCreate( "Annotator", enzyme_, "info", 1 );
            assert( enzInfo != Id() );
            enzNotes.erase(std::remove(enzNotes.begin(), enzNotes.end(), '\n'), enzNotes.end());
            enzNotes.erase(std::remove(enzNotes.begin(), enzNotes.end(), '\t'), enzNotes.end());
            Field< string >::set( enzInfo, "notes", enzNotes );
        } //xCord.empty
    }//enzyme_
}

/*  get annotation  */
pair<string, pair< string, string> > moose::SbmlReader :: getAnnotation_Spe_Reac(XMLNode * annotationSpe_Rec)
{   string groupName = "";
    string xcord = "";
    string ycord = "";
    //XMLNode * annotationSpe_Rec = spe_rec->getAnnotation();
    if( annotationSpe_Rec != NULL ) {
        unsigned int num_children = annotationSpe_Rec->getNumChildren();
        for( unsigned int child_no = 0; child_no < num_children; child_no++ ) {
            XMLNode childNode = annotationSpe_Rec->getChild( child_no );
            if ( childNode.getPrefix() == "moose" && childNode.getName() == "ModelAnnotation" ) {
                unsigned int num_gchildren = childNode.getNumChildren();
                for( unsigned int gchild_no = 0; gchild_no < num_gchildren; gchild_no++ ) {
                    XMLNode &grandChildNode = childNode.getChild( gchild_no );
                    string nodeName = grandChildNode.getName();
                    if (nodeName == "Group")
                    {   groupName = (grandChildNode.getChild(0).toXMLString()).c_str();
                        //group = shell->doCreate( "Neutral", mgr, "groups", 1, MooseGlobal );
                        // assert( group != Id() );
                    }
                    else if (nodeName == "xCord")
                        xcord = (grandChildNode.getChild(0).toXMLString()).c_str();
                    else if (nodeName == "yCord")
                        ycord = (grandChildNode.getChild(0).toXMLString()).c_str();
                    
                } //gchild
            } //moose and modelAnnotation
        } //child
        }//annotation Node
    return make_pair(groupName, make_pair(xcord,ycord));
}
string moose::SbmlReader::getAnnotation( Reaction* reaction,map<string,EnzymeInfo> &enzInfoMap ) {
    XMLNode * annotationNode = reaction->getAnnotation();
    EnzymeInfo einfo;
    string grpname = "",stage,group;
    string xcord,ycord;

    if( annotationNode != NULL ) {
        unsigned int num_children = annotationNode->getNumChildren();
        for( unsigned int child_no = 0; child_no < num_children; child_no++ ) {
            XMLNode childNode = annotationNode->getChild( child_no );
            if ( childNode.getPrefix() == "moose" && childNode.getName() == "EnzymaticReaction" ) {
                unsigned int num_gchildren = childNode.getNumChildren();
                for( unsigned int gchild_no = 0; gchild_no < num_gchildren; gchild_no++ ) {
                    XMLNode &grandChildNode = childNode.getChild( gchild_no );
                    string nodeName = grandChildNode.getName();
                    string nodeValue;
                    if (grandChildNode.getNumChildren() == 1 ) {
                        nodeValue = grandChildNode.getChild(0).toXMLString();

                    } else {
                        cout << "Error: expected exactly ONE child of " << nodeName << endl;
                    }
                    if ( nodeName == "enzyme" )
                        einfo.enzyme = molSidMIdMap_.find(nodeValue)->first;

                    else if ( nodeName == "complex" )
                        einfo.complex=molSidMIdMap_.find(nodeValue)->second;

                    else if ( nodeName == "substrates") {
                        Id elem = molSidMIdMap_.find(nodeValue)->second;
                        einfo.substrates.push_back(elem);
                    } 
                    else if ( nodeName == "product" ) {
                        Id elem = molSidMIdMap_.find(nodeValue)->second;
                        einfo.products.push_back(elem);
                    }
                    else if ( nodeName == "groupName" )
                        grpname = nodeValue;
                    
                    else if ( nodeName == "stage" )
                        stage = nodeValue;

                    else if ( nodeName == "Group" )
                        einfo.group = nodeValue;
                    
                    else if ( nodeName == "xCord" )
                        einfo.xcord = nodeValue;
                    
                    else if ( nodeName == "yCord" )
                        einfo.ycord = nodeValue;
                }
                if ( stage == "1" ) {
                    enzInfoMap[grpname].substrates = einfo.substrates;
                    enzInfoMap[grpname].enzyme = einfo.enzyme;
                    einfo.stage = 1;
                    enzInfoMap[grpname].stage = einfo.stage;
                    enzInfoMap[grpname].group = einfo.group;
                    enzInfoMap[grpname].xcord = einfo.xcord;
                    enzInfoMap[grpname].ycord = einfo.ycord;
                    KineticLaw * klaw=reaction->getKineticLaw();
                    vector< double > rate ;
                    rate.clear();
                    string amt_Conc;
                    getKLaw( klaw,true,rate,amt_Conc );
                    if ( errorFlag_ )
                        exit(0);
                    else if ( !errorFlag_ ) {
                        enzInfoMap[grpname].k1 = rate[0];
                        enzInfoMap[grpname].k2 = rate[1];
                    }
                }
                //Stage =='2' means ES* -> E+P;
                else if ( stage == "2" ) {
                    enzInfoMap[grpname].complex = einfo.complex;
                    enzInfoMap[grpname].products = einfo.products;
                    einfo.stage = 2;
                    enzInfoMap[grpname].stage += einfo.stage;
                    KineticLaw * klaw=reaction->getKineticLaw();
                    vector< double > rate;
                    rate.clear();
                    string amt_Conc;
                    getKLaw( klaw,false,rate,amt_Conc);
                    if ( errorFlag_ )
                        exit(0);
                    else if ( !errorFlag_ )
                        enzInfoMap[grpname].k3 = rate[0];
                }
            }
        }
    }
    return grpname;
}

/*    set up Michalies Menten reaction  */
void moose::SbmlReader::setupMMEnzymeReaction( Reaction * reac,string rid,string rname,const map< string, Id > &molSidcmptMIdMap ) {
    string::size_type loc = rid.find( "_MM_Reaction_" );
    if( loc != string::npos ) {
        int strlen = rid.length();
        rid.erase( loc,strlen-loc );
    }
    unsigned int num_modifr = reac->getNumModifiers();
    // Get groupName if exist in annotation (in case of Genesis)
    XMLNode * annotationRea = reac->getAnnotation();
    //string groupName = getAnnotation_Spe_Reac(annotationRea);
    pair<string, pair<string,string> > group = getAnnotation_Spe_Reac(annotationRea);
    string groupName = group.first;
    string xCord = group.second.first;
    string yCord = group.second.second;
    string MMEnznotes = " ";
    if (reac->isSetNotes())
        {
            XMLNode* xnode = reac->getNotes();
            string testnotes = reac->getNotesString();
            XMLNode nodec = xnode->getChild(0);
            XMLNode tnodec = nodec.getChild(0);
            MMEnznotes = tnodec.getCharacters();
        }

    for ( unsigned int m = 0; m < num_modifr; m++ ) {
        const ModifierSpeciesReference* modifr=reac->getModifier( m );
        std::string sp = modifr->getSpecies();
        Id enzyme_;
        Id E = molSidMIdMap_.find(sp)->second;
        string Epath = Field<string> :: get(E,"path");
        //cout << " \n \n  epath" << Epath;
        Id comptRef = molSidcmptMIdMap.find(sp)->second; //gives compartment of sp
        Id meshEntry = Neutral::child( comptRef.eref(), "mesh" );
        Shell* shell = reinterpret_cast< Shell* >( Id().eref().data() );
        enzyme_ = shell->doCreate("MMenz",E,rname,1);
        if (enzyme_ != Id())
        {   //shell->doAddMsg( "Single", meshEntry, "remeshReacs", enzyme_, "remesh");
            if (E != Id())
                shell->doAddMsg("Single",E,"nOut",enzyme_,"enzDest");
            if (!xCord.empty() and !yCord.empty())
            {  Id enzInfo;
                string enzPath = Field<string> :: get(enzyme_,"path");
                enzInfo = Id( enzPath + "/info");
                if ( enzInfo == Id() ) 
                    enzInfo = shell->doCreate( "Annotator", enzyme_, "info", 1 );
                assert( enzInfo != Id() );
                double x = atof( xCord.c_str() );
                double y = atof( yCord.c_str() );
                Field< double >::set( enzInfo, "x", x );
                Field< double >::set( enzInfo, "y", y );
            }
            if(!MMEnznotes.empty())
            {
                Id enzInfo;
                string enzPath = Field<string> :: get(enzyme_,"path");
                enzInfo = Id( enzPath + "/info");
                if ( enzInfo == Id() ) 
                    enzInfo = shell->doCreate( "Annotator", enzyme_, "info", 1 );
                assert( enzInfo != Id() );
                MMEnznotes.erase(std::remove(MMEnznotes.begin(), MMEnznotes.end(), '\n'), MMEnznotes.end());
                MMEnznotes.erase(std::remove(MMEnznotes.begin(), MMEnznotes.end(), '\t'), MMEnznotes.end());
                Field< string >::set( enzInfo, "notes", MMEnznotes );
            }

            KineticLaw * klaw=reac->getKineticLaw();
            vector< double > rate;
            rate.clear();
            string amt_Conc;
            getKLaw( klaw,true,rate,amt_Conc);
            if ( errorFlag_ )
                return;
            else if ( !errorFlag_ ) {
                for ( unsigned int rt = 0; rt < reac->getNumReactants(); rt++ ) {
                    const SpeciesReference* rct = reac->getReactant( rt );
                    sp=rct->getSpecies();
                    Id S = molSidMIdMap_.find(sp)->second;
                    if (S != Id())
                        shell->doAddMsg( "OneToOne", enzyme_, "sub" ,S , "reac" );
                }
                for ( unsigned int pt = 0; pt < reac->getNumProducts(); pt++ ) {
                    const SpeciesReference* pdt = reac->getProduct(pt);
                    sp = pdt->getSpecies();
                    Id P = molSidMIdMap_.find(sp)->second;
                    if (P != Id())
                        shell->doAddMsg( "OneToOne", enzyme_, "prd" ,P, "reac" );
                }
                Field < double > :: set( enzyme_, "kcat", rate[0] );
                Field < double > :: set( enzyme_, "numKm", rate[1] );
            }
     } //if Enzyme_
    }
}

/*    get Parameters from Kinetic Law  */
void moose::SbmlReader::getParameters( const ASTNode* node,vector <string> & parameters ) {
    assert( parameters.empty() );
    //cout << " parameter type " <<node->getType();

    if ( node->getType() == AST_MINUS ) {
        const ASTNode* lchild = node->getLeftChild();
        pushParmstoVector( lchild,parameters );

        if ( parameters.size() == 1 ) {
            const ASTNode* rchild = node->getRightChild();
            pushParmstoVector( rchild,parameters );
        }
    } else if ( node->getType() == AST_DIVIDE ) {
        const ASTNode* lchild = node->getLeftChild();
        pushParmstoVector( lchild,parameters );
        if (( parameters.size() == 1 ) || ( parameters.size() == 0 )) {
            const ASTNode* rchild = node->getRightChild();
            pushParmstoVector( rchild,parameters );
        }
    } else if ( node->getType() == AST_TIMES ) {
        //cout << " time " <<endl;
        pushParmstoVector( node,parameters );
    } else if ( node->getType() == AST_PLUS )
        pushParmstoVector( node,parameters );
    else if ( node->getType() == AST_NAME )
        pushParmstoVector( node,parameters );
    if ( parameters.size() > 2 ) {
        cout<<"Sorry! for now MOOSE cannot handle more than 2 parameters ."<<endl;
        errorFlag_ = true;
    }

}

/*   push the Parameters used in Kinetic law to a vector  */

void moose::SbmlReader::pushParmstoVector(const ASTNode* p,vector <string> & parameters) {
    string parm = "";
    //cout << "\n there " << p->getType();
    //cout << "_NAME" << " = " <<p->getName();
    if ( p->getType() == AST_NAME ) {
        pvm_iter = parmValueMap.find( std::string(p->getName()) );
        if ( pvm_iter != parmValueMap.end() ) {
            parm = pvm_iter->first;
            parameters.push_back( parm );
        }
    }
    int num = p->getNumChildren();
    for( int i = 0; i < num; ++i ) {
        const ASTNode* child = p->getChild(i);
        pushParmstoVector( child,parameters );
    }
}

/*     get Kinetic Law  */
void moose::SbmlReader::getKLaw( KineticLaw * klaw,bool rev,vector< double > & rate,string &amt_Conc) {
    std::string id;
    amt_Conc = "amount";
    double value = 0.0;
    UnitDefinition * kfud;
    UnitDefinition * kbud;
    int np = klaw->getNumParameters();
    bool flag = true;
    for ( int pi = 0; pi < np; pi++ ) {
        Parameter * p = klaw->getParameter(pi);

        if ( p->isSetId() )
            id = p->getId();
        if ( p->isSetValue() )
            value = p->getValue();
        parmValueMap[id] = value;
        flag = false;
    }
    double kf = 0.0,kb = 0.0,kfvalue,kbvalue;
    string kfparm,kbparm;
    vector< string > parameters;
    parameters.clear();
    const ASTNode* astnode=klaw->getMath();
    //cout << "\nkinetic law is :" << SBML_formulaToString(astnode) << endl;
    getParameters( astnode,parameters );
    //cout << "getKLaw " << errorFlag_;
    if ( errorFlag_ )
        return;
    else if ( !errorFlag_ ) {
        if ( parameters.size() == 1 ) {
            kfparm = parameters[0];
            kbparm = parameters[0];
        } else if ( parameters.size() == 2 ) {
            kfparm = parameters[0];
            kbparm = parameters[1];
        }
        //cout << "\n parameter "<< parameters.size();
        //cout << "$$ "<< parmValueMap[kfparm];
        //cout << " \t \t " << parmValueMap[kbparm];

        kfvalue = parmValueMap[kfparm];
        kbvalue = parmValueMap[kbparm];
        Parameter* kfp;
        Parameter* kbp;
        if ( flag ) {
            kfp = model_->getParameter( kfparm );
            kbp = model_->getParameter( kbparm );
        } else {
            kfp = klaw->getParameter( kfparm );
            kbp = klaw->getParameter( kbparm );
        }
        //cout << "\t \n \n" << kfp << " " <<kbp;

        if ( kfp->isSetUnits() ) {
            kfud = kfp->getDerivedUnitDefinition();
            //cout << "parameter unit :" << UnitDefinition::printUnits(kfp->getDerivedUnitDefinition())<< endl;
            //cout << " rate law ";
            double transkf = transformUnits( 1,kfud ,"substance",true);
            //cout << " transkf " << transkf<<endl;
            kf = kfvalue * transkf;
            kb = 0.0;
        } 
        else {
            double lvalue =1.0;
            /* If rate units are not defined then trying to get substanceUnit*/
            if (model_->getNumUnitDefinitions() > 0)
                lvalue = unitsforRates();
                //cout << "Substrate units are specified " << lvalue <<endl;
            /* If neither RateUnits nor substanceUnit is defined, then assuming SubstanceUnit are in mole,
               so converting mole to millimole
            */
            amt_Conc = "concentration";
            //cout << " rate law ";
            if (noOfsub_ >1)
                lvalue /= pow(1e+3,(noOfsub_-1));
            kf = kfvalue*lvalue;
            
        }// !kfp is notset
        if ( ( kbp->isSetUnits() ) && ( rev ) ) {
            kbud = kbp->getDerivedUnitDefinition();
            double transkb = transformUnits( 1,kbud,"substance",true );
            kb = kbvalue * transkb;
        }
        if ( (! kbp->isSetUnits() ) && ( rev ) ) {
            double lvalue =1.0;
            /* If rate units are not defined then,trying to get substanceUnit*/
            if (model_->getNumUnitDefinitions() > 0)
                lvalue = unitsforRates();
            /* If neither RateUnits nor substanceUnit is defined, then assuming SubstanceUnit are in mole (concentration)
               and hasOnlySubstanceUnit =false so converting mole to millimole
            */
            if (noOfprd_ >1)
                lvalue /= pow(1e+3,(noOfprd_-1));
            kf = kfvalue*lvalue;

        }
        rate.push_back( kf );
        rate.push_back( kb );
    }
}
double moose::SbmlReader::unitsforRates() {
    double lvalue =1;
    for (unsigned int n=0; n < model_->getNumUnitDefinitions(); n++) {
        UnitDefinition * ud = model_->getUnitDefinition(n);
        for (unsigned int ut=0; ut <ud->getNumUnits(); ut++) {
            Unit * unit = ud->getUnit(ut);
            if (ud->getId() == "substance") {
                if ( unit->isMole() ) {
                    double exponent = unit->getExponent();
                    double multiplier = unit->getMultiplier();
                    int scale = unit->getScale();
                    double offset = unit->getOffset();
                    lvalue *= pow( multiplier * pow(10.0,scale), exponent ) + offset;
                    return lvalue;
                }
            }
        }
    }
    return lvalue;
}//unitforRates
void moose::SbmlReader::addSubPrd(Reaction * reac,Id reaction_,string type) {
    if (reaction_ != Id())
    {
        map< string,double > rctMap;
        map< string,double >::iterator rctMap_iter;
        double rctcount=0.0;
        Shell * shell = reinterpret_cast< Shell* >( Id().eref().data() );
        rctMap.clear();
        unsigned int nosubprd;
        const SpeciesReference* rct;
        if (type == "sub") {
            nosubprd = reac->getNumReactants();
        } else
            nosubprd = reac->getNumProducts();
        for ( unsigned int rt=0; rt<nosubprd; rt++ ) {
            if (type == "sub")
                rct = reac->getReactant(rt);
            else
                rct = reac->getProduct(rt);
            std:: string sp = rct->getSpecies();
            rctMap_iter = rctMap.find(sp);
            if ( rctMap_iter != rctMap.end() )
                rctcount = rctMap_iter->second;
            else
                rctcount = 0.0;
            rctcount += rct->getStoichiometry();
            rctMap[sp] = rctcount;
            if (type =="sub")
                noOfsub_ +=rctcount;
            for ( int i=0; (int)i<rct->getStoichiometry(); i++ )
                shell->doAddMsg( "OneToOne", reaction_, type ,molSidMIdMap_[sp] , "reac" );
        }
    }
}
/* Transform units from SBML to MOOSE
   MOOSE units for
   volume -- cubic meter
*/

double moose::SbmlReader::transformUnits( double mvalue,UnitDefinition * ud,string type, bool hasonlySubUnit ) {
    assert (ud);
    double lvalue = mvalue;
    if (type == "compartment") 
    {   if(ud->getNumUnits() == 0)
            unitsDefined = false;
        else
        {   for ( unsigned int ut = 0; ut < ud->getNumUnits(); ut++ ) {
            Unit * unit = ud->getUnit(ut);
            if ( unit->isLitre() ) {
                double exponent = unit->getExponent();
                double multiplier = unit->getMultiplier();
                int scale = unit->getScale();
                double offset = unit->getOffset();
                lvalue *= pow( multiplier * pow(10.0,scale), exponent ) + offset;
                // Need to check if spatial dimension is less than 3 then,
                // then volume conversion e-3 to convert cubicmeter shd not be done.
                lvalue *= pow(1e-3,exponent);
                }
            }
        }
    } 
    else if(type == "substance")
    {   double exponent = 1.0;
        if(ud->getNumUnits() == 0)
            unitsDefined = false;
        else {
            for ( unsigned int ut = 0; ut < ud->getNumUnits(); ut++ ) {
                Unit * unit = ud->getUnit(ut);
                if ( unit->isMole() ) {
                    exponent = unit->getExponent();
                    double multiplier = unit->getMultiplier();
                    int scale = unit->getScale();
                    double offset = unit->getOffset();
                    lvalue *= pow( multiplier * pow(10.0,scale), exponent ) + offset;
                }//if unit is Mole
                else if (unit->isItem()){
                    exponent = unit->getExponent();
                    double multiplier = unit->getMultiplier();
                    int scale = unit->getScale();
                    double offset = unit->getOffset();
                    lvalue *= pow( multiplier * pow(10.0,scale), exponent ) + offset;
                }    
            }//for
        } //else
    } // type="substance"
return lvalue;
}//transformUnits
void moose::SbmlReader::printMembers( const ASTNode* p,vector <string> & ruleMembers ) {
    if ( p->getType() == AST_NAME ) {
        //cout << "_NAME" << " = " << p->getName() << endl;
        ruleMembers.push_back( p->getName() );
    }
    int num = p->getNumChildren();
    for( int i = 0; i < num; ++i ) {
        const ASTNode* child = p->getChild(i);
        printMembers( child,ruleMembers );
    }
}

void moose::SbmlReader ::findModelParent( Id cwe, const string& path, Id& parentId, string& modelName ) {
    //Taken from LoadModels.cpp
    //If path exist example /model when come from GUI it creates model under /model/filename
    // i.e. because by default we creat genesis,sbml models under '/model', which is created before and path exist
    // at the time it comes to MooseSbmlReader.cpp
    //When run directly (command line readSBML() )it ignores the path and creates under '/' and filename takes as "SBMLtoMoose"
    //modelName = "test";
    string fullPath = path;
    if ( path.length() == 0 )
        parentId = cwe;

    if ( path == "/" )
        parentId = Id();

    if ( path[0] != '/' ) {
        string temp = cwe.path();
        if ( temp[temp.length() - 1] == '/' )
            fullPath = temp + path;
        else
            fullPath = temp + "/" + path;
    }
    Id paId( fullPath );
    if ( paId == Id() ) { // Path includes new model name
        string::size_type pos = fullPath.find_last_of( "/" );
        assert( pos != string::npos );
        string head = fullPath.substr( 0, pos );
        Id ret( head );
        // When head = "" it means paId should be root.
        if ( ret == Id() && head != "" && head != "/root" )
            ;//return 0;
        parentId = ret;
        modelName = fullPath.substr( pos + 1 );
    }

    else { // Path is an existing element.
        parentId = paId;

    }
}

/**
 * @brief Populate parmValueMap; a member variable with keeps all the globals
 * parameters of SBML model.
 */
void moose::SbmlReader::getGlobalParameter() {
    for ( unsigned int pm = 0; pm < model_->getNumParameters(); pm++ ) {
        Parameter* prm = model_->getParameter( pm );
        std::string id,unit;
        if ( prm->isSetId() ) {
            id = prm->getId();
        }
        double value = 0.0;
        if ( prm->isSetValue() ) {
            value=prm->getValue();
        }
        parmValueMap[id] = value;
    }
}

string moose::SbmlReader::nameString( string str ) {
    string str1;

    int len = str.length();
    int i= 0;
    do {
        switch( str.at(i) ) {
        case ' ':
            str1 = "_space_";
            str.replace( i,1,str1 );
            len += str1.length()-1;
            break;
        }
        i++;
    } while ( i < len );
    return str;
}
#endif // USE_SBML
