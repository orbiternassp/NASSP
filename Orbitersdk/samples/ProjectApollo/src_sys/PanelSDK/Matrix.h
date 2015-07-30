/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2003-2005 Radu Poenaru

  System & Panel SDK (SPSDK)

  Project Apollo is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Project Apollo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Project Apollo; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  See http://nassp.sourceforge.net/license/ for more details.

  **************************************************************************/

#ifndef _MATRIX_
#define _MATRIX_

#include "vectors.h"
#include <string.h>
enum {_XX=0,_XY,_XZ,_XF,_YX,_YY,_YZ,_YF,_ZX,_ZY,_ZZ,_ZF,_FX,_FY,_FZ,_FF};
class vector3;
class matrix
{ public:
	double p[16];
	//constructors
	matrix(){ memset(p,0,sizeof(double)*16);} //null constructor
	matrix(double xx,double xy,double xz,
		   double yx,double yy,double yz,
		   double zx,double zy,double zz)		//constructor from data
				{set(xx,xy,xz,
					 yx,yy,yz,
					 zx,zy,zz);}
   matrix( double ax,double ay,double az)		//rot angle matrix
				{setang(ax,ay,az);}
   matrix(matrix &m)	{memcpy(p,m.p,sizeof(double)*16);}	//copy matrix

   //operators
   matrix  operator* (matrix &m);
   vector3 operator* (vector3 &v);
   matrix  operator* (double);
   void    operator*= (matrix &m);
   void    operator*= (double);
   matrix operator! ();		//inverse matrix
   matrix operator/ (double);

   //functions
   void set (double xx,double xy,double xz,
			 double yx,double yy,double yz,
			 double zx,double zy,double zz);
   void setang (double ax,double ay,double az);
   void identity();
   matrix invert();

   void trans(double ax,double ay,double az);
   void rotx(double aa);
   void roty(double aa);
   void rotz(double aa);
   void rot(double aa,vector3 v1,vector3 v2);
   double det();
   void setrot(double theta, vector3 axis);
   void setrot(vector3 p1, vector3 p2);
};
#endif
