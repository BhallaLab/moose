/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**   copyright (C) 2003-2007 Upinder S. Bhalla, Niraj Dudani and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _HSOLVE_H
#define _HSOLVE_H

#include <set>
#include <chrono>
using namespace std::chrono;

/**
 * HSolve adapts the integrator HSolveActive into a MOOSE class.
 */
class HSolve: public HSolveActive
{
public:
    HSolve();
    ~HSolve();

    void process( const Eref& hsolve, ProcPtr p );
    void reinit( const Eref& hsolve, ProcPtr p );

    void setSeed( Id seed );
    Id getSeed() const; 		/**< For searching for compartments:
								 *   seed is the starting compt.     */

    void setPath( const Eref& e, string path );
    string getPath( const Eref& e ) const;
    /**< Path to the compartments */

    void setDt( double dt );
    double getDt() const;

    void setCaAdvance( int caAdvance );
    int getCaAdvance() const;

    void setVDiv( int vDiv );
    int getVDiv() const;

    void setVMin( double vMin );
    double getVMin() const;

    void setVMax( double vMax );
    double getVMax() const;

    void setCaDiv( int caDiv );
    int getCaDiv() const;

    void setCaMin( double caMin );
    double getCaMin() const;

    void setCaMax( double caMax );
    double getCaMax() const;

    // Interface functions defined in HSolveInterface.cpp
    double getInitVm( Id id ) const;
    void setInitVm( Id id, double value );

    double getVm( Id id ) const;
    void setVm( Id id, double value );

    double getCm( Id id ) const;
    void setCm( Id id, double value );

    double getEm( Id id ) const;
    void setEm( Id id, double value );

    double getRm( Id id ) const;
    void setRm( Id id, double value );

    double getRa( Id id ) const;
    void setRa( Id id, double value );

    // Im is read-only
    double getIm( Id id ) const;

    // Ia is read-only
    double getIa( Id id ) const;

    double getInject( Id id ) const;
    void setInject( Id id, double value );

    void addInject( Id id, double value );

    /// Interface to compartments
    //~ const vector< Id >& getCompartments() const;

    void addGkEk( Id id, double v1, double v2 );
    void addConc( Id id, double conc );
    /// Interface to channels
    //~ const vector< Id >& getHHChannels() const;
    void setPowers(
        Id id,
        double Xpower,
        double Ypower,
        double Zpower );

    int getInstant( Id id ) const;
    void setInstant( Id id, int instant );

    double getHHChannelGbar( Id id ) const;
    void setHHChannelGbar( Id id, double value );

    double getEk( Id id ) const;
    void setEk( Id id, double value );

    double getGk( Id id ) const;
    void setGk( Id id, double value );

    // Ik is read-only
    double getIk( Id id ) const;

    double getX( Id id ) const;
    void setX( Id id, double value );

    double getY( Id id ) const;
    void setY( Id id, double value );

    double getZ( Id id ) const;
    void setZ( Id id, double value );

    /// Assign scale factor for HH channel conductance.
    void setHHmodulation( Id id, double value );

    /// Interface to CaConc
    //~ const vector< Id >& getCaConcs() const;
    double getCa( Id id ) const;
    void setCa( Id id, double Ca );
    void iCa( Id id, double iCa ); // Add incoming calcium current.

    double getCaBasal( Id id ) const;
    void setCaBasal( Id id, double CaBasal );

    void setTauB( Id id, double tau, double B );

    double getCaCeiling( Id id ) const;
    void setCaCeiling( Id id, double floor );

    double getCaFloor( Id id ) const;
    void setCaFloor( Id id, double floor );

    /// Interface to external channels
    //~ const vector< vector< Id > >& getExternalChannels() const;

    static const Cinfo* initCinfo();

    static const std::set<string>& handledClasses();
    /**< Returns the set of classes "handled" by HSolve */
    static void deleteIncomingMessages( Element * orig, const string finfo);
    /**< Delete messages coming into this particular
     *   element if its class that is handled by HSolve */

private:
    static vector< Id > children( Id obj );
    static Id deepSearchForCompartment( Id base );

    void setup( Eref hsolve );
    void zombify( Eref hsolve ) const;
    void unzombify() const;

    // Mapping global Id to local index. Defined in HSolveInterface.cpp.
    void mapIds();
    void mapIds( vector< Id > id );
    unsigned int localIndex( Id id ) const;
    map< Id, unsigned int > localIndex_;

    double dt_;
    string path_;
    Id seed_;

    double totalTime_ = 0.0;
    high_resolution_clock::time_point t0_, t1_;
};

#endif // _HSOLVE_H
