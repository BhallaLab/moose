/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2012 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <iomanip>
#include <fstream>

#include "../shell/Wildcard.h"

#include "../basecode/header.h"
#include "../utility/utility.h"

#include "PoolBase.h"
#include "Pool.h"
#include "BufPool.h"
#include "ReacBase.h"
#include "EnzBase.h"
#include "lookupVolumeFromMesh.h"
#include <sstream>
#include <set>


void writeHeader( ofstream& fout,
                  double simdt, double plotdt, double maxtime, double defaultVol)
{
    time_t rawtime;
    time( &rawtime );

    fout <<
         "//genesis\n"
         "// kkit Version 11 flat dumpfile\n\n";
    fout << "// Saved on " << ctime( &rawtime ) << endl;
    fout << "include kkit {argv 1}\n";
    fout << "FASTDT = " << simdt << endl;
    fout << "SIMDT = " << simdt << endl;
    fout << "CONTROLDT = " << plotdt << endl;
    fout << "PLOTDT = " << plotdt << endl;
    fout << "MAXTIME = " << maxtime << endl;
    fout << "TRANSIENT_TIME = 2\n"
         "VARIABLE_DT_FLAG = 0\n";
    fout << "DEFAULT_VOL = " << defaultVol << endl;
    fout << "VERSION = 11.0\n"
         "setfield /file/modpath value ~/scripts/modules\n"
         "kparms\n\n";

    fout <<
         "initdump -version 3 -ignoreorphans 1\n"
         "simobjdump table input output alloced step_mode stepsize x y z\n"
         "simobjdump xtree path script namemode sizescale\n"
         "simobjdump xcoredraw xmin xmax ymin ymax\n"
         "simobjdump xtext editable\n"
         "simobjdump xgraph xmin xmax ymin ymax overlay\n"
         "simobjdump xplot pixflags script fg ysquish do_slope wy\n"
         "simobjdump group xtree_fg_req xtree_textfg_req plotfield expanded movealone \\\n"
         "  link savename file version md5sum mod_save_flag x y z\n"
         "simobjdump geometry size dim shape outside xtree_fg_req xtree_textfg_req x y z\n"
         "simobjdump kpool DiffConst CoInit Co n nInit mwt nMin vol slave_enable \\\n"
         "  geomname xtree_fg_req xtree_textfg_req x y z\n"
         "simobjdump kreac kf kb notes xtree_fg_req xtree_textfg_req x y z\n"
         "simobjdump kenz CoComplexInit CoComplex nComplexInit nComplex vol k1 k2 k3 \\\n"
         "  keepconc usecomplex notes xtree_fg_req xtree_textfg_req link x y z\n"
         "simobjdump stim level1 width1 delay1 level2 width2 delay2 baselevel trig_time \\\n"
         "  trig_mode notes xtree_fg_req xtree_textfg_req is_running x y z\n"
         "simobjdump xtab input output alloced step_mode stepsize notes editfunc \\\n"
         "  xtree_fg_req xtree_textfg_req baselevel last_x last_y is_running x y z\n"
         "simobjdump kchan perm gmax Vm is_active use_nernst notewriteReacs xtree_fg_req \\\n"
         "  xtree_textfg_req x y z\n"
         "simobjdump transport input output alloced step_mode stepsize dt delay clock \\\n"
         "  kf xtree_fg_req xtree_textfg_req x y z\n"
         "simobjdump proto x y z\n";
    //"simundump geometry /kinetics/geometry 0 1.6667e-19 3 sphere \"\" white black 0 0 0\n\n";
}

string trimPath(Id id, Id  comptid)
{
    string msgpath = Field <string> :: get(id,"path");
    ObjId compartment(msgpath);
    string path1;
    cout << " trimpath " << msgpath <<endl;
    // triming the string compartment Level
    while( Field<string>::get(compartment,"className") != "CubeMesh"
            && Field<string>::get(compartment,"className") != "CylMesh"
         )
        compartment = Field<ObjId> :: get(compartment, "parent");
    string cmpt	 = Field < string > :: get(compartment,"name");
    if (cmpt != "kinetics")
    {
        std::size_t found = msgpath.find(cmpt);
        if (found !=std::string::npos)
            path1 = msgpath.substr(found-1,msgpath.length());
        else
        {
            path1 = msgpath;
        }
    }
    else
    {
        std :: size_t found = msgpath.find(cmpt);
        if (found !=std::string::npos)
        {
            string pathC = msgpath.substr(found-1,msgpath.length());
            std :: size_t slash = pathC.find('/',found);
            if (slash !=std::string::npos)
                path1 = pathC.substr(slash,pathC.length());
            else
            {
                path1 = msgpath;
            }
        }
    }
    cout << " path " << path1 << endl;
    return path1;
}

Id getEnzCplx( Id id )
{
    vector < Id > srct = LookupField <string,vector < Id> >::get(id, "neighbors","cplxDest");
    return srct[0];
}

Id getEnzMol( Id id )
{
    vector < Id > srct = LookupField <string,vector < Id> >::get(id, "neighbors","enzDest");
    return srct[0];
}
void writeEnz( ofstream& fout, Id id,
               string colour, string textcolour,
               double x, double y, Id comptid )
{
    string path = id.path();
    string comptname = Field < string > :: get(comptid,"name");
    string poolpath = trimPath(id,comptid);


    double k1 = 0;
    double k2 = 0;
    double k3 = 0;
    double nInit = 0;
    double concInit = 0;
    double n = 0;
    double conc = 0;
    Id enzMol = getEnzMol( id );
    assert( enzMol != Id() );
    double vol = Field< double >::get( enzMol, "volume" ) * NA * 1e-3;
    unsigned int isMichaelisMenten = 0;
    string enzClass = Field < string > :: get(id,"className");
    if (enzClass == "ZombieMMenz" or enzClass == "MMenz")
    {
        k1 = Field < double > :: get (id,"numKm");
        k3 = Field < double > :: get (id,"kcat");
        k2 = 4.0*k3;
        k1 = (k2 + k3) / k1;
        isMichaelisMenten = 1;
    }
    else if (enzClass == "ZombieEnz" or enzClass == "Enz")
    {
        k1 = Field< double >::get( id, "k1" );
        k2 = Field< double >::get( id, "k2" );
        k3 = Field< double >::get( id, "k3" );
        Id cplx = getEnzCplx( id );
        assert( cplx != Id() );
        nInit = Field< double >::get( cplx, "nInit" );
        n = Field< double >::get( cplx, "n" );
        concInit = Field< double >::get( cplx, "concInit" );
        conc = Field< double >::get( cplx, "conc" );
    }
    fout << "simundump kenz /kinetics" << poolpath << " 0 " <<
         concInit << " " <<
         conc << " " <<
         nInit << " " <<
         n << " " <<
         vol << " " <<
         k1 << " " <<
         k2 << " " <<
         k3 << " " <<
         0 << " " <<
         isMichaelisMenten << " " <<
         "\"\"" << " " <<
         colour << " " << textcolour << " \"\"" <<
         " " << x << " " << y << " 0\n";
}

void writeReac( ofstream& fout, Id id,
                string colour, string textcolour,
                double x, double y, Id comptid )
{
    string reacPar  = Field <string> :: get(comptid,"name");
    string reacname = Field<string> :: get(id, "name");
    double kf = Field< double >::get( id, "numKf" );
    double kb = Field< double >::get( id, "numKb" );

    unsigned int numSub =
        Field< unsigned int >::get( id, "numSubstrates" );
    unsigned int numPrd =
        Field< unsigned int >::get( id, "numProducts" );
    fout << "simundump kreac /kinetics" << trimPath(id,comptid) << " 0 " <<
         kf << " " << kb << " \"\" " <<
         colour << " " << textcolour << " " << x << " " << y << " 0\n";
}

unsigned int getSlaveEnable( Id id )
{
    static const Finfo* setNinitFinfo =
        PoolBase::initCinfo()->findFinfo( "set_nInit" );
    static const Finfo* setConcInitFinfo =
        PoolBase::initCinfo()->findFinfo( "set_concInit" );
    unsigned int ret = 0;
    vector< Id > src;
    if ( id.element()->cinfo()->isA( "BufPool" ) )
    {
        if ( id.element()->getNeighbors( src, setConcInitFinfo ) > 0 )
        {
            ret = 2;
        }
        else if ( id.element()->getNeighbors( src, setNinitFinfo ) > 0 )
        {
            ret = 4;
        }
    }
    else
    {
        return 0;
    }
    if ( ret == 0 )
        return 4; // Just simple buffered molecule
    if ( src[0].element()->cinfo()->isA( "StimulusTable" ) )
        return ret; // Following a table, this is fine.

    // Fallback: I have no idea what sent it the input, assume it is legit.
    return ret;
}

void writePool( ofstream& fout, Id id,
                string colour, string textcolour,
                double x, double y, Id comptid,int comptIndex )
{
    string poolPar  = Field <string> :: get(comptid,"name");
    string pooltype = Field < string > :: get(id,"className");
    double diffConst = Field< double >::get( id, "diffConst" );
    double concInit = Field< double >::get( id, "concInit" );
    //double conc = Field< double >::get( id, "conc" );
    double nInit = Field< double >::get( id, "nInit" );
    //double n = Field< double >::get( id, "n" );
    double volume = Field< double >::get( id, "volume" );
    string geometry;
    stringstream geometryTemp ;
    unsigned int slave_enable = 0;
    if (pooltype == "BufPool" or pooltype == "ZombieBufPool")
    {
        vector< Id > children = Field< vector< Id > >::get( id, "children" );
        if (children.size() == 0)
            slave_enable = 4;
        for ( vector< Id >::iterator i = children.begin(); i != children.end(); ++i )
        {
            string funcpath = Field <string> :: get(*i,"path");
            string clsname = Field <string> :: get(*i,"className");
            if (clsname == "Function" or clsname == "ZombieFunction")
                slave_enable = 0;
            else
                slave_enable = 4;

        }
    }
    if (comptIndex > 0)
        geometryTemp << "/geometry[" << comptIndex <<"]";
    else
        geometryTemp << "/geometry";

    // simobjdump kpool DiffConst CoInit Co n nInit mwt nMin vol slave_enable

    fout << "simundump kpool /kinetics" << trimPath(id,comptid) << " 0 " <<
         diffConst << " " <<
         0 << " " <<
         0 << " " <<
         0 << " " <<
         nInit << " " <<
         0 << " " << 0 << " " << // mwt, nMin
         volume * NA * 1e-3 << " " << // volscale
         slave_enable << //GENESIS FIELD HERE.
         " /kinetics"<< geometryTemp.str() << " " <<
         colour << " " << textcolour << " " << x << " " << y << " 0\n";
}
void writePlot( ofstream& fout, Id id,
                string colour, string textcolour,
                double x, double y )
{
	string path = id.path();
	size_t pos = path.find( "/graphs" );
	if ( pos == string::npos ) {
		pos = path.find( "/moregraphs" );
		if ( pos == string::npos ) {
			return;
		}
	}
	path = path.substr( pos );
	fout << "simundump xplot " << path << " 3 524288 \\\n" <<
	"\"delete_plot.w <s> <d>; edit_plot.D <w>\" " << textcolour << " 0 0 1\n";
}

void writeGui( ofstream& fout )
{
    fout << "simundump xgraph /graphs/conc1 0 0 99 0.001 0.999 0\n"
         "simundump xgraph /graphs/conc2 0 0 100 0 1 0\n"
         "simundump xgraph /moregraphs/conc3 0 0 100 0 1 0\n"
         "simundump xgraph /moregraphs/conc4 0 0 100 0 1 0\n"
         "simundump xcoredraw /edit/draw 0 -6 4 -2 6\n"
         "simundump xtree /edit/draw/tree 0 \\\n"
         "  /kinetics/#[],/kinetics/#[]/#[],/kinetics/#[]/#[]/#[][TYPE!=proto],/kinetics/#[]/#[]/#[][TYPE!=linkinfo]/##[] \"edit_elm.D <v>; drag_from_edit.w <d> <S> <x> <y> <z>\" auto 0.6\n"
         "simundump xtext /file/notes 0 1\n";
}

void writeFooter( ofstream& fout )
{
    fout << "\nenddump\n";
    fout << "complete_loading\n";
}

void storeMMenzMsgs( Id enz, vector< string >& msgs, Id comptid )
{
    Id enzparentId = Field <ObjId>  :: get(enz,"parent");
    string enzPar  = Field <string> :: get(enzparentId,"name");
    string enzName = Field<string> :: get(enz,"name");

    vector < Id > srct = LookupField <string,vector < Id> >::get(enz, "neighbors","sub");
    for (vector <Id> :: iterator rsub = srct.begin(); rsub != srct.end(); rsub++)
    {
        string s = "addmsg /kinetics" + trimPath(*rsub, comptid) + " /kinetics" + trimPath(enz, comptid) + " SUBSTRATE n";
        msgs.push_back( s );
        s = "addmsg /kinetics" + trimPath(enz, comptid) + " /kinetics" + trimPath( *rsub, comptid ) + 	" REAC sA B";
        msgs.push_back( s );
    }
    vector < Id > prct = LookupField <string,vector < Id> >::get(enz, "neighbors","prd");
    for (vector <Id> :: iterator rprd = prct.begin(); rprd != prct.end(); rprd++)
    {
        string s = "addmsg /kinetics" + trimPath( enz, comptid ) + " /kinetics" + trimPath(*rprd, comptid) + " MM_PRD pA";
        msgs.push_back( s );
    }
    vector < Id > enzC = LookupField <string,vector < Id> >::get(enz, "neighbors","enzDest");
    for (vector <Id> :: iterator enzCl = enzC.begin(); enzCl != enzC.end(); enzCl++)
    {
        string s = "addmsg /kinetics" + trimPath( *enzCl, comptid ) + " /kinetics" + trimPath(enz, comptid) + " ENZYME n";
        msgs.push_back( s );
    }
}

void storeCplxEnzMsgs( Id enz, vector< string >& msgs, Id comptid )
{
    Id enzparentId = Field <ObjId>  :: get(enz,"parent");
    string enzPar  = Field <string> :: get(enzparentId,"name");
    string enzName = Field<string> :: get(enz,"name");

    vector < Id > srct = LookupField <string,vector < Id> >::get(enz, "neighbors","sub");
    for (vector <Id> :: iterator rsub = srct.begin(); rsub != srct.end(); rsub++)
    {
        string s = "addmsg /kinetics" + trimPath(*rsub, comptid) + " /kinetics" + trimPath(enz, comptid) + " SUBSTRATE n";
        msgs.push_back( s );
        s = "addmsg /kinetics" + trimPath(enz, comptid) + " /kinetics" + trimPath( *rsub, comptid ) + 	" REAC sA B";
        msgs.push_back( s );
    }
    vector < Id > prct = LookupField <string,vector < Id> >::get(enz, "neighbors","prd");
    for (vector <Id> :: iterator rprd = prct.begin(); rprd != prct.end(); rprd++)
    {
        string s = "addmsg /kinetics" + trimPath( enz, comptid ) + " /kinetics" + trimPath(*rprd, comptid) + " MM_PRD pA";
        msgs.push_back( s );
    }
    vector < Id > enzC = LookupField <string,vector < Id> >::get(enz, "neighbors","enzOut");
    for (vector <Id> :: iterator enzCl = enzC.begin(); enzCl != enzC.end(); enzCl++)
    {
        string s = "addmsg /kinetics" + trimPath( *enzCl, comptid ) + " /kinetics" + trimPath(enz, comptid) + " ENZYME n";
        msgs.push_back( s );
        s = "addmsg /kinetics" + trimPath( enz, comptid ) + " /kinetics" + trimPath(*enzCl, comptid) + " REAC eA B";
        msgs.push_back( s );
    }
}

void storeEnzMsgs( Id enz, vector< string >& msgs, Id comptid )
{
    string enzClass = Field < string > :: get(enz,"className");
    if (enzClass == "ZombieMMenz" or enzClass == "MMenz")
        storeMMenzMsgs(enz, msgs, comptid);
    else
        storeCplxEnzMsgs( enz, msgs, comptid );
}

void storeReacMsgs( Id reac, vector< string >& msgs, Id comptid )
{
    vector < Id > srct = LookupField <string,vector < Id> >::get(reac, "neighbors","sub");
    for (vector <Id> :: iterator rsub = srct.begin(); rsub != srct.end(); rsub++)
    {
        string s = "addmsg /kinetics" + trimPath(*rsub, comptid) + " /kinetics" + trimPath(reac, comptid) + " SUBSTRATE n";
        msgs.push_back( s );
        s = "addmsg /kinetics" + trimPath(reac, comptid) + " /kinetics" + trimPath( *rsub, comptid ) + 	" REAC A B";
        msgs.push_back( s );
    }
    vector < Id > prct = LookupField <string,vector < Id> >::get(reac, "neighbors","prd");
    for (vector <Id> :: iterator rprd = prct.begin(); rprd != prct.end(); rprd++)
    {
        string rpath = Field <string> :: get(reac,"path");
        string cpath = Field <string> :: get(comptid,"path");
        string prdPath = Field <string> :: get(*rprd,"path");
        string s = "addmsg /kinetics" + trimPath( *rprd, comptid ) + " /kinetics" + trimPath(reac, comptid) + " PRODUCT n";
        msgs.push_back( s );
        s = "addmsg /kinetics" + trimPath(reac, comptid) + " /kinetics" + trimPath( *rprd,comptid ) + " REAC B A";
        msgs.push_back( s );
    }
}

void storeFunctionMsgs( Id func, vector< string >& msgs,map < double, pair<Id, int> > & compt_vol )
{
    // Get the msg sources into this Function object.
    ObjId poolPath = Neutral::parent( func.eref() );
    double poolvol = Field < double > :: get(poolPath,"Volume");
    Id poolParentId = compt_vol[poolvol].first;
    string poolParent = Field <string> :: get(compt_vol[poolvol].first,"name");
    Id xi(func.value()+1);
    vector < Id > func_input = LookupField <string,vector < Id> >::get(xi, "neighbors","input");

    for (vector <Id> :: iterator funcIp = func_input.begin(); funcIp != func_input.end(); funcIp++)
    {
        string funcIp_path = Field < string > :: get(*funcIp,"path");
        double vol = Field < double > :: get(*funcIp,"Volume");
        Id parentId = compt_vol[vol].first;
        string parentname = Field <string> :: get(parentId,"name");
        string s = "addmsg /kinetics" + trimPath(*funcIp, parentId)+ " /kinetics" + trimPath(poolPath,poolParentId) +
                   " SUMTOTAL n nInit";
        msgs.push_back(s);
    }
}

void storePlotMsgs( Id tab, vector< string >& msgs, Id pool, string bg, Id comptid)
{
    string tabPath = tab.path();
    string poolPath = Field <string> :: get(pool,"path");
    string poolName = Field <string> :: get(pool,"name");

    size_t pos = tabPath.find( "/graphs" );
    if ( pos == string::npos )
        pos = tabPath.find( "/moregraphs" );
    assert( pos != string::npos );
    tabPath = tabPath.substr( pos );
    string s = "addmsg /kinetics" + trimPath( poolPath, comptid) + " " + tabPath +
               " PLOT Co *" + poolName + " *" + bg;
    msgs.push_back( s );
}

/**
 * A bunch of heuristics to find good SimTimes to use for kkit.
 * Returns runTime.
 */
double estimateSimTimes( double& simDt, double& plotDt )
{
    double runTime = Field< double >::get( Id( 1 ), "runTime" );
    if ( runTime <= 0 )
        runTime = 100.0;
    vector< double > dts =
        Field< vector< double> >::get( Id( 1 ), "dts" );

    simDt = dts[11];
    plotDt = dts[18];
    cout <<  "estimatesimtimes" << simDt << plotDt;
    if ( plotDt <= 0 )
        plotDt = runTime / 200.0;
    if ( simDt == 0 )
        simDt = 0.01;
    if ( simDt > plotDt )
        simDt = plotDt / 100;

    return runTime;
}

/// Returns an estimate of the default volume used in the model.
double estimateDefaultVol( Id model )
{
    vector< Id > children =
        Field< vector< Id > >::get( model, "children" );
    vector< double > vols;
    double maxVol = 0;
    for ( vector< Id >::iterator i = children.begin();
            i != children.end(); ++i )
    {
        if ( i->element()->cinfo()->isA( "ChemCompt" ) )
        {
            double v = Field< double >::get( *i, "volume" );
            if ( i->element()->getName() == "kinetics" )
                return v;
            vols.push_back( v );
            if ( maxVol < v )
                maxVol = v;
        }
    }
    if ( maxVol > 0 )
        return maxVol;
    return 1.0e-15;
}
void writeMsgs( ofstream& fout, const vector< string >& msgs )
{
    for ( vector< string >::const_iterator i = msgs.begin();
            i != msgs.end(); ++i )
        fout << *i << endl;
}
void writeGroup(ofstream& fout,Id model)
{
    vector < ObjId> group;
    int x = 10;
    int y = 20;
    int num = wildcardFind( model.path() + "/##[TYPE=Neutral]", group );
    for ( vector< ObjId >::iterator itr = group.begin(); itr != group.end(); itr++)
    {
        string path = Field<string>::get(*itr,"path");
        size_t pos = path.find( "/kinetics" );
        if (pos != std::string::npos)
        {
            path = path.substr( pos );
            fout << "simundump group " << path << " 0 " <<
                 "blue" << " " << "green"	 << " x 0 0 \"\" defaultfile \\\n";
            fout << "  defaultfile.g 0 0 0 " << x << " " << y << " 0\n";
        }
    }
}
void writeKkit( Id model, const string& fname )
{
    ofstream fout( fname.c_str(), ios::out );
    vector< ObjId > chemCompt;
    vector< string > msgs;
    double simDt;
    double plotDt;

    double runTime = estimateSimTimes( simDt, plotDt );
    double defaultVol = estimateDefaultVol( model );
    writeHeader( fout, simDt, plotDt, runTime, defaultVol );
    string bg = "cyan";
    string fg = "black";
    double x = 0;
    double y = 0;

    map < double, pair<Id, int> > compt_vol;

    unsigned int num = wildcardFind( model.path() + "/##[ISA=ChemCompt]", chemCompt );
    if ( num == 0 )
    {
        cout << "Warning: writeKkit:: No model found on " << model <<
             endl;
        return;
    }
    for ( vector< ObjId >::iterator itr = chemCompt.begin(); itr != chemCompt.end(); itr++)
    {
        vector < unsigned int>dims;
        unsigned int dims_size;
        dims_size = 1;
        unsigned index = 0;
        string comptPath = Field<string>::get(*itr,"path");
        string comptname = Field<string>::get(*itr,"name");
        if (comptname != "kinetics")
        {
            fout << "simundump group /kinetics/" << comptname << " 0 " <<
                 "blue" << " " << "green"	 << " x 0 0 \"\" defaultfile \\\n";
            fout << "  defaultfile.g 0 0 0 " << rand() % 10 + 1 << " " << rand() % 10 + 1 << " 0\n";
        }
        double size = Field<double>::get(ObjId(*itr,index),"Volume");
        unsigned int ndim = Field<unsigned int>::get(ObjId(*itr,index),"NumDimensions");
        ostringstream geometry;
        int vecIndex = itr-chemCompt.begin();
        if (vecIndex > 0)
            geometry << "simundump geometry /kinetics" <<  "/geometry[" << vecIndex <<"] 0 " << size << " " << ndim << " sphere " <<" \"\" white black 0 0 0\n";
        else
            geometry << "simundump geometry /kinetics"  <<  "/geometry 0 " << size << " " << ndim << " sphere " <<" \"\" white black 0 0 0\n";
        fout << geometry.str();
        compt_vol.insert(make_pair(size,make_pair(*itr,vecIndex)));
    } // compartmentclose
    writeGroup(fout, model);
    map<double, pair<Id, int> >::iterator compt;
    for (compt = compt_vol.begin(); compt != compt_vol.end(); compt++)
    {
        string comptPath = Field<string>::get(compt->second.first,"path");
        //  Species
        vector< ObjId > Compt_spe;
        wildcardFind(comptPath+"/##[ISA=PoolBase]",Compt_spe);
        int species_size = 1;
        string objname;
        for (vector <ObjId> :: iterator itrp = Compt_spe.begin(); itrp != Compt_spe.end(); itrp++)
        {
            string path = Field <string> :: get (*itrp,"path");
            Id enzPoolparent = Field <ObjId>  :: get(*itrp,"parent");
            string enzpoolClass = Field <string> :: get(enzPoolparent,"className");
            if (enzpoolClass != "ZombieEnz" or enzpoolClass != "Enz")
            {
                Id annotaId( path+"/info");
                if ( annotaId != Id() )
                {
                    double x = Field <double> :: get(annotaId,"x");
                    double y = Field <double> :: get(annotaId,"y");
                    string fg = Field <string> :: get(annotaId,"textColor");
                    string bg = Field <string> :: get(annotaId,"color");
                    writePool(fout, *itrp,bg,fg,x,y,compt->second.first,compt->second.second);
                }
            }
        } //species is closed
        vector< ObjId > Compt_Func;
        wildcardFind(comptPath+"/##[ISA=Function]",Compt_Func);
        for (vector <ObjId> :: iterator itrF= Compt_Func.begin(); itrF != Compt_Func.end(); itrF++)
        {
            storeFunctionMsgs( *itrF, msgs,compt_vol);
        }
        // Reaction
        vector< ObjId > Compt_Reac;
        wildcardFind(comptPath+"/##[ISA=ReacBase]",Compt_Reac);
        for (vector <ObjId> :: iterator itrR= Compt_Reac.begin(); itrR != Compt_Reac.end(); itrR++)
        {
            string path = Field<string> :: get(*itrR,"path");
            Id annotaId( path+"/info");
            string noteClass = Field<string> :: get(annotaId,"className");
            string notes;
            double x = Field <double> :: get(annotaId,"x");
            double y = Field <double> :: get(annotaId,"y");
            string fg = Field <string> :: get(annotaId,"textColor");
            string bg = Field <string> :: get(annotaId,"color");
            writeReac( fout, *itrR, bg, fg, x, y, compt->second.first);
            storeReacMsgs( *itrR, msgs, compt->second.first );
        }// reaction
        vector< ObjId > Compt_Enz;
        wildcardFind(comptPath+"/##[ISA=EnzBase]",Compt_Enz);
        for (vector <ObjId> :: iterator itrE= Compt_Enz.begin(); itrE != Compt_Enz.end(); itrE++)
        {
            string path = Field<string> :: get(*itrE,"path");
            Id annotaId( path+"/info");
            string noteClass = Field<string> :: get(annotaId,"className");
            string notes;
            double x = Field <double> :: get(annotaId,"x");
            double y = Field <double> :: get(annotaId,"y");
            string fg = Field <string> :: get(annotaId,"textColor");
            string bg = Field <string> :: get(annotaId,"color");
            writeEnz( fout, *itrE, bg, fg, x, y, compt->second.first);
            storeEnzMsgs( *itrE, msgs, compt->second.first);
        }// reaction

    } //compatment loop
    writeGui ( fout);

    /* Table */

    vector< ObjId > table;
    wildcardFind(model.path()+"/##[ISA=Table2]",table);
    for (vector <ObjId> :: iterator itrT= table.begin(); itrT != table.end(); itrT++)
    {
        string tabPath = Field <string> :: get(*itrT,"path");
        vector < Id > tabSrc = LookupField <string,vector < Id> >::get(*itrT, "neighbors","requestOut");
        for (vector <Id> :: iterator tabItem= tabSrc.begin(); tabItem != tabSrc.end(); tabItem++)
        {
            string path = Field <string> :: get(*tabItem,"path");
            double vol = Field < double> :: get (*tabItem,"Volume");
            //Trying to find the compartment name via volume
            //otherwise if we go via parent, sometimes it might
            // get groupname so maped compartment Id and volume

            Id parentId = compt_vol[vol].first;
            string parentname = Field <string> :: get(parentId,"name");
            Id annotaId(path+"/info");
            double x = Field <double> :: get(annotaId,"x");
            double y = Field <double> :: get(annotaId,"y");
            string bg = Field <string> :: get(annotaId,"textColor");
            string fg = Field <string> :: get(annotaId,"color");
            writePlot( fout, *itrT, bg, fg, x, y );
            storePlotMsgs( *itrT, msgs,*tabItem,fg, parentId);

        }
    }// table
    writeMsgs( fout, msgs );
    writeFooter( fout );
}
