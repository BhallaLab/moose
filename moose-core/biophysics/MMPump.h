/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Multiscale Object Oriented Simulation Environment.
 **           copyright (C) 2003-2008
 **           Upinder S. Bhalla, Niraj Dudani and NCBS
 ** It is made available under the terms of the
 ** GNU Lesser General Public License version 2.1
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/

#ifndef _MMPUMP_H
#define _MMPUMP_H
/*This is base class for MMPump*/
class MMPump
{
public:
  MMPump();
  void process( const Eref & e, ProcPtr p);
  void reinit(const Eref &e, ProcPtr p);

  double getVmax(const Eref& e) const; //Vmax of the pump
  void setVmax(const Eref& e,double value);

  int getVal(const Eref& e) const; //Valence
  void setVal(const Eref& e,int value);

  double getKd(const Eref& e) const;           //  Pump's Kd
  void setKd(const Eref& e,double value);



  static SrcFinfo2< double,double >* PumpOut(); //Pump parameters;

  static const Cinfo * initCinfo();

private:

  double Vmax_;
  double Kd_;
};




#endif //_MMPUMP_BASE_H
