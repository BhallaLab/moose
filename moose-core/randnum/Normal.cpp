/*******************************************************************
 * File:            Normal.cpp
 * Description:      
 * Author:          Subhasis Ray
 * E-mail:          ray.subhasis@gmail.com
 * Created:         2007-10-31 13:48:53
 ********************************************************************/
/**********************************************************************
 ** This program is part of 'MOOSE', the
 ** Messaging Object Oriented Simulation Environment,
 ** also known as GENESIS 3 base code.
 **           copyright (C) 2003-2005 Upinder S. Bhalla. and NCBS
 ** It is made available under the terms of the
 ** GNU General Public License version 2
 ** See the file COPYING.LIB for the full notice.
 **********************************************************************/

#ifndef _NORMAL_CPP
#define _NORMAL_CPP
#include "Normal.h"
#include "randnum.h"
#include "../utility/numutil.h"
#include <cmath>
#include <iostream>
using namespace std;

extern unsigned long genrand_int32(void);
Normal::Normal(double mean, double variance, NormalGenerator method):mean_(mean), variance_(variance), method_(method)
{
    if (variance <= 0.0 )
    {
        cout << "Warning: cannot set variance <= 0. Setting to 1.0." << endl;
        variance_ = 1.0;
    }
    
    isStandard_ = isClose< double >(0.0, mean, DBL_EPSILON) && isClose < double > (1.0, variance, DBL_EPSILON);
    switch(method)
    {
        case ALIAS:
            generator_ = &(Normal::aliasMethod);
            break;
        case BOX_MUELLER:
            generator_ = &(Normal::BoxMueller);
            break;
#ifdef USE_GSL
        case ZIGGURAT:
            generator_ = &(Normal::gslZiggurat);
            break;            
#endif
        default:
            cerr << "ERROR: Normal() - generator method# " << method << ". Don't know how to do this. Using alias method."<<endl;
            generator_ = &(Normal::aliasMethod);
    }
}

double dummy()
{
    return 0.0;    
}

double Normal::getNextSample() const
{
    double sd = sqrt(variance_);
    double sample = generator_();
    if (!isStandard_)
    {
        sample = mean_ + sd*sample;
    }
    return sample;    
}

double Normal::getMean() const
{
    return mean_;
}

void Normal::setMean( double mean)
{
    mean_ = mean;
    isStandard_ = isClose< double >(0.0, mean_, DBL_EPSILON) && isClose< double >(1.0, variance_, DBL_EPSILON);
}

double Normal::getVariance() const
{
    return variance_;
}

void Normal::setVariance( double variance )
{
    if (variance <= 0.0)
    {
        cout << "Warning: cannot set variance < 0." << endl;
        return;
    }    
    variance_ = variance;
    isStandard_ = isClose< double > (0.0, mean_, DBL_EPSILON) && isClose< double >(1.0, variance_, DBL_EPSILON);
}

NormalGenerator Normal::getMethod(void)
{
    return method_;
}

void Normal::setMethod(NormalGenerator method)
{
    method_ = method;
    switch(method)
    {
        case ALIAS:
            generator_ = &(Normal::aliasMethod);
            break;
        case BOX_MUELLER:
            generator_ = &(Normal::BoxMueller);
            break;
#ifdef USE_GSL
        case ZIGGURAT:
            generator_ = &(Normal::gslZiggurat);
            break;
#endif
        default:
            cerr << "ERROR: Normal() - generator method# " << method << ". Don't know how to do this. Using alias method."<<endl;
            generator_ = &(Normal::aliasMethod);
            method_ = ALIAS;            
    }
}

#ifdef USE_GSL
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

double Normal::gslZiggurat()
{
    static const gsl_rng_type * T;
    static gsl_rng * r;
    static bool inited = false;
    if (!inited)
    {
        /* create a generator chosen by the 
           environment variable GSL_RNG_TYPE */
    
        gsl_rng_env_setup();
        
        T = gsl_rng_default;
        r = gsl_rng_alloc (T);
        inited = true;        
    }
    return gsl_ran_gaussian_ziggurat(r, 1.0);    
}

#endif // !USE_GSL
/**
   Very simple but costly implementation
*/
double Normal::BoxMueller()
{
    double result;    
    double a, b, r;
    do
    {
        a = 2.0 * mtrand() - 1.0;
        b = 2.0 * mtrand() - 1.0;
        r = a * a + b * b;
    } while ( r >= 1.0 );
    r = sqrt( - 2.0 * log(r) / r );
    result = r * a;        
    
    return result;    
}
/**
   Refer to:
   Eine Alias-Methode zur Stichprohenentnahme aus Normalverteilungen.
   JH Ahrens and U Dieter, 19 89, Computing

   We are assuming size of long to be 32 bit
*/

const unsigned long y[] = 
{
    200,     199,     199,     198,     197,     196,     195,     193,     192,     190,     188,     186,     184,     181,     179,     176,     173,     170,     167,     164,     161,     157,     154,     151,     147,     143,     140,     136,     132,     128,     125,     121,     117,     113,     110,     106,     102,     98,     95,     91,     88,     84,     81,     77,     74,     71,     68,     64,     61,     59,     56,     53,     50,     48,     45,     43,     40,     38,     36,     34,     32,     30,     28,     27,     25,     23,     22,     20,     19,     18,     17,     15,     14,     13,     12,     11,     11,     10,     9,     8,     8,     7,     6,     6,     5,     5,     4,     4,     4,     3,     3,     3,     2,     2,     2,     2,     2,     1,     1,     1,     1,     1,     1,     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0
};

const long a[] = 
{
    31,     30,     29,     28,     27,     26,     25,     24,     23,     22,     21,     20,     27,     26,     25,     24,     23,     22,     21,     -1,     19,     19,     21,     20,     19,     18,     17,     16,     15,     14,     13,     12,     19,     18,     17,     16,     15,     14,     13,     12,     11,     10,     9,     8,     7,     6,     5,     4,     3,     2,     1,     0,     23,     22,     21,     20,     19,     18,     17,     16,     15,     14,     13,     12,     11,     10,     9,     8,     7,     6,     5,     4,     3,     2,     1,     0,     51,     50,     49,     48,     47,     46,     45,     44,     43,     42,     41,     40,     39,     38,     37,     36,     35,     34,     33,     32,     31,     30,     29,     28,     27,     26,     25,     24,     23,     22,     21,     20,     19,     18,     17,     16,     15,     14,     13,     12,     11,     10,     9,     8,     7,     6,     5,     4,     3,     2,     1,     0       
};

const unsigned long q[] = 
{
    28,     32,   33,   36,   40,   43,   45,   47,   51,   53,   57,   59,   27,   37,   43,   54,   28,   45,   60,   63,   49,   61,   52,   34,   63,   46,   34,   18,   47,   40,   36,   29,   61,   57,   53,   51,   47,   43,   40,   37,   33,   31,   27,   25,   21,   19,   17,   14,   11,   9 ,   8,   5,   54,   51,   49,   46,   44,   41,   39,   37,   35,   33,   31,   29,   28,   26,   24,   23,   21,   20,   19,   18,   16,   15,   14,   13,   12,   12,   11,   10,   9,   9,   8,   7,   7,   6,   6,   5,   5,   5,   4,   4,   4,   3,   3,   3,   3,   3,   2,   2,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1   
};
const double c = 0.004996971959878404;

const double d = 1.861970434352886050;
/*********************************************************************************************************************************/
/* TODO: This implementation has a bug. The sample mean deviates a lot
 * from the generator mean for very small numbers, like 1e-7
 * Need to make sure that this is not due to 32/64 bit incompatibility. 
 */
/*********************************************************************************************************************************/
double Normal::aliasMethod()
{
    double result;
    
    unsigned long uniform;
    unsigned long uniform_prime;
    unsigned long x_num;
    unsigned long t_num;
    unsigned long v_num;
    
    unsigned long i_num, k_num, sgn;

    while (true)
    {
        
        // 1) u = .B1..B7 (base 256) - we take the little endian approach
        uniform = genrand_int32(); // .B1B2B3B4 - limited by precision
    
        // 1a) s = B1[0]    
        sgn = uniform  & 0x80000000UL;
        // 1b) B1[0] = 0
        uniform = uniform & 0x7fffffffUL;
    
        // 2) i = B1[1..7]
        i_num = uniform >> 24;
        // 3) k = (B2 ^ B4) and retain least significant 6 bits of k
        k_num = ((uniform >> 16) ^ uniform ) & 0x0000003fUL;
    
        // 4) k <q[i]? goto 7
        if ( k_num >= q[i_num] )
        {
            // 5) i = a[i]
            i_num = a[i_num];
        
            // 5a)i = ~0? goto 11
            if (i_num != ~(0UL))
            {
            
                // 6) B1 = i
                uniform = (uniform & 0x00ffffffUL) | (i_num << (WORD_LENGTH-8));
            
                // 6a) x = 8*u
                x_num = uniform << 3;
            
                // 6b) goto 8 - s = 0? return x: return -x
         //        result = x_num/4294967296.0; // convert to double
//                 result = (sgn == 0) ? result : -result;
                //return result;                
                break;
            }
        }
        else
        {
            
    
            // 7) x = 8*u
            x_num = uniform << 3;
    
            // 7a) if (k <= y[i-1] - y[i]) goto 9
            if (k_num > y[i_num-1] - y[i_num])
            {
                // 8) s = 0? return x: return -x        
//                 result = x_num/4294967296.0; // convert to double
//                 result = (sgn == 0) ? result : -result;
                //return result;                
                break;
                
            }
    
            // 9) u' = .B1'..B7' .. using B1-B4 for precision limitation
            uniform_prime = genrand_int32();
            // 9a) t = x*x/2
            t_num = x_num*(x_num/2);
    
            // 9b) v = c*(y[i] + u'*(y[i-1] - y[i] + 1))
            v_num = (unsigned long)(c*(y[i_num] + uniform_prime*(y[i_num - 1] - y[i_num] + 1)));
    
            // 10) v > exp(-t)? goto 1: goto 8
        
        
            if ( testAcceptance(t_num, v_num ))
            {
                result = x_num/4294967296.0; // convert to double
                result = (sgn == 0) ? result : -result;
                //return result;
                break;                
                
            }else
            {
                continue;
            }
        }        
    
        // 11) u = .B1..B7
        uniform = genrand_int32();
    
        // 11a) u < 1/9? goto 1
        if ( uniform/4294967296.0 < 1.0/9.0)
        {
            continue;
        }
    
        // 12) x = 3.75 + 0.25/u
        unsigned long divider = ((uniform << 24) +
                                 ((uniform << 16) & 0x00ff0000UL) +
                                 ((uniform << 8) & 0x0000ff00UL) +
                                 (uniform & 0x000000ffUL));
    
        x_num = (unsigned long)( 3.75 + (1.0*0x40000000UL)/divider);
        // 12a) t = x*x/2-8, v = d*u*u*u'
        t_num = x_num*x_num/2 - 0x8UL;
        v_num = (unsigned long)(d*uniform*uniform*uniform_prime);
    
        // 12b) goto 10
        // 10) v > exp(-t)? goto 1: goto 8
        if ( testAcceptance(t_num, v_num) )
        {
            // 8)
//             result = x_num/4294967296.0; // convert to double
//             result = (sgn == 0) ? result : -result;
//            return result;
            break;
            
            
        }else // goto 1
        {
            continue;
        }
    }
    // 8)
    result = (sgn == 0) ? x_num/4294967296.0 : -(x_num/4294967296.0);
    
    return result;
    
}
/**
   Method to check if v > exp(-t) without doing the exponentiation
   TODO: try to do everything in integer arithmetic
*/
bool Normal::testAcceptance(unsigned long t_num, unsigned long v_num)
{
    bool accept = false;
    bool cont = true;
    
    double t = t_num/4294967296.0;
    double v = v_num/4294967296.0;
//    return (v > exp(-t));
     
    while(cont)
    {   // a)
        if ( t >= LN2 )
        {
            // b)
            t = t - LN2;
            v += v;
                
            if ( v > 1 )
            {
                return false;                
            }
            else
            {                
                continue;
            }
        }
        // c)
        v = v + t - 1;
        if ( v <= 0 )
        {
            return true;            
        }
        // d)
        double r = t * t;
        v = v + v - r;
        if ( v > 0)
        {
            return false;            
        }
        double c = 3.0;
        // e)
        while(true)
        {
            r *= t;
            v = v * c + r;
            if ( v <= 0)
            {
                return true;
            }
            c += 1.0;
            // f)
            r *= t;
            v = v * c - r;
            if ( v > 0 )
            {
                return false;
            }
                c += 1.0;
        }           
    }
    return accept;    
}

/**
   See Knuth, TAOCP Vol 2 Sec 3.4.1
   Algorithm M: The Rectangle-wedge-tail method discovered by G Marsaglia
   TODO: implement it by filling the gaps
*/
#if 0
double algorithmM() // not implemented
{
    cout << "algorithmM() - not yet implemented." << endl;
    return 0.0;
    
    double result;
    double u;
    int psi;
    // TODO: construct these auxiliary tables.
    static int P[32];
    static int Q[32];
    static int Y[32];
    static int Z[32];
    static int S[32];
    static int D[32];
    static int E[32];
    // M1 [ Get U ]
    // U = (.b0b1b2....bt)2
    u = mtrand();
    // M2 [Rectangle?]
    // psi = b0
    // j = (b1b2b3b4b5)2
    // f = (.b6b7..bt)2
    // if ( f >= Pj ) { X = Yj + f*Zj; goto M9 }
    // else if ( j <= 15, i.e. b1 == 0 ) { X = Sj + f*Qj; goto M9 }

    // M3 [ Wedge or tail? ]
    // Now 15 <= j <= 31 and each particular value j occurs with probability pj
    // if ( j == 31 ) goto M7

    // M4 [ Get U <= V ]
    // Generate two new uniform deviates, U and V; if U > V, exchange U <-> V
    // Set X =S(j-15) + U/5

    // M5 [ Easy case? ]
    // if ( V <= Dj ) goto M9

    // M6 [ Another try? ]
    // if ( V > U + Ej * (exp[( S(j-14)^2 - X^2 )/2] - 1 ) ) goto M4
    // else goto M9

    // M7 [ Get supertail deviate ]
    // Generate two new independent uniform deviates U and V
    // set X = sqrt(9-2*lnV)

    // M8 [ Reject? ]
    // if ( U*X >= 3 ) goto M7

    // M9 [ Attach sign ]
    // if ( psi == 1 ) X = -X

    return result;    
}
#endif
/**
   TODO: Not yet implemented.
*/
double algorithmF()
{
    cout << "algorithmF() - not implemented." << endl;
    
    double result = 0;
    return result;    
}


#ifdef DO_UNIT_TESTS
#include <cassert>
#include <vector>

void checkMeanVariance(Normal& normalGen, unsigned seqLen)
{
    vector < double > seq;
    double mean = 0.0;
    for ( unsigned ii = 0; ii < seqLen; ++ii)
    {
        double sample = normalGen.getNextSample();
        seq.push_back(sample);
        mean +=  sample;        
    }
    mean /= seqLen;
    cout << "Testing Normal: " << normalGen.getMethod() << " method for " << seqLen << " samples :: mean set - " << normalGen.getMean() << ", sample mean - " << mean << ". ";
    double variance = 0.0;
    for ( unsigned ii = 0; ii < seqLen; ++ ii)
    {
        variance += (mean - seq[ii]) * (mean - seq[ii]);
    }
    variance /= seqLen;
    cout << "variance set - " << normalGen.getVariance() << ", sample variance - " << variance << endl;
}

void testNormal()
{
    //  const unsigned SEQ_LEN = 1000;
    
    Normal* normalGen = new Normal();
    // make sure default constructor creates standard normal
    assert(isClose< double >(normalGen->getMean(), 0.0, DBL_EPSILON)); 
    assert(isClose< double > (normalGen->getVariance(), 1.0, DBL_EPSILON));
    // check if setting method is effective
    normalGen->setMethod(ALIAS);
    assert(normalGen->getMethod() == ALIAS);
    checkMeanVariance(*normalGen, 1000); // check the sample mean and variance for "alias method"
    delete normalGen;
    normalGen = new Normal(100.0, 33.0, BOX_MUELLER);
    assert(normalGen->getMethod() == BOX_MUELLER);
    assert(isClose< double >(normalGen->getMean(), 100.0, DBL_EPSILON)); 
    assert(isClose(normalGen->getVariance(), 33.0, DBL_EPSILON));
    checkMeanVariance(*normalGen, 1000); // check the sample mean and variance for "Box-Mueller method"
    delete normalGen;
    
    // non standard normal distr.
    normalGen = new Normal();
    normalGen->setMean(-100.0);
    normalGen->setVariance(3.0);
    checkMeanVariance(*normalGen, 1000);
}
#if 0 // keep it out of compilation
void testByte2Double()
{
    double res;
    
    unsigned char bytes[] = 
        {
            0, 0, 0, 0, 0
        };
    bytes[4] = 0x5;
    res = byte2double(bytes, 5);
    printf("0.0000000005 H = %.20g\n", res);
    bytes[3] = 0x4;
    res = byte2double(bytes, 4);
    printf("0.00000004 H = %.20g\n", res);
    bytes[2] = 0x3;
    res = byte2double(bytes, 3);
    printf("0.000003 H = %.20g\n", res);
    bytes[1] = 0x2;    
    res = byte2double(bytes, 2);
    printf("0.0002 H = %.20g\n", res);
    assert(isClose< double >(res, 0.000030517578125));
    bytes[0] = 0x1;
    res = byte2double(bytes, 1);
    printf("0.01 H = %.20g\n", res);
    assert(res == 0.00390625);
    res = byte2double(bytes, 5);    
    printf("0.0102030405 in hex = %.20g\n", res);    
    assert(isClose< double >(res,  0.0039369473279293742962, 1e-9));// the right comparison is from the same program. Should check for various architectures.
}
#endif // #if 0
#endif  // !DO_UNIT_TESTS

#if defined(DO_UNIT_TESTS) && defined(TEST_MAIN)
// Test main

int main(void)
{
    double mean = 0.0;
    double sd = 0.0;
    double sum = 0.0;
    int freq [200];
    Normal n(BOX_MUELLER);
    
    for ( int i = 0; i < 200; ++i )
    {
        freq[i] = 0;
    }
    
    for ( int i = 0; i < 10000; ++i )
    {
        double p = n.getNextSample();//aliasMethod();
        int index = (int)(p*100)+99;
        cout << index << " ] " << p << endl;
        if ( index < 0 )
            index = 0;
        else if ( index > 199 )
            index = 199;
        
        freq[index]++;
        
        sum += p;
        sd = p*p;        
    }
    mean = sum/1000;
    sd = sd/1000;
    cout << mean << " " << sd << endl;
    for ( int i = 0; i < 200; ++i )
    {
        cout << freq[i] << endl;
    }
 
    testNormal();
 
    return 0;
}
#endif // ! define(DO_UNIT_TESTS) && defined(TEST_MAIN)


#endif
