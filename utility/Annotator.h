/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2012 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/
#ifndef _M_ANNOTATOR_H
#define _M_ANNOTATOR_H

class Annotator
{
	public:
		Annotator();

		////////////////////////////////////////////////////////////////
		// Field assignment stuff.
		////////////////////////////////////////////////////////////////

		double getX() const;
		void setX( double v );
		double getY() const;
		void setY( double v );
		double getZ() const;
		void setwidth( double v );
		double getwidth() const;
		void setheight( double v );
		double getheight() const;
		void setZ( double v );
		string getNotes() const;
		void setNotes( string v );
		string getColor() const;
		void setColor( string v );
		string getTextColor() const;
		void setTextColor( string v );
		string getIcon() const;
		void setIcon( string v );
		string getSolver() const;
		void setSolver( string v );
		double getRuntime() const;
		void setRuntime( double v );
		string getdirpath() const;
		void setdirpath( string v );
		string getmodeltype() const;
		void setmodeltype( string v );
		static const Cinfo* initCinfo();
	private:
		double x_;
		double y_;
		double z_;
		double height_;
		double width_;
		string notes_;
		string color_;
		string textColor_;
		string icon_;
		string solver_;
		double runtime_;
		string dirpath_;
		string modeltype_;
};

#endif // _M_ANNOTATOR_H
