/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _MATRIXOPS_H
#define _MATRIXOPS_H
/////////////////////////////
//Few functions for performing simple matrix operations.
//Note that all matrices here are square, which is the type encountered in
//solving the differential equations associated with first-order kinetics of
//the Markov channel model.
//
//Author : Vishaka Datta S, 2011, NCBS
////////////////////////////
using std::vector;

typedef vector< vector< double > > Matrix;
typedef vector< double > Vector;

//Idea taken from the implementation of the DGETRF method in LAPACK. When
//the pivot is zero, we divide by a small number instead of simply throwing up
//an error and not returning a result.
#define EPSILON 1e-15

#define FIRST 1
#define SECOND 2

#define DUMMY 0

//Just a debug function to print the matrix.
void matPrint( Matrix* );

void vecPrint( Vector* );

//Computes product of two square matrices.
//Version 1 : Returns the result in a new matrix.
Matrix* matMatMul( Matrix*, Matrix* );

//Version 2 : Returns the result in either of the matrices passed in.
//The third parameter determines which matrix to destroy in order to return
//the result in it.
void matMatMul( Matrix*, Matrix*, unsigned int );

//Special version to multiply upper and lower triangular matrices (in that
//order). Used specially by the matInv method. The result is stored in the
//first matrix.
//Thanks to the structure of this multiplication, the multiplication can be
//carried out in place.
void triMatMul( Matrix*, Matrix* );

//Special matrix multiplication when the second matrix is a permutation matrix
//i.e. the columns are to be permuted.
//This helps in avoiding a matrix multiplication.
void matPermMul( Matrix*, vector< unsigned int >* );

//Computes sum of two square matrices.
Matrix* matMatAdd( const Matrix*, const Matrix*, double, double );

//Version 2 : Returns the result in either of the matrices that are passed as
//arguments.
void matMatAdd( Matrix*, Matrix*, double, double, unsigned int );

//Adds k*I to given matrix. Original is intact.
Matrix* matEyeAdd( const Matrix*, double );

//Version 2 : Returns the matrix in first argument i.e. original is destroyed.
void matEyeAdd( Matrix*, double, unsigned int );

//Computes the result of multiplying all terms of a matrix by a scalar and then
//adding another scalar i.e. B = a*A + b.
//First argument is scale i.e. 'a' and second argument is shift i.e. 'b'.
Matrix* matScalShift( const Matrix*, double, double );

void matScalShift( Matrix*, double, double, unsigned int );

//Computes the result of multiplying a row vector with a matrix (in that order).
Vector* vecMatMul( const Vector*, Matrix* );

//Computes the result of scaling and shifting a vector.
Vector* vecScalShift( const Vector*, double, double );

void vecScalShift( Vector*, double, double, unsigned int );

//Computes the result of multiplying a matrix with a column vector (in that order).
Vector* matVecMul( Matrix*, Vector* );

//Computes the sum of two vectors.
Vector* vecVecScalAdd( const Vector*, const Vector*, double, double );

//Version 2 : Returns the result in the first vector.
void vecVecScalAdd( Vector*, Vector*, double, double, unsigned int );

//Trace of matrix.
double matTrace( Matrix* );

//Calculate column norm of matrix.
double matColNorm( Matrix* );

//Plain old matrix transpose i.e. done out-of-place.
Matrix* matTrans( Matrix* );

//Matrix inverse implemented using LU-decomposition (Doolittle algorithm)
//Returns NULL if matrix is singular.
void matInv( Matrix*, vector< unsigned int >*, Matrix* );

//Carry out partial pivoting.
double doPartialPivot( Matrix*, unsigned int, unsigned int, vector< unsigned int >*);

/////////
//Memory allocation routines.
////////
Matrix* matAlloc( unsigned int );

Vector* vecAlloc( unsigned int );

#endif
