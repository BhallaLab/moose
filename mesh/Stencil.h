/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2011 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#ifndef _STENCIL_H
#define _STENCIL_H

class Stencil
{
	public:
		Stencil();

		virtual ~Stencil();

		/**
		 * computes the Flux f in the voxel on meshIndex. Takes the
		 * matrix of molNumber[meshIndex][pool] and
		 * the vector of diffusionConst[pool] as arguments.
		 */
		virtual void addFlux( unsigned int meshIndex,
			vector< double >& f, const vector< vector< double > >& S,
			const vector< double >& diffConst ) const = 0;

	private:
};

/**
 * Dummy stencil, used when there is no diffusion.
 */
class DummyStencil: public Stencil
{
	public:
		DummyStencil();
		~DummyStencil();
		void addFlux( unsigned int meshIndex, vector< double >& f,
			const vector< vector< double > >& S,
			const vector< double >& diffConst ) const;
	private:
};

/**
 * Assumes a one-D uniform cartesian array, so the index offset for
 * neighboring mesh points is always 1.
 */
class LineStencil: public Stencil
{
	public:
		LineStencil( double h );
		~LineStencil();
		void addFlux( unsigned int meshIndex, vector< double >& f,
			const vector< vector< double > >& S,
			const vector< double >& diffConst ) const;
	private:
		double h_;
		double invHsq_;
};

/**
 * Assumes a 2-D rectangular array, so the index offset for
 * neighboring mesh points in x is 1, and in y is nx.
 */
class RectangleStencil: public Stencil
{
	public:
		RectangleStencil( double dx, double dy, unsigned int nx );
		~RectangleStencil();
		void addFlux( unsigned int meshIndex, vector< double >& f,
			const vector< vector< double > >& S,
			const vector< double >& diffConst ) const;
	private:
		double dx_;
		double dy_;
		double invDxSq_;
		double invDySq_;
		unsigned int nx_;
};

/**
 * Assumes a 3-D cuboid array
 */
class CuboidStencil: public Stencil
{
	public:
		CuboidStencil( double dx, double dy, double dz,
			unsigned int nx, unsigned int ny );
		~CuboidStencil();
		void addFlux( unsigned int meshIndex, vector< double >& f,
			const vector< vector< double > >& S,
			const vector< double >& diffConst ) const;
	private:
		double dx_;
		double dy_;
		double dz_;
		double invDxSq_;
		double invDySq_;
		double invDzSq_;
		unsigned int nx_;
		unsigned int ny_;
};

#endif // _STENCIL_H
