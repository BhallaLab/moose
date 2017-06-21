// PulseGen.h --- 
// 
// Filename: PulseGen.h
// Description: 
// Author: Subhasis Ray
// Maintainer: 
// Created: Mon Feb 20 01:21:32 2012 (+0530)
// Version: 
// Last-Updated: Mon Feb 20 16:42:41 2012 (+0530)
//           By: Subhasis Ray
//     Update #: 60
// URL: 
// Keywords: 
// Compatibility: 
// 
// 

// Commentary: 
// 
// 
// 
// 

// Change log:
// 
// 2012-02-20 01:22:01 (+0530) Subha - started porting old moose code
// to dh_branch.
// 

// Code:

#ifndef _PULSEGEN_H
#define _PULSEGEN_H
/**
 * PulseGen acts as a pulse generator. It generates square pulses of
 * specified duration and amplitude. Two consecutive pulses are
 * separated by specified delay.
 */

class PulseGen
{
    /**
       With trigMode = FREE_RUN the pulse generator will create a
       train of pulses determined by the firstDealy, firstWidth,
       firstLevel, secondDelay, secondWidth and secondLevel.       
    */
    static const int FREE_RUN = 0;
    /**
       With trigMode = EXT_TRIG the pulse generator may be triggered
       or may generate a single pulse and then stop.  The logic
       depends on trigTime field. If trigTime is negative ( which it
       is by default and remains so unless there is a non zero input
       trigger to the PulseGen object ), the output remains at
       baselevel. For trigTime >= 0, if there is no input trigger, the
       PulseGen object will produce pair of pulses, the first one
       starting at firstDelay and staying at firstLevel for firstWidth
       time. And then the second one starting at secondDelay time
       after the start of the first pulse, staying at secondLevel for
       secondWidth time. If secondDelay is less than firstWidth, there
       will be no second pulse.

     */
    static const  int EXT_TRIG = 1;
    /**
       With trigMode = EXT_GATE, the pulse occurs firstDelay later
       from the leading edge of the input.
     */
    static const int EXT_GATE = 2;    
    
#ifdef DO_UNIT_TESTS
    friend void testPulseGen();
#endif
  public:
    PulseGen();
    ~PulseGen();
    
    /////////////////////////////////////////////////////////////
    // Value field access function definitions
    /////////////////////////////////////////////////////////////
    
    void setFirstDelay(double value );
    double getFirstDelay() const;
    void setFirstWidth(double value );
    double getFirstWidth(  ) const;
    void setFirstLevel(double value );
    double getFirstLevel(  ) const;
    void setSecondDelay(double value );
    double getSecondDelay(  ) const;
    void setSecondWidth(double value );
    double getSecondWidth(  ) const;
    void setSecondLevel(double value );
    double getSecondLevel(  ) const;
    void setBaseLevel(double value);
    double getBaseLevel() const;
    void setTrigMode(unsigned int value);
    unsigned int getTrigMode() const;
    double getOutput() const ;
    double getTrigTime() const;
    int getPreviousInput() const;
    void setCount(unsigned int count);
    unsigned int getCount() const;
    void setLevel(unsigned int pulseNo, double level);
    void setWidth(unsigned int pulseNo, double width);
    void setDelay(unsigned int pulseNo, double delay);    
    double getWidth(unsigned int index) const;
    double getDelay(unsigned int index) const;
    double getLevel(unsigned int index) const;
    
    /////////////////////////////////////////////////////////////
    // Dest function definitions
    /////////////////////////////////////////////////////////////
    
    void input(double input);

    void process( const Eref& e, ProcPtr p );
    
    void reinit( const Eref& e, ProcPtr p );

    /////////////////////////////////////////////////////////////
    static const Cinfo* initCinfo();

  protected:
    vector <double> delay_;
    vector <double> level_;
    vector <double> width_;
    
    double output_;
    double baseLevel_;
    double trigTime_;
    unsigned int trigMode_;
    bool secondPulse_;
    
    int prevInput_;
    int input_;    
};

#endif // _PULSEGEN_H
// 
// PulseGen.h ends here
